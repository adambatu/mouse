#include <ntddk.h>
#include "Driver.h"

#define DEVICE_NAME L"\\Device\\SimpleDriver"
#define SYMLINK_NAME L"\\DosDevices\\SimpleDriver"

PDEVICE_OBJECT g_DeviceObject = NULL;
MOUSE_INPUT g_LastInput = {0};

typedef struct _INJECT_WORKITEM_CONTEXT {
    MOUSE_INPUT Input;
    PIO_WORKITEM WorkItem;
} INJECT_WORKITEM_CONTEXT, *PINJECT_WORKITEM_CONTEXT;

VOID
InjectWorker(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_opt_ PVOID Context
    );

VOID
QueueMouseInjection(
    _In_ PMOUSE_INPUT Input
    );

NTSTATUS
DriverCreateClose(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

NTSTATUS
DriverDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

VOID
DriverUnload(
    PDRIVER_OBJECT DriverObject
    );

NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
    )
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    UNICODE_STRING deviceName;
    UNICODE_STRING symLinkName;

    RtlInitUnicodeString(&deviceName, DEVICE_NAME);
    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &g_DeviceObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);
    status = IoCreateSymbolicLink(&symLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(g_DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    return STATUS_SUCCESS;
}

VOID
DriverUnload(
    PDRIVER_OBJECT DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);

    UNICODE_STRING symLinkName;
    RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);
    IoDeleteSymbolicLink(&symLinkName);

    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
    }
}

NTSTATUS
DriverCreateClose(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS
DriverDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    ULONG_PTR info = 0;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_MY_OPERATION:
        // Handle IOCTL here
        status = STATUS_SUCCESS;
        break;
    case IOCTL_SEND_MOUSE_INPUT:
        if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(MOUSE_INPUT)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }
        {
            PMOUSE_INPUT input = (PMOUSE_INPUT)Irp->AssociatedIrp.SystemBuffer;
            g_LastInput = *input;
            QueueMouseInjection(&g_LastInput);
            status = STATUS_SUCCESS;
            info = sizeof(MOUSE_INPUT);
        }
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = info;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

VOID
InjectWorker(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_opt_ PVOID Context
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PINJECT_WORKITEM_CONTEXT ctx = (PINJECT_WORKITEM_CONTEXT)Context;
    if (ctx) {
        // Placeholder for real mouse injection using class driver APIs.
        DbgPrint("[SimpleDriver] Injecting mouse: X=%ld Y=%ld L=%d R=%d\n",
                ctx->Input.XMovement,
                ctx->Input.YMovement,
                ctx->Input.LeftButton,
                ctx->Input.RightButton);

        IoFreeWorkItem(ctx->WorkItem);
        ExFreePool(ctx);
    }
}

VOID
QueueMouseInjection(
    _In_ PMOUSE_INPUT Input
    )
{
    if (!Input || !g_DeviceObject) {
        return;
    }

    PINJECT_WORKITEM_CONTEXT ctx =
        (PINJECT_WORKITEM_CONTEXT)ExAllocatePoolWithTag(NonPagedPoolNx,
                                                       sizeof(INJECT_WORKITEM_CONTEXT),
                                                       'mInj');
    if (!ctx) {
        return;
    }

    ctx->Input = *Input;
    ctx->WorkItem = IoAllocateWorkItem(g_DeviceObject);
    if (!ctx->WorkItem) {
        ExFreePool(ctx);
        return;
    }

    IoQueueWorkItem(ctx->WorkItem, InjectWorker, DelayedWorkQueue, ctx);
}

