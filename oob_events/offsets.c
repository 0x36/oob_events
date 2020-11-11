#import <stdio.h>

#import "offsets.h"

int* offsets = NULL;
// 13.5 and up
int kstruct_offsets_13_5[] = {
    // task_t
    [KSTRUCT_OFFSET_TASK_LCK_MTX_TYPE]              = 0xb,
    [KSTRUCT_OFFSET_TASK_REF_COUNT]         = 0x10,
    [KSTRUCT_OFFSET_TASK_ACTIVE]            = 0x14,
    [KSTRUCT_OFFSET_TASK_VM_MAP]             = 0x28,
    [KSTRUCT_OFFSET_TASK_PREV]             = 0x38,
    [KSTRUCT_OFFSET_TASK_ITK_SELF]             = 0xf8, //ipc_task_init
    [KSTRUCT_OFFSET_TASK_ITK_HOST]                  = 0x2d0,
    [KSTRUCT_OFFSET_TASK_ITK_SPACE]                 = 0x320,
    [KSTRUCT_OFFSET_TASK_T_FLAGS]                 = 0x3b8,


#if __arm64e__
    [KSTRUCT_OFFSET_TASK_BSD_INFO]             = 0x388,
#else
    [KSTRUCT_OFFSET_TASK_BSD_INFO]             = 0x380,
#endif
    //ipc_port_t
    [KSTRUCT_OFFSET_IPC_PORT_IP_RECEIVER]         = 0x60, // ipc_port_alloc_special
    [KSTRUCT_OFFSET_IPC_PORT_IO_REFERENCES]     = 4,    // ipc_port_alloc_special
    [KSTRUCT_OFFSET_IPC_PORT_LCK_MTX_TYPE]         = 0x10, // ipc_port_alloc_special
    [KSTRUCT_OFFSET_IPC_PORT_IP_KOBJECT]         = 0x68,
    [KSTRUCT_OFFSET_IPC_PORT_IP_SRIGHTS]         = 0xa0,
    [KSTRUCT_OFFSET_IPC_PORT_IP_CONTEXT]         = 0x90,


    // ipc_kmsg_t
    [KSTRUCT_OFFSET_KMSG_IKM_HEADER]                = 0x18,
    //ipc_space_t
    [KSTRUCT_OFFSET_IPC_SPACE_IS_TABLE]             = 0x20,
    [KSTRUCT_OFFSET_IPC_SPACE_IS_TASK]         = 0x30,

    //proc_t
    [KSTRUCT_OFFSET_PROC_PID]             = 0x68,

    [IOUSERCLIENT_AGXGLCONTEXT_SHM_TYPE_1]          = 0x6a0,
    [IOUSERCLIENT_AGXGLCONTEXT_SHM_SIZE]          =   0x6d8,

    [IOUSERCLIENT_AGXGLCONTEXT_PROVIDER]            = 0x5a8,

    [IOSURFACE_INDEXEDTIMESTAMP] = 0x360,

};

// taken from a public exploit
int koffset(enum kstruct_offset offset) {
    if (offsets == NULL) {
        printf("[-] Please call init_offsets() prior to querying offsets\n");
        return 0;
    }
    return offsets[offset];
}

//uint32_t create_outsize;

int init_offsets() {
    //printf("[i] Offsets selected for iOS 13.5 and above \n");
    offsets  = kstruct_offsets_13_5;
    return 0;
}
