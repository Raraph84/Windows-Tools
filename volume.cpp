#include <iostream>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <chrono>
#include <thread>

float oldVolume;
BOOL oldMuted;
BOOL initialized = FALSE;
IAudioEndpointVolume *endpointVolume = nullptr;
long interval;

void pool() {

    while (true) {

        float volume = 0;
        endpointVolume->GetMasterVolumeLevelScalar(&volume);
        BOOL muted = FALSE;
        endpointVolume->GetMute(&muted);

        if (volume != oldVolume || !initialized) {
            std::cout << "volume " << volume * 100 << std::endl;
            oldVolume = volume;
        }

        if (muted != oldMuted || !initialized) {
            std::cout << "muted " << (muted ? "true" : "false") << std::endl;
            oldMuted = muted;
        }

        initialized = TRUE;

        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

int main(int argc, char *argv[]) {

    const char *exe = strrchr(argv[0], '\\');
    if (exe != nullptr) exe++;
    else exe = argv[0];

    if (argc < 2) {
        std::cout << "Usage: " << exe << " <getvolume, getmuted, setvolume, setmuted, pool> [args]" << std::endl;
        return 1;
    }

    CoInitialize(nullptr);
    IMMDeviceEnumerator *deviceEnumerator = nullptr;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator),
                     (LPVOID *) &deviceEnumerator);

    IMMDevice *defaultDevice = nullptr;
    deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = nullptr;

    defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID *) &endpointVolume);
    defaultDevice->Release();
    defaultDevice = nullptr;

    if (strcasecmp(argv[1], "getvolume") == 0) {

        if (argc != 2) {
            std::cout << "Usage: " << exe << " " << argv[1] << std::endl;
            return 1;
        }

        float volume = 0;
        endpointVolume->GetMasterVolumeLevelScalar(&volume);

        std::cout << "volume " << volume * 100 << std::endl;

    } else if (strcasecmp(argv[1], "getmuted") == 0) {

        if (argc != 2) {
            std::cout << "Usage: " << exe << " " << argv[1] << std::endl;
            return 1;
        }

        BOOL muted = FALSE;
        endpointVolume->GetMute(&muted);

        std::cout << "muted " << (muted ? "true" : "false") << std::endl;

    } else if (strcasecmp(argv[1], "setvolume") == 0) {

        if (argc != 3) {
            std::cout << "Usage: " << exe << " " << argv[1] << " <volume>" << std::endl;
            return 1;
        }

        char *end;
        float volume = strtof(argv[2], &end);
        if (end == argv[2]) {
            std::cout << "Invalid volume" << std::endl;
            return 1;
        }

        if (volume < 0 || volume > 100) {
            std::cout << "Invalid volume" << std::endl;
            return 1;
        }

        endpointVolume->SetMasterVolumeLevelScalar((float) volume / 100, nullptr);

        std::cout << "volume " << volume << std::endl;

    } else if (strcasecmp(argv[1], "setmuted") == 0) {

        if (argc != 3) {
            std::cout << "Usage: " << exe << " " << argv[1] << " <muted>" << std::endl;
            return 1;
        }

        BOOL muted = FALSE;
        if (strcasecmp(argv[2], "true") == 0) muted = TRUE;
        else if (strcasecmp(argv[2], "false") == 0) muted = FALSE;
        else {
            std::cout << "Invalid muted" << std::endl;
            return 1;
        }

        endpointVolume->SetMute(muted, nullptr);

        std::cout << "muted " << (muted ? "true" : "false") << std::endl;

    } else if (strcasecmp(argv[1], "pool") == 0) {

        if (argc != 3) {
            std::cout << "Usage: " << exe << " pool [interval]" << std::endl;
            return 1;
        }

        char *end;
        interval = strtol(argv[2], &end, 10);
        if (end == argv[2]) {
            std::cout << "Invalid interval" << std::endl;
            return 1;
        }

        std::thread t(pool);

        std::string line;
        while (std::getline(std::cin, line)) {

            if (line.substr(0, 7) == "volume ") {

                const char *line2 = line.substr(7).c_str();
                char *end2;
                float volume = strtof(line2, &end2);
                if (end2 == line2) {
                    std::cout << "Invalid volume" << std::endl;
                    continue;
                }

                if (volume < 0 || volume > 100) {
                    std::cout << "Invalid volume" << std::endl;
                    continue;
                }

                endpointVolume->SetMasterVolumeLevelScalar((float) volume / 100, nullptr);

            } else if (line.substr(0, 6) == "muted ") {

                BOOL muted = FALSE;
                if (line.substr(6) == "true") muted = TRUE;
                else if (line.substr(6) == "false") muted = FALSE;
                else {
                    std::cout << "Invalid muted" << std::endl;
                    continue;
                }

                endpointVolume->SetMute(muted, nullptr);

            } else
                std::cout << "Invalid command" << std::endl;
        }

    } else {
        std::cout << "Usage: " << exe << " <getvolume, getmuted, setvolume, setmuted, pool> [args]" << std::endl;
        return 1;
    }

    endpointVolume->Release();
    CoUninitialize();

    return 0;
}
