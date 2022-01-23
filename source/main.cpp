#include <iostream>
#include <random>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "SoundEffects.h"
#include <math.h>
#include <switch.h>
#define SDL_STBIMAGE_IMPLEMENTATION
#include "../stb_image/SDL_stbimage.h" //sdl_image but better lol

// some switch buttons
#define JOY_A     0
#define JOY_B     1
#define JOY_X     2
#define JOY_Y     3
#define JOY_PLUS  10
#define JOY_MINUS 11
#define JOY_LEFT  12
#define JOY_UP    13
#define JOY_RIGHT 14
#define JOY_DOWN  15

#define SCREEN_W 1280
#define SCREEN_H 720

enum GameFlags {
    Menu = 1,
    Gameplay = 2,
};

enum PauseOption {
    Continue = 0,
    Exit = 1,
};

enum MenuOption {
    Start = 0,
    Quit = 1,
    Credit = 2,
};

MenuOption menuOption;
PauseOption pauseOption;
GameFlags flag = GameFlags::Menu;

int HEIGHT = SCREEN_H;
int WIDTH = SCREEN_W;
int FONT_SIZE = 32;
int SIZE = 16;
int BALL_SPEED = 19;
int SPEED = 11;
int PLAYER_SPEED = 11;
double PI = 3.14159265358979323846;

bool key_up = false;
bool key_down = false;

SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font = TTF_OpenFont("data/Peepo.ttf", FONT_SIZE / 2);
SDL_Color color;
SDL_Color Rcolor;

bool running;
bool allRunning;
bool mRunning;
int frameCount, timerFPS, lastFrame, FPS;
int selR;
bool isPaused = false;
bool isCredit = false;

SDL_Rect l_paddle, r_paddle, ball, score_board;
int sizeMultiplier = 4;
float velX, velY;
std::string score;
int l_s, r_s;
bool turn;
SoundEffects se;

void serve() {
    l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    if (turn) {
        ball.x = l_paddle.x + (l_paddle.w * 4);
        velX = BALL_SPEED / 2;
    }
    else {
        ball.x = r_paddle.x - (r_paddle.w * 4);
        velX = -BALL_SPEED / 2;
    }


    velY = 0;
    ball.y = HEIGHT / 2 - (SIZE / 2);
    turn = !turn;
}

void writeText(const char* text, int x, int y, int w, int h) {
    SDL_Color White = { 255, 255, 255 };
    //text
    SDL_Surface* stringTxt =
        TTF_RenderText_Solid(font, text, White);
    SDL_Texture* stringTx = SDL_CreateTextureFromSurface(renderer, stringTxt);
    SDL_Rect stringRct; //create a rect
    stringRct.x = x;  //controls the rect's x coordinate
    stringRct.y = y; // controls the rect's y coordinte
    stringRct.w = w; // controls the width of the rect
    stringRct.h = h; // controls the height of the rect
    SDL_RenderCopy(renderer, stringTx, NULL, &stringRct);

    //free memory
    SDL_FreeSurface(stringTxt);
    SDL_DestroyTexture(stringTx);
}

// This is a function that waits for a number to start generating a bonus
bool isSpawned = false;
int timeBonus = 0;
std::string bonusImg = "data/speed.png";
int x_ = WIDTH / 2;
int y_ = HEIGHT / 2 - 20;
void spawnBonus() {

    //runs when no bonus
    if (timeBonus == 0)
    {
        //reset
        PLAYER_SPEED = SPEED = 9;
        sizeMultiplier = 4;
        l_paddle.h = HEIGHT / sizeMultiplier;
        Rcolor.r = Rcolor.g = Rcolor.b = Rcolor.a = 255;
    }
    else { //runs when bonus
       //time left for counter
        std::string txtx = std::to_string((timeBonus / 60) + 1);
        txtx.append("'s left...");
        const char* txxt2 = txtx.c_str();
        writeText(txxt2, WIDTH / 2 + FONT_SIZE - 66, FONT_SIZE * 2, 70, FONT_SIZE + 7);
        std::cout << std::to_string(timeBonus).c_str() << std::endl;
        timeBonus--;
    }

    if (!isSpawned)
    {

        if ((rand() % 400) == 2) {
            std::cout << "Bonus spawning" << std::endl;
            y_ = rand() % HEIGHT - 100 + 35;
            x_ = rand() % WIDTH - 450 + 450;
            switch (rand() % 3)
            {
            case 1:
                std::cout << "Bonus 1" << std::endl;
                bonusImg = "data/speed.png";
                isSpawned = true;
                break;
            case 2:
                std::cout << "Bonus 2" << std::endl;
                bonusImg = "data/size.png";
                isSpawned = true;
                break;
            case 3:
                std::cout << "Bonus 3" << std::endl;
                bonusImg = "data/freeze.png";
                isSpawned = true;
                break;
            }
        }
    }
    else
    {
        //load triangle to texture
        SDL_Surface* bonus = STBIMG_Load(bonusImg.c_str());
        SDL_Texture* bonus_texture = SDL_CreateTextureFromSurface(renderer, bonus);


        SDL_Rect bonus_rect = { x_, y_, 64, 64 };
        SDL_RenderCopy(renderer, bonus_texture, nullptr, &bonus_rect);
        y_ += 1;

        if (y_ > HEIGHT || !running)
        {
            SDL_FreeSurface(bonus);
            SDL_DestroyTexture(bonus_texture);
            isSpawned = false;
            y_ = HEIGHT / 2;
            return;
        }


        if (SDL_HasIntersection(&ball, &bonus_rect)) {
            se.playSoundEffect(0);
            std::cout << "bonus rect touched!" << std::endl;
            std::cout << bonusImg << std::endl;
            if (bonusImg == "data/speed.png")
            {
                timeBonus = 60 * 5;
                PLAYER_SPEED = 15;
            }
            else if (bonusImg == "data/size.png") {
                timeBonus = 60 * 5;
                sizeMultiplier = 3;
                l_paddle.h = HEIGHT / sizeMultiplier;
            }
            else if (bonusImg == "assets/freeze.png") {
                timeBonus = 60 * 5;
                SPEED -= 6;
                Rcolor.r = 3;
                Rcolor.g = 138;
                Rcolor.b = 255;
                Rcolor.a = 1;
            }
            isSpawned = false;
        }
        SDL_FreeSurface(bonus);
        SDL_DestroyTexture(bonus_texture);
    }
}

void write(std::string text, int x, int y) {
    SDL_Surface* surface;
    SDL_Texture* texture;
    const char* t = text.c_str();
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

void input() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            allRunning = false;
            mRunning = false;
            running = false;
        }

        //check if key is pressed
        if (e.type == SDL_JOYBUTTONDOWN) {
            switch (e.jbutton.button) {
            case JOY_UP:
                if (!isPaused) {
                    key_down = true;
                    key_up = false;
                }
                else {
                    selR = 1;
                    if (pauseOption != PauseOption::Continue) se.playSoundEffect(0);
                    pauseOption = PauseOption::Continue;
                }
                break;
            case JOY_DOWN:
                if (!isPaused) {
                    key_up = true;
                    key_down = false;
                }
                else {
                    selR = 2;
                    if (pauseOption != PauseOption::Exit) se.playSoundEffect(0);
                    pauseOption = PauseOption::Exit;
                }
                break;
            case JOY_PLUS:
                pauseOption == PauseOption::Continue;
                std::cout << pauseOption << std::endl;
                selR = 1;
                isPaused = !isPaused;
                break;
            case JOY_A:
                if (pauseOption == PauseOption::Exit) {
                    flag = GameFlags::Menu;
                    //reset scores
                    l_s = r_s = 0;
                    mRunning = true;
                    running = false;
                    isPaused = false;
                    selR = 1;
                }
                else if (pauseOption == PauseOption::Continue) {
                    isPaused = false;
                }
                break;
            }
        }

        //check if key is released
        if (e.type == SDL_JOYBUTTONUP)
        {
            key_down = false;
            key_up = false;
        }
    }
}

void inputMenu() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            allRunning = false;
            mRunning = false;
            running = false;
        }

        //check if key is pressed
        if (e.type == SDL_JOYBUTTONDOWN) {
            switch (e.jbutton.button) {
            case JOY_UP:
                if (!isCredit)
                {
                    if (menuOption == MenuOption::Credit)
                    {
                        selR = 2;
                        se.playSoundEffect(0);
                        menuOption = MenuOption::Quit;
                    }
                    else if (menuOption == MenuOption::Quit) {
                        selR = 1;
                        se.playSoundEffect(0);
                        menuOption = MenuOption::Start;
                    }
                }
                
                break;
            case JOY_DOWN:
                if (!isCredit)
                {
                    if (menuOption == MenuOption::Start)
                    {
                        selR = 2;
                        se.playSoundEffect(0);
                        menuOption = MenuOption::Quit;
                    }
                    else if (menuOption == MenuOption::Quit) {
                        selR = 3;
                        se.playSoundEffect(0);
                        menuOption = MenuOption::Credit;
                    }
                }
                
                break;
            case JOY_A:
                if (menuOption == MenuOption::Quit) {
                    mRunning = false;
                    allRunning = false;
                }
                else if (menuOption == MenuOption::Start) {
                    flag = GameFlags::Gameplay;
                    running = true;
                    mRunning = false;
                }
                else if (menuOption == MenuOption::Credit) {
                    isCredit = true;
                }
                break;
            case JOY_B:
                isCredit = false;
                break;
            }
        }

        //check if key is released
        if (e.type == SDL_JOYBUTTONUP)
        {
            key_down = false;
            key_up = false;
        }
    }
}

void update() {

    if (SDL_HasIntersection(&ball, &r_paddle)) {
        se.playSoundEffect(0);
        double rel = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (SIZE / 2));
        double norm = rel / (r_paddle.h / 2);
        double bounce = norm * (5 * PI / 12);
        velX = -BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);
    }
    if (SDL_HasIntersection(&ball, &l_paddle)) {
        se.playSoundEffect(0);
        double rel = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (SIZE / 2));
        double norm = rel / (l_paddle.h / 2);
        double bounce = norm * (5 * PI / 12);
        velX = BALL_SPEED * cos(bounce);
        velY = BALL_SPEED * -sin(bounce);
    }
    if (ball.y > r_paddle.y + (r_paddle.h / 2)) r_paddle.y += SPEED;
    if (ball.y < r_paddle.y + (r_paddle.h / 2)) r_paddle.y -= SPEED;
    if (ball.x <= 0) { r_s++; serve(); se.playSoundEffect(1); }
    if (ball.x + SIZE >= WIDTH) { l_s++; serve(); se.playSoundEffect(1); }
    if (ball.y <= 0 || ball.y + SIZE >= HEIGHT) velY = -velY;
    ball.x += velX;
    ball.y += velY;
    score = std::to_string(l_s) + "   " + std::to_string(r_s);
    if (l_paddle.y < 0) l_paddle.y = 0;
    if (l_paddle.y + l_paddle.h > HEIGHT) l_paddle.y = HEIGHT - l_paddle.h;
    if (r_paddle.y < 0) r_paddle.y = 0;
    if (r_paddle.y + r_paddle.h > HEIGHT) r_paddle.y = HEIGHT - r_paddle.h;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    SDL_SetRenderDrawColor(renderer, Rcolor.r, Rcolor.g, Rcolor.b, Rcolor.a);
    SDL_RenderFillRect(renderer, &r_paddle);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &l_paddle);
    SDL_RenderFillRect(renderer, &ball);
    write(score, WIDTH / 2 + FONT_SIZE, FONT_SIZE * 2);

    if (isPaused) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        int x, y;

        //Start game option
        writeText("Continue", WIDTH / 2 - 90, HEIGHT / 2 - 50, 170, FONT_SIZE * 2);
        //Quit game option
        writeText("Main Menu", WIDTH / 2 - 90, HEIGHT / 2 + 20, 170, FONT_SIZE * 2);
        

        //load triangle to texture
        SDL_Surface* triangle = STBIMG_Load("data/triangle.png");
        SDL_Texture* image_surface = SDL_CreateTextureFromSurface(renderer, triangle);

        // start rect
        if (selR == 1) {
            x = WIDTH / 2 - 140;
            y = HEIGHT / 2 - 30;
            SDL_Rect image_rect = { x, y, 32, 32 };
            SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
        }
        else if (selR == 2) {
            x = WIDTH / 2 - 140;
            y = HEIGHT / 2 + 40;
            SDL_Rect image_rect = { x, y, 32, 32 };
            SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
        }
        else if (selR == 3) {
            x = WIDTH / 2 - 140;
            y = HEIGHT / 2 + 110;
            SDL_Rect image_rect = { x, y, 32, 32 };
            SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
        }

        //free memory
        SDL_FreeSurface(triangle);
        SDL_DestroyTexture(image_surface);
    }
    else { spawnBonus(); }

    SDL_RenderPresent(renderer);

}

void renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);


    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }

    //Title text
    writeText("Pong", WIDTH / 2 - 100, 50, 200, FONT_SIZE * 3);
    //Start game option
    writeText("Start Game", WIDTH / 2 - 90, HEIGHT / 2 - 50, 170, FONT_SIZE * 2);
    //Quit game option
    writeText("Quit Game", WIDTH / 2 - 90, HEIGHT / 2 + 20, 170, FONT_SIZE * 2);
    //Credit game option
    writeText("Credits", WIDTH / 2 - 90, HEIGHT / 2 + 90, 115, FONT_SIZE * 2);
    //Credit text
    writeText("By Yot", WIDTH / 2 - 75, HEIGHT / 2 + 280, 120, FONT_SIZE + 10);

    int x, y;

    SDL_SetRenderDrawColor(renderer, 100, 0, 255, 0);

    //load triangle to texture
    SDL_Surface* triangle = STBIMG_Load("data/triangle.png");
    SDL_Texture* image_surface = SDL_CreateTextureFromSurface(renderer, triangle);

    // start rect
    if (selR == 1) {
        x = WIDTH / 2 - 140;
        y = HEIGHT / 2 - 30;

        SDL_Rect image_rect = { x, y, 32, 32 };
        SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
    }
    else if (selR == 2) {
        x = WIDTH / 2 - 140;
        y = HEIGHT / 2 + 40;
        SDL_Rect image_rect = { x, y, 32, 32 };
        SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
    }
    else if (selR == 3) {
        x = WIDTH / 2 - 140;
        y = HEIGHT / 2 + 110;
        SDL_Rect image_rect = { x, y, 32, 32 };
        SDL_RenderCopy(renderer, image_surface, nullptr, &image_rect);
    }
    if (isCredit) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        //title
        writeText("Credits", WIDTH / 2 - 90, 50, 170, FONT_SIZE * 2);
        //dev/owner
        writeText("Developer & Owner", WIDTH / 2 - 120, 200, 240, FONT_SIZE + 25);
        writeText("Yot (me)", WIDTH / 2 - 50, 280, 90, FONT_SIZE);
        //graphics/design
        writeText("Graphics & Some Ideas", WIDTH / 2 - 125, HEIGHT / 2, 250, FONT_SIZE + 25);
        writeText("My friend Thibaud", WIDTH / 2 - 90, HEIGHT / 2 + 80, 170, FONT_SIZE);
        writeText("Yot (me)", WIDTH / 2 - 50, HEIGHT / 2 + 110, 90, FONT_SIZE);
        //esc
        writeText("<-- B", 35, HEIGHT / 2 + 280, 120, FONT_SIZE + 10);
    }

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(triangle);
    SDL_DestroyTexture(image_surface);
}

int main(int, char**) {
    std::cout << flag << std::endl;
    std::cout << "Pong started!" << std::endl;

    //load romfs
    romfsInit();
    chdir("romfs:/");

    //load sticks
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_JoystickOpen(0);

    //default to start game
    selR = 1;
    menuOption = MenuOption::Start;

    //add sound
    se.addSoundEffect("data/pong_hit.mp3");
    se.addSoundEffect("data/pong_wall.mp3");
    std::cout << "Added sound effects!" << std::endl;

    //debug
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "Failed to init" << std::endl;
    }
    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) {
        std::cout << "Failed to create window" << std::endl;
    }

    //load fonts
    TTF_Init();
    font = TTF_OpenFont("data/Peepo.ttf", FONT_SIZE);

    static int lastTime = 0;
    Rcolor.r = Rcolor.g = Rcolor.b = Rcolor.a = color.r = color.g = color.b = color.a = 255;

    l_s = r_s = 0;
    l_paddle.x = 32;
    l_paddle.h = HEIGHT / sizeMultiplier;
    l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
    l_paddle.w = 12;
    r_paddle = l_paddle;
    r_paddle.x = WIDTH - r_paddle.w - 32;
    ball.w = ball.h = SIZE;

    allRunning = true;
    while (allRunning) {
        //start main menu
        if (flag & GameFlags::Menu) {
            std::cout << "Menu" << std::endl;
            mRunning = true;
            while (mRunning) {
                lastFrame = SDL_GetTicks();
                if (lastFrame >= (lastTime + 1000)) {
                    lastTime = lastFrame;
                    FPS = frameCount;
                    frameCount = 0;
                }
                inputMenu();
                renderMenu();
            }

        }

        //start game
        if (flag & GameFlags::Gameplay) {
            std::cout << "Game" << std::endl;
            serve();

            running = true;
            while (running) {
                lastFrame = SDL_GetTicks();
                if (lastFrame >= (lastTime + 1000)) {
                    lastTime = lastFrame;
                    FPS = frameCount;
                    frameCount = 0;
                }
                if (key_up)
                    l_paddle.y += PLAYER_SPEED;
                else if (key_down)
                    l_paddle.y -= PLAYER_SPEED;
                if (!isPaused)   update();
                input();
                render();
            }
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}