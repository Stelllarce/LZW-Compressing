#include "catch2/catch_all.hpp"
#include "compressor-lib/Archiver.h"
#include <sstream>

const char* test_input = "../../template/test/test_files/test_input.txt";
const char* test_input2 = "../../template/test/test_files/test_input2.txt";
const char* test_input3 = "../../template/test/test_files/test_input3.txt";

const char* test_encoded = "../../template/test/test_files/test_output/test_encoded.lzw";
const char* test_encoded2 = "../../template/test/test_files/test_output/test_encoded2.lzw";
const char* test_encoded3 = "../../template/test/test_files/test_output/test_encoded3.lzw";

const char* test_decoded = "../../template/test/test_files/test_output/test_decoded.txt";
const char* test_decoded2 = "../../template/test/test_files/test_output/test_decoded2.txt";
const char* test_decoded3 = "../../template/test/test_files/test_output/test_decoded3.txt";

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
    testEncode(test_input, test_encoded, expected);
    testEncode(test_input2, test_encoded2, expected2);
    testEncode(test_input3, test_encoded3, expected3);
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
    testDecode(test_encoded, test_decoded, test_input);
    testDecode(test_encoded2, test_decoded2, test_input2);
    testDecode(test_encoded3, test_decoded3, test_input3);
}

TEST_CASE("Testing archiver")
{
    Archiver archiver;
    std::vector<std::string> files = {test_input, test_input2, test_input3};
    std::string archive_name = "../../template/test/test_files/test_output/test_archive.lzw";

    REQUIRE_NOTHROW(archiver.zip(archive_name, files));
    std::ifstream archive;
    archive.open(archive_name, std::ios::binary | std::ios::in);

    REQUIRE(archive.is_open());

    SECTION("Checking if saved number of files is correct")
    {
        int number_of_files, expected_num_files = files.size();
        archive.read((char*)(&number_of_files), sizeof(number_of_files));
        REQUIRE(number_of_files == expected_num_files);
    }

    SECTION("Checking if saved file path is correct")
    {
        int path_length;
        archive.seekg(sizeof(int), std::ios::cur); // skip number of files
        archive.read((char*)(&path_length), sizeof(path_length));

        CHECK(path_length == files[0].size());
        
        char* path = new char[path_length + 1];
        archive.read(path, path_length);
        path[path_length] = '\0';        archive.seekg(sizeof(int) * 2 + files[0].size(), std::ios::cur);

        std::string path_str(path);
        delete[] path;

        REQUIRE(path_str == files[0]);
    }

    SECTION("Checking if saved file content is correct and delimiter is correctly read")
    {
        const int delimiter = -1;
        std::vector<int> input, expected = {66, 65, 256, 257, 65, 260};
        int current;
        archive.seekg(sizeof(int) * 2, std::ios::cur); // skip number of files and file name lenght
        archive.seekg(files[0].size(), std::ios::cur); // skip file name
        while (archive.read((char*)(&current), sizeof(current)))
        {
            if (current == delimiter)
            {
                break;
            }
            input.push_back(current);
        }
        
        CHECK(input == expected);
    }
}