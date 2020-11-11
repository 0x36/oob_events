#include "utils.h"

io_connect_t iokit_get_connection(const char *name,u32 type)
{
    kern_return_t kr = KERN_SUCCESS;
    io_connect_t conn = MACH_PORT_NULL;
    io_service_t service  = IOServiceGetMatchingService(kIOMasterPortDefault,
                                IOServiceMatching(name));
    if (service == IO_OBJECT_NULL) {
        printf("unable to find service %s \n",name);
        exit(0);
    }

    kr = IOServiceOpen(service, self, type, &conn);
    CHECK_MACH_ERR(kr,"IOServiceOpen");
    return conn;
}
CFNumberRef uintcf(uint value)
{
    return CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
}

void hexdump(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else
            ascii[i % 16] = '.';

        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0)
                printf("|  %s \n", ascii);
            else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j)
                    printf("   ");

                printf("|  %s \n", ascii);
            }
        }
    }
}

