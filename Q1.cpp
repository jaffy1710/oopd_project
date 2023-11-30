#include "Command.h"

using namespace std;


int main() {
  
  
    string command;
    string sourcePath, destinationPath, filePath, newPath;

    cout << "Enter command (ls, cp, mv, rm, cd): ";
    cin >> command;

    
    Command* cmd = nullptr;

    if (command == "ls") {
        cmd = new LsCommand("C:/Users/shazr/Desktop/OPD_Project", false,false);
    } else if (command == "cp") {
        cout << "Enter source file path: ";
        cin >> sourcePath;
        cout << "Enter destination file path: ";
        cin >> destinationPath;
        cmd = new CpCommand(sourcePath, destinationPath,false,true,true);
    } else if (command == "mv") {
        cout << "Enter source file path: ";
        cin >> sourcePath;
        cout << "Enter destination file path: ";
        cin >> destinationPath;
        cmd = new MvCommand(sourcePath, destinationPath,false,false,true);
    } else if (command == "rm") {
        cout << "Enter file path: ";
        cin >> filePath;
        cmd = new RmCommand(filePath,false,true,true);
    } else if (command == "cd") {
        cout << "Enter new directory path: ";
        cin >> newPath;
        cmd = new CdCommand(newPath);
        cmd->printCurrentDirectory();
        
    } else {
        cout << "Invalid command\n";
        return 1;
    }

    if (cmd) {
        cmd->execute();
        delete cmd; 
    }

   



    return 0;
}