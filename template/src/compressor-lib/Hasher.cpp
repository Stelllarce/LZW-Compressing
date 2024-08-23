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

    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    std::string archive_data(buffer.begin(), buffer.end()); // Read the archive data into a string
    std::string archive_hash = md5(archive_data); // Calculate the hash of the archive
    out.seekp(0, std::ios::end); // Move put pointer to the end of the file
    out.write(archive_hash.c_str(), archive_hash.size()); // Write the hash at the end of the file
}