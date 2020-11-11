#ifndef _H_KERN_RW_H
#define _H_KERN_RW_H
#include "utils.h"

extern mach_port_t tfp0;
typedef uint64_t kaddr_t;

kern_return_t mach_vm_read(
                           vm_map_t target_task,
                           mach_vm_address_t address,
                           mach_vm_size_t size,
                           vm_offset_t *data,
                           mach_msg_type_number_t *dataCnt);

kern_return_t mach_vm_write(
                            vm_map_t target_task,
                            mach_vm_address_t address,
                            vm_offset_t data,
                            mach_msg_type_number_t dataCnt);

kern_return_t mach_vm_read_overwrite(
                                     vm_map_t target_task,
                                     mach_vm_address_t address,
                                     mach_vm_size_t size,
                                     mach_vm_address_t data,
                                     mach_vm_size_t *outsize);

uint64_t read64(kaddr_t addr);
uint32_t read32(kaddr_t addr);

void *read_bytes_with_size(kaddr_t addr, vm_size_t rsize);
size_t write_bytes_with_size(uint64_t addr, const void *buf, size_t wsize);

uint64_t kmem_alloc(vm_size_t size);
void write64(uint64_t address,uint64_t value);
void write32(uint64_t address,uint32_t value);
#endif
