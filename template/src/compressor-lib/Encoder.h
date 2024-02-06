#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <unordered_map>
#include <cstring>

#define TEST

#define FIRST_INIT_SIZE 256 // Initializing the table with ASCII characters
#define TABLE_SIZE 4095 // Maximum size of the table

class Encoder {
public:
    Encoder();

    void encode(std::ifstream&, std::ofstream&);

    #ifdef TEST
    void print_table(std::ostream&) const;
    #endif

private:
    std::unordered_map<std::string, int> encode_table;
};

#endif // _ENCODER_H_