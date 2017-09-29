#include "ShadowSystem.h"

#include "PositionComponent.h"
#include "SightComponent.h"
#include "WorldMap.h"

extern std::unordered_map<int,PositionComponent> PositionList;
extern std::unordered_map<int,SightComponent> SightList;
extern WorldMap dungeonMap;

extern int coordToInt(int x, int y);
extern sf::Vector2i intToCoord(int i);

std::unordered_map<int,bool> shadowScan; // Holds the result of shadowcasting.

ShadowSystem::ShadowSystem() {}

// Casts light and shadow from an entity, determining what they can see.
void ShadowSystem::castShadow(int id)
{
    int vision = SightList[id].visionRange;
    sf::Vector2i entityPos(PositionList[id].xCoord,PositionList[id].yCoord); // Entity's current position.

    // Shadowcasting scans octants in eight directions.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,1,0,0,1); // North-west.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,-1,0,0,1); // North-east.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,0,1,1,0); // West-north.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,0,1,-1,0); // West-south.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,0,-1,1,0); // East-north.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,0,-1,-1,0); // East-south.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,1,0,0,-1); // South-west.
    scanOctant(1,vision,1.0f,0.0f,entityPos.x,entityPos.y,-1,0,0,-1); // South-east.

    // The vector of visible areas is assigned based on the shadow scan.
    std::vector<sf::Vector2i> canSee;
    canSee.push_back(entityPos);
    for (auto it = shadowScan.begin(); it != shadowScan.end(); ++it)
    {
        if (it->second) { canSee.push_back(intToCoord(it->first)); }
    }
    SightList[id].visionArea = canSee;
    shadowScan.clear(); // The storage map is cleaned out for the next scan.
}

// Scans an octant for shadow cast FOV.
void ShadowSystem::scanOctant(int depth, int maxDepth, float startSlope, float endSlope, int startX, int startY, int xx, int xy, int yx, int yy)
{
    sf::Vector2i currentTile; // Holds the current tile.
    // If we've gone past the end slope, we're done.
    if (startSlope < endSlope) { return; }
    float currentStart = startSlope; // Holds changes to the starting slope.
    float newStart = 0.0f; // Holds any changes to the starting scope we may have incurred through encountering walls.
    // The prevBlocked boolean will tell us when we have finished encountering a wall, allowing for recursion.
    bool prevBlocked = false;
    // While we have tiles to scan and have not encountered an obstacle;
    // For each different depth in our scan (only start a new depth if we didn't end blocked);
    for (int d = depth; ((d < maxDepth) && (!prevBlocked)); d++)
    {
        int deltaY = -d;
        // For each row in our scan;
        for (int deltaX = -d; deltaX <= 0; deltaX++)
        {
            // The current tile is determined based on the starting tile, our current progress in the scan, and the multipliers for scan direction.
            currentTile.x = startX + (deltaX * xx) + (deltaY * xy);
            currentTile.y = startY + (deltaX * yx) + (deltaY * yy);
            // The left and right slope for this row of scanning is generated.
            float leftSlope = (deltaX - 0.5f) / (deltaY + 0.5f);
            float rightSlope = (deltaX + 0.5f) / (deltaY - 0.5f);

            // If the currently scanned tile is outside of the map, its lighting process is skipped.
            if ((currentTile.x < 0) || (currentTile.y < 0) || (currentTile.x >= dungeonMap.getWidth()) || (currentTile.y >= dungeonMap.getHeight()) || (currentStart < rightSlope)) { continue; }
            else if (endSlope > leftSlope) { break; }

            // With the checks complete, light the tile.  (Give this some criteria if things go wrong?)
            shadowScan[coordToInt(currentTile.x,currentTile.y)] = true;

            // If we were blocked last tile;
            if (prevBlocked)
            {
                // If this tile is a wall;
                if (dungeonMap.getFloorType(currentTile.x,currentTile.y) == 1)
                {
                    newStart = rightSlope; // The start slope for the next scan is changed to compensate for the wall blocking vision.
                }
                // Otherwise;
                else
                {
                    currentStart = newStart; // Our starting slope is adjusted.
                    prevBlocked = false; // We can now go on with the scan.
                }
            }
            // Otherwise;
            else
            {
                // If this tile is a wall;
                if (dungeonMap.getFloorType(currentTile.x,currentTile.y) == 1)
                {
                    prevBlocked = true; // We are now blocked.
                    scanOctant(depth+1,maxDepth,currentStart,leftSlope,startX,startY,xx,xy,yx,yy); // A new recursive scan begins to compensate for the wall blocking vision.
                    newStart = rightSlope; // The start slope for the next scan is changed to compensate for the wall blocking vision.
                }
                // Otherwise, nothing special needs to happen.
            }
        }
    }
}
