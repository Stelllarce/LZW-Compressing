#include "Decoder.h"

// Constructor for the decoder, initializing the table with ASCII characters
Decoder::Decoder()
{
    decode_table.reserve(TABLE_SIZE);
    for (int i = 0; i < FIRST_INIT_SIZE; i++) 
        decode_table[i] = std::string(1, i);
}

// Decoding algorithm
void Decoder::decode(std::ifstream& in, std::ofstream& out)
{
    int old_code; 
    in.read((char*)(&old_code), sizeof(int));
    out << decode_table[old_code].c_str();
    std::string s = decode_table[old_code];
    std::string s2 = "";
    int new_code;
    while (in.read((char*)(&new_code), sizeof(int)))
    {
        if (decode_table.find(new_code) == decode_table.end())
        {
            s = decode_table[old_code];
            s += s2;
        }
        else
        {
            s = decode_table[new_code];
        }
        out << s.c_str();
        s2 = s[0];
        if (decode_table.size() < TABLE_SIZE)
            decode_table[decode_table.size()] = decode_table[old_code] + s2;
        old_code = new_code;
    }

    if(in.eof())
        in.clear();
    else
        throw std::runtime_error("Error reading file");

    if(out.bad() || in.bad())
        throw std::runtime_error("File error");
    
}

// Print function for testing purposes
#ifdef TEST
void Decoder::print_table(std::ostream& out) const
{
    out << "---------------------------------\n";
    out << "|\tKey\t|\tValue\t|\n";
    out << "---------------------------------\n";
    for(const auto& elem : decode_table)
        out << "\t" << elem.first << "\t\t" << elem.second << "\t\n";
}
#endif