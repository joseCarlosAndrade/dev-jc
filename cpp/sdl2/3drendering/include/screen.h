#ifndef SCREEN_H
#define SCREEN_H

#include<SDL2/SDL.h>
#include<vector>
#include<map>
#include<numeric>

#include"Matrices.hpp" // matrices handling

#include"Object.hpp" // geometric objects

#define WIDTH 460
#define HEIGHT 300
#define RESOLUTION_SCALE 2  

/* TODO:
Different projection modes. Currently theres only orthogonal
Depth Buffer (how will i do this i have no clue)
Better depth illustration, maybe increase the point thickness?
Inser Text Type
Double buffer - while filling one, black the other, and swap them on the iteration end
*/


class Screen {
    private:
        SDL_Event e;
        SDL_Window *window;
        SDL_Renderer * renderer;
        // std::vector<F3DPoint> points; // vector thar stores everypoint to be drawn

        /* Raw 3d points buffer map */
        std::map<int, std::vector<F3DPoint>> point_map;

        /* Object buffer map */
        std::map<int, std::vector<Object*>> object_map;

        int rate;

        std::string window_title;

        int current_buffer;
        int current_obj_buffer;

        /* Pre initalized matrix to speed up geometric transformations */
        MatrixRotationX _rotation_matrix_x;
        MatrixRotationY _rotation_matrix_y;
        MatrixRotationZ _rotation_matrix_z;

        MatrixTranslation _translation_matrix;

        MatrixScale _scale_matrix;

        /* Screen */
        float gray_scale_screen_black[HEIGHT][WIDTH];
        float gray_scale_buffer[HEIGHT][WIDTH];
        

    public:
        Screen() {
            float temp_buffer[HEIGHT][WIDTH] = {0};
            memcpy(gray_scale_screen_black, temp_buffer, HEIGHT*WIDTH*sizeof(float)); // or sizeof gray_scale_screen

            SDL_Init(SDL_INIT_VIDEO);
            SDL_CreateWindowAndRenderer(WIDTH*RESOLUTION_SCALE, HEIGHT*RESOLUTION_SCALE, 0, &window, &renderer);
            SDL_RenderSetScale(renderer, RESOLUTION_SCALE, RESOLUTION_SCALE);
            window_title = "desenho!";

            SDL_SetWindowTitle(window, window_title.c_str());

            // main point list

            current_buffer = 0;

            _rotation_matrix_x.rot(0);
            _rotation_matrix_y.rot(0);
            _rotation_matrix_z.rot(0);

            _translation_matrix.x(0);
            _translation_matrix.y(0);
            _translation_matrix.z(0);

            _scale_matrix.x(1);
            _scale_matrix.y(1);
            _scale_matrix.z(1);
        }
        /* Handles input events. */
        void input() {
            while(SDL_PollEvent(&e)) {

                // handles exit function
                if(e.type == SDL_QUIT) {
                    SDL_Quit();
                    exit(EXIT_SUCCESS);
                }
            }
        }

        /* Actually renders everything.*/
        void show() {
            /* Correlating the coordinate system so that it gets easier to rotate them 
            
            ""
                          ^y
                        (0,0) ->x
            
                                           ""
            
            to:

            ""
                (0,0)->x
                  V y

                                            ""
            
            X = X'+WIDTH/2
            Y = -Y'+HEIGHT/2
            */
            // x = x + WIDTH/2;
            // y = -y + HEIGHT/2;

            SDL_SetRenderDrawColor(renderer, 0, 0 ,0, 255); // draw color: black, to clean the screen
            SDL_RenderClear(renderer);
            // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white, drawing pixels
            
            // draws for each raw buffer
            // for ( auto& cpoints : point_map) {

            //     for (auto &point : cpoints.second) {
            //         if(point.x < -WIDTH/2 || point.x > WIDTH/2 || point.y < -HEIGHT/2 || point.y > HEIGHT/2) continue; 
                   
            //         int depth = (uint)255-(point.z);
            //         if (depth < 0) depth = 0;

            //         else if(depth >255) depth = 255;

            //         SDL_SetRenderDrawColor(renderer, depth, depth, depth, 255);
            //         // std::cout<< (uint)255-point.z << " " ;
            //         float xf = point.x+WIDTH/2;
            //         float yf = -point.y+HEIGHT/2;
            //         SDL_RenderDrawPointF(renderer, xf, yf);

            //         float r = (depth*2)/255; // maximum radius is 4 (closest to the screen)

            //         if ( depth > 50) { // less than 50 it doesnt enter, for optimization
            //             for ( float a = 0; a < 3*3.1415; a += 3.1415/4) { // increases with a 45° step

            //                 SDL_RenderDrawPointF(renderer, xf+std::cos(a)*r, yf+std::sin(a)*r);
            //             }
            //         }
            //     } 
                

            // }

            // draws for each obj buffer
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (auto & obj_buffer : object_map) {
                for ( auto objects : obj_buffer.second) { // objects is pointer to Object
                    for ( auto point : objects->getPoints()) {

                        // if (point.z >= 130) continue;

                        // int depth = (uint)(point.z);
                        // if (depth < 100) depth = 100;

                        // else if(depth >255) depth = 255;
                        int depth = point.z + 40;

                        if (depth < 120) depth = 120;
                        else if (depth > 255) depth = 255;

                        SDL_SetRenderDrawColor(renderer, depth, depth, depth, 255);

                        // screen inversion 
                        float d = 300; // distance from the camera to the screen
                        float D = 200; // distance from the screen to the world origin
                        float factor = (1+ (D-point.z)/d);
                        float xf = point.x/factor;
                        float yf = point.y/factor;

                        if(xf < -WIDTH/2 || xf > WIDTH/2 || yf < -HEIGHT/2 || yf > HEIGHT/2 || point.z >= D+d) continue; 
                        
                        xf = xf+WIDTH/2;
                        yf = -yf+HEIGHT/2;
                        SDL_RenderDrawPointF(renderer, xf, yf);

                        // (didnt really like the result) attempt of better illustrating depth
                        // float r = (depth*1)/255; // maximum radius is 4 (closest to the screen)

                        // if ( depth > 50) { // less than 50 it doesnt enter, for optimization
                        //     for ( float a = 0; a < 3*3.1415; a += 3.1415/4) { // increases with a 45° step

                        //         SDL_RenderDrawPointF(renderer, xf+std::cos(a)*r, yf+std::sin(a)*r);
                        //     }
                        // }
                    }
                }
            }

            SDL_RenderPresent(renderer); // actually draws
            memcpy(gray_scale_buffer, gray_scale_screen_black, HEIGHT*WIDTH*sizeof(float)); // resets the screen
        }

        /* Appends to the point list the pixel coordinates (on the range of WIDTH and HEIGHT).*/
        void pixel(float x, float y, float z=0, int buffer=0) {
            

            F3DPoint point = {x,y,z};
            point_map[buffer].emplace_back(point);

        }

        /* Erase points list. */
        void clear_points() {
            for (auto& points : point_map) {
                points.second.clear();
            }
            
        }

        /* Centralizes the object (using its center as reference) and pushes its center coordinate so it can be translated back
        to the original position again. */
        void centralize(Object &obj) {
            F3DPoint c = obj.getCenter();
            // std::cout << "translating to center: " << obj.getCenter().x << " " << -c.y << " " << -c.z << std::endl;
         
            obj.pushPosition(c);
            translate_points(-c.x, -c.y, -c.z, obj);

        }

        /* Pops the last position pushed on position stack so it can be translated to the original position. Its useful
        to rotate and scale objects that are not in the world origin:
        
        centralize(obj);
        '' rotate, scale, do whatever you want''
        back_to_last_position(obj);
         */
        void back_to_last_position(Object &obj) {
            F3DPoint c = obj.popPosition();

            translate_points(c.x, c.y, c.z, obj);
        }


        void rotate_points_x(float angle, int buffer =-1) {
            
            _rotation_matrix_x.rot(angle);

            if (buffer < 0) {
                for ( auto & buffer : object_map) {

                    for (auto obj : buffer.second) {
                        rotate_points_x(angle, *obj);
                    } 
                }
            }

            else {
                for (auto obj : object_map[buffer]) {
                        rotate_points_x(angle, *obj);
                    }     
            }
        }

        void rotate_points_x(float angle, Object &obj) {
            
            _rotation_matrix_x.rot(angle);

            for (auto &point : obj.getPoints() ) {
                GenMatrix4x4f pt(point.x, point.y, point.z);
                pt = _rotation_matrix_x * pt;
                point.x = pt(0, 0);
                point.y = pt(1, 0);
                point.z = pt(2, 0);

            }   

            F3DPoint c = obj.getCenter();
            GenMatrix4x4f pt(c.x, c.y, c.z); 
            pt = _rotation_matrix_x * pt;

            obj.setCenter(pt(0, 0), pt(1, 0), pt(2, 0));
        }

        void rotate_points_y(float angle, int buffer=-1) {

            _rotation_matrix_y.rot(angle);

            if (buffer < 0) {
                for ( auto & buffer : object_map) {

                    for (auto obj : buffer.second) {
                        rotate_points_y(angle, *obj);

                    } 
                }
            }

            else {
                for (auto obj : object_map[buffer]) {
                        rotate_points_y(angle, *obj);  
                    }     
            }
        }

        void rotate_points_y(float angle, Object &obj) {
            
            _rotation_matrix_y.rot(angle);

            for (auto &point : obj.getPoints() ) {
                GenMatrix4x4f pt(point.x, point.y, point.z);
                pt = _rotation_matrix_y * pt;
                point.x = pt(0, 0);
                point.y = pt(1, 0);
                point.z = pt(2, 0);

            }    

            F3DPoint c = obj.getCenter();
            GenMatrix4x4f pt(c.x, c.y, c.z); 
            pt = _rotation_matrix_y * pt;

            obj.setCenter(pt(0, 0), pt(1, 0), pt(2, 0));

        }

        void rotate_points_z(float angle, int buffer=-1) {

            _rotation_matrix_z.rot(angle);

            if (buffer < 0) {
                for ( auto & buffer : object_map) {

                    for (auto obj : buffer.second) {
                        rotate_points_z(angle, *obj);
                    } 
                }
            }

            else {
                for (auto obj : object_map[buffer]) {
                        rotate_points_z(angle, *obj); 
                    }     
            }
        }

        void rotate_points_z(float angle, Object &obj) {
            
            _rotation_matrix_z.rot(angle);

            for (auto &point : obj.getPoints() ) {
                GenMatrix4x4f pt(point.x, point.y, point.z);
                pt = _rotation_matrix_z * pt;
                point.x = pt(0, 0);
                point.y = pt(1, 0);
                point.z = pt(2, 0);

            }    

            F3DPoint c = obj.getCenter();
            GenMatrix4x4f pt(c.x, c.y, c.z); 
            pt = _rotation_matrix_z * pt;

            obj.setCenter(pt(0, 0), pt(1, 0), pt(2, 0));

        }

        // void rotate_objects(float ang_x, float ang_y, float ang_z) {

        // }

        void translate_points(float tx, float ty, float tz, int buffer= -1) {
            /*
            1 0 tx
            0 1 ty
            0 0 1
            */

            
            if (buffer < 0) {
                for ( auto & buffer : object_map) {

                    for (auto obj : buffer.second) {
                        translate_points(tx, ty, tz, *obj);
                    } 
                }
            }

            else {
                for (auto obj : object_map[buffer]) {
                        translate_points(tx, ty, tz, *obj); 
                    }     
            }
        }

        void translate_points(float tx, float ty, float tz, Object & obj) {

            _translation_matrix.x(tx);
            _translation_matrix.y(ty);
            _translation_matrix.z(tz);
                 
            for (auto &point : obj.getPoints()) {
                GenMatrix4x4f pt(point.x, point.y, point.z);

                pt = _translation_matrix * pt;
                point.x = pt(0, 0);
                point.y = pt(1, 0);
                point.z = pt(2, 0);

            }        
                  
            F3DPoint c = obj.getCenter();
            GenMatrix4x4f pt(c.x, c.y, c.z); 
            pt = _translation_matrix * pt;
            // c.x = pt(0, 0);
            // c.y = pt(1, 0);
            // c.z = pt(2, 0);
            // std::cout << c.x << "translating the center " << c.y << " " << c.z << std::endl;
            obj.setCenter(pt(0, 0), pt(1, 0), pt(2, 0));
            
        }

        void sleep_rate(int r = -1) {
            if (r > 0) SDL_Delay(1000/r);

            else SDL_Delay(1000);
        }


        void circle( int xo , int yo , int radius, int buffer) {
            
 
            for ( float i = 0; i < 2*3.14159; i+=3.14159/360) {
                pixel(xo + radius*std::cos(i), yo + radius*std::sin(i), 0, buffer);
            }
        }


        void torus (int buffer =0) {
            int t_radius = 100;
            int c_radius = 50;

            for ( float phi = 0 ; phi < 2*3.1415; phi+=3.1415/50) {
                MatrixRotationY rotation_y(phi);
                
                for ( float i = 0; i < 2*3.14159; i+=3.14159/100) {
                    
                    // creating first circle
                    float xf =t_radius + c_radius*std::cos(i);
                    float yf = c_radius*std::sin(i);
                    float zf = 0;

                    // rotating it arround y axis
                    GenMatrix4x4f p(xf, yf,zf);
                    p = rotation_y * p;

                    pixel(p(0,0),  p(1,0), p(2,0), buffer);
                }
            }
            

        }


        /* Adds the object to the scene by referece and stores it on the specified buffer.
        If no value is passed to the buffer, it will be stored on location 0 by default. 
        This means that every modification on the object outside will still take place inside the scene. */
        bool addObjectToScene(Object* obj, int buffer= 0) {
            if (obj == nullptr) return false;

            object_map[buffer].emplace_back(obj);
            return true;
        }
};

#endif




/*
OLD
void rotate_points_z(float angle, int buffer=-1) {

            _rotation_matrix_z.rot(angle);

            if (buffer < 0) {
                for ( auto & points : point_map) {

                    for (auto &point : points.second ) {
                        GenMatrix4x4f pt(point.x, point.y, point.z);
                        pt = _rotation_matrix_z * pt;
                        point.x = pt(0, 0);
                        point.y = pt(1, 0);
                        point.z = pt(2, 0);

                    }    
                }
            }

            else {
                for (auto &point : point_map[buffer] ) {
                    GenMatrix4x4f pt(point.x, point.y, point.z);
                    pt = _rotation_matrix_z * pt;
                    point.x = pt(0, 0);
                    point.y = pt(1, 0);
                    point.z = pt(2, 0);

                }    
            }
        }


               //Get the line properties from 2 points (start and end)
        void line(float x1, float y1, float z1, float x2, float y2, float z2) {
            float dx = x2 - x1;
            float dy = y2 - y1;
            float dz = z2 - z1;

            float length = std::sqrt(dx*dx + dy*dy + dz*dz);

            float vector[3] = {(x2-x1)/length, (y2-y1)/length, (z2-z1)/length };

            for (float i = 0; i < length ; i++) {
                pixel(x1 + vector[0]*i, y1 + vector[1]*i, z1 + vector[2]*i, current_buffer);
            }

        }

*/