#include <uefi.h>

struct pdraw 
{
    int pixel_size;
    int x_max;
    int y_max;
    efi_gop_t *gop;
};

void print_gop_info()
{
    efi_status_t status;
    efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    efi_gop_t *gop = NULL;

    status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    if(!EFI_ERROR(status) && gop) {
        printf("Framebuffer address: %x, size: %d, width: %d, height: %d, pixelsperline %d\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferSize,
        gop->Mode->Information->HorizontalResolution,
        gop->Mode->Information->VerticalResolution,
        gop->Mode->Information->PixelsPerScanLine);
    }
}

struct pdraw setup_graphics(int pixel_size)
{
    struct pdraw p;

    efi_status_t status;
    efi_guid_t gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    efi_gop_t *gop = NULL;

    /* set video mode */
    status = BS->LocateProtocol(&gopGuid, NULL, (void**)&gop);
    printf("test\n");

    /* construct pdraw struct */
    p.pixel_size = pixel_size;

    int screen_width = gop->Mode->Information->HorizontalResolution;
    int screen_height = gop->Mode->Information->VerticalResolution;
    p.x_max = screen_width / pixel_size;
    if (screen_width % pixel_size != 0) {
        printf("pdraw: WARNING: Pixel size does not divide screen's horizontal  \
        resolution: right margin will be visible\n");
    }
    p.y_max = screen_height / pixel_size;
    if (screen_height % pixel_size != 0) {
        printf("pdraw: WARNING: Pixel size does not divide screen's vertical \
        resolution: bottom margin will be visible\n");
    }
    p.gop = gop;
    printf("%d %d %d\n", p.pixel_size, p.x_max, p.y_max);
    printf("%x %d", p.gop->Mode->FrameBufferBase, p.gop->Mode->Information->PixelsPerScanLine);
    // sleep(3);

    // printf("Pixel format: %x\n", gop->Mode->Information->PixelFormat);
    // sleep(5);
    if(!EFI_ERROR(status) && gop) {
        status = gop->SetMode(gop, 0);
        ST->ConOut->Reset(ST->ConOut, 0);
        ST->StdErr->Reset(ST->StdErr, 0);
        if(EFI_ERROR(status)) {
            printf("unable to set video mode\n");
            sleep(5);
            exit(1);
        }
    } else {
        printf("unable to get graphics output protocol\n");
        sleep(5);
        exit(1);
    }

    return p;
}

void pixel_on(struct pdraw p,  int x, int y, uint32_t color)
{
    // if (x >= 0 && x < p.x_max && y >= 0 && y < p.y_max) {      
        int pix_per_scanl = p.gop->Mode->Information->PixelsPerScanLine;
        uint32_t *ppix_base = (uint32_t*)(p.gop->Mode->FrameBufferBase + 4 * pix_per_scanl * p.pixel_size * y + 4 * p.pixel_size * x);
        for (int y_i=0; y_i<p.pixel_size; ++y_i) {
            for (int x_i=0; x_i<p.pixel_size; ++x_i) {
                *((uint32_t*)(ppix_base + pix_per_scanl * y_i + x_i)) = color;
            }
        }
    // }
}

void pixel_off(struct pdraw p, int x, int y)
{
    pixel_on(p, x, y, 0x00000000);
}
