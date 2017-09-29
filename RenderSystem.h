#ifndef RENDERSYSTEM_H_INCLUDED
#define RENDERSYSTEM_H_INCLUDED

// Deals with manipulating an entity's visual representation and rendering it to the screen.

class RenderSystem
{
public:
    RenderSystem();
    // Draws entity 'id' to the window 'window'.
    void drawEntity(sf::RenderWindow* window, int id);
    // Updates the sprite of entity 'id' to properly represent any alterations to the other qualities of 'id'.
    void updateSprite(int id);
};

#endif // RENDERSYSTEM_H_INCLUDED
