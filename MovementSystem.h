#ifndef MOVEMENTSYSTEM_H_INCLUDED
#define MOVEMENTSYSTEM_H_INCLUDED

// Deals with movement of entities from one square on the map to another, whether voluntary or involuntary.

class MovementSystem
{
public:
    MovementSystem();
    // Moves entity 'id' 'xmove' spaces on the X-axis and 'ymove' spaces on the Y-axis, and returns true.
    // If that space is blocked, does nothing and returns false.
    bool moveEntity(int id, int xmove, int ymove);
};

#endif // MOVEMENTSYSTEM_H_INCLUDED
