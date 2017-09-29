#include "RenderSystem.h"

#include "SpriteComponent.h"
#include "PositionComponent.h"

extern std::unordered_map<int,SpriteComponent> SpriteList;
extern std::unordered_map<int,PositionComponent> PositionList;

RenderSystem::RenderSystem() {}

// Draws entity 'id' to the window 'window'.
void RenderSystem::drawEntity(sf::RenderWindow* window, int id)
{
    window->draw(SpriteList[id].sprite);
}

// Updates the sprite of entity 'id' to properly represent any alterations to the other qualities of 'id'.
void RenderSystem::updateSprite(int id)
{
    // Alters the sprite's position to match the current position of the entity.
    SpriteList[id].xPosition = PositionList[id].xCoord*32;
    SpriteList[id].yPosition = PositionList[id].yCoord*32;
    SpriteList[id].sprite.setPosition(PositionList[id].xCoord*32,PositionList[id].yCoord*32);
}
