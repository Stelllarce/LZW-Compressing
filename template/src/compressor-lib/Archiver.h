#ifndef _ARCHIVER_H_
#define _ARCHIVER_H_

#include "Encoder.h"
#include "Decoder.h"
#include "Path.h"
#include <vector>
#include <set>
#include <filesystem>

class Archiver: public Encoder, public Decoder 
{
public:
    Archiver();

    // Compresses files into an archive
    void zip(const std::string&, std::vector<std::string>&, std::vector<std::string>&);

    // Decompresses an archive
    void unzip(const std::string&, const std::string&, const std::set<std::string>&);
    
    // Displays information about the archive
    void info(const std::string&);

    // Refreshes an archive with its new version
    void refresh(const std::string&, std::set<std::string>&);

    // Checks for damaged file in an archive
    void errorCheck(const std::string&);
private:

    // Zips a file
    void zipSingle(std::fstream& archive, const std::string& file, const std::string& relative_path);
    
    // Unzips a file
    void unzipSingle(std::ifstream&, const std::string&);

    // Unzips selected files from the archive
    void unzipSelected(std::ifstream&, const std::string&, const std::set<std::string>&);
};

#endif // _ARCHIVER_H_