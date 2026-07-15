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

/*
 * Build the labyrinth
 */
void LabyrinthGen::buildLabyrinth(float density, float structureBias, 
    float loopFreq, int dupes, int sX, int sY) {
        this->startX = sX;
        this->startY = sY;

        // reset grid after each floor
        std::fill(grid.begin(), grid.end(), 0);
        std::vector<int> distances(width * height, -1);

        int expected = applyMask(density, distances);
        int actual = generateSpanningTree(structureBias, distances);
        //check threshold 
        if (expected > 0 && actual > 0 && static_cast<float>(actual) / expected < threshold) {
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
int LabyrinthGen::applyMask(float density, std::vector<int>& distances) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0,1.0);

    int expected = width * height;

    for (int i = 0; i < width * height; i++) {
        int x = i % width;
        int y = i / width;

        if (x == startX && y == startY) continue; // don't mask start cell

        if (dis(gen) < density) {
            distances[i] = -2;
            expected--;
        }
    }

    return expected;
}

/* 
 * Generate the labyrinth
 * @param structureBias: 0 to 1, used to decide frequency of using pop front or back
 * returns the number of visited cells
 */ 
int LabyrinthGen::generateSpanningTree(float structureBias, std::vector<int>& distances) {
    std::deque<int> activeCells;

    // for random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<>dis(0.0,1.0);

    // first cell is visited 
    int startIdx = getIndex(startX, startY);
    distances[startIdx] = 0;
    activeCells.push_back(startIdx);
    int visited = 1;
    
    while (!activeCells.empty()) {
        int currCell;
        bool fromBack;
        if (dis(gen) < structureBias) {
            // for long, windy paths
            currCell = activeCells.back();
            activeCells.pop_back();
            fromBack = true;
        } else {
            // for more branching paths
            currCell = activeCells.front();
            activeCells.pop_front();
            fromBack = false;
        }

        std::vector<int> neighbours = getUnvisitedNeighbours(currCell, distances);

        if (!neighbours.empty()) {
            std::uniform_int_distribution<> neighbour_dist(0,neighbours.size() - 1);
            int nextCell = neighbours[neighbour_dist(gen)];

            connectCells(currCell, nextCell);
            distances[nextCell] = distances[currCell] + 1;
            visited++;
            activeCells.push_back(nextCell);

            if (neighbours.size() > 1) {
                if (fromBack) {
                    activeCells.push_back(currCell);
                } else {
                    activeCells.push_front(currCell);
                }
            }
        }

    }

    // distance[next cell] = distance [curr cell] + 1;

    return visited;

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

// Returns the vector index of the cell at coords x and y
int LabyrinthGen::getIndex(int x, int y) const {
    return y * width + x;
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
    
std::vector<int> LabyrinthGen::getUnvisitedNeighbours(int currCell, 
    const std::vector<int>& distances) const {
    std::vector<int> neighbours;

    // currCell coords
    int x = currCell % width;
    int y = currCell / width;
    
    // North, if exists (ie. currCell isn't top row)
    if (y > 0) {
        int north = getIndex(x, y - 1);
        if (distances[north] == -1) neighbours.push_back(north);
    } 

    // South, if exists
    if (y < height - 1) {
        int south = getIndex(x, y + 1);
        if (distances[south] == -1) neighbours.push_back(south);
    }

    // East, if exists
    if (x < width - 1) {
        int east = getIndex(x + 1, y);
        if (distances[east] == -1) neighbours.push_back(east);
    }

    // West, if exists
    if (x > 0) {
        int west = getIndex(x - 1, y);
        if (distances[west] == -1) neighbours.push_back(west);
    }

    return neighbours;
}
    
void LabyrinthGen::connectCells(int cellA, int cellB) {
    //N = 1, S = 2, E = 4, W = 8

    // xDirection: x direction of B relative to A
    int xDirection = (cellB % width) - (cellA % width);
    // yDirection: y direction of B relative to A
    int yDirection = (cellB / width) - (cellA / width);

    if (yDirection == -1) {
        grid[cellA] |= 1;
        grid[cellB] |= 2;
    } else if (yDirection == 1) {
        grid[cellA] |= 2;
        grid[cellB] |= 1;
    } else if (xDirection == 1) {
        grid[cellA] |= 4;
        grid[cellB] |= 8;
    } else if (xDirection == -1) {
        grid[cellA] |= 8;
        grid[cellB] |= 4;
    }
}
