#include <iostream>
#include <string>
#include <cstdlib>
#include <Windows.h>

using namespace std;

class CopyCommand {
private:
    string source;
    string destination;

public:
    CopyCommand(const string& src, const string& dest) : source(src), destination(dest) {}

    void execute() {
        copyFilesRecursive(source, destination);
    }

    static DWORD WINAPI ThreadFunc(LPVOID param) {
        CopyCommand* copyCommand = static_cast<CopyCommand*>(param);
        copyCommand->execute();
        return 0;
    }

private:
    void copyFilesRecursive(const string& srcDir, const string& destDir) {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((srcDir + "\\*").c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            cerr << "Error finding source directory: " << srcDir << endl;
            return;
        }

        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // Ignore '.' and '..' directories
                if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                    string subSrcDir = srcDir + "\\" + findFileData.cFileName;
                    string subDestDir = destDir + "\\" + findFileData.cFileName;

                    // Create a thread for each subdirectory
                    HANDLE threadHandle = CreateThread(NULL, 0, CopyCommand::ThreadFunc, new CopyCommand(subSrcDir, subDestDir), 0, NULL);
                    WaitForSingleObject(threadHandle, INFINITE);
                    CloseHandle(threadHandle);
                }
            } else {
                copyFile(srcDir + "\\" + findFileData.cFileName, destDir + "\\" + findFileData.cFileName);
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }

    void copyFile(const string& srcFile, const string& destFile) {
        if (!CopyFile(srcFile.c_str(), destFile.c_str(), FALSE)) {
            cerr << "Error copying file: " << srcFile << " to " << destFile << endl;
        }
    }
};

int main() {
    string source, destination;

    cout << "Enter source folder path: ";
    getline(cin, source);

    cout << "Enter destination folder path: ";
    getline(cin, destination);

    // Get the number of cores available
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    unsigned int numCores = sysinfo.dwNumberOfProcessors;

    // Example usage of the multi-threaded 'cp' command
    CopyCommand* copyCommand = new CopyCommand(source, destination);

    // Create a thread for each core
    for (unsigned int i = 0; i < numCores; ++i) {
        HANDLE threadHandle = CreateThread(NULL, 0, CopyCommand::ThreadFunc, copyCommand, 0, NULL);
        WaitForSingleObject(threadHandle, INFINITE);
        CloseHandle(threadHandle);
    }

    delete copyCommand;

    return 0;
}
