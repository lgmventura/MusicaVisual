#ifndef FILES_H
#define FILES_H

#include <string>
#include <filesystem> // needs C++17

//namespace fs = std::filesystem; // optional, not being used


// appendExtension: appends a file extension at the end if it is not there already:
std::string appendExtension(std::string filePath, std::string extension);


#endif // FILES_H
