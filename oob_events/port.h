#ifndef port_h
#define port_h

#include <stdio.h>
#include <sys/socket.h>
#include "utils.h"

struct simple_msg {
    mach_msg_header_t hdr;
    u8 buf[0];
};

struct msg_ool_ports {
    mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_ports_descriptor_t ool_ports;
};

struct msg_ool_ports_multi {
    mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_ports_descriptor_t ool_ports[0];
};


struct msg_ool_mem_multi {
    mach_msg_header_t hdr;
    mach_msg_body_t body;
    mach_msg_ool_descriptor_t ool[0];
};

void send_payload(mach_port_t service_port, const char *buf,uint32_t size);
mach_port_t allocate_port_full(uint32_t size);
mach_port_t allocate_port(void);
mach_port_t allocate_port_with_sright(void);
mach_port_t * allocate_ports(u32 count,bool sright);
mach_port_t send_kalloc_message(uint8_t *replacer_message_body, uint32_t replacer_body_size);
void deallocate_ports(mach_port_t *ports,u32 count);
void destroy_ports(mach_port_t *ports,u32 count);
void get_attributes_of_port(mach_port_t port);
bool has_a_send_right(mach_port_t port);
mach_voucher_t  * allocate_vouchers(uint32_t count);
bool got_message(mach_port_t q);
void read_message_once(mach_port_t q);
mach_port_t * allocate_fileport(uint32_t count);
u32 message_size_for_kalloc_size(u32 kalloc_size);
void send_inline_msg(mach_port_t rcvr,u8 *body, u32 sz);
void set_port_for_notification(io_connect_t c,mach_port_t port);
//mach_port_t do_ipc_port_kalloc(mach_port_t dest, mach_port_t target_port,u32 kalloc_size);
mach_port_t do_ipc_port_kalloc(mach_port_t dest,u32 kalloc_count ,mach_port_t target_port,uint32_t port_count);
struct msg_ool_ports * receive_ool_port_kalloc(mach_port_t dest);
//mach_port_t do_ool_port_kalloc(mach_port_t dest, mach_port_t target_port,u32 kalloc_size,u32 send_count);
mach_port_t do_ool_port_kalloc(mach_port_t dest, mach_port_t target_port,u32 port_count,u32 send_count);
mach_port_t do_ool_mem_kalloc(mach_port_t dest, u32 kalloc_count, char *buffer,u32 kalloc_size);
struct msg_ool_mem_multi * do_ool_mem_kalloc_recv(mach_port_t dest, u32 kalloc_count);
kern_return_t send_message(mach_port_t destination, void *buffer, mach_msg_size_t size);
struct simple_msg* receive_message(mach_port_t source, mach_msg_size_t size);
#endif /* port_h */
