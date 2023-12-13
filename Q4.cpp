#include <iostream>
#include <vector>
#include <windows.h>
#include <dirent.h>
#include <cstring>
#include <chrono>

using namespace std;

class LsCommand {
private:
    string path;
    bool recursive;

public:
    LsCommand(string p, bool r) : path(p), recursive(r) {}

    void execute() {
        auto start = chrono::steady_clock::now();  // Start measuring time

        // Get the number of available cores
        unsigned int numCores = 4;  

        vector<HANDLE> threads;
        vector<unsigned int> threadIds;
        for (unsigned int i = 0; i < numCores; ++i) {
            DWORD threadId;
            HANDLE threadHandle = CreateThread(NULL, 0, &LsCommand::threadFunctionWrapper, this, 0, &threadId);
            threads.push_back(threadHandle);
            threadIds.push_back(threadId);
        }

        // Wait for all threads to finish
        WaitForMultipleObjects(threads.size(), threads.data(), TRUE, INFINITE);

        // Close thread handles
        for (auto handle : threads) {
            CloseHandle(handle);
        }

        auto end = chrono::steady_clock::now();  // Stop measuring time
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << "Total execution time: " << elapsed << " milliseconds" << endl;
    }

    

private:
    static DWORD WINAPI threadFunctionWrapper(LPVOID lpParam) {
        LsCommand* lsCommand = static_cast<LsCommand*>(lpParam);
        lsCommand->threadFunction();
        return 0;
    }

    void threadFunction() {
        auto start = chrono::steady_clock::now();  

        unsigned int startIdx = 0;  
        unsigned int stride = 1;    

        DIR *dir;
        struct dirent *entry;
        string fullPath;

        if ((dir = opendir(path.c_str())) != nullptr) {
            unsigned int i = 0;
            while ((entry = readdir(dir)) != nullptr) {
                // Distribute work among threads
                if (i % stride == startIdx) {
                    fullPath = path + "/" + entry->d_name;

                    if (entry->d_type == DT_DIR) {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                            listFilesRecursive(fullPath);
                        }
                    }
                }

                i++;
            }
            closedir(dir);
        } else {
            perror("ls");
        }

        auto end = chrono::steady_clock::now();  // Stop measuring time
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << "Thread execution time: " << elapsed << " milliseconds" << endl;
    }

    // ... (rest of the code remains unchanged)

    void listFilesRecursive(const string &dirPath) {
        // Perform the recursive listing
        cout << dirPath << " (directory)" << endl;
        // Additional code for recursive listing...

        // Recursive call for each directory entry
        DIR *dir;
        struct dirent *entry;
        if ((dir = opendir(dirPath.c_str())) != nullptr) {
            while ((entry = readdir(dir)) != nullptr) {
                string subPath = dirPath + "/" + entry->d_name;
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    listFilesRecursive(subPath);
                }
            }
            closedir(dir);
        }
    }
};

int main() {
    auto start = chrono::steady_clock::now();  // Start measuring time

    LsCommand ls("E:/oopd_project", true); // Set your directory path and recursion flag
    ls.execute();

    auto end = chrono::steady_clock::now();  // Stop measuring time
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Total execution time: " << elapsed << " milliseconds" << endl;

    return 0;
}
