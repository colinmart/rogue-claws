#ifndef PSICOMPONENT_H_INCLUDED
#define PSICOMPONENT_H_INCLUDED

// Gives an entity the ability to use psychic powers.

class PsiComponent
{
public:
    int id;
    int maxPsi; // Maximum number of psychic power uses.
    int psiCharges; // Current number of psychic power uses.
    bool chargingPsi; // Whether PSI is currently being charged.
    PsiComponent();
    // Initializes the component for a particular entity.
    void setComponent(int i, int mp);
};

#endif // PSICOMPONENT_H_INCLUDED
