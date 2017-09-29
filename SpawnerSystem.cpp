#include "SpawnerSystem.h"

#include "WorldMap.h"
#include "TextureContainer.h"
#include "PositionComponent.h"
#include "StatusComponent.h"
#include "SpriteComponent.h"
#include "BrainComponent.h"
#include "SightComponent.h"
#include "PowerupComponent.h"
#include "PsiComponent.h"

extern WorldMap dungeonMap;
extern TextureContainer globalTextures;
extern std::unordered_map<int,PositionComponent> PositionList;
extern std::unordered_map<int,SpriteComponent> SpriteList;
extern std::unordered_map<int,StatusComponent> StatusList;
extern std::unordered_map<int,BrainComponent> BrainList;
extern std::unordered_map<int,SightComponent> SightList;
extern std::unordered_map<int,PowerupComponent> PowerupList;
extern std::unordered_map<int,PsiComponent> PsiList;

extern int nextID;

SpawnerSystem::SpawnerSystem() {}

// Spawns in the player character at (x,y).  For use in initialization.
void SpawnerSystem::spawnPlayer(int x, int y)
{
    PositionComponent newPosition;
    newPosition.setComponent(0,x,y,true);
    PositionList[0] = newPosition;
    dungeonMap.addPresence(0,x,y);
    SpriteComponent newSprite;
    newSprite.setComponent(0,&globalTextures.charaTextures[0],0,0,0,0);
    SpriteList[0] = newSprite;
    StatusComponent newStatus;
    newStatus.setComponent(0,"Cress",50,10,5,5,1);
    StatusList[0] = newStatus;
    SightComponent newSight;
    newSight.setComponent(0,10);
    SightList[0] = newSight;
    PsiComponent newPsi;
    newPsi.setComponent(0,3);
    PsiList[0] = newPsi;
}

// Spawns a powerup.
void SpawnerSystem::spawnPowerup(int x, int y, int level, int hr, int hg, int ag, int dg, int pr)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,false);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);

    SpriteComponent newSprite;
    // The sprite of the power-up is determined by the stats it raises.
    int pwrSprite;
    if (hr > 0) { pwrSprite = 0; }
    else if (hg > 0 ) {pwrSprite = 32; }
    else if (ag > 0 ) {pwrSprite = 64; }
    else if (dg > 0 ) {pwrSprite = 96; }
    else if (pr > 0 ) {pwrSprite = 128; }
    newSprite.setComponent(nextID,&globalTextures.charaTextures[2],0,0,pwrSprite,0);
    SpriteList[nextID] = newSprite;

    // All stats on power-ups are affected by level scaling.  (Except PSI gain.)
    int hpRes = hr;
    int hpPlus = hg;
    int atkPlus = ag;
    int defPlus = dg;

    if (hpRes < 0) { hpRes += (hpRes + rand()%2) * (level/2.0f); }
    if (hpPlus < 0) { hpPlus += (hpPlus + rand()%2) * (level/3.0f); }
    if (atkPlus < 0) { atkPlus += (atkPlus + rand()%2) * (level/4.0f); }
    if (defPlus < 0) { defPlus += (defPlus + rand()%2) * (level/4.0f); }

    PowerupComponent newPowerup;
    newPowerup.setComponent(nextID,hr,hpPlus,atkPlus,defPlus,pr);
    PowerupList[nextID] = newPowerup;
    nextID++;
}

// Spawns a random enemy, based on the current depth.
void SpawnerSystem::spawnEnemy(int x, int y, int level)
{
    // An enemy is randomly generated; a new enemy type can spawn in every odd level (not counting level 1).
    // New enemies stop appearing past level 15.
    int monsterSpawn = std::min(rand()%((level/2)+1),7);
    switch (monsterSpawn)
    {
    case 0: // Level 1+.
        spawnBrute(x,y,level);
        break;
    case 1: // Level 3+.
        spawnGunner(x,y,level);
        break;
    case 2: // Level 5+.
        spawnHealer(x,y,level);
        break;
    case 3: // Level 7+.
        spawnTurret(x,y,level);
        break;
    case 4: // Level 9+.
        spawnSniper(x,y,level);
        break;
    case 5: // Level 11+.
        spawnHeavy(x,y,level);
        break;
    case 6: // Level 13+.
        spawnNinja(x,y,level);
        break;
    default: // Level 15+.
        spawnElite(x,y,level);
        break;
    }
}

// Spawns a Brute.
void SpawnerSystem::spawnBrute(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,0,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 30; // The enemy's HP.
    int enemyATK = 8; // The enemy's ATK.
    int enemyDEF = 4; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%5) * (level/4.0f);
    enemyATK += (enemyATK + rand()%5) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/30)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Brute Splicer",enemyHP,enemyATK,enemyDEF,enemyRGN,1);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,2);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,5);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Gunner.
void SpawnerSystem::spawnGunner(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,32,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 40; // The enemy's HP.
    int enemyATK = 6; // The enemy's ATK.
    int enemyDEF = 3; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%4) * (level/4.0f);
    enemyATK += (enemyATK + rand()%4) * (level/7.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/40)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Abaddon Mercenary",enemyHP,enemyATK,enemyDEF,enemyRGN,3);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,2);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,7);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Healer.
void SpawnerSystem::spawnHealer(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,64,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 100; // The enemy's HP.
    int enemyATK = 3; // The enemy's ATK.
    int enemyDEF = 3; // The enemy's DEF.
    int enemyRGN = 100; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/2.0f);
    enemyATK += (enemyATK + rand()%2) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/100)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Stone Splicer",enemyHP,enemyATK,enemyDEF,enemyRGN,1);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,2);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,3);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Turret.
void SpawnerSystem::spawnTurret(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,96,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 30; // The enemy's HP.
    int enemyATK = 10; // The enemy's ATK.
    int enemyDEF = 5; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/3.0f);
    enemyATK += (enemyATK + rand()%2) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/30)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Autocannon",enemyHP,enemyATK,enemyDEF,enemyRGN,4);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,0);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,5);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Sniper.
void SpawnerSystem::spawnSniper(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,128,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 30; // The enemy's HP.
    int enemyATK = 5; // The enemy's ATK.
    int enemyDEF = 2; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/3.0f);
    enemyATK += (enemyATK + rand()%2) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/30)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Abaddon Exterminator",enemyHP,enemyATK,enemyDEF,enemyRGN,7);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,2);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,8);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Heavy.
void SpawnerSystem::spawnHeavy(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,160,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 40; // The enemy's HP.
    int enemyATK = 10; // The enemy's ATK.
    int enemyDEF = 5; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/3.0f);
    enemyATK += (enemyATK + rand()%2) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/50)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Stumbling Splicer",enemyHP,enemyATK,enemyDEF,enemyRGN,1);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,1);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,12);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns a Ninja.
void SpawnerSystem::spawnNinja(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,192,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 5; // The enemy's HP.
    int enemyATK = 50; // The enemy's ATK.
    int enemyDEF = 0; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/3.0f);
    enemyATK += (enemyATK + rand()%2) * (level/6.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/8.0f);
    enemyRGN = ceil((enemyRGN/5)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Abaddon Operative",enemyHP,enemyATK,enemyDEF,enemyRGN,1);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,2);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,7);
    SightList[nextID] = newSight;
    nextID++;
}

// Spawns an Elite.
void SpawnerSystem::spawnElite(int x, int y, int level)
{
    PositionComponent newPosition;
    newPosition.setComponent(nextID,x,y,true);
    PositionList[nextID] = newPosition;
    dungeonMap.addDirectPresence(nextID,x,y,level);
    SpriteComponent newSprite;
    newSprite.setComponent(nextID,&globalTextures.charaTextures[1],0,0,224,0);
    SpriteList[nextID] = newSprite;

    StatusComponent newStatus;

    int enemyHP = 50; // The enemy's HP.
    int enemyATK = 10; // The enemy's ATK.
    int enemyDEF = 5; // The enemy's DEF.
    int enemyRGN = 5; // The enemy's regeneration.

    // The stats for the enemy are given a certain degree of random variance, and scaled based on level.
    enemyHP += (enemyHP + rand()%3) * (level/3.0f);
    enemyATK += (enemyATK + rand()%2) * (level/4.0f);
    enemyDEF += (enemyDEF + rand()%2) * (level/4.0f);
    enemyRGN = ceil((enemyRGN/50)*enemyHP);
    // The enemy's stats are assigned and it spawns in.
    newStatus.setComponent(nextID,"Psychic Splicer",enemyHP,enemyATK,enemyDEF,enemyRGN,1);
    StatusList[nextID] = newStatus;

    BrainComponent newBrain;
    newBrain.setComponent(nextID,3);
    BrainList[nextID] = newBrain;
    SightComponent newSight;
    newSight.setComponent(nextID,10);
    SightList[nextID] = newSight;
    // Elites have psychic powers.
    PsiComponent newPsi;
    newPsi.setComponent(nextID,3);
    PsiList[nextID] = newPsi;
    nextID++;
}
