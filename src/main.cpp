/*
DISCLAIMER: Piece rotation is by far the worst part of this project, it still is pretty glitchy.

DISCLAIMER 2: The way the code is organised, ..., it isn't. There are CPP files for headers that already define everything, stuff
is defined all over the place, it's bad.

DISCLAIMER 3: The way collisions / gameboard works is terribly bad.
On short: I have made ~4 bugs, that manage to cancel eachother out and produce working code.
Newest piece in vector -> piece at the toppest of screen
Every time a game frame runs, I clear the matrix, and render from bottom to top
This means I can get away with dumb collision code.
*/

#include <SFML/Graphics.hpp>
#include "Headers/GLOBAL.hpp"
#include "Headers/Tetramino.hpp"

#include <string>
#include <cmath>
#include <iostream>
#include <random>

/*
Make game frame independant:
Have a fixed time and run a "game frame" once every X seconds.
*/

// Courtesy of KofyBrek(https://www.youtube.com/c/Kofybrek)
void RenderText(unsigned short i_x, unsigned short i_y, const std::string &i_text, sf::RenderWindow &i_window)
{
    // We're gonna align the text ot the left top
    short character_x = i_x;
    short character_y = i_y;

    unsigned char character_width;

    sf::Sprite character_sprite;

    sf::Texture font_texture;
    font_texture.loadFromFile("../res/Font.png");

    // We're gonna calculate the width of the character based on the font image size
    // 96 because there are 96 character in the image
    character_width = font_texture.getSize().x / 96;

    character_sprite.setTexture(font_texture);

    for (const char a : i_text)
    {
        if ('\n' == a)
        {
            // After every newline we put increase the y-coordinate and reset the x-coordinate
            character_x = i_x;
            character_y += font_texture.getSize().y;

            continue;
        }

        // Change the position of the next character
        character_sprite.setPosition(character_x, character_y);
        // Pick the character from the font image
        character_sprite.setTextureRect(sf::IntRect(character_width * (a - 32), 0, character_width, font_texture.getSize().y));
        character_sprite.setScale(sf::Vector2f(0.5f, 0.5f));

        // Increase the x-coordinate
        character_x += character_width * .5f;

        // Draw the character
        i_window.draw(character_sprite);
    }
}

int main()
{
    // Make sure game runs independent of frame rate.
    unsigned int timeElapsed = 0;
    sf::Clock clock;

    // Random number generator:
    std::random_device rnd_dev;
    std::mt19937 rng(rnd_dev());
    std::uniform_int_distribution<std::mt19937::result_type> gen_random_shape(1, 7);

    // Initialise window.
    sf::RenderWindow window(sf::VideoMode(2 * CELL_SIZE * COLS * SCREEN_RESIZE, CELL_SIZE * ROWS * SCREEN_RESIZE), "Tetris", sf::Style::Close);
    window.setView(sf::View(sf::FloatRect(0, 0, 2 * CELL_SIZE * COLS, CELL_SIZE * ROWS)));
    window.setFramerateLimit(60);

    // TODO(calco): this shouldbe done at some point.
    // sf::Image icon;
    // icon.loadFromFile("../res/icon.png");
    // window.SetIcon(icon.GetWidth(), icon.GetHeight(), icon.GetPixelsPtr());

    sf::Event event;

    Tetramino currTetramino(gen_random_shape(rng));
    unsigned char nextShape = gen_random_shape(rng);

    unsigned long long score = 0;

    sf::RectangleShape nextPiecePreview(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
    nextPiecePreview.setFillColor(sf::Color(0, 0, 0));
    nextPiecePreview.setOutlineThickness(-1);
    nextPiecePreview.setPosition(CELL_SIZE * (1.5f * COLS - 2.5f), CELL_SIZE * (0.25f * ROWS - 2.5f));

    bool pressedLeft = 0;
    bool pressedRight = 0;

    bool gameOver = 0;

    unsigned long long linesCleared = 0;

    // Game matrix:
    char matrix[ROWS][COLS];
    for (size_t y = 0; y < ROWS; y++)
    {
        for (size_t x = 0; x < COLS; x++)
        {
            matrix[y][x] = 0;
        }
    }

    while (window.isOpen())
    {
        timeElapsed = clock.getElapsedTime().asMilliseconds();
        pressedLeft = 0;
        pressedRight = 0;

        // Poll events:
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
            {
                window.close();
                break;
            }
            case sf::Event::KeyPressed:
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                {
                    window.close();
                    break;
                }
                case sf::Keyboard::Left:
                {
                    pressedLeft = 1;
                    break;
                }
                case sf::Keyboard::Right:
                {
                    pressedRight = 1;
                    break;
                }
                case sf::Keyboard::Down: // "Drop" piece
                {
                    if (gameOver)
                        break;

                    while (currTetramino.MoveDown(matrix))
                        ;

                    // Probably some better way to do this.
                    currTetramino.UpdateMatrix(matrix);
                    currTetramino = Tetramino(nextShape);
                    nextShape = gen_random_shape(rng);
                    currTetramino.UpdateMatrix(matrix);

                    currTetramino.MoveDown(matrix); // See if it would collide with anything
                    if (currTetramino.CollidedWithATetramino)
                        for (sf::Vector2i mino : currTetramino.GetMinos())
                            if (mino.y == 0)
                            {
                                gameOver = 1;
                                break; // Break out of
                            }

                    // Move the tetramino back up
                    for (sf::Vector2i &mino : currTetramino.GetMinos())
                        mino.y -= 1;

                    // Check if a line was filled:
                    unsigned char currentLinesCleared = 0;
                    for (size_t y = 0; y < ROWS; y++)
                    {
                        bool filledLine = true;
                        for (size_t x = 0; x < COLS; x++)
                            filledLine = filledLine && matrix[y][x] != 0;

                        if (filledLine)
                        {
                            currentLinesCleared += 1;
                            // This sketchy code ensures that my sketchy collision system works.
                            for (sf::Vector2i &mino : currTetramino.GetMinos())
                            {
                                matrix[mino.y][mino.x] = 0;
                            }

                            // Move all rows down by one:
                            for (size_t y1 = y; y1 >= 1; y1--)
                                for (size_t x1 = 0; x1 < COLS; x1++)
                                    matrix[y1][x1] = matrix[y1 - 1][x1];

                            // Reset current tetraminos
                            for (sf::Vector2i &mino : currTetramino.GetMinos())
                            {
                                matrix[mino.y][mino.x] = currTetramino.GetShape();
                            }
                        }
                    }
                    linesCleared += currentLinesCleared;
                    // We're using "Original BPS" scoring system.
                    switch (currentLinesCleared)
                    {
                    case 1:
                        score += 40;
                        break;
                    case 2:
                        score += 100;
                        break;
                    case 3:
                        score += 300;
                        break;
                    case 4:
                        score += 1200;
                        break;
                    }

                    break;
                }
                case sf::Keyboard::R:
                {
                    if (gameOver)
                        break;

                    // This sketchy code ensures that my sketchy collision system works.
                    for (sf::Vector2i &mino : currTetramino.GetMinos())
                    {
                        matrix[mino.y][mino.x] = 0;
                    }
                    currTetramino.Rotate(1, matrix);
                    currTetramino.UpdateMatrix(matrix);
                }
                case sf::Keyboard::Space:
                {
                    if (gameOver)
                    {
                        gameOver = 0;

                        for (size_t y = 0; y < ROWS; y++)
                        {
                            for (size_t x = 0; x < COLS; x++)
                            {
                                matrix[y][x] = 0;
                            }
                        }

                        currTetramino = Tetramino(gen_random_shape(rng));
                        currTetramino.UpdateMatrix(matrix);
                        score = 0;
                        linesCleared = 0;
                    }
                }
                }
                break;
            }
            }
        }

        // Render game over screen:
        if (gameOver)
        {
            window.clear(sf::Color::Black);

            RenderText(
                static_cast<unsigned short>(0),
                static_cast<unsigned short>(0),
                "Game over.\nPress [SPACE] to restart the game.\nScore: " + std::to_string(score),
                window);

            window.display();
        }
        else
        {
            if ((pressedRight || pressedLeft))
            {
                currTetramino.MoveSide(matrix, pressedRight - pressedLeft);
                currTetramino.UpdateMatrix(matrix);
            }

            // Run game frame:
            while (timeElapsed >= FRAME_DURATION_MS)
            {
                timeElapsed -= FRAME_DURATION_MS;
                clock.restart();

                currTetramino.MoveDown(matrix);
                currTetramino.UpdateMatrix(matrix);

                if (currTetramino.CollidedWithATetramino)
                {
                    for (sf::Vector2i mino : currTetramino.GetMinos())
                    {
                        if (mino.y == 0)
                            gameOver = 1;
                    }

                    currTetramino = Tetramino(nextShape);
                    nextShape = gen_random_shape(rng);
                    currTetramino.UpdateMatrix(matrix);
                }

                // Check if a line was filled:
                for (size_t y = 0; y < ROWS; y++)
                {
                    bool filledLine = true;
                    for (size_t x = 0; x < COLS; x++)
                        filledLine = filledLine && matrix[y][x] != 0;

                    if (filledLine)
                    {
                        // This sketchy code ensures that my sketchy collision system works.
                        for (sf::Vector2i &mino : currTetramino.GetMinos())
                        {
                            matrix[mino.y][mino.x] = 0;
                        }

                        // Move all rows down by one:
                        for (size_t y1 = y; y1 >= 1; y1--)
                            for (size_t x1 = 0; x1 < COLS; x1++)
                                matrix[y1][x1] = matrix[y1 - 1][x1];

                        // Reset current tetraminos
                        for (sf::Vector2i &mino : currTetramino.GetMinos())
                        {
                            matrix[mino.y][mino.x] = currTetramino.GetShape();
                        }
                    }
                }
            }

            // Render:
            window.clear(sf::Color::Black);
            sf::RectangleShape cellShape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

            // Render background:
            for (size_t y = 0; y < ROWS; y++)
            {
                for (size_t x = 0; x < COLS; x++)
                {
                    if (matrix[y][x] == 0)
                    {
                        cellShape.setFillColor(sf::Color(42, 45, 46));
                        cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                        window.draw(cellShape);
                    }
                }
            }

            // Render drop highlight:
            cellShape.setFillColor(sf::Color(83, 86, 87));
            for (sf::Vector2i mino : currTetramino.GetDropHighlight(matrix))
            {
                cellShape.setPosition(mino.x * CELL_SIZE, mino.y * CELL_SIZE);
                window.draw(cellShape);
            }

            // Render blocks:
            for (size_t y = 0; y < ROWS; y++)
            {
                for (size_t x = 0; x < COLS; x++)
                {
                    if (matrix[y][x] != 0)
                    {
                        cellShape.setFillColor(Tetramino::ShapeToColour(matrix[y][x]));
                        cellShape.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                        window.draw(cellShape);
                    }
                }
            }

            // Render next piece preview:
            window.draw(nextPiecePreview);

            for (sf::Vector2i mino : Tetramino::GetTetramino(nextShape, static_cast<unsigned char>(1.5f * COLS), static_cast<unsigned char>(0.25f * ROWS)))
            {
                // Shifting the tetromino to the center of the preview border
                unsigned short nextTetraminoX = CELL_SIZE * mino.x;
                unsigned short nextTetraminoY = CELL_SIZE * mino.y;

                if (nextShape == 1)
                {
                    nextTetraminoY += static_cast<unsigned char>(round(0.5f * CELL_SIZE));
                }
                else if (nextShape != 4)
                {
                    nextTetraminoX -= static_cast<unsigned char>(round(0.5f * CELL_SIZE));
                }

                cellShape.setPosition(nextTetraminoX, nextTetraminoY);
                cellShape.setFillColor(Tetramino::ShapeToColour(nextShape));
                window.draw(cellShape);
            }

            // Render score:
            RenderText(
                static_cast<unsigned short>(CELL_SIZE * (0.5f + COLS)),
                static_cast<unsigned short>(0.5f * CELL_SIZE * ROWS),
                "Lines: " + std::to_string(linesCleared) + "\nScore: " + std::to_string(score),
                window);

            window.display();
        }
    }

    return 0;
}