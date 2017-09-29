#ifndef POSITIONCOMPONENT_H_INCLUDED
#define POSITIONCOMPONENT_H_INCLUDED

// Gives an entity a position in the world, and the associated attributes of that position.

class PositionComponent
{
public:
    int id;
    PositionComponent();
    int xCoord; // The position's X coordinate.
    int yCoord; // The position's Y coordinate.
    bool isSolid; // No two solid objects can occupy the same position.
    // Initializes the component for a particular entity.
    void setComponent(int i, int x, int y, bool solid);
};

#endif // POSITIONCOMPONENT_H_INCLUDED
