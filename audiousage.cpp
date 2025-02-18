#include <windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <initguid.h> // Required for next include
#include <functiondiscoverykeys_devpkey.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ole32.lib")

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

int main()
{
    HRESULT hr = S_OK;

    hr = CoInitialize(nullptr);
    if (FAILED(hr)) return -1;

    IMMDeviceEnumerator* pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator));
    if (FAILED(hr)) return -1;

    IMMDeviceCollection* pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr)) return -1;

    UINT count;
    hr = pCollection->GetCount(&count);
    if (FAILED(hr)) return -1;

    for (UINT i = 0; i < count; i++)
    {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);
        if (FAILED(hr)) return -1;

        LPWSTR pwszID = nullptr;
        hr = pDevice->GetId(&pwszID);
        if (FAILED(hr)) return -1;

        IPropertyStore* pProps = nullptr;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
        if (FAILED(hr)) return -1;

        PROPVARIANT varName;
        PropVariantInit(&varName);
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        if (FAILED(hr)) return -1;

        std::wcout << pwszID << " " << varName.pwszVal << std::endl;

        IAudioSessionManager2* pSessionManager = nullptr;
        hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(&pSessionManager));
        if (FAILED(hr)) return -1;

        IAudioSessionEnumerator* pSessionEnumerator = nullptr;
        hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);
        if (FAILED(hr)) return -1;

        int sessionCount;
        hr = pSessionEnumerator->GetCount(&sessionCount);
        if (FAILED(hr)) return -1;

        for (int j = 0; j < sessionCount; j++)
        {
            IAudioSessionControl* pSessionControl = nullptr;
            hr = pSessionEnumerator->GetSession(j, &pSessionControl);
            if (FAILED(hr)) return -1;

            IAudioSessionControl2* pSessionControl2 = nullptr;
            hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void**>(&pSessionControl2));
            if (FAILED(hr)) return -1;

            AudioSessionState state;
            hr = pSessionControl2->GetState(&state);
            if (FAILED(hr)) return -1;
            if (state != AudioSessionStateActive) continue;

            DWORD processId;
            hr = pSessionControl2->GetProcessId(&processId);
            if (FAILED(hr)) return -1;

            if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId); hProcess != nullptr)
            {
                WCHAR szProcessName[MAX_PATH] = {};
                DWORD dwSize = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, szProcessName, &dwSize))
                    std::wcout << "- " << szProcessName << std::endl;

                CloseHandle(hProcess);
            }

            pSessionControl2->Release();
            pSessionControl->Release();
        }

        pSessionEnumerator->Release();
        pSessionManager->Release();

        hr = PropVariantClear(&varName);
        if (FAILED(hr)) return -1;

        if (pProps) pProps->Release();
        if (pwszID) CoTaskMemFree(pwszID);
        pDevice->Release();
    }

    pCollection->Release();
    pEnumerator->Release();

    CoUninitialize();

    return 0;
}
