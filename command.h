#include <iostream>
#include <fstream>
#include <cstring>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include<vector>

using namespace std;

// Base class for commands
class Command
{
public:
    virtual void execute() = 0;
    virtual void showHelpMessage() = 0;
    virtual ~Command() = default;
    char *printCurrentDirectory()
    {
        char *currentPath = nullptr;
        if ((currentPath = getcwd(nullptr, 0)) != nullptr)
        {
            cout << "Current directory: " << currentPath << endl;
            return currentPath;
            free(currentPath);
        }
        else
        {
            perror("getcwd");
        }
        return NULL;
    }
};

class CpCommand : public Command {
private:
    string sourcePath;
    string destinationPath;
    bool recursive;
    bool interactive;
    bool createBackup;

public:
    CpCommand(string source, string destination, bool rec, bool i,bool c) : sourcePath(source), destinationPath(destination), recursive(rec), interactive(i), createBackup(c) {}

    void execute() override {
        if (recursive) {
            copyDirectoryRecursive(sourcePath, destinationPath);
        } else {
            copyFile(sourcePath, destinationPath);
        }
    }

    string generateBackupFileName(const string &filePath)
    {
        size_t lastSlash = filePath.find_last_of('/');
        string fileName = (lastSlash != string::npos) ? filePath.substr(lastSlash + 1) : filePath;
        string newFilePath = "C:/Users/shazr/Desktop/OPD_Project/Backup/backupCopy.txt";

        std::ofstream newFile(newFilePath);
        if (!newFile.is_open())
        {
            std::cerr << "Error creating new file: " << newFilePath << std::endl;
        }

        newFile << "Hello, creating a backup file." << std::endl;
        newFile << "File is copies." + filePath << std::endl;

        newFile.close();

        std::cout << "New file created: " << newFilePath << std::endl;
        return newFilePath;
    }

    void showHelpMessage() override {
        cout << "Usage: cp [options] source destination" << endl;
        cout << "Options:" << endl;
        cout << "  -h       Show help message" << endl;
        cout << "  -r       Recursively copy files and directories" << endl;
        cout << "  -i       interactive mode" << endl;
        cout << "  -b       Create backups before copying " << endl;
    }

private:
    void copyFile(const string &source, const string &destination) {

        if(createBackup){
            string ff= generateBackupFileName(destination);
        }
        if (interactive && fileExists(destination)) {
            if (!promptForConfirmation(destination)) {
                cout << "Copy operation canceled." << endl;
                return;
            }
        }
        ifstream sourceFile(source, ios::binary);
        ofstream destFile(destination, ios::binary);

        destFile << sourceFile.rdbuf();

        sourceFile.close();
        destFile.close();

        cout << "Copied " << source << " to " << destination << endl;
    }
    bool fileExists(const string &filePath) {
        struct stat st;
        return stat(filePath.c_str(), &st) == 0;
    }

    bool promptForConfirmation(const string &filePath) {
        cout << "File " << filePath << " already exists. Overwrite? (y/n): ";
        char response;
        cin >> response;
        return (response == 'y' || response == 'Y');
    }

    void copyDirectoryRecursive(const string &source, const string &destination) {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(source.c_str())) != nullptr) {
            // Check if the destination directory already exists
            struct stat st;
            if (stat(destination.c_str(), &st) != 0) {
                // If it doesn't exist, create it
                if (mkdir(destination.c_str()) == -1) {
                    perror("cp");
                    return;
                }
            }

            while ((entry = readdir(dir)) != nullptr) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    string sourceFilePath = source + "/" + entry->d_name;
                    string destinationFilePath = destination + "/" + entry->d_name;

                    if (entry->d_type == DT_DIR) {
                        copyDirectoryRecursive(sourceFilePath, destinationFilePath);
                    } else {
                        copyFile(sourceFilePath, destinationFilePath);
                    }
                }
            }
            closedir(dir);
        } else {
            perror("cp");
        }

        cout << "Recursively copied " << source << " to " << destination << endl;
    }
};




class MvCommand : public Command
{
private:
    string sourcePath;
    string destinationPath;
    bool recursive;
    bool interactive;
    bool createBackup;

    string generateBackupFileName(const string &filePath)
    {
        size_t lastSlash = filePath.find_last_of('/');
        string fileName = (lastSlash != string::npos) ? filePath.substr(lastSlash + 1) : filePath;
        string newFilePath = "C:/Users/shazr/Desktop/OPD_Project/Backup/backup1.txt";

        std::ofstream newFile("C:/Users/shazr/Desktop/OPD_Project/Backup/backup1.txt");
        if (!newFile.is_open())
        {
            std::cerr << "Error creating new file: " << newFilePath << std::endl;
        }

        newFile << "Hello, this is the content of the new file!" << std::endl;
        newFile << "File is moved." << std::endl;

        newFile.close();

        std::cout << "New file created: " << newFilePath << std::endl;
        return newFilePath;
    }

public:
    MvCommand(string source, string destination, bool r, bool i, bool b) : sourcePath(source), destinationPath(destination), recursive(r), interactive(i), createBackup(b) {}

    void execute() override
    {

        glob_t globResult;
        int globStatus = glob(sourcePath.c_str(), GLOB_NOCHECK | GLOB_BRACE, nullptr, &globResult);

        if (globStatus == 0)
        {
            for (size_t i = 0; i < globResult.gl_pathc; ++i)
            {
                const char *expandedSource = globResult.gl_pathv[i];
                if (recursive && isDirectory(expandedSource))
                {
                    moveDirectoryRecursive(expandedSource, destinationPath);
                }
                else
                {
                    moveFile(expandedSource, destinationPath);
                }
            }
        }
        else if (globStatus == GLOB_NOMATCH)
        {
            std::cerr << "No match found for: " << sourcePath << std::endl;
        }
        else
        {
            perror("glob");
        }

        globfree(&globResult);
    }

    void showHelpMessage() override
    {
        cout << "Usage: mv [options] source destination" << endl;
        cout << "Options:" << endl;
        cout << "  -h       Show help message" << endl;
        cout << "  -r       Recursively move files and directories" << endl;
        cout << "  -i       Interactive mode (prompt before overwrite)" << endl;
    }

private:
    bool isDirectory(const std::string &path)
    {
        struct stat st;
        if (stat(path.c_str(), &st) != 0)
        {
            return false;
        }
        return S_ISDIR(st.st_mode);
    }

    void moveFile(const string &source, const string &destination)
    {
        cout << destination.c_str();
        if (interactive && fileExists(destination))
        {
            if (!promptForConfirmation(destination))
            {
                cout << "Move operation canceled." << endl;
                return;
            }
        }

        if (createBackup)
        {
            string backupFileName = generateBackupFileName(destination);
        }

        if (rename(source.c_str(), destination.c_str()) == 0)
        {
            cout << "Moved " << source << " to " << destination << endl;
        }
        else
        {
            perror("mv");
        }
    }

    bool fileExists(const string &filePath)
    {
        struct stat st;
        return stat(filePath.c_str(), &st) == 0;
    }

    bool promptForConfirmation(const string &filePath)
    {
        cout << "File " << filePath << " already exists. Overwrite? (y/n): ";
        string response;
        getline(cin, response);
        return (response == "y" || response == "Y");
    }

    void moveDirectoryRecursive(const string &source, const string &destination)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(source.c_str())) != nullptr)
        {
            // Check if the destination directory already exists
            struct stat st;
            if (stat(destination.c_str(), &st) != 0)
            {
                // If it doesn't exist, create it
                if (mkdir(destination.c_str()) == -1)
                {
                    perror("mv");
                    return;
                }
            }

            while ((entry = readdir(dir)) != nullptr)
            {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    string sourceFilePath = source + "/" + entry->d_name;
                    string destinationFilePath = destination + "/" + entry->d_name;

                    if (entry->d_type == DT_DIR)
                    {
                        moveDirectoryRecursive(sourceFilePath, destinationFilePath);
                    }
                    else
                    {
                        moveFile(sourceFilePath, destinationFilePath);
                    }
                }
            }
            closedir(dir);

            if (rmdir(source.c_str()) != 0)
            {
                perror("mv");
            }
        }
        else
        {
            perror("mv");
        }
    }
};

class RmCommand : public Command
{
private:
    string target;
    bool recursive;
    bool interactive;
    bool backup;

    void removeFile(const string &filePath)
    {
        if (remove(filePath.c_str()) != 0)
        {
            cerr << "Error removing file " << filePath << endl;
        }
    }

    string generateBackupFileName(const string &filePath)
    {
        size_t lastSlash = filePath.find_last_of('/');
        string fileName = (lastSlash != string::npos) ? filePath.substr(lastSlash + 1) : filePath;
        string newFilePath = "C:/Users/shazr/Desktop/OPD_Project/Backup/backupRemove.txt";

        std::ofstream newFile(newFilePath);
        if (!newFile.is_open())
        {
            std::cerr << "Error creating new file: " << newFilePath << std::endl;
        }

        newFile << "Hello files are removed!" << std::endl;
        newFile << "File is removed." << std::endl;

        newFile.close();

        std::cout << "New file created: " << newFilePath << std::endl;
        return newFilePath;
    }

    void removeFilesInDirectory(const string &directoryPath)
    {
        _finddata_t fileInfo;
        intptr_t findHandle = _findfirst((directoryPath + "\\*").c_str(), &fileInfo);
        if (findHandle != -1)
        {
            do
            {
                string filePath = directoryPath + "\\" + fileInfo.name;
                if (fileInfo.attrib & _A_SUBDIR)
                {
                    // Skip directories
                }
                else
                {
                    removeFile(filePath);
                }
            } while (_findnext(findHandle, &fileInfo) == 0);

            _findclose(findHandle);
        }
    }

    void removeRecursive(const string &path)
    {

        removeFilesInDirectory(path);

        // Remove the directory itself
        if (_rmdir(path.c_str()) != 0)
        {
            cerr << "Error removing directory " << path << endl;
        }
    }

public:
    RmCommand(string tgt, bool rec, bool i, bool b) : target(tgt), recursive(rec), interactive(i), backup(b) {}

    void execute() override
    {
        if (interactive)
        {
            std::cout << "Are you sure you want to remove '" << target << "'? (y/n): ";
            char response;
            std::cin >> response;

            if (response != 'y' && response != 'Y')
            {
                std::cout << "Removal canceled." << std::endl;
                return;
            }
        }

        if (backup)
        {
            string f = generateBackupFileName(target);
        }

        if (recursive)
        {
            removeRecursive(target);
            cout << "Removed: " << target << " (recursively)" << endl;
        }
        else
        {
            glob_t globResult;
            int globStatus = glob(target.c_str(), GLOB_NOCHECK, nullptr, &globResult);

            if (globStatus == 0)
            {
                for (size_t i = 0; i < globResult.gl_pathc; ++i)
                {
                    const char *fileToRemove = globResult.gl_pathv[i];

                    if (remove(fileToRemove) == 0)
                    {
                        std::cout << "Removed: " << fileToRemove << std::endl;
                    }
                    else
                    {
                        perror("rm");
                    }
                }
            }
            else if (globStatus == GLOB_NOMATCH)
            {
                std::cerr << "No match found for: " << target << std::endl;
            }
            else
            {
                perror("glob");
            }

            globfree(&globResult);
        }
    }

    void showHelpMessage() override
    {
        cout << "Usage: rm [options] source destination" << endl;
        cout << "Options:" << endl;
        cout << "  -h       Show help message" << endl;
        cout << "  -r       Recursively remove files and directories" << endl;
    }
};

class LsCommand : public Command
{
private:
    string path;
    bool recursive;
    bool longFormat;

public:
    LsCommand(string p, bool r, bool l) : path(p), recursive(r), longFormat(l) {}

    void execute() override
    {
        if (recursive)
        {
            listFilesRecursive(path);
        }
        else
        {
            if (longFormat)
            {
                listFilesLongFormat(path);
            }
            else
            {
                listFiles(path);
            }
        }
    }

    void showHelpMessage() override
    {
        cout << "Usage: ls [options] list all files" << endl;
        cout << "Options:" << endl;
        cout << "  -h       Show help message" << endl;
        cout << "  -r       Recursively show files and directories" << endl;
    }

private:
    void listFiles(string dirPath)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(dirPath.c_str())) != nullptr)
        {
            while ((entry = readdir(dir)) != nullptr)
            {
                cout << entry->d_name << endl;
            }
            closedir(dir);
        }
        else
        {
            perror("ls");
        }
    }

    void listFilesLongFormat(string dirPath)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(dirPath.c_str())) != nullptr)
        {
            while ((entry = readdir(dir)) != nullptr)
            {
                string fullpath = dirPath + "/" + entry->d_name;
                struct stat fileStat;

                if (stat(fullpath.c_str(), &fileStat) == -1)
                {
                    perror("ls");
                    continue;
                }

                printFileDetails(entry->d_name, fileStat);
            }
            closedir(dir);
        }
        else
        {
            perror("ls");
        }
    }

    void listFilesRecursive(string dirPath)
    {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(dirPath.c_str())) != nullptr)
        {
            while ((entry = readdir(dir)) != nullptr)
            {
                if (entry->d_type == DT_DIR)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        cout << dirPath << "/" << entry->d_name << " (directory)" << endl;
                        listFilesRecursive(dirPath + "/" + entry->d_name);
                    }
                }
                else
                {
                    cout << dirPath << "/" << entry->d_name << endl;
                }
            }
            closedir(dir);
        }
        else
        {
            perror("ls");
        }
    }

    void printFileDetails(const char *filename, const struct stat &fileStat)
    {
        cout << setw(20) << left << filename;
        cout << setw(10) << left << ((S_ISDIR(fileStat.st_mode)) ? "(directory)" : "");
        cout << "Size: " << setw(10) << left << fileStat.st_size;
        cout << "Last Modified: " << ctime(&fileStat.st_mtime);
    }
};



class CdCommand : public Command
{
private:
    string path;

public:
    CdCommand(string p) : path(p) {}

    void execute() override
    {
        changeDirectory(path);
    }

    void showHelpMessage() override
    {
        cout << "Usage: cd : change the directory" << endl;
        cout << "Options:" << endl;
        cout << "  -h       Show help message" << endl;
    }
    char *printCurrentDirectory()
    {
        char *currentPath = nullptr;
        if ((currentPath = getcwd(nullptr, 0)) != nullptr)
        {
            cout << "Current directory: " << currentPath << endl;
            return currentPath;
            free(currentPath);
        }
        else
        {
            perror("getcwd");
        }
        return NULL;
    }

private:
    void changeDirectory(const string &newPath)
    {
        const size_t bufferSize = 4096; // You can adjust this value as needed
        char cwd[bufferSize];

        if (chdir(newPath.c_str()) == 0)
        {
            if (getcwd(cwd, sizeof(cwd)) != nullptr)
            {
                cout << "Changed directory to: " << cwd << endl;
                // printCurrentDirectory();
            }
            else
            {
                perror("getcwd");
            }
        }
        else
        {
            perror("cd");
        }
    }
};