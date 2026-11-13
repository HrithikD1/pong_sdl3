#include <SDL3/SDL.h>
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>
#include "game.h"
using namespace std;

Game::Game()
    : mWindow(nullptr), mRenderer(nullptr), mFont(nullptr), isRunning(true), mPaddle1{50, 200, 10, 100}, mPaddle2{580, 200, 10, 100}, mBall{315, 235, 10, 10}, ballVelocityX(150.0f), ballVelocityY(75.0f), player1Score(0), player2Score(0)
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
    mFont = TTF_OpenFont("path/to/font.ttf", 24);
    if (!mFont)
    {
        cerr << "Error loading font: " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void Game::shutdown()
{
    if (mRenderer) SDL_DestroyRenderer(mRenderer);
    if (mWindow) SDL_DestroyWindow(mWindow);
    SDL_Quit();
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
        if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
        {
            isRunning = false;
        }
    }

    const bool* keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_UP]) mPaddle1.y -= 5.0f;
    if (keystates[SDL_SCANCODE_S] || keystates[SDL_SCANCODE_DOWN]) mPaddle1.y += 5.0f;
    if (mPaddle1.y < 0) mPaddle1.y = 0;
    if (mPaddle1.y > 480 - mPaddle1.h) mPaddle1.y = 480 - mPaddle1.h;

    if (keystates[SDL_SCANCODE_I] || keystates[SDL_SCANCODE_LEFT]) mPaddle2.y -= 5.0f;
    if (keystates[SDL_SCANCODE_K] || keystates[SDL_SCANCODE_RIGHT]) mPaddle2.y += 5.0f;
    if (mPaddle2.y < 0) mPaddle2.y = 0;
    if (mPaddle2.y > 480 - mPaddle2.h) mPaddle2.y = 480 - mPaddle2.h;
}

void Game::updateGame()
{
    // Update game state here
    // v = d/t, so d = v * t
    float distanceX = ballVelocityX * 0.016f;
    float distanceY = ballVelocityY * 0.016f;

    mBall.x += distanceX; // Assuming 60 FPS, so delta time is ~0.016 seconds
    mBall.y += distanceY; // Assuming 60 FPS, so delta time is ~0.016 seconds

    // Check for collisions with walls and reverse direction if necessary
    if(mBall.y <= 0 || mBall.y > 480 - mBall.h) ballVelocityY = -ballVelocityY; // Bounce off top and bottom; (velocity switches direction)
    if (mBall.x <= 0 || mBall.x > 640 - mBall.h) ballVelocityX = -ballVelocityX; // Bounce off left and right;

    // Check for collisions with paddles
    if(SDL_HasRectIntersectionFloat(&mBall, &mPaddle1) || SDL_HasRectIntersectionFloat(&mBall, &mPaddle2))
    {
        ballVelocityX = -ballVelocityX; // Bounce off paddles
    }

    if(mBall.x < 0 || mBall.x > 640) // If the ball goes off the left or right side of the screen
    {
        // Reset ball position to center
        mBall.x = 315;
        mBall.y = 235;
        // Reset ball velocity
        ballVelocityX = 200.0f;
        ballVelocityY = 150.0f;
    }

    if(mBall.x < 0.0f) // If the ball goes off the left side of the screen
    {
        player2Score++; // Player 2 scores a point
        std::string scoreMessage = "Player 1: " + std::to_string(player1Score) + " | Player 2: " + std::to_string(player2Score);
        SDL_SetWindowTitle(mWindow, scoreMessage.c_str()); // Update window title with scores

        mBall.x = 315;
        mBall.y = 235;
        // Reset ball velocity
        ballVelocityX = 200.0f;
        ballVelocityY = 150.0f;
    }
    else if (mBall.x > 640.0f) // If the ball goes off the right side of the screen
    {
        player1Score++; // Player 1 scores a point
        std::string scoreMessage = "Player 1: " + std::to_string(player1Score) + " | Player 2: " + std::to_string(player2Score);
        SDL_SetWindowTitle(mWindow, scoreMessage.c_str()); // Update window title with scores

        mBall.x = 315;
        mBall.y = 235;
        // Reset ball velocity
        ballVelocityX = -200.0f;
        ballVelocityY = 150.0f;
    }

}

void Game::runLoop()
{
    while (isRunning)
    {
        processInput();
        updateGame();
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

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(mRenderer, &mPaddle1);
    SDL_RenderFillRect(mRenderer, &mPaddle2);
    SDL_RenderFillRect(mRenderer, &mBall);

    // Draw the score string near the top center of the screen
    std::string scoreText = std::to_string(player1Score) + "    " + std::to_string(player2Score);
    renderText(scoreText, 300.0f, 20.0f);

    SDL_RenderPresent(mRenderer);
}