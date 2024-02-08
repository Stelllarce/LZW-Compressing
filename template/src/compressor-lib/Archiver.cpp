#include "Archiver.h"

Archiver::Archiver() : Encoder(), Decoder() {}

/**
 * @brief Compress the files into an archive
 * 
 * @param archive_name - name of the archive
 * @param files - files to be compressed
*/
void Archiver::zip(std::string& archive_name, const std::vector<std::string>& files)
{
    //TODO: may need to check if archive already exists

    // Create archive file
    std::fstream archive(archive_name, std::ios::binary | std::ios::out);

    if (!archive.is_open())
        throw std::runtime_error("Failed to create file");
    
    // Append new content to the end of the file
    archive.seekp(0, std::ios::end);

    // Write the number of files to be written in the archive
    int num_files = files.size();
    archive.write((char*)(&num_files), sizeof(num_files));

    // Write the files to the archive
    for (const std::string& file : files)
    {
        // Write the lenght of the path/name of the file that is being written
        int path_length = file.size();
        archive.write((char*)(&path_length), sizeof(path_length));
        // Write the path/name of the file that is being written
        archive.write(file.c_str(), path_length);

        // Open the file to be compressed
        std::ifstream to_compress(file, std::ios::binary);

        if (!to_compress.is_open())
            throw std::runtime_error("Failed to open file to compress");
        
        // Write the size of the compressed content
        int size_placeholder = 0; // Placeholder to be overwritten later by the actual size
        std::streampos size_pos = archive.tellp(); // Save position to write the size later
        archive.write((char*)(&size_placeholder), sizeof(size_placeholder)); // Write the placeholder

        // Calculate the size of the compressed content and compress the file
        std::streampos start = archive.tellp();
        encode(to_compress, archive); // Encoding the file
        std::streampos end = archive.tellp();

        // Write the size of the compressed content
        int size = end - start; // Size of the compressed file
        archive.seekp(size_pos); // Move to the position of the placeholder
        archive.write((char*)(&size), sizeof(size)); // Overwrite the placeholder with the actual size

        // Move to the end of the file to append next file
        archive.seekp(0, std::ios::end);
        
        // Close the file that was compressed
        to_compress.close();
    }
    // All files have been compressed and written to the archive, close the archive
    archive.close();
}

/** 
 * @brief Restore all files from the archive
 * 
 * @param num_files - number of files in the archive
 * @param archive - archive file
 * @param extract_to - directory to extract the files to
*/
void Archiver::unzipAll(const int num_files, std::ifstream& archive, const std::string& extract_to)
{
    for (int i = 0; i < num_files; ++i)
    {
        // Read the length of the path/name of the file
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        // Temp buffer to store the path/name of the file
        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        // Read the size of the compressed content
        int file_size;
        archive.read((char*)(&file_size), sizeof(file_size));

        // TODO: figure out how to create directories and not duplicate existing ones
        std::filesystem::path full_extr_pth = std::filesystem::path(extract_to) / std::filesystem::path(file_path);
        std::filesystem::create_directories(full_extr_pth.parent_path());

        // File to be restored
        std::ofstream to_decompress(full_extr_pth, std::ios::binary);

        if (!to_decompress.is_open())
            throw std::runtime_error("Failed to create file to extract");

        // Decode the file
        decode(archive, to_decompress, file_size);
        to_decompress.close(); // Close the file
        delete[] file_path; // Free temp buffer
    }
}
/**
 * @brief Restores selected files from the archive
 * 
 * @param archive - archive file
 * @param extract_to - directory to extract the files to
 * @param files_to_extract - name of files to be extracted
*/
void Archiver::unzipSelected(std::ifstream& archive, const std::string& extract_to, const std::set<std::string>& files_to_extract)
{
    for (int i = 0; i < files_to_extract.size(); ++i)
    {
        // Read the length of the path/name of the file
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        // Temp buffer to store the path/name of the file
        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        // Read the size of the compressed content
        int file_size;
        archive.read((char*)(&file_size), sizeof(file_size));

        // MAYBE PUT INSIDE A FUNCTION
        // If name of file inside the archive is not among the listed names, skip
        if (files_to_extract.find(file_path) == files_to_extract.end())
        {
            archive.seekg(file_size, std::ios::cur);
            archive.seekg(sizeof(int), std::ios::cur);
            delete[] file_path;
            continue;
        }

        // TODO: figure out how to create directories and not duplicate existing ones
        // std::filesystem::path full_extr_pth = extract_to / std::filesystem::path(file_path);
        // std::filesystem::create_directories(full_extr_pth.parent_path());

        // File to be restored
        std::ofstream to_decompress(file_path, std::ios::binary);

        if (!to_decompress.is_open())
            throw std::runtime_error("Failed to create file to extract");

        // Decode the file
        decode(archive, to_decompress, file_size); // Decode the file
        to_decompress.close(); // Close the file
        delete[] file_path; // Free temp buffer
    }
}

/**
 * @brief Wrapper function for restoring files from an archive
 * 
 * @param archive_name - name of the archive
 * @param extract_to - directory to extract the files to
 * @param files_to_extract - name of files to be extracted
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
        // Restore all files from the archive
        unzipAll(num_files, archive, extract_to);
    }
    else
    {
        // If the number of files to extract is greater than the number of files in the archive
        if (files_to_extract.size() > num_files) 
            throw std::runtime_error("Too many files to extract");

        // Restore selected files from the archive
        unzipSelected(archive, extract_to, files_to_extract);
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

        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        std::cout << "File path: <<" << file_path << ">> ";

        int file_size;
        archive.read((char*)(&file_size), sizeof(file_size));

        std::cout << "size of compressed content: " << file_size << "\n";

        delete[] file_path;
    }
    archive.close();
}