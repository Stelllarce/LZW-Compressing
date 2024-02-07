#include "Archiver.h"

Archiver::Archiver() : Encoder(), Decoder() {}

void Archiver::zip(std::string& archive_name, const std::vector<std::string>& files)
{
    std::fstream archive(archive_name, std::ios::binary | std::ios::out);

    if (!archive.is_open())
        throw std::runtime_error("Failed to create file");
    
    archive.seekp(0, std::ios::end);

    int num_files = files.size();
    archive.write((char*)(&num_files), sizeof(num_files));

    for (const std::string& file : files)
    {
        int path_length = file.size();
        archive.write((char*)(&path_length), sizeof(path_length));
        archive.write(file.c_str(), path_length);

        std::ifstream to_compress(file, std::ios::binary);

        if (!to_compress.is_open())
            throw std::runtime_error("Failed to open file to compress");
        
        int size_placeholder = 0;
        std::streampos size_pos = archive.tellp();
        archive.write((char*)(&size_placeholder), sizeof(size_placeholder));

        std::streampos start = archive.tellp();
        encode(to_compress, archive);
        std::streampos end = archive.tellp();

        int size = end - start;
        archive.seekp(size_pos);
        archive.write((char*)(&size), sizeof(size));

        archive.seekp(0, std::ios::end);

        int delimiter = -1;
        archive.write((char*)(&delimiter), sizeof(delimiter));
        to_compress.close();
    }
    archive.close();
}

void Archiver::unzipAll(const int num_files, std::ifstream& archive, const std::string& extract_to)
{
    for (int i = 0; i < num_files; ++i)
    {
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        std::filesystem::path full_extr_pth = std::filesystem::path(extract_to) / std::filesystem::path(file_path);
        std::filesystem::create_directories(full_extr_pth.parent_path());

        std::ofstream to_decompress(full_extr_pth, std::ios::binary);

        if (!to_decompress.is_open())
            throw std::runtime_error("Failed to create file to decompress");

        decode(archive, to_decompress);
        to_decompress.close();
        delete[] file_path;
    }
}

void Archiver::unzipSelected(std::ifstream& archive, const std::string& extract_to, const std::set<std::string>& files_to_extract)
{
    for (int i = 0; i < files_to_extract.size(); ++i)
    {
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        char* file_path = new char[path_length + 1];
        archive.read(file_path, path_length);
        file_path[path_length] = '\0';

        if (files_to_extract.find(file_path) == files_to_extract.end())
        {
            int file_size;
            archive.read((char*)(&file_size), sizeof(file_size));
            archive.seekg(file_size, std::ios::cur);
            archive.seekg(sizeof(int), std::ios::cur);
            delete[] file_path;
            continue;
        }

        std::filesystem::path full_extr_pth = extract_to / std::filesystem::path(file_path);
        std::filesystem::create_directories(full_extr_pth.parent_path());

        std::ofstream to_decompress(full_extr_pth, std::ios::binary);

        if (!to_decompress.is_open())
            throw std::runtime_error("Failed to create file to decompress");

        decode(archive, to_decompress);
        to_decompress.close();
        delete[] file_path;
    }
}

void Archiver::unzip(const std::string& archive_name, const std::string& extract_to, const std::set<std::string>& files_to_extract)
{
    std::ifstream archive(archive_name, std::ios::binary);

    if (!archive.is_open())
        throw std::runtime_error("Failed to open file");

    int num_files;
    archive.read((char*)(&num_files), sizeof(num_files));

    if (files_to_extract.find("#") != files_to_extract.end())
    {
        unzipAll(num_files, archive, extract_to);
    }
    else
    {
        if (files_to_extract.size() > num_files)
            throw std::runtime_error("Too many files to extract");

        unzipSelected(archive, extract_to, files_to_extract);
    }
    archive.close();
}