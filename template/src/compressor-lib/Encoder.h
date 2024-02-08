#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <cstring>

#define TESTING // Enable testing

#define FIRST_INIT_SIZE 256 // Initializing the table with ASCII characters
#define TABLE_SIZE 4095 // Maximum size of the table

class Encoder {
public:
    Encoder();

    // Encoding algorithm
    void encode(std::ifstream&, std::fstream&);

    // Print function for testing
    #ifdef TESTING
    void print_table(std::ostream&) const;
    #endif

protected:
    // Reinitializes the table
    void refresh_table();
    
    // Initializes the table
    void init_table();

    // Table for encoding
    std::unordered_map<std::string, int> encode_table;
};

#endif // _ENCODER_H_