#include "PositionComponent.h"

PositionComponent::PositionComponent() { id = -1; xCoord = 0; yCoord = 0; isSolid = false; }

// Initializes the component for a particular entity.
void PositionComponent::setComponent(int i, int x, int y, bool solid) { id = i; xCoord = x; yCoord = y; isSolid = solid; }
