#include<SDL2/SDL.h>
#include<iostream>

#include"snake.hpp"

#define WIDTH 500
#define HEIGHT 500

#define W_SQUARES 25
#define H_SQUARES 25

#define UNIVERSAL_W 5*WIDTH/(6*W_SQUARES-1)


#define FPS 30

bool running, fullscreen;

int frameCount, timerFPS, lastFrame, fps;
int lastTime;

int calculateFps();
void update(SDL_Window*window);
void input(SnakeGame::Snake &snake, SnakeGame::Direction &direction);
void draw(SDL_Renderer * renderer, SDL_Window *window, SnakeGame::Snake * snake, SnakeGame::Food*food);
void drawSnakes(SDL_Renderer * renderer, int step, SnakeGame::Snake * snake, int i);
void drawFood(SDL_Renderer * renderer, int step, SnakeGame::Food*food);

int main () {
    running = 1;
    fullscreen = 0;
    lastTime = 0;
    int drawTimer = 0;
    int snakeTimer = 0, lastSnakeTimer = 0;
    int first = 1;

    
    SDL_Renderer * renderer;
    SDL_Window *window;


    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed to initialize sdl2" << std::endl;
    // to let it resizable SDL_WINDOW_RESIZABLE ord
    if(SDL_CreateWindowAndRenderer(WIDTH, HEIGHT,  SDL_RENDERER_ACCELERATED, &window, &renderer) < 0) std::cout << "Failed to create window and renderer" << std::endl;

    SDL_SetWindowTitle(window, "my little window");
    SDL_ShowCursor(SDL_TRUE);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    
    // draw_background(renderer, window);
    // draw_rec_background(renderer);

    SnakeGame::Snake* snake =  new SnakeGame::Snake(10, 10, W_SQUARES, H_SQUARES, true);    
    SnakeGame::Direction direction = snake->getSnakeDir();
    SnakeGame::Food *food = new SnakeGame::Food(2, 2);

    while(running) {
        snakeTimer++;
        int fps = calculateFps();
        std::cout << fps << std::endl;

        // draw(renderer, window);
        input(*snake, direction);
        update(window);

        if (snakeTimer - lastSnakeTimer >= fps/6 && snakeTimer > FPS + 5 && snake->state!=SnakeGame::DEAD)
        {
            
            snake->setSnakeDirection(direction);
            snake->moveAllSnakes();
            snake->checkCollision(food);
            lastSnakeTimer = snakeTimer;
            if (snake->state==SnakeGame::DEAD)snake->changeAllStates();
        }
        draw(renderer, window, snake, food);
       
    }

    // closing all
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

int calculateFps() {
    lastFrame = SDL_GetTicks();
        if (lastFrame >= lastTime+1000) {
            // a second has passed
            lastTime=lastFrame;
            fps=frameCount;
            frameCount = 0;
        }

        frameCount++;
        int timerFPS = SDL_GetTicks() - lastFrame;
        if(timerFPS<1000/FPS) {
            SDL_Delay(1000/FPS - timerFPS);
        }
        return fps;
}

void update(SDL_Window*window) {
    if(fullscreen) SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    if(!fullscreen) SDL_SetWindowFullscreen(window, 0);
}

void input(SnakeGame::Snake &snake, SnakeGame::Direction &direction) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
       if (e.type == SDL_QUIT) running= false;

       const Uint8*keystates = SDL_GetKeyboardState(NULL);
       if (keystates[SDL_SCANCODE_ESCAPE]) running =false;
       
    //    if(keystates[SDL_SCANCODE_F11]) fullscreen = !fullscreen;
        if (keystates[SDL_SCANCODE_UP] && snake.getSnakeDir() != SnakeGame::DOWN) direction = SnakeGame::UP;
        else if (keystates[SDL_SCANCODE_RIGHT]&& snake.getSnakeDir() != SnakeGame::LEFT) direction = SnakeGame::RIGHT;
        else if (keystates[SDL_SCANCODE_LEFT]&& snake.getSnakeDir() != SnakeGame::RIGHT) direction = SnakeGame::LEFT;
        else if (keystates[SDL_SCANCODE_DOWN]&& snake.getSnakeDir() != SnakeGame::UP) direction = SnakeGame::DOWN;
        else if (keystates[SDL_SCANCODE_SPACE]) snake.addSnake();

        if (e.type == SDL_KEYDOWN)
        std::cout << SDL_GetKeyName(e.key.keysym.sym) << std::endl ;

    }
}



void draw(SDL_Renderer*renderer, SDL_Window*window, SnakeGame::Snake *snake, SnakeGame::Food*food) {

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 0);

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = WIDTH;
    rect.h = HEIGHT;
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
 
    // SDL_Rect rect;

    int step = (int)(6*UNIVERSAL_W)/5 ;

    // rect.x = rect.y = step;
    rect.w = rect.h = (int)UNIVERSAL_W;

    for ( int i = 0; i <= W_SQUARES; i++) {
        
        for (int j = 0 ; j <= H_SQUARES; j++ ){
            rect.x = step*j + UNIVERSAL_W/5; 
            rect.y = step*i + UNIVERSAL_W/5;
        
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
    drawSnakes(renderer, step, snake, 0);

    drawFood(renderer, step, food);

    SDL_RenderPresent(renderer);
}

void drawFood(SDL_Renderer * renderer, int step, SnakeGame::Food*food) {
    SDL_SetRenderDrawColor(renderer, 230, 0, 50, 255);
    SDL_Rect food_r;
    food_r.w = food_r.h = (int)UNIVERSAL_W;
    food_r.x = food->x * step + UNIVERSAL_W/5;
    food_r.y = food->y * step + UNIVERSAL_W/5;
    SDL_RenderFillRect(renderer, &food_r);
}


void drawSnakes(SDL_Renderer * renderer, int step, SnakeGame::Snake * snake, int i) {
        if (i > 100) i = 100;
    
        if(snake->state==SnakeGame::ALIVE)
            SDL_SetRenderDrawColor(renderer, 0 + i*0.7, 255 - snake->isHead*30 - i*1.5, 0 + snake->isHead*70, 255);
    
        else if (snake->state==SnakeGame::DEAD)
            SDL_SetRenderDrawColor(renderer, 255 - i*0.7, 0, 0 + snake->isHead*70, 255);

        SDL_Rect snake_r;
        snake_r.w = snake_r.h = (int)UNIVERSAL_W;
        snake_r.x = snake->getX() * step + UNIVERSAL_W/5;
        snake_r.y = snake->getY() * step + UNIVERSAL_W/5;
        SDL_RenderFillRect(renderer, &snake_r);

        if (snake->nxtSnake == NULL) return;

        drawSnakes(renderer, step, snake->nxtSnake, ++i);
        // std::cout << " drawing one piece" ;

}


/*
while (SDL_PollEvent(&event)) {
    switch (event.type) {
        // Handle non-input stuff here like SDL_QUIT
        // Since you're using the other API, ignore keyboard events
    }
}

const Uint8* kb = SDL_GetKeyboardState(NULL);

shootDirectionY = -kb[SDL_SCANCODE_UP] + kb[SDL_SCANCODE_DOWN];
shootDirectionX = -kb[SDL_SCANCODE_LEFT] + kb[SDL_SCANCODE_RIGHT]

moveDirectionY = -kb[SDL_SCANCODE_W] + kb[SDL_SCANCODE_S];
moveDirectionX = -kb[SDL_SCANCODE_A] + kb[SDL_SCANCODE_D];
*/



/* 
array of rects , spaced by some pixels to fill the backgound
*/