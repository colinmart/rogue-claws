#ifndef RANDOMSYSTEM_H_INCLUDED
#define RANDOMSYSTEM_H_INCLUDED

class RandomSystem
{
public:
    RandomSystem();
    // Generates a random number between 'minN' and 'maxN'.
    int numberInRange(int minN, int maxN);
    // Generates a random dungeon level with width 'xlim', height 'ylim' and 'density' number of rooms, then adds it to the game.
    void makeDungeonLevel(int xlim, int ylim, int density);
};

#endif // RANDOMSYSTEM_H_INCLUDED
