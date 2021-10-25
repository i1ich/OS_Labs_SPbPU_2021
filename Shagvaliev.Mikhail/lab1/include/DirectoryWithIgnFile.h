#pragma once

class DirectoryWithIgnFile {
public:
    std::string directory;
    std::string ignFilename;

    DirectoryWithIgnFile(std::string directory_, std::string ignFilename_):
            directory(std::move(directory_)), ignFilename(std::move(ignFilename_)) {}
};
