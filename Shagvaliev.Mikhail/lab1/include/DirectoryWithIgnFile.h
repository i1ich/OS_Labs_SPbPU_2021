#pragma once

class DirectoryWithIgnFile {
public:
    std::string directory;
    std::string ignFilename;

    DirectoryWithIgnFile(const std::string& directory_, const std::string& ignFilename_): directory(directory_), ignFilename(ignFilename_) {}
};
