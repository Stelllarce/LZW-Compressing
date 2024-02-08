#include "Encoder.h"

Encoder::Encoder()
{
    encode_table.reserve(TABLE_SIZE);
    init_table();
}

// Encoding to a .lzw file
void Encoder::encode(std::ifstream& in, std::fstream& out)
{
    if (!in.is_open() || !out.is_open())
        throw std::runtime_error("File not open");

    std::string repeating = ""; // Repeating string
    char read; // Currently read character
    while (in.get(read))
    {
        // Concat the previous with current char
        std::string concat = repeating + read;
        // If the concat is not in the table
        if (encode_table.find(concat) == encode_table.end())
        {
            // Output code
            out.write((char*)(&encode_table[repeating]), sizeof(int));
            // Write code to the table
            if (encode_table.size() < TABLE_SIZE)
                encode_table[concat] = encode_table.size();
            // Update repeating
            repeating = read;
        }
        else
        {
            // Update repeating
            repeating = concat;
        }
    }

    // Write the last repeating string
    if (!repeating.empty())
        out.write((char*)(&encode_table[repeating]), sizeof(int));
    
    // Clear flags at the end of file
    if(in.eof())
        in.clear();
    else
        in.close(), out.close(),
        throw std::runtime_error("Error reading file");

    if(out.bad() || in.bad())
        in.close(), out.close(),
        throw std::runtime_error("File error");

    // Refresh the table to reuse same object
    refresh_table();
}

// Print function for testing purposes
#ifdef TESTING
void Encoder::print_table(std::ostream& out) const 
{
    out << "---------------------------------\n";
    out << "|\tKey\t|\tValue\t|\n";
    out << "---------------------------------\n";
    for(const auto& elem : encode_table)
        out << "\t" << elem.first << "\t\t" << elem.second << "\t\n";
    
}
#endif

// Refreshing the code table
void Encoder::refresh_table()
{
    encode_table.clear();
    init_table();
}

// Initializing the table
inline void Encoder::init_table()
{
    for (int i = 0; i < FIRST_INIT_SIZE; i++)
        encode_table[std::string(1, i)] = i;
}
