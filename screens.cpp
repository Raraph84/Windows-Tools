#include <iostream>
#include <windows.h>
#include <winuser.h>
#include <vector>

std::vector<MONITORINFOEX> monitors() {
    std::vector<MONITORINFOEX> monitors;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) -> BOOL {
        auto *monitors = reinterpret_cast<std::vector<MONITORINFOEX> *>(dwData);
        MONITORINFOEX info;
        info.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &info))
            monitors->push_back(info);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

int main(int argc, char *argv[]) {

    const char *exe = strrchr(argv[0], '\\');
    if (exe != nullptr) exe++;
    else exe = argv[0];

    if (argc < 2) {
        std::cout << "Usage: " << exe << " <list, setresolution, setrefreshrate> [args]" << std::endl;
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
            std::cout << "Refresh rate: " << GetDeviceCaps(GetDC(nullptr), VREFRESH) << std::endl;
            // TODO print zoom and orientation
        }

    } else if (strcasecmp(argv[1], "setresolution") == 0) {

        if (argc != 5) {
            std::cout << "Usage: " << exe << " " << argv[1] << " <screen> <width> <height>" << std::endl;
            return 1;
        }

        char *end;
        int screen = strtol(argv[2], &end, 0);
        int width = strtol(argv[3], &end, 0);
        int height = strtol(argv[4], &end, 0);

        if (screen < 0 || screen >= GetSystemMetrics(SM_CMONITORS)) {
            std::cout << "This screen does not exist" << std::endl;
            return 1;
        }

        if (width < 0 || height < 0) {
            std::cout << "Invalid resolution" << std::endl;
            return 1;
        }

        DEVMODE desiredMode = {0};
        desiredMode.dmSize = sizeof(DEVMODE);
        desiredMode.dmPelsWidth = width;
        desiredMode.dmPelsHeight = height;
        desiredMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

        LONG res = ChangeDisplaySettingsEx(monitors()[screen].szDevice, &desiredMode, nullptr, CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET, nullptr);
        if (res != DISP_CHANGE_SUCCESSFUL) {
            std::cout << "Failed to set resolution" << std::endl;
            return 1;
        }

    } else if (strcasecmp(argv[1], "setrefreshrate") == 0) {

        if (argc != 4) {
            std::cout << "Usage: " << exe << " " << argv[1] << " <screen> <refresh rate>" << std::endl;
            return 1;
        }

        char *end;
        int screen = strtol(argv[2], &end, 0);
        int hz = strtol(argv[3], &end, 0);

        if (screen < 0 || screen >= GetSystemMetrics(SM_CMONITORS)) {
            std::cout << "This screen does not exist" << std::endl;
            return 1;
        }

        if (hz < 0) {
            std::cout << "Invalid refresh rate" << std::endl;
            return 1;
        }

        DEVMODE desiredMode = {0};
        desiredMode.dmSize = sizeof(DEVMODE);
        desiredMode.dmDisplayFrequency = hz;
        desiredMode.dmFields = DM_DISPLAYFREQUENCY;

        LONG res = ChangeDisplaySettingsEx(monitors()[screen].szDevice, &desiredMode, nullptr, CDS_UPDATEREGISTRY | CDS_GLOBAL | CDS_RESET, nullptr);
        if (res != DISP_CHANGE_SUCCESSFUL) {
            std::cout << "Failed to set refresh rate" << std::endl;
            return 1;
        }

    } else {
        std::cout << "Usage: " << exe << " <list, setresolution, setrefreshrate> [args]" << std::endl;
        return 1;
    }

    // TODO setmode / setzoom / setorientation

    return 0;
}
