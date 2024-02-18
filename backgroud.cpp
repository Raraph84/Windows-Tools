#include <iostream>
#include <windows.h>
#include <winuser.h>

int main(int argc, char *argv[]) {

    const char *exe = strrchr(argv[0], '\\');
    if (exe != nullptr) exe++;
    else exe = argv[0];

    if (argc != 2) {
        std::cout << "Usage: " << exe << " <command>" << std::endl;
        return 1;
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(nullptr, // No module name (use command line)
                  argv[1], // Command line
                  nullptr, // Process handle not inheritable
                  nullptr, // Thread handle not inheritable
                  FALSE, // Set handle inheritance to FALSE
                  CREATE_NO_WINDOW, // No creation flags
                  nullptr, // Use parent's environment block
                  nullptr, // Use parent's starting directory
                  &si, // Pointer to STARTUPINFO structure
                  &pi); // Pointer to PROCESS_INFORMATION structure

    return 0;
}
