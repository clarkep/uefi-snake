#include <uefi.h>
#include "pc-uefi.h"

void draw_measures(struct pdraw pd) 
{
    uint32_t pixel = 0x0000ff00;
    for (int i=0; i<pd.y_max; i+=2) {
        pixel_on(pd, i, pd.x_max-1, (i%8==0) ? pixel+0x00ff00ff : pixel);
    }
}

void draw_screen(struct pdraw pd, uint32_t color)
{
    for (int y=0; y<pd.y_max; ++y)
        for (int x=0; x<pd.x_max; ++x)
            pixel_on(pd, y, x, color);
}

void clear_screen(struct pdraw pd)
{
    for (int y=0; y<pd.y_max; ++y)
        for (int x=0; x<pd.x_max; ++x)
            pixel_off(pd, y, x);
}

void redraw_snake(struct pdraw pd, size_t head, size_t tail, uint16_t snake_buffer[][2], uint32_t snake_color)
{
    for (int i=tail; i<=head; i++)
    {
        pixel_on(pd, snake_buffer[i][0], snake_buffer[i][1], snake_color);
    }
}

void place_new_apple(struct pdraw pd, int32_t apple_color, uint8_t avoid_buffer[pd.y_max][pd.x_max], uint16_t *apple_y, uint16_t *apple_x)
{
    do {
        *apple_y = rand() % pd.y_max;
        *apple_x = rand() % pd.x_max;
    } while (*apple_y > pd.y_max || *apple_x > pd.x_max || avoid_buffer[*apple_y][*apple_x]);
    pixel_on(pd, *apple_y, *apple_x, apple_color);
}

void snake_round(struct pdraw pd)
{   
    // draw_measures(pd);
    clear_screen(pd);
    uint32_t snake_color = 0x0000ff00;
    uint32_t screen_color = 0x00000000;
    int game_over = 0;
    const size_t max_snake_len = pd.x_max * pd.y_max;
    uint16_t snake_buffer[max_snake_len][2];
    uint8_t world_buffer[pd.y_max][pd.x_max];
    memset(world_buffer, 0, pd.x_max*pd.y_max);
    size_t sbuf_head_index = 3;
    size_t sbuf_tail_index = 0;
    for (int i=0; i<4; ++i) 
    {
        world_buffer[20][17+i] = 1;
        pixel_on(pd, 20, 17+i, snake_color);
        snake_buffer[i][0] = 20;
        snake_buffer[i][1] = 17+i;
    }
    uint16_t head_y = 20, head_x = 20;
    int v_x = 1, v_y = 0;

    uint32_t apple_color = 0x00ff0022;
    srand(123);
    uint16_t apple_y, apple_x;
    place_new_apple(pd, apple_color, world_buffer, &apple_y, &apple_x); 
    while (!game_over) { 
        int key;
        switch (key=getchar_ifany()) {
            case 105:
                if (v_y > 0) break;
                v_y = -1;
                v_x = 0;
                break;
            case 106:
                if (v_x > 0) break;
                v_y = 0;
                v_x = -1;
                break;
            case 107:
                if (v_y < 0) break;
                v_y = 1;
                v_x = 0;
                break;
            case 108:
                if (v_x < 0) break;
                v_y = 0;
                v_x = 1;
                break;
        }
        head_y += v_y;
        head_x += v_x;
        if (world_buffer[head_y][head_x] || head_x < 0 || head_x >= pd.x_max ||\
            head_y < 0 || head_y >= pd.y_max) {
            game_over = 1;
        } else {
            world_buffer[head_y][head_x] = 1;
            pixel_on(pd, head_y, head_x, snake_color);
            sbuf_head_index = (sbuf_head_index + 1) % max_snake_len;
            snake_buffer[sbuf_head_index][0] = head_y;
            snake_buffer[sbuf_head_index][1] = head_x;
            if (head_y == apple_y && head_x == apple_x) {
                screen_color = ((uint32_t) rand()) >> 2;
                draw_screen(pd, screen_color);
                redraw_snake(pd, sbuf_head_index, sbuf_tail_index, snake_buffer, snake_color);
                place_new_apple(pd, apple_color, world_buffer, &apple_y, &apple_x);
                // We've just done a fair amount of calculation, so we should sleep less.
                // How much less, I'm not sure.
                usleep(25000);
                continue;
            } else {
                pixel_on(pd, snake_buffer[sbuf_tail_index][0], 
                          snake_buffer[sbuf_tail_index][1], screen_color);
                world_buffer[snake_buffer[sbuf_tail_index][0]][snake_buffer[sbuf_tail_index][1]] = 0;
                sbuf_tail_index = (sbuf_tail_index + 1) % max_snake_len;
            }
        }
        usleep(50000);
    }
    clear_screen(pd);
    pixel_on(pd, 0, 0, 0x00ffaa66);
    sleep(1);
}

int main(int argc, char **argv)
{
    printf("Starting graphics...\n");
    struct pdraw pd = setup_graphics(20);
    /*
    for (int i=0; i<15; i++) {
        int ry = rand();
        int rx = rand();
        int iy = ry % pd.y_max;
        int ix = rx % pd.x_max;
        uint16_t y = (uint16_t) iy;
        uint16_t x = (uint16_t) ix; 
        printf("%d, %d : %d, %d : %x, %x\n", ry, rx, iy, ix, y, x);
    }
    sleep(5);
    */
    while (1) {
        snake_round(pd);
        getchar_ifany(); // clear keypresses
        char key = getchar();
        if (key=='q') {
            break;
        }
    }
    sleep(2);
    return 0;
}

