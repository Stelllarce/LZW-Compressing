#include "Path.h"

std::string Path::getRelativePath(const std::string& base, const std::string& path, OS os)
{
    char delim = os == OS::UNIX ? '/' : '\\';
    if (base == path) 
    {
        std::size_t last = path.find_last_of(delim);
        if (last != std::string::npos)
            return path.substr(last + 1);
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