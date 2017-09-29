#include "SightComponent.h"

SightComponent::SightComponent() { id = -1; visionRange = 0; }

void SightComponent::setComponent(int i, int vr) { id = i; visionRange = vr; }
