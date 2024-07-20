#include "Hasher.h"

void Hasher::hash(std::ifstream& in, std::ofstream& out)
{
    if(!in.is_open())
    {
        throw std::runtime_error("Error: File not open in read mode");
    }
    if(!out.is_open())
    {
        throw std::runtime_error("Error: File not open in write mode");
    }

    // Section to write the hash of the archive
    out.seekp(0, std::ios::end); // Move put pointer to the end to calculate archive size
    int archive_size = out.tellp(); // Archive size
    in.seekg(0, std::ios::beg); // Move get pointer at the start to read from file
    char* archive_data = new char[archive_size + 1]; // Buffer to store the archive data
    in.read(archive_data, archive_size);
    archive_data[archive_size] = '\0';
    std::string archive_hash = md5(archive_data); // Calculate the hash of the archive
    // Here the put pointer should be already at the end of the file
    out.write(archive_hash.c_str(), archive_hash.size()); // Write the hash at the end of the file
    delete[] archive_data; // Free the memory
}