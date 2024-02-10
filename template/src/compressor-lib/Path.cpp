#include "Path.h"

OS Path::os = CURR_SYSTEM;

bool Path::isDirectoryPath(const std::string& path)
{
    return path.rfind('.') < path.rfind((os == OS::UNIX ? '/' : '\\')) || path.rfind('.') == std::string::npos;
}

bool Path::isFilePath(const std::string& path)
{
    return path.rfind('.') > path.rfind((os == OS::UNIX ? '/' : '\\')) && path.rfind('.') != std::string::npos;
}

/**
 * @brief Gets all files in a directory and its subdirectories
 * 
 * @param path - directory path
 * @param files - vector to store the file names
 * @param os - operating system
 */
void Path::getAllFiles(const std::string& path, std::vector<std::string>& files)
{
    for (const auto & file : std::filesystem::directory_iterator(path))
    {
        if (isFilePath(file.path().string()))
        {
            files.push_back(file.path().string());
        }
        else
        {
            getAllFiles(file.path().string(), files);
        }
    }    
}

/**
 * @brief Gets the path part of a full path and file
 * 
 * @param path - full path and file name
 * @param os - operating system
 * 
 * @return path part of the absolute path
*/
std::string Path::getPath(const std::string& path)
{
    std::string result = "";
    char delim = os == OS::UNIX ? '/' : '\\';

    std::size_t last = path.find_last_of(delim);
    if (last != std::string::npos)
        result = path.substr(0, last);

    return result;
}

/**
 * @brief Gets the file part of a full path and file
 * 
 * @param path - full path and file name
 * @param os - operating system
 * 
 * @return file part of the absolute path
*/
std::string Path::getFile(const std::string& path)
{
    std::string result = "";
    char delim = os == OS::UNIX ? '/' : '\\';

    std::size_t last = path.find_last_of(delim);
    if (last != std::string::npos)
        result = path.substr(last + 1);

    return result;
}

/**
 * @brief Outputs the relative path of second file in terms of the first
 * 
 * @param base - base path
 * @param path - path to get relative to base
 * @param os - operating system
 * 
 * @return relative path of the second file in terms of the given base
*/
std::string Path::getRelativePath(const std::string& base, const std::string& path)
{
    char delim = os == OS::UNIX ? '/' : '\\';
    if (base == path) 
    {
        return getFile(base);
    }

    std::queue<std::string> base_dirs;
    std::queue<std::string> path_dirs;

    std::string buff = "";
    for (char ch : base)
    {
        if (ch == delim)
        {
            base_dirs.push(buff);
            buff = "";
        }
        else buff += ch;
    }
    base_dirs.push(buff);
    
    buff = "";
    for (char ch : path)
    {
        if (ch == delim)
        {
            path_dirs.push(buff);
            buff = "";
        }
        else buff += ch;
    }
    path_dirs.push(buff);

    while (!base_dirs.empty() && !path_dirs.empty() && base_dirs.front() == path_dirs.front())
    {
        base_dirs.pop();
        path_dirs.pop();
    }

    std::string relative = "";
    while (!path_dirs.empty())
    {
        if (path_dirs.size() == 1)
        {
            relative += path_dirs.front();
            path_dirs.pop();
        }
        else
        {
            relative += path_dirs.front() + (os == OS::UNIX ? "/" : "\\");
            path_dirs.pop();
        }
    }

    return relative;
}

/**
 * @brief Creates a directory
 * 
 * @param dest - destination directory
 * @param dirs_to_create - directories to be created
 * @param os - operating system
 * 
 * @return path of the created directory
*/
std::string Path::createDirectory(const std::string& dest, const std::string& dirs_to_create)
{
    std::string path = dest + (os == OS::UNIX ? "/" : "\\") + dirs_to_create;
    std::filesystem::create_directories(path);
    return path;
}

void Path::getAllFilesSingleDirectory(const std::string& path, std::vector<std::string>& files)
{
    for (const auto & file : std::filesystem::directory_iterator(path))
    {
        if (isFilePath(file.path().string()))
        {
            files.push_back(file.path().string());
        }
    }    
}