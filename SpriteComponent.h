#ifndef SPRITECOMPONENT_H_INCLUDED
#define SPRITECOMPONENT_H_INCLUDED

// Gives an entity a representative image, and allows it to be manipulated.
// Implies PositionComponent.

class SpriteComponent
{
public:
    int id;
    sf::Sprite sprite; // The entity's graphical representation.
    int xPosition; // The X-position of the entity's sprite in pixels.
    int yPosition; // The Y-position of the entity's sprite in pixels.
    int xAnchor; // The X-position of the entity's sprite in its texture.
    int yAnchor; // The Y-position of the entity's sprite in its texture.
    int xOffset; // The X-offset of the entity's sprite due to animation.
    int yOffset; // The Y-offset of the entity's sprite due to animation.
    int animFrame; // The entity's current animation frame.
    SpriteComponent();
    // Initializes the component for a particular entity.
    void setComponent(int i, sf::Texture* target, int x, int y, int xa, int ya);
};

#endif // SPRITECOMPONENT_H_INCLUDED
