#ifndef _PATH_H_
#define _PATH_H_

#include <vector>
#include <string>
#include <queue>
#include <filesystem>
#include <sys/stat.h>

enum class OS
{
    UNIX,
    WINDOWS
};

#define CURR_SYSTEM OS::UNIX // Change this to OS::WINDOWS if you are using Windows
// #define CURR_SYSTEM OS::WINDOWS 

// Path methods
class Path
{
public:
    // Checks if the path is a path to a directory
    static bool isDirectoryPath(const std::string&);

    // Checks if the path is path to a file
    static bool isFilePath(const std::string&);

    // Fills vector with all files in a directory and its subdirectories
    static void getAllFiles(const std::string&, std::vector<std::string>&);

    // Gets the path part of a full path and file
    static std::string getPath(const std::string&);

    // Gets the file part of a full path and file
    static std::string getFile(const std::string&);

    // Outputs the relative path of second file in terms of the first
    static std::string getRelativePath(const std::string&, const std::string&);

    // Creates a directory at a specified location
    static std::string createDirectory(const std::string&, const std::string&);

    // Fills vector with all files from a single directory
    static void getAllFilesSingleDirectory(const std::string&, std::vector<std::string>&);
private:
    static OS os;
};

#endif // _PATH_H_