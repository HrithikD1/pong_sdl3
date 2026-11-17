#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>
#include "game.h"
using namespace std;

Game::Game()
: 
    mWindow(nullptr), 
    mRenderer(nullptr), 
    mFont(nullptr), isRunning(true), 
    mPaddle1{50, 200, 10, 100}, mPaddle2{580, 200, 10, 100}, 
    mBall{315, 235, 10, 10}, 
    ballVelocityX(300.0f), ballVelocityY(200.5f), 
    player1Score(0), player2Score(0), 
    isPaused(false),
    hasHitOccured(false),
    mLastTicks(0), deltaTime(0.0f)
{
}

Game::~Game()
{
    shutdown();
}

bool Game::init()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        cerr << "Error initializing SDL: " << SDL_GetError() << endl;
        return false;
    }
    if (!SDL_CreateWindowAndRenderer("pong", 640, 480, 0, &mWindow, &mRenderer))
    {
        cerr << "Error creating window and renderer: " << SDL_GetError() << endl;
        return false;
    }
    if(!TTF_Init()) 
    {
        cerr << "Error initializing SDL_ttf: " << SDL_GetError() << endl;
        return false;
    }
    mFont = TTF_OpenFont("font.ttf", 24);
    if (!mFont)
    {
        cerr << "Error loading font: " << SDL_GetError() << endl;
        return false;
    }

    SDL_SetWindowTitle(mWindow, "PONG in SDL3 by hrithik"); 

    return true;
}

void Game::shutdown()
{
    if(mFont) TTF_CloseFont(mFont);
    if (mRenderer) SDL_DestroyRenderer(mRenderer);
    if (mWindow) SDL_DestroyWindow(mWindow);
    SDL_Quit();
    TTF_Quit();
}

void Game::processInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            isRunning = false;
        }
        if(event.type == SDL_EVENT_KEY_DOWN)
        {
            if(event.key.key == SDLK_P)
            {
                isPaused = !isPaused;
            }
            if(event.key.key == SDLK_RETURN || event.key.key == SDLK_SPACE)
            {
                isStarted = true;
            }
        }
        if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        {
            isRunning = false;
        }
    }

    if(isStarted && !isPaused)
    {
        const bool* keystates = SDL_GetKeyboardState(NULL);
        float paddleSpeed = 400.0f; // Speed at which paddles move
        if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) mPaddle1.y -= paddleSpeed * deltaTime;
        if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) mPaddle1.y += paddleSpeed * deltaTime;
        if (mPaddle1.y < 0) mPaddle1.y = 0;
        if (mPaddle1.y > 480 - mPaddle1.h) mPaddle1.y = 480 - mPaddle1.h;

        if (keystates[SDL_SCANCODE_I] || keystates[SDL_SCANCODE_LEFT]) mPaddle2.y -= paddleSpeed * deltaTime;
        if (keystates[SDL_SCANCODE_K] || keystates[SDL_SCANCODE_RIGHT]) mPaddle2.y += paddleSpeed * deltaTime;
        if (mPaddle2.y < 0) mPaddle2.y = 0;
        if (mPaddle2.y > 480 - mPaddle2.h) mPaddle2.y = 480 - mPaddle2.h;
    }
}

void Game::updateGame()
{
    // Update game state here
    // v = d/t, so d = v * t
    float distanceX = ballVelocityX * deltaTime;
    float distanceY = ballVelocityY * deltaTime;

    mBall.x += distanceX; // Assuming 60 FPS, so delta time is ~0.016 seconds
    mBall.y += distanceY; // Assuming 60 FPS, so delta time is ~0.016 seconds
    // Check for collisions with paddles
    if(SDL_HasRectIntersectionFloat(&mBall, &mPaddle1))
    {
        ballVelocityX = -ballVelocityX; // Bounce off paddles
        mBall.x = mPaddle1.x + mPaddle1.w;
        hasHitOccured = true; // Set the flag to true when a hit occurs
    }
    if(SDL_HasRectIntersectionFloat(&mBall, &mPaddle2))
    {
        ballVelocityX = -ballVelocityX; // Bounce off paddles
        mBall.x = mPaddle2.x - mBall.w;
        hasHitOccured = true; // Set the flag to true when a hit occurs
    }
    // Check top and bottom walls
    if (mBall.y <= 0)
    {
        mBall.y = 0;
        ballVelocityY = -ballVelocityY;
        hasHitOccured = true; // Set the flag to true when a hit occurs
    }
    else if (mBall.y >= 480 - mBall.h)
    {
        mBall.y = 480 - mBall.h;
        ballVelocityY = -ballVelocityY;
        hasHitOccured = true; // Set the flag to true when a hit occurs
    }

    if(mBall.x < 0.0f) // If the ball goes off the left side of the screen
    {
        player2Score++; // Player 2 scores a point
        mBall.x = 315;
        mBall.y = 235;
        // Reset ball velocity
        ballVelocityX = 300.0f;
        ballVelocityY = 200.5f;
    }
    else if (mBall.x > 640.0f) // If the ball goes off the right side of the screen
    {
        player1Score++; // Player 1 scores a point
        mBall.x = 315;
        mBall.y = 235;
        // Reset ball velocity
        ballVelocityX = -300.0f;
        ballVelocityY = -200.5f;
    }

}

void Game::runLoop()
{
    uint64_t lastTicks = SDL_GetTicks();

    while (isRunning)
    {
        uint64_t currentTicks = SDL_GetTicks();
        deltaTime = static_cast<float>(currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        if(deltaTime > 0.05f) // Cap deltaTime to avoid large jumps
        {
            deltaTime = 0.05f;
        }

        processInput();
        if(isStarted && !isPaused)
        {
            updateGame();
        }

        render();
    }
}

void Game::renderText(const std::string& text, float x, float y)
{
    SDL_Color textColor = {255, 255, 255, 255};
    
    // 1. Create a surface from the string data
    SDL_Surface* textSurface = TTF_RenderText_Blended(mFont, text.c_str(), 0, textColor);
    if (!textSurface) return;

    // 2. Convert the CPU surface into a GPU texture
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
    if (!textTexture)
    {
        SDL_DestroySurface(textSurface);
        return;
    }

    // 3. Define destination rectangle using surface width and height
    SDL_FRect destRect = {
        x, 
        y, 
        static_cast<float>(textSurface->w), 
        static_cast<float>(textSurface->h)
    };

    // 4. Copy texture to the rendering target
    SDL_RenderTexture(mRenderer, textTexture, nullptr, &destRect);

    // 5. Clean up temporary memory to prevent leaks
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::render()
{
    SDL_SetRenderDrawColor(mRenderer, 30, 35, 45, 255);
    SDL_RenderClear(mRenderer);

    if(!isStarted)
    {
        renderText("Welcome to PONG! Press Enter or Space to Start.", 50.0f, 200.0f);
        renderText("Press Enter or Space to Start the Game.", 50.0f, 300.0f);
        renderText("Created by hrithik using SDL3 and C++14.", 50.0f, 250.0f);
        SDL_RenderPresent(mRenderer);
    }
    else
    {   
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        float lineWidth = 4.0f;
        float centerX = (640.0f - lineWidth) / 2.0f;
        SDL_FRect centerLine = {centerX, 0.0f, lineWidth, 480.0f};
        SDL_RenderFillRect(mRenderer, &centerLine);

        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
        SDL_RenderFillRect(mRenderer, &mPaddle1);
        SDL_RenderFillRect(mRenderer, &mPaddle2);
        SDL_RenderFillRect(mRenderer, &mBall);

        // Draw the score string near the top center of the screen
        std::string score1Text = std::to_string(player1Score);
        renderText(score1Text, 290.0f, 20.0f);

        std::string score2Text = std::to_string(player2Score);
        renderText(score2Text, 340.0f, 20.0f);


        if(isPaused)
        {
            // 1. Enable blending so alpha transparency works
            SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

            // 2. Draw a dark, semi-transparent rectangle covering the whole window (640x480)
            SDL_FRect overlayRect = { 0.0f, 0.0f, 640.0f, 480.0f };
            SDL_SetRenderDrawColor(mRenderer, 10, 15, 25, 180); // RGBA (180 out of 255 alpha)
            SDL_RenderFillRect(mRenderer, &overlayRect);

            // 3. Reset blend mode back to none for standard drawing
            SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
            renderText("Game Paused. Press P to Resume.", 150.0f, 200.0f);
        }    
        else if(!hasHitOccured)
        {
            renderText("Press P to Pause the Game.", 150.0f, 200.0f);
        }

        SDL_RenderPresent(mRenderer);
    }
}