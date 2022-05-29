#include <uefi.h>
#include "pc-uefi.h"

int main(int argc, char **argv)
{
    printf("Starting graphics...\n");
    //sleep(3);
    struct pdraw pd = setup_graphics(10); 
    uint32_t pixel = 0x00ffffff;
    pixel_on(pd, 0, 20, pixel);
    pixel_on(pd, 0, 21, pixel);
    for (int i=1; i<100; i++) { 
        pixel_on(pd, i, 20, pixel);
        pixel_on(pd, i, 21, pixel);
        usleep(100000);
        pixel_off(pd, i-1, 20);
        pixel_off(pd, i-1, 21);
    }
    pixel_on(pd, 0, 0, pixel);
    sleep(300);
    return 0;
}























