#include "files.h"

std::string appendExtension(std::string filePath, std::string extension)
/*
 *  Appends an extension onto a file path if it is not already there.
 * */
{
    extension = "." + extension;
    std::string currentExt = std::filesystem::path(filePath).extension();
    if (currentExt != extension)
    {
        filePath = filePath + extension;
    }
    return filePath;
}
