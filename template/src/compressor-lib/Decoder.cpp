#include "Decoder.h"

Decoder::Decoder()
{
    decode_table.reserve(TABLE_SIZE); // Reserve space for the table
    initTable(); // Initialize the table
}

/**
 * @brief Decodes a .lzw compressed file
 * 
 * @param in - input file
 * @param out - output file
 * @param read_size - size of the file to decode (can be omitted)
 */
void Decoder::decode(std::ifstream& in, std::ofstream& out, int read_size)
{
    if (!in.is_open() || !out.is_open())
        throw std::runtime_error("File not open");

    int old_code, read_sz = 0; // Previous code
    in.read((char*)(&old_code), sizeof(int)); // Read the first code
    if (read_size > 0)
        read_sz += sizeof(old_code);
    out << decode_table[old_code].c_str(); // Write the first code's value

    // Variables for decoding
    std::string decoded = decode_table[old_code]; 
    std::string decoded_next = "";

    // New code is the currently read code
    int new_code;

    while (in.read((char*)(&new_code), sizeof(new_code)))
    {
        // If the new code is not in the table
        if (decode_table.find(new_code) == decode_table.end())
        {
            // Decoded is old code's value
            decoded = decode_table[old_code];
            decoded += decoded_next;
        }
        else
        {
            // Decoded is the new code's value
            decoded = decode_table[new_code];
        }
        // Write the decoded value
        out << decoded.c_str();
        // Update decoded next
        decoded_next = decoded[0];
        // Add the new code's value to the table
        if (decode_table.size() < TABLE_SIZE)
            decode_table[decode_table.size()] = decode_table[old_code] + decoded_next;
        // Update the old code
        old_code = new_code;

        // When reading a file with a specified size
        if (read_size > 0)
        {
            read_sz += sizeof(new_code);
            if (read_sz >= read_size)
                break;
        }
    }

    // Clear flags after reaching eof
    if (!in.good())
    {
        in.seekg(0, std::ios::beg);
        in.clear();
    }
    
    // Refresh the table to reuse same object
    refreshTable();
}

// Printing table for testing
// Works if TESTING is enabled
#ifdef TESTING
void Decoder::printTable(std::ostream& out) const
{
    out << "---------------------------------\n";
    out << "|\tKey\t|\tValue\t|\n";
    out << "---------------------------------\n";
    for(const auto& elem : decode_table)
        out << "\t" << elem.first << "\t\t" << elem.second << "\t\n";
}
#endif

// Refreshing the code table
void Decoder::refreshTable()
{
    decode_table.clear();
    initTable();
}

// Initializing the table
inline void Decoder::initTable()
{
    for (int i = 0; i < FIRST_INIT_SIZE; i++) 
        decode_table[i] = std::string(1, i);
}