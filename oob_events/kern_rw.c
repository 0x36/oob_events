#include "kern_rw.h"

mach_port_t tfp0 = MACH_PORT_NULL;

uint64_t read64(kaddr_t addr)
{
    assert(tfp0 != MACH_PORT_NULL);
    uint32_t lo = read32(addr);
    uint32_t hi = read32(addr+4);
    //printf("lo,hi = %x, %x\n",lo,hi);
    return (((uint64_t)hi << 0x20) | lo);
}

uint32_t read32(kaddr_t addr)
{
    assert(tfp0 != MACH_PORT_NULL);
    kern_return_t kr = KERN_SUCCESS;
    uint32_t data = 0;

    vm_size_t out_data = 4;

    kr = vm_read_overwrite(tfp0, addr, 4, (vm_address_t)&data, &out_data);
    //kr = mach_vm_read_overwrite(tfp0, addr, 4, (vm_address_t)&data, &out_data);
    if(kr != KERN_SUCCESS) {
        printf("mach_vm_read_overwrite() failed : %s \n",mach_error_string(kr));
        return -1;
    }
    if(out_data != 4) {
        printf("Failed to read exact size request via read32()\n");
        return -1;
    }

    return data;
}


uint64_t kmem_alloc(vm_size_t size) {
    vm_address_t address = 0;
    kern_return_t kr = vm_allocate(tfp0, (mach_vm_address_t *)&address, size, VM_FLAGS_ANYWHERE);
    //assert(kr == KERN_SUCCESS);
    CHECK_MACH_ERR(kr, "kmem_alloc()");
    return address;
}

void write32(uint64_t address,uint32_t value)
{
    assert(tfp0 != MACH_PORT_NULL);

    kern_return_t kr = vm_write(tfp0, (vm_address_t)address, (vm_offset_t)&value, sizeof(uint32_t));
    if (kr != KERN_SUCCESS) {
        printf("kernel write32 failed: %s %x\n", mach_error_string(kr), kr);
        return;
    }
}

void write64(uint64_t address, uint64_t value) {
  kern_return_t err =
  mach_vm_write(tfp0,
                address,
                (vm_offset_t)&value,
                8);
  if (err != KERN_SUCCESS) {
    printf("write failed\n");
  }
}
