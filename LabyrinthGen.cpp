#include "LabyrinthGen.h"
#include <iostream>

// ----- LABYRINTH GEN ----- //

LabyrinthGen::LabyrinthGen() {
    threshold = 0.95; // acceptance threshold for 'islands'
    // todo
}

void LabyrinthGen::setupGrid(int w, int h) {
    width = w;
    height = h;
    grid.assign(width * height, 0);
}

void LabyrinthGen::buildLabyrinth(float density, float structureBias, 
    float loopFreq, int dupes, int sX, int sY) {
        this->startX = sX;
        this->startY = sY;

        // reset grid after each floor
        std::fill(grid.begin(), grid.end(), 0);

        int expected = applyMask(density);
        int actual = generateSpanningTree(structureBias);
        //check threshold 
        if (static_cast<floar>(actual) / expected < threshold) {
            repairIslands();
        }
        carveLoops(loopFreq);
        mirrorGrid(dupes);
        // find exit
    }

/*
 * Create predefined 'void' space (to fill with no path tiles)
 * @param density: how many void spaces to include
 */
// create predefined 'void' space (to fill with no path tiles)
int LabyrinthGen::applyMask(float density) {
    //todo
    // use bit 0x80

    // don't mask start ex:
    // if (x == startX && y = startY) continue;
    
    // maybe return expected reachable tiles?
    return 0;
}

/* 
 * Generate the labyrinth
 * @param structureBias: 0 to 1, used to decide frequency of using pop front or back
 */ 
int LabyrinthGen::generateSpanningTree(float structureBias) {
    std::deque<int> activeCells;
    std::vector<int> distances(width * height, -1);
    distances[getIndex(startX, startY)] = 0;

    // todo
    // start with visited = 1 for first cell
    // if visited less than expected, run repair

    // track distance as moving cells
    // distance[next cell] = distance [curr cell] + 1;

    // return visited for build? 
    return 0;

}

// reconnects any traversable tiles not connected to main path network
void LabyrinthGen::repairIslands() {
    //todo
    // find unvisited, non void cells
    // flood fill from there for island coords
    // resolve
    // repeat check
}

/* 
 * Removes walls to make loops
 * @param loopFreq: 0 to 1 to increase/decrease number of loops
*/ 
void LabyrinthGen::carveLoops(float loopFreq) {
    //todo
}

// generates a mirror of the map that's connected along an edge
void LabyrinthGen::mirrorGrid(int dupes) {
    //todo
}

std::vector<uint8_t> LabyrinthGen::getGrid() const {
    //todo
}

void LabyrinthGen::printLabyToTerm() const {
    //todo
}

int LabyrinthGen::getIndex(int x, int y) const {
    //todo
}

int LabyrinthGen::getStartX() const {
    return startX;
}

int LabyrinthGen::getStartY() const {
    return startY;
}

int LabyrinthGen::getExitX() const {
    return exitX;
}

int LabyrinthGen::getExitY() const {
    return exitY;
}
    
std::vector<int> LabyrinthGen::getUnvisitedNeighbours(int currCell) const {
    //todo
}
    
void LabyrinthGen::connectCells(int cellA, int cellB) {
    //todo
}


