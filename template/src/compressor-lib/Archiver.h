#ifndef _ARCHIVER_H_
#define _ARCHIVER_H_

#include "Encoder.h"
#include "Decoder.h"
#include <vector>

class Archiver: public Encoder, public Decoder 
{
public:
    Archiver();

    void zip(std::string&, const std::vector<std::string>&);
    void unzip(const std::string&, const std::string&);
    void info(const std::string&);
    void refresh(const std::string&);
    void errorCheck(const std::string&);
};

#endif // _ARCHIVER_H_