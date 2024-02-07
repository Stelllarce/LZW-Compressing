#include "Encoder.h"

// Constructor for the encoder, initializing the table with ASCII characters
Encoder::Encoder()
{
    encode_table.reserve(TABLE_SIZE);
    init_table();
}

// Encoding algorithm
void Encoder::encode(std::ifstream& in, std::fstream& out)
{
    if (!in.is_open() || !out.is_open())
        throw std::runtime_error("File not open");

    std::string repeating = "";
    char read;
    while (in.get(read))
    {
        std::string concat = repeating + read;
        if (encode_table.find(concat) != encode_table.end())
        {
            repeating = concat;
        }
        else
        {
            out.write((char*)(&encode_table[repeating]), sizeof(int));
            if (encode_table.size() < TABLE_SIZE)
                encode_table[concat] = encode_table.size();
            repeating = read;
        }
    }
        
        
    if (!repeating.empty())
        out.write((char*)(&encode_table[repeating]), sizeof(int));
    
    if(in.eof())
        in.clear();
    else
        in.close(), out.close(),
        throw std::runtime_error("Error reading file");

    if(out.bad() || in.bad())
        in.close(), out.close(),
        throw std::runtime_error("File error");

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

void Encoder::refresh_table()
{
    encode_table.clear();
    init_table();
}

inline void Encoder::init_table()
{
    for (int i = 0; i < FIRST_INIT_SIZE; i++)
        encode_table[std::string(1, i)] = i;
}
