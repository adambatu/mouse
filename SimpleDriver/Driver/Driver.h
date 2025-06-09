#pragma once

// Custom IOCTLs
#define IOCTL_MY_OPERATION       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SEND_MOUSE_INPUT   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Structure describing mouse input data
typedef struct _MOUSE_INPUT {
    LONG XMovement;
    LONG YMovement;
    BOOLEAN LeftButton;
    BOOLEAN RightButton;
} MOUSE_INPUT, *PMOUSE_INPUT;
