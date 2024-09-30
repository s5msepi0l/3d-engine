#ifndef GAME_MAIN
#define GAME_MAIN

#include "util.h"
#include "renderer.h"
#include <cmath>
#include <cstring>


#define WIDTH 1280
#define HEIGHT 720

#define BLOCK_SIZE 64
#define ROTATION_SPEED 0.05
#define SPEED 1
#define NUM_RAYS 320
#define FOV  90 * M_PI/180
#define STEP_ANGLE FOV / NUM_RAYS

#define min(a, b) ((a) < (b) ? (a) : (b))

// point 2 dest, essentially just f_vec2 but unsinged int
typedef struct{ i32 x; i32 y; }p2d;

typedef struct{
    p2d a, b;
        
    /* textures, actions and other misc stuff... */
}wall;

typedef struct map{
    map() {

    }
    
    map(size_t s): n(s) {
        walls = new wall[n];
    }

    void set_n(int s) {
        n = s;
        walls = new wall[n];
    }

    ~map() {
        delete[] walls;
    }

    size_t n;
    wall *walls;


}map;

namespace game {
    typedef struct {
        u16 fov;

        f_vec2 pos{
            40,
            40
        };

        struct angle {
            f32 x = 0;
        }angle;
    }player;

    /* https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
        Given two points on each line segment

    * Half this shit goes over my head but it does (somewhat) explain the
    * geometry wizard fuckery
    * 
    * Calculate intersection between two line segments(wall & raycast origin and dest) 
    * return {NAN, NAN} if none is found */
    f_vec2 ray_intersects_wall(f_vec2 ro, f32 r_angle, p2d ws, p2d we) {
        //ray directions
        f32 dx = ro.x + cos(r_angle);
        f32 dy = ro.y + sin(r_angle);

        //denominator ?
        f32 d = (ro.x - dx) * (ws.y - we.y) - (ro.y - dy) * (ws.x - we.x);
        std::cout << "demonimator = " << d << std::endl;
        if (fabs(d) < 1e-6) {
            std::cout << "NONE\n";
            return {NAN, NAN};
        }
        
        f32 t = ((ro.x - ws.x) * (ws.y - we.y) - (ro.y - ws.y) * (ws.x - we.x)) / d;
        f32 u = ((ro.x - dx) * (ro.y - ws.y) - (ro.y - dy) * (ro.x - ws.x)) / d;
//t >= 0 && 0 <= u <= 1
//t >= 0 && t <= 1 && u >= 0 && u <= 1
        if (t >= 0 && 0 <= u <= 1)
            return {ro.x + t * (dx - ro.x), ro.y + t * (dy - ro.y)};
        //(Px, Py) = ( x1 + t(x2 - x1), y1 + t(y2 - y1))  

        return {NAN, NAN};
    }
    
    inline f32 normalize_angle(f32 a) {
        return a - (2.0 * M_PI * floor((a + M_PI) / (2.0 * M_PI)));
    }

    bool wall_visible(f_vec2 pos, f32 a, u32 fov, p2d ws, p2d we) {
        f32 t1 = atan2(ws.y - pos.y, ws.x - pos.x);
        f32 t2 = atan2(we.y - pos.y, we.x - pos.x);

        f32 dt1 = normalize_angle(t1 - a);
        f32 dt2 = normalize_angle(t2 - a);

        //check if either of the walls edges are within the camera fov
        f32 h_fov = fov / 2;
        if (
            //(-h_fov <= dt1 && dt1 <= h_fov) ||
            //(-h_fov <= dt2 && dt2 <= h_fov)
            
            (-h_fov <= dt1 <= h_fov) ||
            (-h_fov <= dt2 <= h_fov) 
            ) return true; //wall is atleast partially visible
        return false;
    }

    class game {
        private:
            keyboard_input input;
            window_manager window;
            frametime_manager frame;

            bool running = true;
            player Player;
            map world;

        public:
            game(u32 fov, std::string map0): window(WIDTH, HEIGHT, "Doom"), frame(60) {
                Player.fov =  fov * M_PI/180;
                load_map(map0);
            }

            void run() {
                while (running) {
                    //std::cout << "hello world\n";
                    if (input.poll_key(SDL_SCANCODE_ESCAPE)) {
                        return ;
                    }

                    window.clear_pipeline();

                    f_vec2 keys = input.poll_axis();
                    
                    frame.set_start();
                    Player.pos.x += keys.x * ROTATION_SPEED;

                    if (keys.y < 0) {
                        Player.pos.y -= SPEED * sin(Player.angle.x);
                        Player.pos.x -= SPEED * cos(Player.angle.x);
                    } else if (keys.y > 0){
                        Player.pos.y += SPEED * sin(Player.angle.x);
                        Player.pos.x += SPEED * cos(Player.angle.x);
                    }
                    if (keys.x < 0) {
                        Player.angle.x -= ROTATION_SPEED;
                    } else if (keys.x > 0) {
                        Player.angle.x += ROTATION_SPEED;
                    }

                    render();

                    //std::cout << "render\n";

                    frame.set_end();
                }                
            }

            // Implement me !
            void load_map(std::string src) {
                //test world manually entered

                //the map is essentially just a linked list in an array form
                world.set_n(1);
                
                world.walls[0] = wall{
                    {0, 0},
                    {80, 0}
                };
                
            }

        private:
            void render() {
                for (i32 i = 0; i<NUM_RAYS; i++) {
                    f32 ray_angle = normalize_angle(Player.angle.x - Player.fov / 2 + i * STEP_ANGLE);
                    
                    // prolly not too fast but I'm not reading any more goddamn wikipedia today
                    for (i32 seg = 0; seg<world.n; seg++) {
                        if (wall_visible(
                            Player.pos,
                            ray_angle,
                            Player.fov,
                            world.walls[seg].a,
                            world.walls[seg].b
                            )) { //wall is visible
                            //xy coordinate of where the ray hit
                            f_vec2 rayhit = ray_intersects_wall(
                                Player.pos,
                                ray_angle,
                                world.walls[seg].b,
                                world.walls[seg].a);

                            if (std::isnan(rayhit.x) || std::isnan(rayhit.y)) continue;

                            //std::cout << "rayhit.x = " << rayhit.x << '\n'; 
                            //std::cout << "rayhit.y = " << rayhit.y << '\n'; 

                            //std::cout << "a = " << Player.angle.x << '\n';
                            //std::cout << "Player.pos.x = " << Player.pos.x << '\n';
                            //std::cout << "Player.pos.y = " << Player.pos.y << '\n'; 

                            //std::cout << "pos.x = " << seg << '\n'; 
                            // calculate distance between the Player and hit
                            // d=√((x2 – x1)² + (y2 – y1)²)


                            f32 depth = sqrt(pow(rayhit.x - Player.pos.x, 2) + pow(rayhit.y - Player.pos.y, 2));
                            
                            
                            if (!std::isnan(depth)) {
                                    
                                f32 wall_height = min(25000.0 / (depth + 0.0001), HEIGHT);
                                    //f32 wall_height = min(600, 15000.0 / (depth + 0.0001));

                                //std::cout << "window.x = " << (i * (WIDTH / NUM_RAYS)) << '\n'; 
                                //std::cout << "window.y = " << std::floor((HEIGHT - wall_height) / 2) << '\n'; 
                                
                                u32 shade = 255 / (1 + depth * depth * 0.0001);
                                std::cout << "window.y = " << std::floor((HEIGHT - wall_height) / 2) << '\n'; 
                                std::cout << "rayhit.x = " << rayhit.x << '\n'; 
                                std::cout << "rayhit.y = " << rayhit.y << '\n'; 

                                
                                window.rect_insert(
                                    rgb{(u8)shade, (u8)shade, (u8)shade, 255},
                                    f_vec2(
                                        (i * (WIDTH / NUM_RAYS)),
                                        std::floor((HEIGHT - wall_height) / 2)
                                    ),
                        
                                    f_vec2(
                                        WIDTH / NUM_RAYS,
                                        wall_height
                                    )
                                        
                                );

                            }
                        }

                    }

                }
            }

    };


}

#endif
