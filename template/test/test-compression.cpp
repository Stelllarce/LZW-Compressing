#include "catch2/catch_all.hpp"
#include "compressor-lib/Encoder.h"
#include "compressor-lib/Decoder.h"
#include <sstream>

TEST_CASE("Testing if encoder table is initialized correctly")
{
    Encoder enc;
    std::stringstream output;
    enc.print_table(output);
    REQUIRE_FALSE(output.str() == "");
}

TEST_CASE("Testing if decoder table is initialized correctly")
{
    Decoder dec;
    std::stringstream output;
    dec.print_table(output);
    REQUIRE_FALSE(output.str() == "");
}

inline void testEncodeResults(const char* result_path, std::vector<int>& expected) 
{
    std::ifstream result;
    result.open(result_path, std::ios::binary | std::ios::in);

    REQUIRE(result.is_open());

    std::vector<int> result_data;
    int value;
    while (result.read((char*)(&value), sizeof(int))) 
    {
        result_data.push_back(value);
    }

    result.close();

    CHECK(result_data == expected);

}

inline void testEncode(const char* refrence_file_path, const char* compressed_file_path, std::vector<int>& expected) 
{
    std::ifstream in;
    in.open(refrence_file_path, std::ios::binary | std::ios::in);
    std::ofstream out;
    out.open(compressed_file_path, std::ios::binary | std::ios::out | std::ios::trunc);

    REQUIRE(in.is_open());
    REQUIRE(out.is_open());

    Encoder enc;
    REQUIRE_NOTHROW(enc.encode(in, out));

    REQUIRE(in.good());
    REQUIRE(out.good());

    in.close();
    out.close();

    testEncodeResults(compressed_file_path, expected);
}

TEST_CASE("Testing if encoding process is working correctly")
{
    std::vector<int> expected = {66, 65, 256, 257, 65, 260};
    std::vector<int> expected2 = {87, 89, 83, 42, 256, 71, 256, 258, 262, 262, 71};
    std::vector<int> expected3 = {119, 97, 256, 87, 65, 259, 87, 32, 65, 42, 65, 68, 266, 268, 259, 267, 42, 94, 260, 267, 273, 83, 68, 39, 268, 83, 266, 115, 261, 65};
    testEncode("../../template/test/test_files/test_input.txt", "../../template/test/test_files/test_output/test_encoded.lzw", expected);
    testEncode("../../template/test/test_files/test_input2.txt", "../../template/test/test_files/test_output/test_encoded2.lzw", expected2);
    testEncode("../../template/test/test_files/test_input3.txt", "../../template/test/test_files/test_output/test_encoded3.lzw", expected3);
}

inline void testDecodeResults(const char* result_path, const char* refrence_file_path) 
{
    std::ifstream result;
    result.open(result_path, std::ios::binary | std::ios::in);
    std::ifstream refrence;
    refrence.open(refrence_file_path, std::ios::binary | std::ios::in);

    REQUIRE(result.is_open());
    REQUIRE(refrence.is_open());

    std::stringstream refrence_data;
    refrence_data << refrence.rdbuf();
    refrence.close();

    std::stringstream result_data;
    result_data << result.rdbuf();
    result.close();

    CHECK(refrence_data.str() == result_data.str());
}

inline void testDecode(const char* compressed_file_path, const char* result_file_path, const char* refrence_file_path) 
{
    std::ifstream in;
    in.open(compressed_file_path, std::ios::binary | std::ios::in);
    std::ofstream out;
    out.open(result_file_path, std::ios::out | std::ios::trunc);

    REQUIRE(in.is_open());
    REQUIRE(out.is_open());

    Decoder dec;
    REQUIRE_NOTHROW(dec.decode(in, out));

    REQUIRE(in.good());
    REQUIRE(out.good());

    in.close();
    out.close();

    testDecodeResults(result_file_path, refrence_file_path);
}

TEST_CASE("Testing if decoding process works correctly")
{
    testDecode("../../template/test/test_files/test_output/test_encoded.lzw", "../../template/test/test_files/test_output/test_decoded.txt", "../../template/test/test_files/test_input.txt");
    testDecode("../../template/test/test_files/test_output/test_encoded2.lzw", "../../template/test/test_files/test_output/test_decoded2.txt", "../../template/test/test_files/test_input2.txt");
    testDecode("../../template/test/test_files/test_output/test_encoded3.lzw", "../../template/test/test_files/test_output/test_decoded3.txt", "../../template/test/test_files/test_input3.txt");
}