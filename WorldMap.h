#ifndef WORLDMAP_H_INCLUDED
#define WORLDMAP_H_INCLUDED

#include "LevelMap.h"

class WorldMap
{
private:
    int currentLevel;
    std::vector<LevelMap> levels;
public:
    WorldMap();
    // Loads a new map into the level vector, adding it to the game.
    void loadNewMap(int x, int y, std::vector<int> aF, std::unordered_map<int,std::vector<int>> aP);
    // Switches the current level to the level stored in position 'newLevel'.
    void changeCurrentLevel(int newLevel);
    // Returns the current level.
    int getCurrentLevel();
    // Returns the current number of levels in the game.
    int getMaxLevels();
    // The functions below function identically to calling the equivalent LevelMap function on levels[currentLevel].
    int getWidth();
    int getHeight();
    int getFloorType(int x, int y);
    std::vector<int> getPresence(int x, int y);
    void addPresence(int id, int x, int y);
    void removePresence(int id, int x, int y);
    void drawFloors(sf::RenderWindow* window);
    std::vector<int> entitiesInPlay();
    // Note: these two functions return the position of the level up and down stairs, respectively, as (x,y).
    sf::Vector2i getEntry();
    sf::Vector2i getExit();
    // This function adds a presence directly to a level of the user's choice, allowing entities to be spawned in early.
    void addDirectPresence(int id, int x, int y, int lv);
    // This function cleans out every map in the game, resetting the world map.
    void clear();
};

#endif // WORLDMAP_H_INCLUDED
