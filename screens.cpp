#include <iostream>
#include <windows.h>
#include <winuser.h>

int main(int argc, char *argv[]) {

    const char *exe = strrchr(argv[0], '\\');
    if (exe != nullptr) exe++;
    else exe = argv[0];

    if (argc < 2) {
        std::cout << "Usage: " << exe << " <list> [args]" << std::endl;
        return 1;
    }

    if (strcasecmp(argv[1], "list") == 0) {

        if (argc != 2) {
            std::cout << "Usage: " << exe << " " << argv[1] << std::endl;
            return 1;
        }

        // TODO print mode

        for (int i = 0; i < GetSystemMetrics(SM_CMONITORS); i++) {
            std::cout << "Screen " << i << std::endl;
            std::cout << "Resolution: " << GetSystemMetrics(SM_CXSCREEN) << "x" << GetSystemMetrics(SM_CYSCREEN) << std::endl;
            // TODO print hz / zoom and orientation
        }

    } else {
        std::cout << "Usage: " << exe << " <list> [args]" << std::endl;
        return 1;
    }

    // TODO setmode / setresolution / sethz / setzoom / setorientation

    return 0;
}
