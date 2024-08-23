#ifndef _HASHER_H_
#define _HASHER_H_

#include <fstream>
#include <string>
#include <vector>
#include "../md5-lib/md5.h"

class Hasher {
public:  
    static void hash(std::ifstream& in, std::ofstream& out);
};

#endif // _HASHER_H_