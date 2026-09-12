// driver.c
// Make sure it is the CORRECT version!!!

#include <ntddk.h>
#include <wdf.h>

#define MY_BUGCHECK_CODE   0x1B

#define NT_DEVICE_NAME     L"\\Device\\CustomKE"
#define DOS_DEVICE_NAME    L"\\DosDevices\\CustomKE"

#define IOCTL_TRIGGER_BUGCHECK \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_WRITE_ACCESS)

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
__drv_dispatchType(IRP_MJ_CREATE) DRIVER_DISPATCH DispatchCreateClose;
__drv_dispatchType(IRP_MJ_CLOSE) DRIVER_DISPATCH DispatchCreateClose;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH DispatchDeviceControl;

static VOID CompleteIrp(_In_ PIRP Irp, _In_ NTSTATUS Status)
{
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
}

NTSTATUS DispatchCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    CompleteIrp(Irp, STATUS_SUCCESS);
    return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    switch (stack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_TRIGGER_BUGCHECK:
        // You can change these paramaters to anything, make sure it is 0-9, A-F only. Windows is very picky about ULONG hex codes.
        KeBugCheckEx(MY_BUGCHECK_CODE,
                     (ULONG_PTR)0,   
                     (ULONG_PTR)0,   
                     (ULONG_PTR)0,        
                     (ULONG_PTR)0);       
        break;

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    CompleteIrp(Irp, status);
    return status;
}

VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING dosDeviceName;
    RtlInitUnicodeString(&dosDeviceName, DOS_DEVICE_NAME);
    IoDeleteSymbolicLink(&dosDeviceName);

    if (DriverObject->DeviceObject)
    {
        IoDeleteDevice(DriverObject->DeviceObject);
    }
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;
    UNICODE_STRING ntDeviceName;
    UNICODE_STRING dosDeviceName;
    PDEVICE_OBJECT deviceObject = NULL;

    RtlInitUnicodeString(&ntDeviceName, NT_DEVICE_NAME);
    RtlInitUnicodeString(&dosDeviceName, DOS_DEVICE_NAME);

    status = IoCreateDevice(
        DriverObject,
        0,
        &ntDeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = IoCreateSymbolicLink(&dosDeviceName, &ntDeviceName);
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(deviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    deviceObject->Flags |= DO_BUFFERED_IO;
    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}
