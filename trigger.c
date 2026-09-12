// trigger.c
// Usermode application that WILL call the driver and bugcheck the VM/machine. 
// Make sure to save your work before double-clicking on the EXE.

#include <windows.h>
#include <stdio.h>

#define IOCTL_TRIGGER_BUGCHECK \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_WRITE_ACCESS)

int main(void)
{
    HANDLE hDevice = CreateFileW(
        L"\\\\.\\CustomKE",
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open device, Error %lu\n", GetLastError());
        printf("Is the driver loaded? Try 'sc query [Your driver's name]'\n");
        return 1;
    }

    printf("Device opened, sending trigger IOCTL...\n");

    DWORD bytesReturned = 0;
    BOOL ok = DeviceIoControl(
        hDevice,
        IOCTL_TRIGGER_BUGCHECK,
        NULL, 0,
        NULL, 0,
        &bytesReturned,
        NULL);

    // If this prints, something failed.
    if (!ok)
    {
        printf("DeviceIoControl failed, Error %lu\n", GetLastError());
    }

    CloseHandle(hDevice);
    return 0;
}
