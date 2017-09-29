#include "StatusComponent.h"

StatusComponent::StatusComponent() { id = -1; alive = true; maxhealth = 1; health = 1; attack = 0; defense = 0; regentick = 0.0; regen = 0.0; range = 0; }

// Initializes the component for a particular entity.
void StatusComponent::setComponent(int i, std::string n, int hp, int atk, int def, int rgn, int rng)
{
    id = i;
    name = n;
    maxhealth = hp; health = hp;
    attack = atk; defense = def;
    regentick = 0; regen = rgn;
    range = rng;
}
