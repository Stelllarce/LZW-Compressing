# LZW-Compressing
### University project realising a compression algorithm with interface

## How to use
***Warning***: The project contains a bash scipt that executes on build of the cmake configuration. It may not work on Windows. Refer to the last few lines of the CMakeLists.txt file in `template/` for the script execute command and comment it out if necessary.

1. Clone the repository
2. Open the project folder (LZW-Compressing) inside a terminal 
(Linux)
3. Compile the project with the following command:
```bash
    cd template/src && g++ application/main.cpp compressor-lib/*.cpp md5-lib/md5.cpp -o archiver
    chmod +x archiver
```
4. Run the project with the following command:
```bash
    ./archiver
```
(Windows)
3. Compile the project with the following command:
```bash
    cd template\\src && g++ application\\main.cpp compressor-lib\\*.cpp md5-lib\\md5.cpp -o archiver.exe
```
4. Run the project with the following command:
```bash
    archiver.exe
```

## Used libraries
- `<set>` - used for log(n) searching
- `<unordered_map>` - used for table of codes
- `<filesystem>` - used to create directories acroos different OS
- `<sstream>` - used for unit testing and input parsing
- `<csdtdef>` - used for size_t
- `<queue>` - used for relative path finding
- `<sys/stat.h>` - used for delete and rename folder functions
<br> <h3>The rest are standart libraries like:</h3>
- `<iostream>`
- `<fstream>`
- `<string>`
- `<vector>` 
- `<cstdexcept>`

## Used tools and code
- [Most of the CMake configuration is from Atanas Semedzhiev's sdp-2023-2024 course repository](https://github.com/semerdzhiev/sdp-2023-24)
- [Article about LZW compression](https://www.scaler.com/topics/lzw-compression/)
- [Another article about LZW compression](https://www.techtarget.com/whatis/definition/LZW-compression)
- [Idea for cmake config to run the bash script](https://stackoverflow.com/questions/25687890/running-a-bash-command-via-cmake)
- [Idea to use std::filesystem to traverse directories](https://stackoverflow.com/questions/67273/how-do-you-iterate-through-every-file-directory-recursively-in-standard-c)