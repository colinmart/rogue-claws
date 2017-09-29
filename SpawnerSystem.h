#ifndef SPAWNERSYSTEM_H_INCLUDED
#define SPAWNERSYSTEM_H_INCLUDED

// Deals with creating new entities on the map.

class SpawnerSystem
{
public:
    SpawnerSystem();
    // Spawns in the player character at (x,y).  For use in initialization.
    void spawnPlayer(int x, int y);
    // Spawns a powerup.
    void spawnPowerup(int x, int y, int level, int hr, int hg, int ag, int dg, int pr);
    // Spawns a random enemy, based on the current depth.
    void spawnEnemy(int x, int y, int level);
    // Spawns a Brute Splicer.  Charges and strikes the player.
    void spawnBrute(int x, int y, int level);
    // Spawns an Abaddon Mercenary.  Charges the player, then fires on them from a safe distance.
    void spawnGunner(int x, int y, int level);
    // Spawns a Healing Splicer.  Weak attacks and poor vision, but high health and regenerates very quickly.
    void spawnHealer(int x, int y, int level);
    // Spawns an Autocannon.  Automatic turret can't move, but will fire on the player for high damage if they get in range.
    void spawnTurret(int x, int y, int level);
    // Spawns an Abaddon Exterminator.  While weak, they abuse their high range to rain bullets.
    void spawnSniper(int x, int y, int level);
    // Spawns a Stumbling Splicer.  Poor AI is stuck easily on walls, but an extremely dangerous opponent if they reach their target.
    void spawnHeavy(int x, int y, int level);
    // Spawns an Abaddon Operative.  Typically perish in one blow, but will deal massive damage if even a single strike is allowed to land.
    void spawnNinja(int x, int y, int level);
    // Spawns a Psychic Splicer.  Incredibly dangerous enemy that can use Mind Blasts.  Fortunately very rare.
    void spawnElite(int x, int y, int level);
};

#endif // SPAWNERSYSTEM_H_INCLUDED
