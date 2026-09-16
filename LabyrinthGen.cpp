#include "LabyrinthGen.h"

// ----- LABYRINTH GEN ----- //

LabyrinthGen::LabyrinthGen()
{
    threshold = 0.95; // acceptance threshold for 'islands'
    // todo
}

void LabyrinthGen::setupGrid(int w, int h)
{
    width = w;
    height = h;
    grid.assign(width * height, 0);
}

void LabyrinthGen::buildLabyrinth(float density, float structureBias,
                                  float loopFreq, int dupes, int sX, int sY)
{
    this->startX = sX;
    this->startY = sY;

    // reset grid after each floor
    std::fill(grid.begin(), grid.end(), 0);
    std::vector<int> distances(width * height, -1);

    int expected = applyMask(density, distances);
    int actual = generateSpanningTree(structureBias, distances);
    // check threshold
    if (expected > 0 && actual > 0 && static_cast<float>(actual) / expected < threshold)
    {
        repairIslands(distances);
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
int LabyrinthGen::applyMask(float density, std::vector<int> &distances)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    int expected = width * height;

    for (int i = 0; i < width * height; i++)
    {
        int x = i % width;
        int y = i / width;

        if (x == startX && y == startY)
            continue; // don't mask start cell

        if (dis(gen) < density)
        {
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
int LabyrinthGen::generateSpanningTree(float structureBias, std::vector<int> &distances)
{
    std::deque<int> activeCells;

    // for random
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // first cell is visited
    int startIdx = getIndex(startX, startY);
    distances[startIdx] = 0;
    activeCells.push_back(startIdx);
    int visited = 1;

    while (!activeCells.empty())
    {
        int currCell;
        bool fromBack;
        if (dis(gen) < structureBias)
        {
            // for long, windy paths
            currCell = activeCells.back();
            activeCells.pop_back();
            fromBack = true;
        }
        else
        {
            // for more branching paths
            currCell = activeCells.front();
            activeCells.pop_front();
            fromBack = false;
        }

        std::vector<int> neighbours = getUnvisitedNeighbours(currCell, distances);

        if (!neighbours.empty())
        {
            std::uniform_int_distribution<> neighbour_dist(0, neighbours.size() - 1);
            int nextCell = neighbours[neighbour_dist(gen)];

            connectCells(currCell, nextCell);
            distances[nextCell] = distances[currCell] + 1;
            visited++;
            activeCells.push_back(nextCell);

            if (neighbours.size() > 1)
            {
                if (fromBack)
                {
                    activeCells.push_back(currCell);
                }
                else
                {
                    activeCells.push_front(currCell);
                }
            }
        }
    }

    // distance[next cell] = distance [curr cell] + 1;

    return visited;
}

// reconnects any traversable tiles not connected to main path network
void LabyrinthGen::repairIslands(std::vector<int> &distances)
{
    bool islandFound = true;
    while (islandFound)
    {
        islandFound = false;
        std::vector<int> currentIsland;

        // find unvisited, non void cells
        for (int i = 0; i < width * height; i++)
        {
            if (distances[i] == -1)
            {
                // flood fill to find connected cells
                std::deque<int> queue;
                queue.push_back(i);

                distances[i] = -3; // temp set to -3 to avoid revisiting

                while (!queue.empty())
                {
                    int currCell = queue.front();
                    queue.pop_front();
                    currentIsland.push_back(currCell);

                    int x = currCell % width;
                    int y = currCell / width;

                    if (y > 0 && distances[getIndex(x, y - 1)] == -1)
                    {
                        distances[getIndex(x, y - 1)] = -3;
                        queue.push_back(getIndex(x, y - 1));
                    }
                    if (y < height - 1 && distances[getIndex(x, y + 1)] == -1)
                    {
                        distances[getIndex(x, y + 1)] = -3;
                        queue.push_back(getIndex(x, y + 1));
                    }
                    if (x > 0 && distances[getIndex(x - 1, y)] == -1)
                    {
                        distances[getIndex(x - 1, y)] = -3;
                        queue.push_back(getIndex(x - 1, y));
                    }
                    if (x < width - 1 && distances[getIndex(x + 1, y)] == -1)
                    {
                        distances[getIndex(x + 1, y)] = -3;
                        queue.push_back(getIndex(x + 1, y));
                    }
                }
                break;
            }
        }
        if (currentIsland.empty())
            continue;

        // find the closest cell in the main labyrinth
        int bestIslandCell = -1;
        int bestMainCell = -1;
        int minDistance = width * height + 1;

        for (int islandCell : currentIsland)
        {
            int ix = islandCell % width;
            int iy = islandCell / width;

            for (int j = 0; j < width * height; j++)
            {
                if (distances[j] >= 0) // It's part of the main maze
                {
                    int mx = j % width;
                    int my = j / width;
                    int dist = std::abs(ix - mx) + std::abs(iy - my);

                    if (dist < minDistance)
                    {
                        minDistance = dist;
                        bestIslandCell = islandCell;
                        bestMainCell = j;
                    }
                }
            }
        }

        // Tunnel from bestMainCell to bestIslandCell
        if (bestIslandCell != -1 && bestMainCell != -1)
        {
            int currX = bestMainCell % width;
            int currY = bestMainCell / width;
            int targetX = bestIslandCell % width;
            int targetY = bestIslandCell / width;

            int prevCell = bestMainCell;

            // Tunnel horizontally then vertically (for L-shape)
            while (currX != targetX || currY != targetY)
            {
                if (currX != targetX)
                {
                    currX += (targetX > currX) ? 1 : -1;
                }
                else
                {
                    currY += (targetY > currY) ? 1 : -1;
                }

                int stepCell = getIndex(currX, currY);
                connectCells(prevCell, stepCell);
                distances[stepCell] = distances[prevCell] + 1;
                prevCell = stepCell;
            }

            // Reset the rest of the island back to -1
            for (int cell : currentIsland)
            {
                if (distances[cell] == -3)
                    distances[cell] = -1;
            }

            // Carve paths through the newly connected island
            // Temporarily change startX/startY to our bridge point so the spanning tree generates from here
            int tempX = startX;
            int tempY = startY;
            startX = targetX;
            startY = targetY;

            generateSpanningTree(0.5f, distances);

            startX = tempX;
            startY = tempY;

            islandFound = true;
        }
    }
}

/*
 * Removes walls to make loops
 * @param loopFreq: 0 to 1 to increase/decrease number of loops
 */
void LabyrinthGen::carveLoops(float loopFreq)
{
    // todo
}

// generates a mirror of the map that's connected along an edge
void LabyrinthGen::mirrorGrid(int dupes)
{
    for (int i = 0; i < dupes; ++i)
    {
        bool mirrorVertical = (i % 2 == 0);
        int new_width = mirrorVertical ? width : width * 2 - 1;
        int new_height = mirrorVertical ? height * 2 - 1 : height;

        std::vector<uint8_t> new_grid(new_height * new_width, 0);

        for (int y_old = 0; y_old < height; ++y_old)
        {
            for (int x_old = 0; x_old < width; ++x_old)
            {
                int old_idx = getIndex(x_old, y_old);
                uint8_t cell_data = grid[old_idx];

                if (mirrorVertical)
                {
                    if (y_old == height - 1)
                    {
                        if (cell_data & 1)
                            cell_data |= 2; // Seam row, S = N
                        new_grid[y_old * new_width + x_old] = cell_data;
                    }
                    else
                    {
                        new_grid[y_old * new_width + x_old] = cell_data; // og top half

                        // mirror for bottom half
                        int mirrored_y = (new_height - 1) - y_old;
                        uint8_t mirrored_data = 0;
                        if (cell_data & 1)
                            mirrored_data |= 2; // N -> S
                        if (cell_data & 2)
                            mirrored_data |= 1;                 // S -> N
                        mirrored_data |= (cell_data & (4 | 8)); // No change for E/W
                        new_grid[mirrored_y * new_width + x_old] = mirrored_data;
                    }
                }
                else
                {
                    if (x_old == width - 1) // Seam column, W = E
                    {
                        if (cell_data & 8)
                            cell_data |= 4;
                        new_grid[y_old * new_width + x_old] = cell_data;
                    }
                    else
                    {
                        new_grid[y_old * new_width + x_old] = cell_data;
                        int mirrored_x = (new_width - 1) - x_old;
                        uint8_t mirrored_data = 0;
                        if (cell_data & 4)
                            mirrored_data |= 8; // E -> W
                        if (cell_data & 8)
                            mirrored_data |= 4;                 // W -> E
                        mirrored_data |= (cell_data & (1 | 2)); // No change N/S
                        new_grid[y_old * new_width + mirrored_x] = mirrored_data;
                    }
                }
            }
        }
        grid = std::move(new_grid);
        width = new_width;
        height = new_height;
    }
}

std::vector<uint8_t> LabyrinthGen::getGrid() const
{
    return grid;
}

// Returns the vector index of the cell at coords x and y
int LabyrinthGen::getIndex(int x, int y) const
{
    return y * width + x;
}

int LabyrinthGen::getStartX() const
{
    return startX;
}

int LabyrinthGen::getStartY() const
{
    return startY;
}

int LabyrinthGen::getExitX() const
{
    return exitX;
}

int LabyrinthGen::getExitY() const
{
    return exitY;
}

int LabyrinthGen::getWidth() const
{
    return width;
}

int LabyrinthGen::getHeight() const
{
    return height;
}

std::vector<int> LabyrinthGen::getUnvisitedNeighbours(int currCell,
                                                      const std::vector<int> &distances) const
{
    std::vector<int> neighbours;

    // currCell coords
    int x = currCell % width;
    int y = currCell / width;

    // North, if exists (ie. currCell isn't top row)
    if (y > 0)
    {
        int north = getIndex(x, y - 1);
        if (distances[north] == -1)
            neighbours.push_back(north);
    }

    // South, if exists
    if (y < height - 1)
    {
        int south = getIndex(x, y + 1);
        if (distances[south] == -1)
            neighbours.push_back(south);
    }

    // East, if exists
    if (x < width - 1)
    {
        int east = getIndex(x + 1, y);
        if (distances[east] == -1)
            neighbours.push_back(east);
    }

    // West, if exists
    if (x > 0)
    {
        int west = getIndex(x - 1, y);
        if (distances[west] == -1)
            neighbours.push_back(west);
    }

    return neighbours;
}

void LabyrinthGen::connectCells(int cellA, int cellB)
{
    // N = 1, S = 2, E = 4, W = 8

    // xDirection: x direction of B relative to A
    int xDirection = (cellB % width) - (cellA % width);
    // yDirection: y direction of B relative to A
    int yDirection = (cellB / width) - (cellA / width);

    if (yDirection == -1)
    {
        grid[cellA] |= 1;
        grid[cellB] |= 2;
    }
    else if (yDirection == 1)
    {
        grid[cellA] |= 2;
        grid[cellB] |= 1;
    }
    else if (xDirection == 1)
    {
        grid[cellA] |= 4;
        grid[cellB] |= 8;
    }
    else if (xDirection == -1)
    {
        grid[cellA] |= 8;
        grid[cellB] |= 4;
    }
}
