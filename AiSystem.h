#ifndef AISYSTEM_H_INCLUDED
#define AISYSTEM_H_INCLUDED

// Deals with decisions made by non-player characters inside the game.

class AiSystem
{
public:
    AiSystem();
    // Performs an appropriate AI behavior.
    void enemyAction(int id);
    // Causes an enemy to attack the player if they are in range.  Returns if an attack was made.
    bool attackPlayer(int id);
    // Causes an enemy to move towards the player's last known location with basic X and Y-axis alignment.
    void seekPlayer(int id);
    // Causes an enemy to move towards the player's last known location using A*.
    void chasePlayer(int id);
    // Causes an enemy to move towards the player's last known location using A*, or cast a psychic power if they can.
    void enemyPsi(int id);
    // Runs the A& algorithm from the entity's position to a point and returns the result.
    int aStar(int id);
};

#endif // AISYSTEM_H_INCLUDED
