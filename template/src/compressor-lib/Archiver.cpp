#include "Archiver.h"

Archiver::Archiver() : Encoder(), Decoder() {}

void Archiver::zip(std::string& archive_name, const std::vector<std::string>& files)
{
    std::ofstream archive(archive_name, std::ios::binary | std::ios::app);

    if (!archive.is_open())
        throw std::runtime_error("Failed to create file");

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

        encode(to_compress, archive);
        int delimiter = -1;
        archive.write((char*)(&delimiter), sizeof(delimiter));
        to_compress.close();
    }
    archive.close();
}