#include "LevelMap.h"

extern int tileSize;
#define TILEWIDTH 10
#include <math.h>

// The graphics cipher will take an 8-bit code for a tile (N,NE,E,SE,S,SW,W,NW) where any adjacent tile of a differing type is (1),
// and output the correct graphic for that tile.
std::unordered_map<int,int> graphicCipher = {
    {1000,0}, {111000,1}, {111110,2}, {1110,3}, {101000,4}, {101010,5}, {1010,6}, {11101111,7}, {11101011,8}, {11111011,9},
    {10001000,10}, {11111000,11}, {11111111,12}, {10001111,13}, {10101000,14}, {0,15}, {10001010,16}, {10101111,17}, {10101010,18}, {11111010,19},
    {10000000,20}, {11100000,21}, {11100011,22}, {10000011,23}, {10100000,24}, {10100010,25}, {10000010,26}, {10111111,27}, {11101011,28}, {11111110,29},
    {2,30}, {100000,31}, {100010,32}, {10,33}, {101110,34}, {10001011,35}, {11101000,36}, {111010,37}, {10101010,38}, {10101011,39},
    {3,40}, {4,41}, {1000100,42}, {11111010,43}, {10111000,44}, {11100010,45}, {10100011,46}, {10001110,47}, {10101110,48}, {10111010,49},
    {11100010, 25}, {10100011, 26}, {11111010, 29}, {1010010, 45}, {10100010,46}, {11101110, 28}
};


LevelMap::LevelMap()
{
    // All items are initialized to base values.
    xLimit = 0; yLimit = 0; entryPoint = 0; exitPoint = 0;
    floorTileset.loadFromFile("tiles/caveTiles.png");
    floorGraphic.setPrimitiveType(sf::Quads);
    floorGraphic.resize(xLimit*yLimit*4);
}

// Initializes the level map by loading in pregenerated tiles and presences.
void LevelMap::loadMap(int x, int y, std::vector<int> aF, std::unordered_map<int,std::vector<int>> aP)
{
    xLimit = x; yLimit = y; areaFloors = aF; areaPresence = aP;
    floorGraphic.resize(xLimit*yLimit*4);
    entryPoint = find(areaFloors.begin(), areaFloors.end(), 2) - areaFloors.begin();
    exitPoint = find(areaFloors.begin(), areaFloors.end(), 3) - areaFloors.begin();
    // Failsafe values in case something goes wrong in map generation.
    if (entryPoint >= areaFloors.size()) { areaFloors[0] = 2; entryPoint = 0; }
    if (exitPoint >= areaFloors.size()) { areaFloors[1] = 3; entryPoint = 1; }
    makeMapGraphic(); // The graphic for the loaded-in map is generated.
}

// Generates a graphic for the level map based on its tiles.
void LevelMap::makeMapGraphic()
{
    bool north, east, south, west;
    // For each tile in the area graphic:
    for (int y = 0; y < yLimit; y++)
    {
        for (int x = 0; x < xLimit; x++)
        {
            int binCode = 0;
            int tileType = getFloorType(x,y);
            // If the tile that needs a graphic is a wall, an eight-digit binary code is generated for it based on which of the tiles nearby are also walls.
            if (tileType == 1)
            {
                north = false; east = false; south = false; west = false;
                if (getFloorType(x,y-1) == tileType) { binCode += 10000000; north = true; } // N
                if (getFloorType(x+1,y) == tileType) { binCode += 100000; east = true; } // E
                if (getFloorType(x,y+1) == tileType) { binCode += 1000; south = true; } // S
                if (getFloorType(x-1,y) == tileType) { binCode += 10; west = true; } // W
                // Corners are only relevant if both bordering the corner are also walls.
                if (north && west && getFloorType(x-1,y-1) == tileType) { binCode += 1; } // NW
                if (north && east && getFloorType(x+1,y-1) == tileType) { binCode += 1000000; } // NE
                if (south && west && getFloorType(x-1,y+1) == tileType) { binCode += 100; } // SW
                if (south && east && getFloorType(x+1,y+1) == tileType) { binCode += 10000; } // SE
            }
            // All other tile-types have their own unique graphic that is used regardless of their position relative to other tiles.
            else
            {
                switch(tileType)
                {
                case 0:
                    binCode = 2;
                    break;
                case 2:
                    binCode = 3;
                    break;
                case 3:
                    binCode = 4;
                    break;
                default:
                    binCode = 2;
                    break;
                }
            }
            areaTileCode[x+(xLimit*y)] = binCode;
        }
    }
    // With the appropriate code determined, each tile has its graphic generated.
    for (int y = 0; y < yLimit; y++)
    {
        for (int x = 0; x < xLimit; x++)
        {
            // The X and Y coordinate of the tile's graphic in the texture is retrieved from the graphic cipher using the binary code.
            int tileNumber = graphicCipher[areaTileCode[x+(xLimit*y)]];
            int tx = tileNumber % TILEWIDTH;
            int ty = tileNumber / TILEWIDTH;

            // With the coordinate retrieved, a quad is formed at the tile's position, and textured with the appropriate graphic.
            sf::Vertex* quad = &floorGraphic[(x+(xLimit*y))*4];

            quad[0].position = sf::Vector2f(x * tileSize, y * tileSize);
            quad[1].position = sf::Vector2f((x + 1) * tileSize, y * tileSize);
            quad[2].position = sf::Vector2f((x + 1) * tileSize, (y + 1) * tileSize);
            quad[3].position = sf::Vector2f(x * tileSize, (y + 1) * tileSize);

            quad[0].texCoords = sf::Vector2f(tx * tileSize, ty * tileSize);
            quad[1].texCoords = sf::Vector2f((tx + 1) * tileSize, ty * tileSize);
            quad[2].texCoords = sf::Vector2f((tx + 1) * tileSize, (ty + 1) * tileSize);
            quad[3].texCoords = sf::Vector2f(tx * tileSize, (ty + 1) * tileSize);
        }
    }
}

// Returns the dimensions of the map.
int LevelMap::getWidth() { return xLimit; }
int LevelMap::getHeight() { return yLimit; }

// Gets the type of tile located at (x,y).
int LevelMap::getFloorType(int x, int y)
{
    int mapLoc = (x+(yLimit*y));
    if ((x < 0) || (x >= xLimit) || (y < 0) || (y >= yLimit)) { return 1; }
    else { return areaFloors[mapLoc]; }
}

 // Gets all the entities located at (x,y).
std::vector<int> LevelMap::getPresence(int x, int y)
{
    if ((x < 0) || (x >= xLimit) || (y < 0) || (y >= yLimit))
        {
            std::vector<int> novector;
            return novector;
        }
    else { return areaPresence[x+(xLimit*y)]; }
}

// Adds an entity's presence to (x,y).
void LevelMap::addPresence(int id, int x, int y) { areaPresence[x+(xLimit*y)].push_back(id); }
// Removes an entity's presence from (x,y).
void LevelMap::removePresence(int id, int x, int y) { areaPresence[x+(xLimit*y)].erase(
    std::remove(areaPresence[x+(xLimit*y)].begin(), areaPresence[x+(xLimit*y)].end(), id), areaPresence[x+(xLimit*y)].end()); }
//int LevelMap::getPresenceCount() { return areaPresence.bucket_count(); }
// Draws the graphic for the level onto window 'window'.
void LevelMap::drawFloors(sf::RenderWindow* window) { window->draw(floorGraphic,&floorTileset); }
// Returns the location of the level's up stairs in the tile vector.
int LevelMap::getEntry() { return entryPoint; }
// Returns the location of th elevel's down stairs in the tile vector.
int LevelMap::getExit() { return exitPoint; }
// Sets the location of the previous level's down stairs.
void LevelMap::setPrevExit(int exit) { prevExit = exit; }
// Sets the location of the next level's up stairs.
void LevelMap::setNextEntry(int entry) { nextEntry = entry; }
// Returns the level's x-limit.  Useful for dealing directly with the vectors in the level.
int LevelMap::getLimits() { return xLimit; }
// Returns every entity on the level.  Useful for iterating over them all at once.
std::vector<int> LevelMap::entitiesInPlay()
{
    // I should find some way to only alter this list when an entity is actually placed on or taken off of it.
    std::vector<int> presentEntities;
    for (auto it = areaPresence.begin(); it != areaPresence.end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); i++)
        {
            presentEntities.push_back(it->second[i]);
        }
    }
    return presentEntities;
}
