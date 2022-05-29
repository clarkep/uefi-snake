#include <uefi.h>
#include "pc-uefi.h"

void draw_measures(struct pdraw pd) {
    uint32_t pixel = 0x0000ff00;
    for (int i=0; i<pd.y_max; i+=2) {
        pixel_on(pd, i, pd.x_max-1, (i%8==0) ? pixel+0x00ff00ff : pixel);
    }
}

void place_new_apple(struct pdraw pd, int32_t apple_color, uint8_t **avoid_buffer, uint16_t *apple_y, uint16_t *apple_x)
{
    do {
        *apple_y = rand() % pd.y_max;
        *apple_x = rand() % pd.x_max;
    } while (avoid_buffer[*apple_y][*apple_x]); 
    pixel_on(pd, *apple_y, *apple_x, apple_color);
}

void snake_round(struct pdraw pd)
{   
    // draw_measures(pd);
    uint32_t snake_color = 0x0000ff00;
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
                v_y = -1;
                v_x = 0;
                break;
            case 106:
                v_y = 0;
                v_x = -1;
                break;
            case 107:
                v_y = 1;
                v_x = 0;
                break;
            case 108:
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
            ++sbuf_head_index;
            snake_buffer[sbuf_head_index][0] = head_y;
            snake_buffer[sbuf_head_index][1] = head_x;
            if (head_y == apple_y && head_x == apple_x) {
                place_new_apple(pd, apple_color, world_buffer, &apple_y, &apple_x);
            } else {
                pixel_off(pd, snake_buffer[sbuf_tail_index][0], 
                          snake_buffer[sbuf_tail_index][1]);
                world_buffer[snake_buffer[sbuf_tail_index][0]][snake_buffer[sbuf_tail_index][1]] = 0;
                sbuf_tail_index++;
            }
        }
        usleep(50000);
    }
    pixel_on(pd, 0, 0, 0x00ffaa66);
}

int main(int argc, char **argv)
{
    printf("Starting graphics...\n");
    //sleep(3);
    struct pdraw pd = setup_graphics(15); 
    snake_round(pd);
    sleep(2);
    return 0;
}

