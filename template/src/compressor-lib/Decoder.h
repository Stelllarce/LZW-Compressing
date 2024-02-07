#ifndef _DECODER_H_
#define _DECODER_H_
////////////////
// Decoder class for lzw compressed files
////////////////

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <cstring>

#define TESTING
#define DELIMITER -1

#define FIRST_INIT_SIZE 256 // Initializing the table with ASCII characters
#define TABLE_SIZE 4095 // Maximum size of the table

class Decoder {
public:
    Decoder();

    void decode(std::ifstream&, std::ofstream&);

    #ifdef TESTING
    void print_table(std::ostream&) const;
    #endif

protected:
    void refresh_table();
    void init_table();
    std::unordered_map<int, std::string> decode_table;
};

#endif // _DECODER_H_