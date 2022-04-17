#include "Tetramino.hpp"

std::vector<sf::Vector2i> Tetramino::GetTetramino(unsigned char p_shape, unsigned char p_x, unsigned char p_y)
{
    std::vector<sf::Vector2i> output_tetromino(4);

    switch (p_shape)
    {
    case 1: // Flat I-Shape
        output_tetromino[0] = {1, -1};
        output_tetromino[1] = {0, -1};
        output_tetromino[2] = {-1, -1};
        output_tetromino[3] = {-2, -1};

        break;
    case 2: // Z shape 1
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {1, 0};
        output_tetromino[2] = {-1, -1};
        output_tetromino[3] = {-1, 0};

        break;
    case 3: // L shape 1
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {1, 0};
        output_tetromino[2] = {1, -1};
        output_tetromino[3] = {-1, 0};

        break;
    case 4: // Square
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {0, -1};
        output_tetromino[2] = {-1, -1};
        output_tetromino[3] = {-1, 0};

        break;
    case 5: // Z shape 2
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {1, -1};
        output_tetromino[2] = {0, -1};
        output_tetromino[3] = {-1, 0};

        break;
    case 6: // T shape
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {1, 0};
        output_tetromino[2] = {0, -1};
        output_tetromino[3] = {-1, 0};

        break;
    case 7: // L shape 2
        output_tetromino[0] = {0, 0};
        output_tetromino[1] = {1, 0};
        output_tetromino[2] = {0, -1};
        output_tetromino[3] = {-1, -1};

        break;
    }

    // Move the tetramino to X and Y
    for (sf::Vector2i &mino : output_tetromino)
    {
        mino.x += p_x;
        mino.y += p_y;
    }

    return output_tetromino;
}

Tetramino::Tetramino(unsigned char p_shape) : shape(p_shape),
                                              minos(Tetramino::GetTetramino(p_shape, COLS / 2, 1)),
                                              pos(COLS / 2, 1),
                                              rotation(0)
{
}