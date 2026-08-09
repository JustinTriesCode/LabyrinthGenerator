#include <iostream>
#include "LabyrinthGen.h"
#include <string>

void printLabyToTerm(const LabyrinthGen &lg)
{
    int width = lg.getWidth();
    int height = lg.getHeight();
    auto grid = lg.getGrid();

    for (int i = 0; i < width * 2 + 1; i++)
    {
        std::cout << "█"; // top wall
    }
    std::cout << std::endl;

    for (int y = 0; y < height; y++)
    {
        std::cout << "█"; // west wall
        for (int x = 0; x < width; x++)
        {
            int cell_idx = y * width + x;
            const uint8_t cell = grid[cell_idx];

            // No passages = solid/masked wall.
            if (cell == 0)
            {
                std::cout << "█";
            }
            else
            {
                std::cout << " "; // cell floor
            }

            if ((cell & 4) != 0)
            {
                std::cout << " "; // east passage
            }
            else
            {
                std::cout << "█"; // east wall
            }
        }
        std::cout << std::endl;

        std::cout << "█"; // south wall
        for (int x = 0; x < width; x++)
        {
            int cell_idx = y * width + x;
            const uint8_t cell = grid[cell_idx];
            std::cout << ((cell & 2) != 0 ? " " : "█"); // south passage
            std::cout << "█";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // Default values
    float density = 0.1f;
    float structureBias = 0.3f;
    float loopFreq = 0.1f;
    int dupes = 2;
    int startX = 0;
    int startY = 0;

    if (argc == 7)
    {
        try
        {
            density = std::stof(argv[1]);
            structureBias = std::stof(argv[2]);
            loopFreq = std::stof(argv[3]);
            dupes = std::stoi(argv[4]);
            startX = std::stoi(argv[5]);
            startY = std::stoi(argv[6]);
            std::cout << "Using command-line arguments." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error parsing arguments: " << e.what() << std::endl;
            std::cerr << "Usage: " << argv[0] << " <density> <bias> <loopFreq> <dupes> <startX> <startY>" << std::endl;
            return 1;
        }
    } // else uses Default

    LabyrinthGen laby;
    laby.setupGrid(12, 6);
    laby.buildLabyrinth(density, structureBias, loopFreq, dupes, startX, startY);
    printLabyToTerm(laby);
    return 0;
}

// g++ main.cpp -o main