#include <iostream>
#include <string>
#include <windows.h>
#include <chrono>

class CopyCommand {
public:
    CopyCommand(const std::wstring& source, const std::wstring& destination)
        : sourcePath(source), destinationPath(destination) {}

    void execute() {
        auto startTime = std::chrono::high_resolution_clock::now();

        if (CopyFileW(sourcePath.c_str(), destinationPath.c_str(), FALSE)) {
            std::wcout << L"Copy successful!" << std::endl;
        } else {
            std::wcerr << L"Error copying file. Error code: " << GetLastError() << std::endl;
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

        std::wcout << L"Copy completed in " << duration << L" milliseconds." << std::endl;
    }

    static DWORD WINAPI ThreadFunc(LPVOID param) {
        CopyCommand* copyCommand = static_cast<CopyCommand*>(param);
        copyCommand->execute();
        return 0;
    }

private:
    std::wstring sourcePath;
    std::wstring destinationPath;
};

int main() {
    std::wcout << L"Enter source path: ";
    std::wstring sourcePath;
    std::getline(std::wcin, sourcePath);

    std::wcout << L"Enter destination path: ";
    std::wstring destinationPath;
    std::getline(std::wcin, destinationPath);

    // Single-threaded copy
    CopyCommand singleThreadedCopy(sourcePath, destinationPath);
    singleThreadedCopy.execute();

    // Multi-threaded copy
    HANDLE threadHandle = CreateThread(NULL, 0, CopyCommand::ThreadFunc, &singleThreadedCopy, 0, NULL);
    WaitForSingleObject(threadHandle, INFINITE);
    CloseHandle(threadHandle);

    return 0;
}
