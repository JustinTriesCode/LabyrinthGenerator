#ifndef LABYRINTH_GENERATOR_H
#define LABYRINTH_GENERATOR_H

#include <vector>
#include <cstdint>
#include <random>
#include <deque>

// Class to build labyrinth
class LabyrinthGen
{
private:
    int width;
    int height;

    // coordinates
    int startX;
    int startY;
    int exitX;
    int exitY;

    float threshold;
    std::vector<uint8_t> grid;

    int getIndex(int x, int y) const;
    std::vector<int> getUnvisitedNeighbours(int currCell,
                                            const std::vector<int> &distances) const;
    void connectCells(int cellA, int cellB);

    // Labyrinth generattion functions
    int applyMask(float density, std::vector<int> &distances);
    int generateSpanningTree(float structureBias, std::vector<int> &distances);
    void repairIslands();
    void carveLoops(float loopFreq);
    void mirrorGrid(int dupes);

public:
    // Constructor
    LabyrinthGen();
    void setupGrid(int w, int h);

    /**
     * @brief Builds the entire labyrinth by running the generation pipeline.
     *
     * @param density The frequency of 'void' or masked-out cells (0.0 to 1.0).
     * @param structureBias A value from 0.0 to 1.0. Higher values create long, windy corridors; lower values create more branching.
     * @param loopFreq The frequency of adding loops to the maze (0.0 to 1.0).
     * @param dupes The number of times to mirror the grid, alternating between vertical and horizontal.
     * @param sX The starting X coordinate for the maze generation.
     * @param sY The starting Y coordinate for the maze generation.
     */
    void buildLabyrinth(float density, float structureBias,
                        float loopFreq, int dupes, int sX, int sY);

    // Getters
    std::vector<uint8_t> getGrid() const;
    int getStartX() const;
    int getStartY() const;
    int getExitX() const;
    int getExitY() const;
    int getWidth() const;
    int getHeight() const;
};

#endif