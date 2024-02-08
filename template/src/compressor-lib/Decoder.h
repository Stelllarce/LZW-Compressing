#ifndef _DECODER_H_
#define _DECODER_H_

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <cstring>

#define TESTING // enable testing

#define FIRST_INIT_SIZE 256 // Initializing the table with ASCII characters
#define TABLE_SIZE 4095 // Maximum size of the table

class Decoder {
public:
    Decoder();

    // Decoding algorithm
    void decode(std::ifstream&, std::ofstream&, int = 0);

    // Print function for testing
    #ifdef TESTING
    void print_table(std::ostream&) const;
    #endif

protected:
    // Reeinitializes the table
    void refresh_table();

    // Initializes the table
    void init_table();

    // Table for decoding
    std::unordered_map<int, std::string> decode_table;
};

#endif // _DECODER_H_