#ifndef GAME_MAIN
#define GAME_MAIN

#include "util.h"
#include "renderer.h"
#include <cmath>
#include <cstring>

#define BLOCK_SIZE 64

    u8 tmp[6][8] =  {
        {1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1},
    };

namespace game {
    struct world { // world and whatnot
        u8 map[6][8];
    };

    typedef struct state {
        world world;
    }state;


    f64 raycast(f32 x , f32 y, f32 angle, f32 max_depth) {
        f32 sin_a = sin(angle);
        f32 cos_a = cos(angle);

        for (i32 i = 1; i<max_depth; i++) {
            f32 target_x = x + i * cos_a;
            f32 target_y = y + i * sin_a;

            i32 grid_x = (i32)target_x / BLOCK_SIZE;
            i32 grid_y = (i32)target_y / BLOCK_SIZE;

            if (
                grid_x < 0 || grid_x >= 8 ||
                grid_y < 0 || grid_y >= 6
            ) return max_depth;

            if (tmp[grid_y][grid_x] == 1) {
                return i;
            }
        }

        return max_depth;
    }

    class game {
        private:
            keyboard_input input;
            window_manager window;

        public:
            game(): window(800, 600, "Doom") {}
            void run() {
                window.clear_pipeline();

                f_vec_2D keys = input.poll_axis();

                

                window.render();
            }

        private:


    };


}

#endif
