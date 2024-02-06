#include "Encoder.h"

// Constructor for the encoder, initializing the table with ASCII characters
Encoder::Encoder()
{
    encode_table.reserve(TABLE_SIZE);
    for (int i = 0; i < FIRST_INIT_SIZE; i++)
        encode_table[std::string(1, i)] = i;
}

// Encoding algorithm
void Encoder::encode(std::ifstream& in, std::ofstream& out)
{
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
        throw std::runtime_error("Error reading file");

    if(out.bad() || in.bad())
        throw std::runtime_error("File error");
}

// Print function for testing purposes
#ifdef TEST
void Encoder::print_table(std::ostream& out) const 
{
    out << "---------------------------------\n";
    out << "|\tKey\t|\tValue\t|\n";
    out << "---------------------------------\n";
    for(const auto& elem : encode_table)
        out << "\t" << elem.first << "\t\t" << elem.second << "\t\n";
    
}
#endif