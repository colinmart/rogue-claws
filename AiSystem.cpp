#include "AiSystem.h"

#include <queue>

#include "PositionComponent.h"
#include "StatusComponent.h"
#include "BrainComponent.h"
#include "SightComponent.h"
#include "PsiComponent.h"
#include "WorldMap.h"
#include "MovementSystem.h"
#include "CombatSystem.h"
#include "CastingSystem.h"

extern std::unordered_map<int,PositionComponent> PositionList;
extern std::unordered_map<int,StatusComponent> StatusList;
extern std::unordered_map<int,BrainComponent> BrainList;
extern std::unordered_map<int,SightComponent> SightList;
extern std::unordered_map<int,PsiComponent> PsiList;
extern MovementSystem MoveSys;
extern CombatSystem CombatSys;
extern CastingSystem CastingSys;
extern WorldMap dungeonMap;
extern int mapWidth;
extern sf::Vector2f playerPos;

extern int coordToInt(int x, int y);
extern sf::Vector2i intToCoord(int i);

AiSystem::AiSystem() {}

// Checks whether a friendly is in a given square.
bool checkFreeToMove(int x, int y)
{
    bool canMove = true;
    // Floor type 1 is solid.
    if (dungeonMap.getFloorType(x,y) == 1) { canMove = false; }
    return canMove;
}

// Performs an appropriate action for the chosen enemy.
void AiSystem::enemyAction(int id)
{
    switch (BrainList[id].aiType)
    {
    case 0:
        attackPlayer(id);
        break;
    case 1:
        seekPlayer(id);
        break;
    case 2:
        chasePlayer(id);
        break;
    case 3:
        enemyPsi(id);
        break;
    default:
        break;
    }
}

// Causes entity 'id' to attack the player if they are in range.  Returns if an attack was made.
bool AiSystem::attackPlayer(int id)
{
    // If an enemy is in range of the player, they'll attack.
    sf::Vector2i myPos(PositionList[id].xCoord,PositionList[id].yCoord);
    sf::Vector2i knownPosition(BrainList[id].lastKnownX,BrainList[id].lastKnownY);
    sf::Vector2i rangeToPlayer(abs(myPos.x-playerPos.x),abs(myPos.y-playerPos.y));
    if ( ( ((rangeToPlayer.x <= StatusList[id].range) && rangeToPlayer.y == 0) || ((rangeToPlayer.y <= StatusList[id].range) && rangeToPlayer.x == 0) ) &&
        (std::find(SightList[id].visionArea.begin(), SightList[id].visionArea.end(), sf::Vector2i(playerPos.x,playerPos.y)) != SightList[id].visionArea.end()) )
    {
        CombatSys.fight(id,0);
        return true;
    }
    return false;
}

// Causes entity 'id' to move towards the player's last known location with basic X and Y-axis alignment.
void AiSystem::seekPlayer(int id)
{
    // If an enemy is in range of the player, they'll attack.
    if (attackPlayer(id)) { return; }
    sf::Vector2i myPos(PositionList[id].xCoord,PositionList[id].yCoord);
    sf::Vector2i knownPosition(BrainList[id].lastKnownX,BrainList[id].lastKnownY);
    // The enemy will pursue the player if they currently know where they are.
    if ((knownPosition.x >= 0) && (knownPosition.y >= 0))
    {
        // If another enemy already made it there, and lost sight of the player, they'll tell their friends to stop chasing.
        bool pursue = true;
        std::vector<int> presenceHolder = dungeonMap.getPresence(knownPosition.x,knownPosition.y);
        for (unsigned int i = 0; i < presenceHolder.size(); ++i)
        {
            // All solid entities with an ID other than 0 are enemies
            if ((PositionList[presenceHolder[i]].isSolid) && (presenceHolder[i] != 0))
            {
                if ((BrainList[presenceHolder[i]].lastKnownX == -1) || (BrainList[presenceHolder[i]].lastKnownY == -1)) { pursue = false; }
                break;
            }
        }
        // If the player still needs to be pursued;
        if (pursue)
        {
            int xDisp = knownPosition.x - myPos.x;
            int yDisp = knownPosition.y - myPos.y;
            if (xDisp != 0) { xDisp = xDisp / abs(xDisp); }
            if (yDisp != 0) { yDisp = yDisp / abs(yDisp); }
            sf::Vector2i nextStep(myPos.x+xDisp,myPos.y+yDisp);
            // AI will not trample over their own allies to reach the player; they're more polite than that!
            bool trample = false;
            // To that end, the location to move into is checked for allies.
            presenceHolder = dungeonMap.getPresence(nextStep.x,nextStep.y);
            for (unsigned int i = 0; i < presenceHolder.size(); ++i)
            {
                // All solid entities with an ID other than 0 are enemies
                if ((PositionList[presenceHolder[i]].isSolid) && (presenceHolder[i] != 0))
                {
                    trample = true;
                    break;
                }
            }
            // Assuming nobody will be trampled, the AI will move in pursuit of the player.
            if (!trample)
            {
                if ((yDisp != 0) && (checkFreeToMove(myPos.x,myPos.y+yDisp))) { MoveSys.moveEntity(id,0,yDisp); }
                else { MoveSys.moveEntity(id,xDisp,0); }
            }
            // If the enemy reached the player's last known location and can no longer see them, they will stop pursuing.
            if ((nextStep.x == knownPosition.x) && (nextStep.y == knownPosition.y)) { BrainList[id].lastKnownX = -1; BrainList[id].lastKnownY = -1; }
        }
    }
}

// Causes entity 'id' to move towards the player's last known location using A*.
void AiSystem::chasePlayer(int id)
{
    // If an enemy is in range of the player, they'll attack.
    if (attackPlayer(id)) { return; }
    sf::Vector2i myPos(PositionList[id].xCoord,PositionList[id].yCoord);
    sf::Vector2i knownPosition(BrainList[id].lastKnownX,BrainList[id].lastKnownY);
    // The enemy will pursue the player if they currently know where they are.
    if ((knownPosition.x >= 0) && (knownPosition.y >= 0))
    {
        // If another enemy already made it there, and lost sight of the player, they'll tell their friends to stop chasing.
        bool pursue = true;
        std::vector<int> presenceHolder = dungeonMap.getPresence(knownPosition.x,knownPosition.y);
        for (unsigned int i = 0; i < presenceHolder.size(); ++i)
        {
            // All solid entities with an ID other than 0 are enemies
            if ((PositionList[presenceHolder[i]].isSolid) && (presenceHolder[i] != 0))
            {
                if ((BrainList[presenceHolder[i]].lastKnownX == -1) || (BrainList[presenceHolder[i]].lastKnownY == -1)) { pursue = false; }
                break;
            }
        }
        // If the player still needs to be pursued;
        if (pursue)
        {
            // Knowledge of the entity's current location allows it to determine the path needed when chasing down its target.
            sf::Vector2i nextStep = intToCoord(aStar(id));
            // AI will not trample over their own allies to reach the player; they're more polite than that!
            bool trample = false;
            // To that end, the location to move into is checked for allies.
            presenceHolder = dungeonMap.getPresence(nextStep.x,nextStep.y);
            for (unsigned int i = 0; i < presenceHolder.size(); ++i)
            {
                // All solid entities with an ID other than 0 are enemies
                if ((PositionList[presenceHolder[i]].isSolid) && (presenceHolder[i] != 0))
                {
                    trample = true;
                    break;
                }
            }
            // Assuming nobody will be trampled, the AI will move in pursuit of the player.
            if (!trample)
            {
                int xDisp = nextStep.x - myPos.x;
                int yDisp = nextStep.y - myPos.y;
                if (xDisp != 0) { xDisp = xDisp / abs(xDisp); }
                if (yDisp != 0) { yDisp = yDisp / abs(yDisp); }
                MoveSys.moveEntity(id,xDisp,yDisp);
            }
            // If the enemy reached the player's last known location and can no longer see them, they will stop pursuing.
            if ((nextStep.x == knownPosition.x) && (nextStep.y == knownPosition.y)) { BrainList[id].lastKnownX = -1; BrainList[id].lastKnownY = -1; }
        }
    }
}

// Causes entity 'id' to move towards the player's last known location using A*, or cast a psychic power if they can.
void AiSystem::enemyPsi(int id)
{
    sf::Vector2i myPos(PositionList[id].xCoord,PositionList[id].yCoord);
    sf::Vector2i knownPosition(BrainList[id].lastKnownX,BrainList[id].lastKnownY);
    // If the enemy has charges left, they'll try to cast offensive PSI at the player.
    if (PsiList[id].psiCharges > 0)
    {
        // If the enemy is charging PSI, they'll cast it; otherwise they'll aim it and start charging.
        if (PsiList[id].chargingPsi)
        {
            // The enemy will shoot on whichever axis is closest to them, and in the direction of the player.  Y-axis is favored.
            bool xLine = false;
            int xDiff = knownPosition.x - myPos.x;
            int yDiff = knownPosition.y - myPos.y;
            if (abs(yDiff) < abs(xDiff)) { xLine = true; }
            if (xLine)
            {
                if (xDiff > 0) { CastingSys.mindBlast(id,1); }
                else { CastingSys.mindBlast(id,3); }
            }
            else
            {
                if (yDiff > 0) { CastingSys.mindBlast(id,2); }
                else { CastingSys.mindBlast(id,0); }
            }
        }
        else
        {
            // Mind Blast has infinite range but requires line of sight.  (Though educated guesses can be taken by the player.)
            sf::Vector2i realPosition(PositionList[0].xCoord,PositionList[0].yCoord);
            // If the enemy can currently see the player and is lined up with them, they'll try to take a shot at them.
            if ((realPosition.x == knownPosition.x) && (realPosition.y == knownPosition.y) &&
                ((myPos.x == knownPosition.x) || (myPos.y == knownPosition.y))) { CastingSys.mindBlast(id,-1); }
            // Otherwise, they'll just chase them down as normal.
            else { chasePlayer(id); }
        }
    }
    // Otherwise, they'll just run at them as normal.
    else { chasePlayer(id); }
}

// The heuristic for A*.  Uses Manhattan Distance.
int heuristic(int next, int goal)
{
    sf::Vector2i pointOne = intToCoord(next);
    sf::Vector2i pointTwo = intToCoord(goal);
    return abs(pointOne.x - pointTwo.x) + abs(pointOne.y - pointTwo.y);
}

// Custom class for storage of integer and heuristic value.
class tileNode
{
public:
    int value;
    int priority;
    tileNode() { value = 0; priority = 0; }
    tileNode(int v, int p) { value = v; priority = p; }
};

// Custom comparator for the priority queue.  Compares heuristic values.
struct heuristicCompare
{
    bool operator() (const tileNode& left, const tileNode& right) const
    {
        return left.priority > right.priority;
    }
};

// Runs the A* algorithm from the entity's position to a point and returns the result.
int AiSystem::aStar(int id)
{
    // The queue for storage of new nodes.  First int is value, second int is heuristic value.
    std::priority_queue<tileNode,std::vector<tileNode>,heuristicCompare> nodeQueue;
    std::unordered_map<int,int> previousNode; // Holds the node that comes before each node in our current path.
    std::unordered_map<int,int> nodeCost; // Holds the cost associated with moving to each node.
    int currentNode = 0; // Holds the currently iterated node in single int value.
    int nextNode = 0; // Holds the node we currently plan on reaching.
    int newCost = 0; // Holds the cost of a new node traversal.
    int goalNode = coordToInt(BrainList[id].lastKnownX,BrainList[id].lastKnownY); // Holds the value of the node we are trying to reach.
    int firstNode = 0; // The first node, used to track which node is second.
    sf::Vector2i nodeConvert; // Holds the result of nodes converted to coordinates.

    // The first A* node, the entity's current position, is pushed in.
    firstNode = coordToInt(PositionList[id].xCoord,PositionList[id].yCoord);
    nodeQueue.push(tileNode(firstNode,0));
    previousNode[firstNode] = -1;
    nodeCost[firstNode] = 0;

    int safety = 0; // In case a target is blocked off, the search will stop once it's unsuccessfully checked 999 nodes.
    // While there are still nodes in the queue;
    while (!nodeQueue.empty())
    {
        safety++;
        if (safety > 999) { break; }
        currentNode = nodeQueue.top().value; nodeQueue.pop(); // Pop the most important node.
        // If it's the node we're trying to reach, we've finished.
        if (currentNode == goalNode) { break; }

        std::vector<int> nextTiles; // The nextTiles vector holds all legitimate possibilities for the next tile to enter.
        nodeConvert = intToCoord(currentNode);
        if (checkFreeToMove(nodeConvert.x+1,nodeConvert.y)) { nextTiles.push_back(currentNode+1); } // East tile
        if (checkFreeToMove(nodeConvert.x-1,nodeConvert.y)) { nextTiles.push_back(currentNode-1); } // West tile
        if (checkFreeToMove(nodeConvert.x,nodeConvert.y+1)) { nextTiles.push_back(currentNode+mapWidth); } // South tile
        if (checkFreeToMove(nodeConvert.x,nodeConvert.y-1)) { nextTiles.push_back(currentNode-mapWidth); } // North tile

        // For all tiles that could potentially lead to the goal;
        for (unsigned int t = 0; t < nextTiles.size(); t++)
        {
            nextNode = nextTiles[t];
            newCost = nodeCost[currentNode] + 1; // Moving to any adjacent node costs 1.
            // If we haven't been to this tile yet, or we can go here on the current path for cheaper than we could before;
            if ((nodeCost.find(nextNode) == nodeCost.end()) || (newCost < nodeCost[nextNode]))
            {
                nodeCost[nextNode] = newCost; // If we've found a new node or a shorter path, its cost is updated.
                nodeQueue.push(tileNode(nextNode,newCost+heuristic(nextNode,goalNode))); // The new node is added to the queue of nodes to investigate.
                previousNode[nextNode] = currentNode; // The node we came to this node from for our path is updated.
            }
        }
    }
    // With the path found, we can now backtrace it to discover the most efficient route.

    if (safety > 999) { return firstNode; } // If something went wrong, just don't move.

    int traversalNode = goalNode; // The node currently being traversed.
    int backNode = previousNode[goalNode]; // The previous node to the traversal node.

    safety = 0;
    // While iterating over the path is still in progress;
    while ((backNode > -1) && (backNode != firstNode))
    {
        safety++;
        if (safety > 999) { break; }
        traversalNode = backNode;
        backNode = previousNode[traversalNode];
    }

    // When finished, we return the value we care about; as it happens, the next node we need to step on to complete our path.
    if (backNode != -1) { return traversalNode; }
    else { return firstNode; }
}
