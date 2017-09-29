#include "WorldMap.h"
#include "LevelMap.h"

WorldMap::WorldMap() { currentLevel = 0; }

// Loads a new map into the level vector, adding it to the game.
void WorldMap::loadNewMap(int x, int y, std::vector<int> aF, std::unordered_map<int,std::vector<int>> aP)
{
    LevelMap newMap;
    newMap.loadMap(x,y,aF,aP);
    levels.push_back(newMap);
    int s = levels.size() - 1;
    if (s != 0)
    {
        levels[s].setPrevExit(levels[s-1].getExit());
        levels[s-1].setNextEntry(levels[s].getEntry());
    }
}

// Returns the level's x-limit.  Useful for dealing directly with the vectors in the level.
void WorldMap::changeCurrentLevel(int newLevel) { currentLevel = newLevel; }
// Returns the current level.
int WorldMap::getCurrentLevel() { return currentLevel; }
// Returns the current number of levels in the game.
int WorldMap::getMaxLevels() { return levels.size() - 1; }

// The functions below function identically to calling the equivalent LevelMap function on levels[currentLevel].
// (In fact, that's precisely what they do.)

int WorldMap::getWidth() { return levels[currentLevel].getWidth(); }
int WorldMap::getHeight() { return levels[currentLevel].getHeight(); }
int WorldMap::getFloorType(int x, int y) { return levels[currentLevel].getFloorType(x,y); }
std::vector<int> WorldMap::getPresence(int x, int y) { return levels[currentLevel].getPresence(x,y); }
void WorldMap::addPresence(int id, int x, int y) {levels[currentLevel].addPresence(id,x,y); }
void WorldMap::removePresence(int id, int x, int y) { levels[currentLevel].removePresence(id,x,y); }
void WorldMap::drawFloors(sf::RenderWindow* window) { levels[currentLevel].drawFloors(window); }
sf::Vector2i WorldMap::getEntry()
{
    int xLimit = levels[currentLevel].getLimits();
    int e = levels[currentLevel].getEntry();
    sf::Vector2i entryVector(e%xLimit,e/xLimit);
    return entryVector;
}
sf::Vector2i WorldMap::getExit()
{
    int xLimit = levels[currentLevel].getLimits();
    int e = levels[currentLevel].getExit();
    sf::Vector2i exitVector(e%xLimit,e/xLimit);
    return exitVector;
}
void WorldMap::addDirectPresence(int id, int x, int y, int lv) {levels[lv].addPresence(id,x,y); }
std::vector<int> WorldMap::entitiesInPlay() { return levels[currentLevel].entitiesInPlay(); }

// Cleans out every map in the game, resetting the world map.
void WorldMap::clear() { levels.clear(); currentLevel = 0; }
