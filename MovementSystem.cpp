#include "MovementSystem.h"

#include "PositionComponent.h"
#include "WorldMap.h"

#include "CombatSystem.h"

extern std::unordered_map<int,PositionComponent> PositionList;
extern WorldMap dungeonMap;
extern CombatSystem CombatSys;

MovementSystem::MovementSystem() {}

// Moves entity 'id' 'xmove' spaces on the X-axis and 'ymove' spaces on the Y-axis, and returns true.
// If that space is blocked, does nothing and returns false.
// Doesn't care about pathing; if there's no path to an open tile, moveEntity will 'teleport' the entity there.
bool MovementSystem::moveEntity(int id, int xmove, int ymove)
{
    // Oldx and oldy record the previous position of the moving entity, allowing us to operate on their position after undertaking the move.
    int oldx = PositionList[id].xCoord;
    int oldy = PositionList[id].yCoord;
    // Nex and newy record the prospective position of the moving entity should it successfully move.
    int newx = oldx + xmove;
    int newy = oldy + ymove;
    // isEmpty will record whether the prospective position is unoccupied (and therefore, can be moved to).
    bool isEmpty = true;
    // If the tile type of the prospective location is 1, it's a solid wall (and can't be moved onto).
    if (dungeonMap.getFloorType(newx,newy) == 1)
    {
        isEmpty = false;
    }
    // If the tile type is not solid, it may still have a solid entity on it which prevents it from moving.
    else
    {
        // If any presence already on the tile is solid, then it cannot be moved onto, as that entity occupies the space.
        std::vector<int> presenceHolder = dungeonMap.getPresence(newx,newy);
        for (unsigned int i = 0; i < presenceHolder.size(); ++i)
        {
            if (PositionList[presenceHolder[i]].isSolid)
            {
              isEmpty = false;
              // Under current rules, an entity will attack another entity whose space it wants to move into.
              CombatSys.fight(id,presenceHolder[i]);
              break;
            }
        }
    }
    // If the tile is empty and contains no solid entity, then it can be properly moved onto.
    if (isEmpty)
    {
        // The entity's position is set to the new location.
        PositionList[id].xCoord = newx;
        PositionList[id].yCoord = newy;
        // The entity's presence is removed from the previous tile and added onto the new tile.
        dungeonMap.removePresence(id,oldx,oldy);
        dungeonMap.addPresence(id,newx,newy);
    }
    // The isEmpty boolean is returned, indicating whether or not the move succeeded.
    return isEmpty;
}

