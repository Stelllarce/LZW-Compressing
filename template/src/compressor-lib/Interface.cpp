#include "Interface.h"

inline void Interface::display()
{
    std::cout << "-----------LZW Compressor-----------\n";
    std::cout << "For available commands, type 'help'.\n"; 
    std::cout << "All paths should be absolute.\n";
}

inline void Interface::help()
{
    std::cout << "Available commands:\n";
    std::cout << "ZIP <path-to-future-archive> <file1 file2 directory1 ...> - archive a file(s) or/and a directory\n";
    std::cout << "UNZIP <path-to-archive> <path-of-extraction> <files1 file2 ...>/(can use * to extract all) - dearchive an archive\n";
    std::cout << "INFO <path-to-archive> - display a list with archived files\n";
    std::cout << "REFRESH <path-to-archive> <file1 file2 ...> - refresh an archived file with its new version\n";
    std::cout << "EC <path-to-archive> - check for damaged file in an archive\n"; 
    std::cout << "EXIT - exit the program\n";
}

void Interface::run()
{
    Archiver archiver;
    std::string line; // Stores whole line
    display();
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, line);
        std::stringstream ss(line); // Parse to stream to extract command and arguments
        std::string command;
        ss >> command; // Extract command

        if (command == "ZIP")
        {
            std::cout << "Compressing files...\n";
            std::string archive_name, file;
            ss >> archive_name;
            std::vector<std::string> files, relatpths;
            try
            {

            while (ss >> file)
            {
                // Case ewhen the input is a directory
                if (Path::isDirectoryPath(file))
                {
                    std::vector<std::string> temps;
                    Path::getAllFiles(file, temps); // Get all files in the directory and its subdirectories
                    for (const std::string& f : temps) 
                    {
                        files.push_back(f); // Add absolute paths here
                        relatpths.push_back(Path::getRelativePath(file, f)); // Add relative paths here
                    }
                }
                // Case when the input contains *
                else if (file.find('*') != std::string::npos) 
                {
                    std::vector<std::string> temps;
                    Path::getAllFilesSingleDirectory(Path::getPath(file), temps); // Get all files in the same directory
                    file.erase(file.find('*')); // Remove the *
                    for (const std::string& f : temps) // Search for files with similar name
                    {
                        if (Path::getFile(f).find(Path::getFile(file)) != std::string::npos)
                        {
                            files.push_back(f); // Add absolute paths here
                            relatpths.push_back(Path::getRelativePath(Path::getPath(file), f)); // Add relative paths here
                        }
                    }
                }
                else // If the input is a single file
                {
                    files.push_back(file); // Add absolute paths here
                    relatpths.push_back(Path::getFile(file)); // Add relative paths here
                }
            }

            archiver.zip(archive_name, files, relatpths); // Compress the files

            } // try
            catch(const std::exception& e) // Remove the archive if an exception is thrown
            {
                std::cerr << e.what() << '\n';
                if (std::filesystem::exists(archive_name))
                    std::remove(archive_name.c_str());
                continue;
            }
            std::cout << "Files compressed.\n";
        }
        else if (command == "UNZIP")
        {
            try
            {
                
            std::cout << "Extracting files...\n";
            std::string archive_name, extract_to, file;
            ss >> archive_name >> extract_to;
            std::set<std::string> files;
            while (ss >> file)
            {
                if (file == "*") // Case when user wants to extract all files
                    break;
                files.insert(file);
            }
            
            archiver.unzip(archive_name, extract_to, files); // Restore the files

            } // try
            catch(const std::exception& e) 
            {
                std::cerr << e.what() << '\n';
                continue;
            }
            std::cout << "Files extracted.\n";
        }
        else if (command == "INFO")
        {
            std::string archive_name;
            ss >> archive_name;
            try
            {
                
                archiver.info(archive_name);
            }
            catch(const std::runtime_error& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
            
        }
        else if (command == "REFRESH")
        {
            std::string archive_name, file;
            std::set<std::string> files;
            ss >> archive_name;
            while (ss >> file)
                files.insert(file);
            try
            {
                archiver.refresh(archive_name, files);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
            
            std::cout << "Archive refreshed.\n";
        }
        else if (command == "EC")
        {
            std::string archive_name;
            ss >> archive_name;
            try
            {
                archiver.errorCheck(archive_name);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                continue;
            }
        }
        else if (command == "help")
        {
            help();
        }
        else if (command == "EXIT")
        {
            std::cout << "Exiting the program...\n";
            break;
        }
        else
        {
            std::cout << "Invalid command. Type 'help' for available commands.\n";
        }
    }
}