#include "Interface.h"

inline void display()
{
    std::cout << "-----------LZW Compressor-----------\n";
    std::cout << "For available commands, type 'help'.\n"; 
}

inline void help()
{
    std::cout << "Available commands:\n";
    std::cout << "ZIP <path-of-created-archive> <file1 file2 ...> - compress a file\n";
    std::cout << "UNZIP <path-of-archive> <path-of-extraction> <files1 file2 ...>/(can use # to extract all) - decompress an archive\n";
    std::cout << "INFO <name-of-archive> - display a list with compressed files\n";
    std::cout << "REFRESH <name-of-archive> - refresh an archive with its new version\n";
    std::cout << "EC <name-of-archive> - check for damaged file in an archive\n"; 
    std::cout << "EXIT - exit the program\n";
}

void executeZIP(std::stringstream& ss, Archiver& archiver)
{
    std::cout << "Compressing files...\n";
    std::string archive_name, file;
    ss >> archive_name;
    std::vector<std::string> files;
    while (ss >> file)
        files.push_back(file);
    
    archiver.zip(archive_name, files);
    std::cout << "Files compressed.\n";
}

void executeUNZIP(std::stringstream& ss, Archiver& archiver)
{
}

void Interface::run()
{
    Archiver archiver;
    std::string line;
    display();
    while (true)
    {
        std::cout << "Enter a command: ";
        std::getline(std::cin, line);
        std::stringstream ss(line);
        std::string command;
        ss >> command;
        if (command == "ZIP")
        {
            executeZIP(ss, archiver);
        }
        else if (command == "UNZIP")
        {
            // decompress an archive
        }
        else if (command == "INFO")
        {
            // display a list with compressed files
        }
        else if (command == "REFRESH")
        {
            // refresh an archive with its new version
        }
        else if (command == "EC")
        {
            // check for damaged file in an archive
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