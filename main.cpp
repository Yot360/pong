#include <iostream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SoundEffects.h"
#include <math.h>

#include "stb_image/SDL_stbimage.h" //sdl_image but better lol

int HEIGHT = 720;
int WIDTH = 720;
int FONT_SIZE = 32;
int SIZE = 16;
int BALL_SPEED = 16;
int SPEED = 9;
double PI = 3.14159265358979323846;

bool key_up = false;
bool key_down = false;

SDL_Renderer *renderer;
SDL_Window *window;
TTF_Font *font;
SDL_Color color;

bool running;
int frameCount, timerFPS, lastFrame, FPS;

SDL_Rect l_paddle, r_paddle, ball, score_board;
float velX, velY;
std::string score;
int l_s, r_s;
bool turn;

SoundEffects se;

//doesn't work
int showmenu(SDL_Surface* screen, TTF_Font* font)
{
    Uint32 time;
    int x, y;
    const int NUMMENU = 2;
    const char* labels[NUMMENU] = {"Continue","Exit"};
    SDL_Surface* menus[NUMMENU];
    bool selected[NUMMENU] = {0,0};
    SDL_Color color[2] = {{255,255,255},{255,0,0}};

    menus[0] = TTF_RenderText_Solid(font,labels[0],color[0]);
    menus[1] = TTF_RenderText_Solid(font,labels[1],color[0]);
    SDL_Rect pos[NUMMENU];
    pos[0].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[0].y = screen->clip_rect.h/2 - menus[0]->clip_rect.h;
    pos[1].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
    pos[1].y = screen->clip_rect.h/2 + menus[0]->clip_rect.h;

    SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0x00,0x00,0x00));

    SDL_Event event;
    while(1)
    {
        time = SDL_GetTicks();
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    SDL_FreeSurface(menus[0]);
                    SDL_FreeSurface(menus[1]);
                    return 1;

                case SDL_MOUSEMOTION:
                    x = event.motion.x;
                    y = event.motion.y;
                    for(int i = 0; i < NUMMENU; i += 1)
                    {

                        if(x >= pos[i].x && x <= pos[i].x + pos[i].w &&
                           y >= pos[i].y && y <= pos[i].y + pos[i].h)
                        {
                            if(!selected[i])
                            {
                                selected[i] = 1;
                                SDL_FreeSurface(menus[i]);
                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[1]);
                            }
                        }

                        else
                        {
                            if(selected[i])
                            {
                                selected[i] = 0;
                                SDL_FreeSurface(menus[i]);
                                menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                            }
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    x = event.button.x;
                    y = event.button.y;
                    for(int i = 0; i < NUMMENU; i += 1)
                    {
                        if(x >= pos[i].x && x <= pos[i].x + pos[i].w &&
                           y >= pos[i].y && y <= pos[i].y + pos[i].h)
                        {
                            SDL_FreeSurface(menus[0]);
                            SDL_FreeSurface(menus[1]);
                            return i;
                        }
                    }
                    break;

                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        SDL_FreeSurface(menus[0]);
                        SDL_FreeSurface(menus[1]);
                        return 0;
                    }
            }
        }

        for(int i = 0; i < NUMMENU; i += 1)
        {
            SDL_BlitSurface(menus[i],NULL,screen,&pos[i]);
        }

    }
}

void serve(){
    l_paddle.y = r_paddle.y = (HEIGHT/2) - (l_paddle.h/2);
    if (turn){
        ball.x = l_paddle.x + (l_paddle.w * 4);
        velX = BALL_SPEED/2;
    }else {
        ball.x = r_paddle.x - (r_paddle.w * 4);
        velX = -BALL_SPEED/2;
    }


    velY = 0;
    ball.y = HEIGHT/2 - (SIZE/2);
    turn=!turn;
}
void write(std::string text, int x, int y){
    SDL_Surface *surface;
    SDL_Texture *texture;
    const char *t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    score_board.w = surface->w;
    score_board.h = surface->h;
    score_board.x = x - score_board.w;
    score_board.y = y - score_board.h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &score_board);
    SDL_DestroyTexture(texture);
}

void input(){
    SDL_Event e;
    while (SDL_PollEvent(&e)){
        if (e.type == SDL_QUIT){
            running = false;
        }

        //check if key is pressed
        if (e.type == SDL_KEYDOWN){
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    key_down = true;
                    key_up = false;
                    break;
                case SDLK_DOWN:
                    key_up = true;
                    key_down = false;
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
            }
        }

        //check if key is released
        if (e.type == SDL_KEYUP)
        {
            key_down = false;
            key_up = false;
        }
    }
}

void update() {

    if(SDL_HasIntersection(&ball, &r_paddle)) {
        se.playSoundEffect(0);
        double rel = (r_paddle.y + (r_paddle.h/2)) - (ball.y + (SIZE/2));
        double norm = rel/(r_paddle.h/2);
        double bounce = norm * (5*PI/12);
        velX = -BALL_SPEED*cos(bounce);
        velY = BALL_SPEED*-sin(bounce);
    }
    if(SDL_HasIntersection(&ball, &l_paddle)) {
        se.playSoundEffect(0);
        double rel = (l_paddle.y + (l_paddle.h/2)) - (ball.y+(SIZE/2));
        double norm = rel/(l_paddle.h/2);
        double bounce = norm * (5*PI/12);
        velX= BALL_SPEED*cos(bounce);
        velY= BALL_SPEED*-sin(bounce);
    }
    if(ball.y > r_paddle.y + (r_paddle.h/2)) r_paddle.y += SPEED;
    if(ball.y < r_paddle.y + (r_paddle.h/2)) r_paddle.y -= SPEED;
    if(ball.x<=0) { r_s++; serve(); se.playSoundEffect(1); }
    if(ball.x + SIZE >= WIDTH) { l_s++; serve(); se.playSoundEffect(1); }
    if(ball.y <= 0 || ball.y+SIZE >= HEIGHT) velY =- velY;
    ball.x += velX;
    ball.y += velY;
    score=std::to_string(l_s) + "   " + std::to_string(r_s);
    if(l_paddle.y<0) l_paddle.y = 0;
    if(l_paddle.y + l_paddle.h > HEIGHT) l_paddle.y = HEIGHT - l_paddle.h;
    if(r_paddle.y<0) r_paddle.y = 0;
    if(r_paddle.y+r_paddle.h>HEIGHT) r_paddle.y=HEIGHT - r_paddle.h;
}

void render(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS<(1000/60)){
        SDL_Delay((1000/60) - timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &r_paddle);
    SDL_RenderFillRect(renderer, &ball);
    write(score, WIDTH/2 + FONT_SIZE, FONT_SIZE * 2);

    SDL_RenderPresent(renderer);

}

int main(int, char **) {
    std::cout << "Pong started!" << std::endl;

    //add sound
    se.addSoundEffect("assets/pong_hit.mp3");
    se.addSoundEffect("assets/pong_wall.mp3");
    std::cout << "Added sound effects!" << std::endl;

    //debug
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        std::cout << "Failed to init" << std::endl;
    }
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0){
        std::cout << "Failed to create window" << std::endl;
    }

    //load fonts
    TTF_Init();
    font = TTF_OpenFont("assets/Peepo.ttf", FONT_SIZE);

    static int lastTime = 0;
    color.r = color.g = color.b = color.a = 255;

    l_s = r_s = 0;
    l_paddle.x = 32; l_paddle.h = HEIGHT/4;
    l_paddle.y = (HEIGHT/2) - (l_paddle.h/2);
    l_paddle.w = 12;
    r_paddle = l_paddle;
    r_paddle.x = WIDTH - r_paddle.w - 32;
    ball.w = ball.h = SIZE;

    serve();

    running = true;
    while (running){
        lastFrame = SDL_GetTicks();
        if (lastFrame>=(lastTime + 1000)){
            lastTime = lastFrame;
            FPS = frameCount;
            frameCount = 0;
        }
        if (key_up)
            l_paddle.y += SPEED;
        else if (key_down)
            l_paddle.y -= SPEED;
        update();
        input();
        render();
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
