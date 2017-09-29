#include "BrainComponent.h"

BrainComponent::BrainComponent() { id = -1; lastKnownX = -1; lastKnownY = -1; aiType = 0; }

void BrainComponent::setComponent(int i, int a) { id = i; aiType = a; }
