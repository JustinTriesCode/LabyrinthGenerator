#ifndef LABYRINTH_GENERATOR_H
#define LABYRINTH_GENERATOR_H

#include <vector>
#include <cstdint>
#include <random>
#include <deque>

// Class to build labyrinth
class LabyrinthGen {
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
    std::vector<int> getUnvisitedNeighbours(int currCell) const;
    void connectCells(int cellA, int cellB);

    // Labyrinth generattion functions
    int applyMask(float density);
    int generateSpanningTree(float structureBias);
    void repairIslands();
    void carveLoops(float loopFreq);
    void mirrorGrid(int dupes); 

public: 
    // Constructor
    LabyrinthGen();
    void setupGrid(int w, int h);

    // Builder
    void buildLabyrinth(float density, float structureBias, 
        float loopFreq, int dupes, int sX, int sY);

    // Getters
    std::vector<uint8_t> getGrid() const;
    int getStartX() const;
    int getStartY() const;
    int getExitX() const;
    int getExitY() const;

    void printLabyToTerm() const;
};

#endif