#include "catch2/catch_all.hpp"
#include "compressor-lib/Archiver.h"
#include "compressor-lib/Path.h"
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

// ------------Path test cases for UNIX----------------

TEST_CASE("Second path is longer than the first")
{
    std::string base = "/home/user/documents.txt";
    std::string path = "/home/user/etc/file.txt";
    std::string expected = "etc/file.txt";
    CHECK(Path::getRelativePath(base, path) == expected);
}

TEST_CASE("First path is longer than the second")
{
    std::string base = "/home/user/etc/file.txt";
    std::string path = "/home/user/documents.txt";
    std::string expected = "documents.txt";
    CHECK(Path::getRelativePath(base, path) == expected);
}

TEST_CASE("Paths are equal")
{
    std::string base = "/home/user/etc/file.txt";
    std::string path = "/home/user/etc/file.txt";
    std::string expected = "file.txt";
    CHECK(Path::getRelativePath(base, path) == expected);
}

TEST_CASE("Path is a directory")
{
    std::string path = "/home/user/etc/";
    CHECK(Path::isDirectoryPath(path));
    CHECK_FALSE(Path::isFilePath(path));
}

TEST_CASE("Path is a file")
{
    std::string path = "/home/user/etc/file.txt";
    CHECK(Path::isFilePath(path));
    CHECK_FALSE(Path::isDirectoryPath(path));
}

TEST_CASE("getAllFiles returns all files in a directory and its subdirectories")
{
    std::string path = "../../template/test/test_files/rec_test";
    std::vector<std::string> files;
    Path::getAllFiles(path, files);
    std::vector<std::string> expected = {
        "../../template/test/test_files/rec_test/dummy.txt",
        "../../template/test/test_files/rec_test/rec3/dummy3.txt",
        "../../template/test/test_files/rec_test/rec2/dummy2.txt"
    };
    CHECK(files == expected);
}

TEST_CASE("getPath returns the path part of a full path to file")
{
    std::string path = "/home/user/etc/file.txt";
    std::string expected = "/home/user/etc";
    CHECK(Path::getPath(path) == expected);
}

TEST_CASE("getFile returns the file part of a full path to file")
{
    std::string path = "/home/user/etc/file.txt";
    std::string expected = "file.txt";
    CHECK(Path::getFile(path) == expected);
}

TEST_CASE("Directory is created correctly at given location")
{
    std::string path = "../../template/test/test_files/test_output";
    std::string dir = "test_dir";
    std::string expected = "../../template/test/test_files/test_output/test_dir";
    CHECK(Path::createDirectory(path, dir) == expected);
    CHECK(std::filesystem::exists(expected));
    std::filesystem::remove(expected);
}

// ------------Path test cases for WINDOWS----------------

// TEST_CASE("Second path is longer than the first (Windows)")
// {
//     std::string base = "C:\\Users\\user\\documents.txt";
//     std::string path = "C:\\Users\\user\\etc\\file.txt";
//     std::string expected = "etc\\file.txt";
//     CHECK(Path::getRelativePath(base, path) == expected);
// }

// TEST_CASE("First path is longer than the second (Windows)")
// {
//     std::string base = "C:\\Users\\user\\etc\\file.txt";
//     std::string path = "C:\\Users\\user\\documents.txt";
//     std::string expected = "documents.txt";
//     CHECK(Path::getRelativePath(base, path) == expected);
// }

// TEST_CASE("Paths are equal (Windows)")
// {
//     std::string base = "C:\\Users\\user\\etc\\file.txt";
//     std::string path = "C:\\Users\\user\\etc\\file.txt";
//     std::string expected = "file.txt";
//     CHECK(Path::getRelativePath(base, path) == expected);
// }

// TEST_CASE("Path is a directory (Windows)")
// {
//     std::string path = "C:\\Users\\user\\etc\\";
//     CHECK(Path::isDirectoryPath(path));
// }

// TEST_CASE("Path is a file (Windows)")
// {
//     std::string path = "C:\\Users\\user\\etc\\file.txt";
//     CHECK(Path::isFilePath(path));
// }

// ------------Encoder and Decoder test cases----------------

TEST_CASE("Encoder table is initialized correctly")
{
    Encoder enc;
    std::stringstream output;
    enc.printTable(output);
    REQUIRE_FALSE(output.str() == "");
}

TEST_CASE("Decoder table is initialized correctly")
{
    Decoder dec;
    std::stringstream output;
    dec.printTable(output);
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

inline void testEncode(Encoder& enc, const char* refrence_file_path, const char* compressed_file_path, std::vector<int>& expected) 
{
    std::ifstream in;
    in.open(refrence_file_path, std::ios::binary | std::ios::in);
    std::fstream out;
    out.open(compressed_file_path, std::ios::binary | std::ios::out | std::ios::trunc);

    REQUIRE(in.is_open());
    REQUIRE(out.is_open());

    REQUIRE_NOTHROW(enc.encode(in, out));

    REQUIRE(in.good());
    REQUIRE(out.good());

    in.close();
    out.close();

    testEncodeResults(compressed_file_path, expected);
}

TEST_CASE("Encoding works correctly with different files")
{
    Encoder enc;
    std::vector<int> expected = {66, 65, 256, 257, 65, 260};
    std::vector<int> expected2 = {87, 89, 83, 42, 256, 71, 256, 258, 262, 262, 71};
    std::vector<int> expected3 = {119, 97, 256, 87, 65, 259, 87, 32, 65, 42, 65, 68, 266, 268, 259, 267, 42, 94, 260, 267, 273, 83, 68, 39, 268, 83, 266, 115, 261, 65};
    testEncode(enc, test_input, test_encoded, expected);
    testEncode(enc, test_input2, test_encoded2, expected2);
    testEncode(enc, test_input3, test_encoded3, expected3);
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

inline void testDecode(Decoder& dec, const char* compressed_file_path, const char* result_file_path, const char* refrence_file_path) 
{
    std::ifstream in;
    in.open(compressed_file_path, std::ios::binary | std::ios::in);
    std::ofstream out;
    out.open(result_file_path, std::ios::out | std::ios::trunc);

    REQUIRE(in.is_open());
    REQUIRE(out.is_open());

    REQUIRE_NOTHROW(dec.decode(in, out));

    REQUIRE(in.good());
    REQUIRE(out.good());

    in.close();
    out.close();

    testDecodeResults(result_file_path, refrence_file_path);
}

TEST_CASE("Decoding works correctly with different files")
{
    Decoder dec;
    testDecode(dec, test_encoded, test_decoded, test_input);
    testDecode(dec, test_encoded2, test_decoded2, test_input2);
    testDecode(dec, test_encoded3, test_decoded3, test_input3);
}

// ------------Archiver test cases----------------

TEST_CASE("Archiver zips particular files")
{
    Archiver archiver;
    std::vector<std::string> files = {test_input, test_input2, test_input3};
    std::vector<std::string> relatpths = {Path::getFile(test_input), Path::getFile(test_input2), Path::getFile(test_input3)};
    std::string archive_name = "../../template/test/test_files/test_output/test_archive.lzw";

    REQUIRE_NOTHROW(archiver.zip(archive_name, files, relatpths));
    
    std::ifstream archive;
    archive.open(archive_name, std::ios::binary | std::ios::in);

    REQUIRE(archive.is_open());

    SECTION("Saved number of files is correct")
    {
        int number_of_files, expected_num_files = files.size();
        archive.read((char*)(&number_of_files), sizeof(number_of_files));
        CHECK(number_of_files == expected_num_files);
    }

    SECTION("Saved file path is correct")
    {
        int path_length;
        archive.seekg(sizeof(int), std::ios::cur); // skip number of files
        archive.read((char*)(&path_length), sizeof(path_length));

        CHECK(path_length == relatpths[0].size());
        
        char* path = new char[path_length + 1];
        archive.read(path, path_length);
        path[path_length] = '\0';       
        archive.seekg(sizeof(int) * 2 + relatpths[0].size(), std::ios::cur);

        std::string path_str(path);
        delete[] path;

        CHECK(path_str == relatpths[0]);
    }

    SECTION("Saved file content is correct")
    {
        int read_bytes = 0;
        std::vector<int> input, expected = {66, 65, 256, 257, 65, 260};
        int current, file_size;
        archive.seekg(sizeof(int) * 2, std::ios::cur); // skip number of files and file name lenght
        archive.seekg(relatpths[0].size(), std::ios::cur); // skip file name

        archive.read((char*)(&file_size), sizeof(file_size));

        CHECK(file_size == expected.size() * sizeof(int));

        while (archive.read((char*)(&current), sizeof(current)))
        {
            if (read_bytes >= file_size)
                break;
            input.push_back(current);
            read_bytes += sizeof(current);
        }
        
        CHECK(input == expected);
    }
}

TEST_CASE("Archiver zips a directory")
{
    Archiver archiver;
    std::string archive_name = "../../template/test/test_files/test_output/test_archive_dir.lzw";
    std::string path = "../../template/test/test_files/rec_test";
    std::vector<std::string> temps, files, relatpths;
    Path::getAllFiles(path, temps);

    // Technically also testing this algorithm from Interface.cpp
    for (const std::string& f : temps)
    {
        files.push_back(f);
        relatpths.push_back(Path::getRelativePath(path, f));
    }

    REQUIRE_NOTHROW(archiver.zip(archive_name, files, relatpths));

    std::ifstream archive;
    archive.open(archive_name, std::ios::binary | std::ios::in);

    REQUIRE(archive.is_open());

    SECTION("Saved number of files is correct")
    {
        int number_of_files, expected_num_files = files.size();
        archive.read((char*)(&number_of_files), sizeof(number_of_files));
        CHECK(number_of_files == expected_num_files);
    }

    archive.seekg(sizeof(int), std::ios::cur); // skip number of files
    SECTION("Saved file path is correct")
    {
        int path_length;
        archive.read((char*)(&path_length), sizeof(path_length));

        CHECK(path_length == relatpths[0].size());
        
        char* path = new char[path_length + 1];
        archive.read(path, path_length);
        path[path_length] = '\0';       
        archive.seekg(sizeof(int) * 2 + relatpths[0].size(), std::ios::cur);

        std::string path_str(path);
        delete[] path;

        CHECK(path_str == relatpths[0]);
    }

    archive.seekg(sizeof(int), std::ios::cur); // skip number of files and file name lenght
    archive.seekg(relatpths[0].size(), std::ios::cur); // skip file name
    int file_size;
    SECTION("Saved file content is correct")
    {
        int read_bytes = 0;
        std::vector<int> input, expected = {66, 65, 256, 257, 65, 260};
        int current;

        archive.read((char*)(&file_size), sizeof(file_size));

        CHECK(file_size == expected.size() * sizeof(int));

        while (archive.read((char*)(&current), sizeof(current)))
        {
            if (read_bytes >= file_size)
                break;
            input.push_back(current);
            read_bytes += sizeof(current);
        }
        
        CHECK(input == expected);
    }
}

void testUnzipResults(const char* result_path, const char* refrence_file_path) 
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

TEST_CASE("Archiver unzips all files")
{
    Archiver archiver;
    std::string archive_name = "../../template/test/test_files/test_output/test_archive.lzw";
    std::string extract_to = "../../template/test/test_files/test_output";
    const char* expected = "../../template/test/test_files/test_output/test_input.txt";
    const char* expected2 = "../../template/test/test_files/test_output/test_input2.txt";
    const char* expected3 = "../../template/test/test_files/test_output/test_input3.txt";
    std::set<std::string> files_to_extract;
    REQUIRE_NOTHROW(archiver.unzip(archive_name, extract_to, files_to_extract));

    testUnzipResults(test_input, expected);
    testUnzipResults(test_input2, expected2);
    testUnzipResults(test_input3, expected3);

    std::filesystem::remove(expected);
    std::filesystem::remove(expected2);
    std::filesystem::remove(expected3);
} 

TEST_CASE("Archiver unzips specific files")
{
    Archiver archiver;
    std::string archive_name = "../../template/test/test_files/test_output/test_archive.lzw";
    std::string extract_to = "../../template/test/test_files/test_output";
    const char* expected = "../../template/test/test_files/test_output/test_input.txt";
    const char* expected3 = "../../template/test/test_files/test_output/test_input3.txt";
    std::set<std::string> files_to_extract = {Path::getFile(test_input), Path::getFile(test_input3)};
    REQUIRE_NOTHROW(archiver.unzip(archive_name, extract_to, files_to_extract));

    testUnzipResults(test_input, expected);
    testUnzipResults(test_input3, expected3);
}

TEST_CASE("Testing archiver refresh/Nothing has changed")
{
    Archiver archiver;
    std::string archive_name = "../../template/test/test_files/test_output/test_archive.lzw";
    std::set<std::string> files = {Path::getFile(test_input), Path::getFile(test_input2), Path::getFile(test_input3)};

    REQUIRE_NOTHROW(archiver.refresh(archive_name, files));
}