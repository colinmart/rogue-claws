#ifndef CASTINGSYSTEM_H_INCLUDED
#define CASTINGSYSTEM_H_INCLUDED

// Deals with the use of special powers.

class CastingSystem
{
public:
    CastingSystem();
    // Causes the entity 'id' to fire a Mind Blast in direction 'dir'.
    // 0 = N, 1 = E, 2 = S, 3 = W.
    void mindBlast(int id, int dir);
};

#endif // CASTINGSYSTEM_H_INCLUDED
