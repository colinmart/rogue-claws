#include "RandomSystem.h"

#include "WorldMap.h"
#include "PositionComponent.h"
#include "SpawnerSystem.h"

extern WorldMap dungeonMap;
extern std::unordered_map<int,PositionComponent> PositionList;
extern SpawnerSystem SpawnSys;

RandomSystem::RandomSystem() {}

// Generates a random number between 'minN' and 'maxN'.
int RandomSystem::numberInRange(int minN, int maxN) { return minN + (rand() % (maxN - minN + 1)); }

// Generates a random dungeon level with width 'xlim', height 'ylim' and 'density' number of rooms, then adds it to the game.
void RandomSystem::makeDungeonLevel(int xlim, int ylim, int density)
{
    std::vector<int> level; // Holds the generated level.
    std::unordered_map<int,std::vector<int>> levelPresence; // Holds the entities generated to occupy the level.
    // Basic dungeon generator.  Will refine later.
    // To begin, the dungeon is filled with walls.
    for (int y = 0; y < ylim; y++)
    {
        for (int x = 0; x < xlim; x++)
        {
            level.push_back(1);
        }
    }

    // The minimum size of a dungeon room is a tenth of its total dimensions; the maximum size is a fifth of its total dimensions.
    int xmin = (xlim / 10); int xmax = (xlim / 5); // Minimum/maximum width of a room.
    int ymin = (ylim / 10); int ymax = (ylim / 5); // Minimum/maximum height of a room.

    // The rooms vector holds all the rooms as convenient rectangles, allowing us to spawn things inside of them.
    std::vector<sf::FloatRect> rooms;
    // Density directly affects the number of rooms that will be spawned.
    for (int d = 0; d < density; d++)
    {
            // Rooms cannot spawn on the edes of the level, and cannot extend past the edges.  This ensures there is a layer of walls about the room.
            int left = numberInRange(1,xlim-xmin-2); int top = numberInRange(1,ylim-ymin-2);
            // Otherwise, rooms can spawn anywhere on the level and be of width and height determined by the minimum and maximum ranges earlier established.
            sf::FloatRect newRoom(left,top,std::min(numberInRange(xmin,xmax),(xlim-left-1)),std::min(numberInRange(ymin,ymax),(ylim-top-1)));
            rooms.push_back(newRoom);
            // Open spaces are carved into the level's walls based on the dimensions of the created rooms.
            for (int x = newRoom.left; x < newRoom.left + newRoom.width; x++)
            {
                for (int y = newRoom.top; y < newRoom.top + newRoom.height; y++)
                {
                    level[x+(xlim*y)] = 0;
                }
            }
    }

    // Each room is connected to the room that was generated previously.
    for (unsigned int i = 1; i < rooms.size(); i++)
    {
        // The corridor-connecting system creates two corridors to connect rooms - a vertical corridor and a horizontal corridor.
        // These corridors each extend from a room's center and will connect in a right angle to connect the two rooms.
        // Each corridor has a start, an end, and a point they occupy on the opposing coordinate plane.
        int xStart, xEnd, xVert, yStart, yEnd, yHorz;
        // The centers of the two rooms to be connected are placed within the center1 and center2 vectors.  The algorithm favors the left and the top,
        // to help prevent corridors from being generated in the wrong place when a room is very small.  (Rooms generate from left to right and top to down.)
        sf::Vector2f center1(rooms[i].left + floor(rooms[i].width/2), rooms[i].top + floor(rooms[i].height/2));
        sf::Vector2f center2(rooms[i-1].left + floor(rooms[i-1].width/2), rooms[i-1].top + floor(rooms[i-1].height/2));
        // The vertical corridor starts at whichever room is above the other and descends to the lower room's center.
        // Its X coordinate is the same as the higher room's center.
        if (center1.y < center2.y) { yStart = center1.y; yEnd = center2.y; yHorz = center1.x; xVert = yEnd; }
        else { yStart = center2.y; yEnd = center1.y; yHorz = center2.x; xVert = yEnd; }
        // The horizontal corridor starts at whichever room is to the left of the other and proceeds to the rightmost room's center.
        // Its Y coordinate is the position where the vertical corridor ends, allowing the two to connect.
        if (center1.x < center2.x) { xStart = center1.x; xEnd = center2.x; }
        else { xStart = center2.x; xEnd = center1.x; }

        // Open corridors are carved into the level's walls for each.
        for (int y = yStart; y <= yEnd; y++) { level[yHorz+(xlim*y)] = 0; }
        for (int x = xStart; x < xEnd; x++) { level[x+(xlim*xVert)] = 0; }
    }

    // Each level has one of each kind of stairway.
    int enterX, enterY, exitX, exitY;
    // The up stairs can be generated on any tile in the first generated room.
    enterX = numberInRange(rooms[0].left,rooms[0].left+rooms[0].width-1);
    enterY = numberInRange(rooms[0].top,rooms[0].top+rooms[0].height-1);
    // The down stairs can be generated on any tile in the last generated room.
    exitX = numberInRange(rooms[rooms.size()-1].left,rooms[rooms.size()-1].left+rooms[rooms.size()-1].width-1);
    exitY = numberInRange(rooms[rooms.size()-1].top,rooms[rooms.size()-1].top+rooms[rooms.size()-1].height-1);
    level[enterX+(xlim*enterY)] = 2;
    level[exitX+(xlim*exitY)] = 3;

    // With generation complete, the level is loaded into the game.
    dungeonMap.loadNewMap(xlim,ylim,level,levelPresence);

    // To conclude, a random enemy has a chance to be spawned for each room, as does a random power-up.
    int thisLevel = dungeonMap.getMaxLevels();

    // Power-ups are spawned first.
    for (unsigned int i = 0; i < 5; i++)
    {
        int randRoom = numberInRange(0,rooms.size()-1);
        int spawnx = numberInRange(rooms[randRoom].left,rooms[randRoom].left+rooms[randRoom].width-1);
        int spawny = numberInRange(rooms[randRoom].top,rooms[randRoom].top+rooms[randRoom].height-1);
        bool isEmpty = true;
        // Power-ups can't spawn on walls or on the stairs.
        if (level[spawnx+(xlim*spawny)] == 0)
        {
            // This shows up in movementsystem too, so maybe put it into a function of Map?  That might make more sense.
            std::vector<int> presenceHolder = dungeonMap.getPresence(spawnx,spawny);
            for (unsigned int i = 0; i < presenceHolder.size(); ++i)
            {
                if (!PositionList[presenceHolder[i]].isSolid)
                {
                    isEmpty = false;
                    break;
                }
            }
        }
        else { isEmpty = false; }
        // If the space is unoccupied by other power-ups, spawn the power-up.
        if (isEmpty)
        {
            int randType;
            // The first three power-ups are stat increases; the last two are healing.
            if (i < 3)
            {
                // Equal chance of HP, ATK, or DEF gain.
                randType = (rand()%3) + 1;
                switch(randType)
                {
                case 1:
                    SpawnSys.spawnPowerup(spawnx,spawny,thisLevel,0,10,0,0,0);
                    break;
                case 2:
                    SpawnSys.spawnPowerup(spawnx,spawny,thisLevel,0,0,1,0,0);
                    break;
                case 3:
                    SpawnSys.spawnPowerup(spawnx,spawny,thisLevel,0,0,0,1,0);
                    break;
                default:
                    break;
                }
            }
            else
            {
                // 1/6 chance of PSI restore.  Otherwise, health restore.
                randType = (rand()%6);
                if (randType == 0) { SpawnSys.spawnPowerup(spawnx,spawny,thisLevel,0,0,0,0,1); }
                else { SpawnSys.spawnPowerup(spawnx,spawny,thisLevel,30,0,0,0,0); }
            }
        }
    }
    // Once power-ups spawn, spawn enemies.
    for (unsigned int i = 0; i < rooms.size(); i++)
    {
        int spawnx = numberInRange(rooms[i].left,rooms[i].left+rooms[i].width-1); int spawny = numberInRange(rooms[i].top,rooms[i].top+rooms[i].height-1);
        bool isEmpty = true;
        // This shows up in movementsystem too, so maybe put it into a function of Map?  That might make more sense.
        std::vector<int> presenceHolder = dungeonMap.getPresence(spawnx,spawny);
        // Enemies can't spawn on walls or on the stairs.
        if (level[spawnx+(xlim*spawny)] == 0)
        {
            for (unsigned int i = 0; i < presenceHolder.size(); ++i)
            {
                if (PositionList[presenceHolder[i]].isSolid)
                {
                isEmpty = false;
                break;
                }
            }
        }
        else { isEmpty = false; }
        // If the space is unoccupied, spawn the enemy.
        if (isEmpty) { SpawnSys.spawnEnemy(spawnx,spawny,thisLevel); }
    }
}
