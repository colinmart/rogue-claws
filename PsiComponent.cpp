#include "PsiComponent.h"

PsiComponent::PsiComponent() { id = -1; maxPsi = 0; psiCharges = 0; chargingPsi = false; }

void PsiComponent::setComponent(int i, int mp) { id = i; maxPsi = mp; psiCharges = mp; }
