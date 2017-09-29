#include "SpriteComponent.h"

SpriteComponent::SpriteComponent() { id = -1; xAnchor = 0; yAnchor = 0; xOffset = 0; yOffset = 0; animFrame = 0; xPosition = 0; yPosition = 0; }

// Initializes the component for a particular entity.
void SpriteComponent::setComponent(int i, sf::Texture* target, int x, int y, int xa, int ya)
{
    id = i;
    xPosition = x; yPosition = y;
    xAnchor = xa; yAnchor = ya;
    xOffset = 0; yOffset = 0; animFrame = 0;
    sprite.setTexture(*target);
    sprite.setTextureRect(sf::IntRect(xAnchor,yAnchor,32,32));
}
