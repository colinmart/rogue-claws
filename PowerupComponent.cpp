#include "PowerupComponent.h"

PowerupComponent::PowerupComponent() { id = -1; healthRestore = 0; healthGain = 0; atkGain = 0; defGain = 0; psiRestore = 0; }

void PowerupComponent::setComponent(int i, int hr, int hg, int ag, int dg, int pr)
{ id = i; healthRestore = hr; healthGain = hg; atkGain = ag; defGain = dg; psiRestore = pr; }
