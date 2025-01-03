#include "Archiver.h"

Archiver::Archiver() : Encoder(), Decoder() {}

/**
 * @brief Compresses a single file and writes it to the archive
 * 
 * @param archive - archive file
 * @param file - absolute path of file to be compressed
 * @param relative_path - relative path of file to be comprssed
 */
void Archiver::zipSingle(std::ofstream& archive, const std::string& file, const std::string& relative_path)
{
    // Open the file to be compressed
    std::ifstream to_compress(file, std::ios::binary);

    // Skip compression if file is empty
    if (to_compress.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "File is empty: " << file << ", skipping...\n";
        return;
    }

    if (!to_compress.is_open())
        throw std::runtime_error("Failed to open file to compress");

    // Write the lenght of the path/name of the file that is being written
    int path_length = relative_path.size();
    archive.write((const char*)(&path_length), sizeof(path_length));
    // Write the path/name of the file that is being written
    archive.write(relative_path.c_str(), path_length);

    // Write the size of the compressed content
    int size_placeholder = 0; // Placeholder to be overwritten later by the actual size
    std::streampos size_pos = archive.tellp(); // Save position to write the size later
    archive.write((const char*)(&size_placeholder), sizeof(size_placeholder)); // Write the placeholder

    // Calculate the size of the compressed content and compress the file
    std::streampos start = archive.tellp();
    std::string encoded_file = encode(to_compress, archive); // Encoding the file
    std::streampos end = archive.tellp();

    // Write the size of the compressed content
    int size = end - start; // Size of the compressed file
    archive.seekp(size_pos); // Move to the position of the placeholder
    archive.write((const char*)(&size), sizeof(size)); // Overwrite the placeholder with the actual size

    // Move to the end of the file to append next file
    archive.seekp(0, std::ios::end);

    // Close the file that was compressed
    to_compress.close();
}


/**
 * @brief Compress the files into an archive
 * 
 * @param archive_name - name of the archive
 * @param files - absolute paths of files to be compressed
 * @param relative_paths - relative paths of files to be compressed
*/
void Archiver::zip(const std::string& archive_name, std::vector<std::string>& files, std::vector<std::string>& relative_paths)
{
    // Create archive file
    std::ofstream archive_out(archive_name, std::ios::binary);
    std::ifstream archive_in(archive_name, std::ios::binary);

    if (!archive_out.is_open())
        throw std::runtime_error("Failed to create archive for zipping");
    if (!archive_in.is_open())
        throw std::runtime_error("Failed to open archive for writing hash");
    
    // Append new content to the end of the file
    archive_out.seekp(0, std::ios::end);

    // Write the number of files to be written in the archive
    int num_files = files.size();
    archive_out.write((char*)(&num_files), sizeof(num_files));
   
    // One iterator for the whole paths to open the files and one for the relative paths to write to the archive
    std::vector<std::string>::iterator file_it = files.begin();
    std::vector<std::string>::iterator rel_path_it = relative_paths.begin();

    // Write the files to the archive
    for (; file_it != files.end(), rel_path_it != relative_paths.end(); ++file_it, ++rel_path_it)
    {
        zipSingle(archive_out, *file_it, *rel_path_it);
    }

    hash(archive_in, archive_out); // Write the hash of the archive

    // All files have been compressed and written to the archive, close the archive
    archive_in.close();
    archive_out.close();
}

/** 
 * @brief Restore all files from the archive
 * 
 * @param archive - archive file
 * @param extract_to - directory to extract the files to
*/
void Archiver::unzipSingle(std::ifstream& archive, const std::string& extract_to)
{
    // Read the length of the path/name of the file
    int path_length = 0;
    archive.read((char*)(&path_length), sizeof(path_length));

    // Temp buffer to store the path/name of the file
    char* file_path = new char[path_length + 1];
    archive.read(file_path, path_length);
    file_path[path_length] = '\0';

    // Read the size of the compressed content
    int file_size;
    archive.read((char*)(&file_size), sizeof(file_size));

    std::string full_extr_path = Path::createDirectory(extract_to, Path::getPath(file_path)); // Create the directory structure
    full_extr_path += file_path; // Append the file name to the directory structure
    
    // File to be restored
    std::ofstream to_decompress(full_extr_path, std::ios::binary);

    if (!to_decompress.is_open())
        throw std::runtime_error("Failed to create file to extract");

    // Decode the file
    decode(archive, to_decompress, file_size);
    to_decompress.close(); // Close the file
    delete[] file_path; // Free temp buffer
}
/**
 * @brief Restores selected files from the archive
 * 
 * @param archive - archive file
 * @param extract_to - directory to extract the files to
 * @param files_to_extract - absolute paths of files to be extracted
*/
void Archiver::unzipSelected(std::ifstream& archive, const std::string& extract_to, const std::set<std::string>& files_to_extract)
{
    // Read the length of the path/name of the file
    int path_length = 0;
    archive.read((char*)(&path_length), sizeof(path_length));

    // Temp buffer to store the path/name of the file
    char* file_path = new char[path_length + 1];
    archive.read(file_path, path_length);
    file_path[path_length] = '\0';

    // Read the size of the compressed content
    int file_size;
    archive.read((char*)(&file_size), sizeof(file_size));

    // If name of file inside the archive is not among the listed names, skip
    if (files_to_extract.find(file_path) == files_to_extract.end())
    {
        std::cerr << "File not found in the list of files to extract: " << file_path << '\n';
        archive.seekg(file_size, std::ios::cur);
        delete[] file_path;
        return;
    }

    std::string full_extr_path = Path::createDirectory(extract_to, Path::getPath(file_path)); // Create the directory structure
    full_extr_path += file_path; // Append the file name to the directory structure

    // File to be restored
    std::ofstream to_decompress(full_extr_path, std::ios::binary);

    if (!to_decompress.is_open())
        throw std::runtime_error("Failed to create file to extract");

    // Decode the file
    decode(archive, to_decompress, file_size); // Decode the file
    to_decompress.close(); // Close the file
    delete[] file_path; // Free temp buffer
}

/**
 * @brief Wrapper function for restoring files from an archive
 * 
 * @param archive_name - name of the archive
 * @param extract_to - directory to extract the files to
 * @param files_to_extract - absolte paths of files to be extracted
*/
void Archiver::unzip(const std::string& archive_name, const std::string& extract_to, const std::set<std::string>& files_to_extract)
{
    // Open the archive file
    std::ifstream archive(archive_name, std::ios::binary);

    if (!archive.is_open())
        throw std::runtime_error("Failed to open archive");

    // Read the number of files in the archive
    int num_files;
    archive.read((char*)(&num_files), sizeof(num_files));

    // Unzip the files
    if (files_to_extract.empty()) // If no files are specified, then user must've input '#'
    {
        for (int i = 0; i < num_files; ++i)
        {
            // Restore all files from the archive
            unzipSingle(archive, extract_to);
        }
    }
    else
    {
        // If the number of files to extract is greater than the number of files in the archive
        if (files_to_extract.size() > num_files) 
            throw std::runtime_error("Too many files to extract");

        for (int i = 0; i < num_files; ++i)
        {
            // Restore selected files from the archive
            unzipSelected(archive, extract_to, files_to_extract);
        }
        
    }
    // Close the archive
    archive.close();
}

/**
 * @brief Refreshes the archive with its new version
 * 
 * @param archive_name - name of the archive
*/
void Archiver::info(const std::string& archive_name) 
{
    // Open the archive file
    std::ifstream archive(archive_name, std::ios::binary | std::ios::in);

    if (!archive.is_open())
        throw std::runtime_error("Archive not found");

    // Read the number of files in the archive
    int num_files;
    archive.read((char*)(&num_files), sizeof(num_files));

    // Print the number of files in the archive
    std::cout << "Number of files inside archive: " << num_files << "\n";

    // Details omitted below, as the procedure is similar to the unzip function
    for (int i = 0; i < num_files; ++i)
    {
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));
        char* file_path;
        try
        {
            file_path = new char[path_length + 1];;
        }
        catch (const std::bad_alloc& e)
        {
            delete[] file_path;
            std::cerr << "Cannot read file name in file " << Path::getFile(archive_name) << '\n';
            return;
        }
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        std::cout << "File path: <<" << file_path << ">> ";

        int file_size;
        archive.read((char*)(&file_size), sizeof(file_size));

        std::cout << "Size of compressed content: " << file_size << "\n";

        delete[] file_path;
    }
    archive.close();
}
/**
 * @brief Refreshes the archive with its new version
 * 
 * @param archive_name - absolute path to archive
 * @param files - absolute paths of files to be included or updated in the archive 
 */
void Archiver::refresh(const std::string& archive_name, std::set<std::string>& files)
{
    // Open the original archive file
    std::ifstream archive(archive_name, std::ios::binary | std::ios::in);

    if (!archive.is_open())
        throw std::runtime_error("Failed to open archive");

    // Create a new archive file
    std::string temp_archive_name = Path::getPath(archive_name) + "/temp_" + Path::getFile(archive_name);
    std::ofstream new_archive(temp_archive_name, std::ios::binary);

    if (!new_archive.is_open())
        throw std::runtime_error("Failed to create temporary archive");

    // Read the number of files in the old archive
    int old_num_files;
    archive.read((char*)(&old_num_files), sizeof(old_num_files));

    // Write the old number of files temporarily to the temp archive
    new_archive.write((char*)(&old_num_files), sizeof(old_num_files));

    // Iterate through the files in the original archive
    for (int i = 0; i < old_num_files; ++i)
    {
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        // Check if the file is in the list of updated files
        bool updated_file = false;
        if (files.find(file_path) != files.end())
        {
            files.erase(files.find(file_path));
            updated_file = true;
        }

        int file_size;
        archive.read((char*)(&file_size), sizeof(file_size));

        // If the file is in the list of updated files, write the updated version to the new archive
        if (updated_file)
        {
            zipSingle(new_archive, file_path, file_path);
            archive.seekg(file_size, std::ios::cur);
        }
        else // Copy the file from the original archive to the new archive 
        {
            new_archive.write((char*)(&path_length), sizeof(path_length));
            new_archive.write(file_path, path_length);

            new_archive.write((char*)(&file_size), sizeof(file_size));
            char* buffer = new char[file_size];
            archive.read(buffer, file_size);
            new_archive.write(buffer, file_size);
            delete[] buffer;
        }

        delete[] file_path;
    }

    // If there are any files not present in the original archive, write them to the new archive
    if (!files.empty())
    {
        for (const std::string& file : files)
        {
            zipSingle(new_archive, file, Path::getFile(file));
        }
        int additional_size = files.size();
        int new_num_files = old_num_files + additional_size;
        new_archive.seekp(0, std::ios::beg);
        new_archive.write((char*)(&new_num_files), sizeof(new_num_files));
    }

    // Section to write the hash of the archive
    hash(archive, new_archive);

    // Closing the original archive
    archive.close();

    // Closing the new archive
    new_archive.close();

    // Deleting the original archive
    std::remove(archive_name.c_str());

    // Renaming the temporary archive to the original archive
    std::rename(temp_archive_name.c_str(), archive_name.c_str());
}
/**
 * @brief Checks for any errors in the archive
 * 
 * @param archive_name - absolute path to archive
 */
bool Archiver::errorCheck(const std::string& archive_name)
{
    std::ifstream archive(archive_name, std::ios::binary | std::ios::in);

    if (!archive.is_open())
        throw std::runtime_error("Failed to open archive");

    archive.seekg(0, std::ios::end); // Move get pointer to the end of the file
    std::streamsize failsafe = archive.tellg(); // Get the size of the archive

    if (failsafe < 32)
    {
        std::cerr << "Error: Archive is too small to contain a hash\n";
        archive.close();
        return false;
    }

    // Check if the hash is correct
    const std::streamsize HASH_SIZE = 32; // Size of the hash
    archive.seekg(-HASH_SIZE, std::ios::end); // Move get pointer to the start of the hash
    int archive_size = archive.tellg(); // Get the size of the archive
    std::vector<char> hash_buff(HASH_SIZE);
    archive.read(hash_buff.data(), HASH_SIZE);

    std::string archive_hash(hash_buff.begin(), hash_buff.end());
    
    if (archive_hash.size() != HASH_SIZE)
    {
        std::cerr << "Error: Hash not found in the archive or is damaged\n";
        archive.close();
        return false;
    }

    archive.seekg(0, std::ios::beg); // Move get pointer to the start of the file
    std::vector<char> buffer(archive_size);
    archive.read(buffer.data(), archive_size); // Read the archive data into a buffer
    std::string archive_data(buffer.begin(), buffer.end()); // Convert the buffer to a string
    std::string curr_hash = md5(archive_data); 

    if (curr_hash != archive_hash)
    {
        std::cerr << "Archive is damaged\n";
        return false;
    }
    else
    {
        std::cout << "Archive is intact\n";
    }

    archive.close();
    
    return true;
}