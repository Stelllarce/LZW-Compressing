#ifndef _PATH_H_
#define _PATH_H_

#include <string>
#include <queue>
#include <filesystem>

enum class OS
{
    UNIX,
    WINDOWS
};

class Path
{
public:
    static std::string getRelativePath(const std::string&, const std::string&, OS);
    static std::string getPath(const std::string&, OS);
    static std::string createDirectory(const std::string&, const std::string&, OS);
};

#endif // _PATH_H_