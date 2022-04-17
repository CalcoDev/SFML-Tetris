#pragma once

#include <vector>
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"
#include "GLOBAL.hpp"

#include <iostream>

class Tetramino
{
private:
    unsigned char shape;
    unsigned char rotation;
    sf::Vector2i pos;
    std::vector<sf::Vector2i> minos;

public:
    bool CollidedWithATetramino = 0; // The name is misleading. It means if it collided with a tetramino OR the floor.

    Tetramino(unsigned char p_shape); // Initialise a tetramino with a given shape.

    // Try to move the tetramino down. TRUE -> could move; FALSE -> couldn't move
    template <size_t rows, size_t cols>
    bool MoveDown(char (&p_matrix)[rows][cols])
    {
        // This sketchy code ensures that my sketchy collision system works.
        for (sf::Vector2i &mino : minos)
        {
            p_matrix[mino.y][mino.x] = 0;
        }

        // Check collisions
        for (sf::Vector2i &mino : minos)
        {
            // One ahead as we are "predicting" movement
            if (mino.y + 1 == ROWS)
            {
                CollidedWithATetramino = 1;

                // Reset minos
                for (sf::Vector2i &mino : minos)
                {
                    p_matrix[mino.y][mino.x] = shape;
                }

                return 0;
            }

            if (p_matrix[mino.y + 1][mino.x] != 0)
            {
                CollidedWithATetramino = 1;

                // Reset minos
                for (sf::Vector2i &mino : minos)
                {
                    p_matrix[mino.y][mino.x] = shape;
                }

                return 0;
            }
        }

        for (sf::Vector2i &mino : minos)
            mino.y += 1;

        pos.y += 1;

        return 1;
    }

    template <size_t rows, size_t cols>
    bool MoveSide(char (&p_matrix)[rows][cols], short p_mod)
    {
        // This sketchy code ensures that my sketchy collision system works.
        for (sf::Vector2i &mino : minos)
        {
            p_matrix[mino.y][mino.x] = 0;
        }

        // Check collisions:
        for (sf::Vector2i &mino : minos)
        {
            if (mino.x + p_mod == COLS || mino.x + p_mod <= -1)
            {

                // Reset minos
                for (sf::Vector2i &mino : minos)
                {
                    p_matrix[mino.y][mino.x] = shape;
                }

                return 0;
            }

            if (p_matrix[mino.y][mino.x + p_mod])
            {
                CollidedWithATetramino = 1;

                // Reset minos
                for (sf::Vector2i &mino : minos)
                {
                    p_matrix[mino.y][mino.x] = shape;
                }

                return 0;
            }
        }

        for (sf::Vector2i &mino : minos)
            mino.x += p_mod;

        pos.x += p_mod;

        return 1;
    }

    template <size_t rows, size_t cols>
    std::vector<sf::Vector2i> GetDropHighlight(char (&p_matrix)[rows][cols])
    {
        short blocksFallen = 0;
        bool collided = 0;
        bool collidedWithSelf = 0;

        // This sketchy code ensures that my sketchy collision system works.
        for (sf::Vector2i &mino : minos)
        {
            p_matrix[mino.y][mino.x] = 0;
        }

        while (!collided)
        {
            for (auto mino : minos)
            {
                if (p_matrix[mino.y + blocksFallen][mino.x] != 0 || mino.y + blocksFallen == 20)
                {
                    collided = 1;
                    break;
                }
            }
            blocksFallen += 1;
        }

        // Reset minos
        for (sf::Vector2i &mino : minos)
        {
            p_matrix[mino.y][mino.x] = shape;
        }

        std::vector<sf::Vector2i> ghostMinos(4);
        for (size_t i = 0; i < 4; i++)
        {
            ghostMinos[i].y = minos[i].y + blocksFallen - 2;
            ghostMinos[i].x = minos[i].x;
        }

        return ghostMinos;
    }

    void Reset(unsigned char p_shape);

    // Courtesy of KofyBrek(https://www.youtube.com/c/Kofybrek)
    std::vector<sf::Vector2i> GetWallKickData(bool p_isShape, unsigned char p_currRotation, unsigned char p_nextRotation)
    {
        if (0 == p_isShape)
        {
            switch (p_currRotation)
            {
            case 0:
            case 2:
            {
                switch (p_nextRotation)
                {
                case 1:
                {
                    return {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}};
                }
                case 3:
                {
                    return {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}};
                }
                }
            }
            case 1:
            {
                return {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}};
            }
            case 3:
            {
                return {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}};
            }
            }

            return {{0, 0}};
        }
        else
        {
            switch (p_currRotation)
            {
            case 0:
            {
                switch (p_nextRotation)
                {
                case 1:
                {
                    return {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}};
                }
                case 3:
                {
                    return {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}};
                }
                }
            }
            case 1:
            {
                switch (p_nextRotation)
                {
                case 0:
                {
                    return {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}};
                }
                case 2:
                {
                    return {{0, 0}, {-1, 0}, {2, 0}, {-1, -2}, {2, 1}};
                }
                }
            }
            case 2:
            {
                switch (p_nextRotation)
                {
                case 1:
                {
                    return {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}};
                }
                case 3:
                {
                    return {{0, 0}, {2, 0}, {-1, 0}, {2, -1}, {-1, 2}};
                }
                }
            }
            case 3:
            {
                switch (p_nextRotation)
                {
                case 0:
                {
                    return {{0, 0}, {1, 0}, {-2, 0}, {1, 2}, {-2, -1}};
                }
                case 2:
                {
                    return {{0, 0}, {-2, 0}, {1, 0}, {-2, 1}, {1, -2}};
                }
                }
            }
            }

            return {{0, 0}};
        }
    }

    // Courtesy of KofyBrek(https://www.youtube.com/c/Kofybrek)
    template <size_t rows, size_t cols>
    void Rotate(bool p_clockwise, const char (&p_matrix)[rows][cols])
    {
        // If not cube
        if (shape != 4)
        {
            std::vector<sf::Vector2i> currentMinos = minos;
            unsigned char nextRotation = ((p_clockwise ? 1 : 3) + rotation) % 4;

            if (shape == 1)
            {
                float centreX = 0.5f * (minos[1].x + minos[2].x);
                float centreY = 0.5f * (minos[1].y + minos[2].y);

                if (rotation == 0)
                    centreY += 0.5f;
                else if (rotation == 1)
                    centreX -= 0.5f;
                else if (rotation == 2)
                    centreY -= 0.5f;
                else if (rotation == 3)
                    centreX += 0.5f;

                for (sf::Vector2i &mino : minos)
                {
                    float x = mino.x - centreX;
                    float y = mino.y - centreY;

                    if (p_clockwise)
                    {
                        mino.x = static_cast<char>(centreX - y);
                        mino.y = static_cast<char>(centreY + x);
                    }
                    else
                    {
                        mino.x = static_cast<char>(centreX + y);
                        mino.y = static_cast<char>(centreY - x);
                    }
                }
            }
            else
            {
                for (unsigned char a = 1; a < minos.size(); a++)
                {
                    char x = minos[a].x - minos[0].x;
                    char y = minos[a].y - minos[0].y;

                    if (p_clockwise)
                    {
                        minos[a].x = minos[0].x - y;
                        minos[a].y = x + minos[0].y;
                    }
                    else
                    {
                        minos[a].x = y + minos[0].x;
                        minos[a].y = minos[0].y - x;
                    }
                }
            }

            for (sf::Vector2i &wallKick : GetWallKickData(shape == 1, rotation, nextRotation))
            {
                bool canTrun = 1;

                for (sf::Vector2i &mino : minos)
                {
                    if (mino.x + wallKick.x < 0 || COLS <= mino.x + wallKick.x || ROWS <= mino.y + wallKick.y)
                    {
                        canTrun = 0;
                        break;
                    }

                    if (mino.y + wallKick.y < 0)
                    {
                        continue;
                    }
                    else if (p_matrix[mino.x + wallKick.x][mino.y + wallKick.y])
                    {
                        canTrun = 0;
                        break;
                    }
                }

                if (canTrun)
                {
                    rotation = nextRotation;

                    for (sf::Vector2i &mino : minos)
                    {
                        mino.x += wallKick.x;
                        mino.y += wallKick.y;
                    }

                    return;
                }
            }

            minos.assign(currentMinos.begin(), currentMinos.end());
        }
    }

    template <size_t rows, size_t cols>
    void UpdateMatrix(char (&p_matrix)[rows][cols])
    {
        for (sf::Vector2i &mino : minos)
        {
            p_matrix[mino.y][mino.x] = shape;
        }
    }

    static std::vector<sf::Vector2i> GetTetramino(unsigned char p_shape, unsigned char p_x, unsigned char p_y);
    static sf::Color ShapeToColour(unsigned char p_shape)
    {
        switch (p_shape)
        {
        case 1:
            return sf::Color(66, 135, 245);
        case 2:
            return sf::Color(61, 237, 12);
        case 3:
            return sf::Color(27, 12, 237);
        case 4:
            return sf::Color(237, 230, 12);
        case 5:
            return sf::Color(237, 12, 12);
        case 6:
            return sf::Color(230, 16, 215);
        case 7:
            return sf::Color(230, 148, 16);
        }

        return sf::Color::Black;
    }

    unsigned char GetShape()
    {
        return shape;
    }

    std::vector<sf::Vector2i> &GetMinos()
    {
        return minos;
    }
};