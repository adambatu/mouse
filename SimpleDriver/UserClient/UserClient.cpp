#include "UserClient.h"
#include <stdio.h>

int wmain(int argc, wchar_t* argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HANDLE hDevice = CreateFileW(L"\\\\.\\SimpleDriver",
                                 GENERIC_READ | GENERIC_WRITE,
                                 0,
                                 nullptr,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open device (%lu)\n", GetLastError());
        return 1;
    }

    MOUSE_INPUT input = {0};
    input.XMovement = 10;
    input.YMovement = 5;
    input.LeftButton = TRUE;
    input.RightButton = FALSE;

    DWORD bytesReturned = 0;
    BOOL result = DeviceIoControl(hDevice,
                                  IOCTL_SEND_MOUSE_INPUT,
                                  &input,
                                  sizeof(input),
                                  nullptr,
                                  0,
                                  &bytesReturned,
                                  nullptr);

    if (!result) {
        printf("DeviceIoControl failed (%lu)\n", GetLastError());
    } else {
        printf("Mouse input sent.\n");
    }

    CloseHandle(hDevice);
    return 0;
}
