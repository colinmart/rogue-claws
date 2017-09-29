#include "CastingSystem.h"

#include "PositionComponent.h"
#include "PsiComponent.h"
#include "WorldMap.h"
#include "CombatSystem.h"

extern std::unordered_map<int,PositionComponent> PositionList;
extern std::unordered_map<int,PsiComponent> PsiList;
extern WorldMap dungeonMap;
extern std::vector<std::string> messageLog;

extern CombatSystem CombatSys;

CastingSystem::CastingSystem() {}

// Causes the entity 'id' to fire a Mind Blast in direction 'dir'.
// 0 = N, 1 = E, 2 = S, 3 = W.
void CastingSystem::mindBlast(int id, int dir)
{
    // If PSI is charged;
    if (PsiList[id].chargingPsi)
    {
        // Casting will use a charge.
        if (PsiList[id].psiCharges < 1) { return; }
        else { PsiList[id].psiCharges--; }
        bool xLine; // If true, fires on X-axis.  If false, fires on Y-axis.
        int shotDir; // Fires south/east if positive, north/west if negative.
        switch(dir)
        {
        case 0: // North.
            xLine = false;
            shotDir = -1;
            break;
        case 1: // East.
            xLine = true;
            shotDir = 1;
            break;
        case 2: // South.
            xLine = false;
            shotDir = 1;
            break;
        default: // West.
            xLine = true;
            shotDir = -1;
            break;
        }
        sf::Vector2i firingPos(PositionList[id].xCoord,PositionList[id].yCoord); // Position to fire from.
        // Blast away at everything in a line until a wall is struck.
        bool hitWall = false;
        messageLog.push_back("FWOOOOOM!");
        while (!hitWall)
        {
            // The mind blast progresses one square.
            if (xLine) { firingPos.x += shotDir; }
            else { firingPos.y += shotDir; }
            // If the current tile is solid wall, stop firing.
            if (dungeonMap.getFloorType(firingPos.x,firingPos.y) == 1)
            {
                hitWall = true;
                break;
            }
            // If the tile type is not solid, blast anything standing in it.
            else
            {
                std::vector<int> presenceHolder = dungeonMap.getPresence(firingPos.x,firingPos.y);
                for (unsigned int i = 0; i < presenceHolder.size(); ++i)
                {
                    if (PositionList[presenceHolder[i]].isSolid)
                    {
                        CombatSys.blast(id,presenceHolder[i]);
                    }
                }
            }
        }
        // PSI is no longer charging.
        PsiList[id].chargingPsi = false;
    }
    // Otherwise, charge PSI.
    else
    {
        PsiList[id].chargingPsi = true;
    }
}
