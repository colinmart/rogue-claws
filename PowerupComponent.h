#ifndef POWERUPCOMPONENT_H_INCLUDED
#define POWERUPCOMPONENT_H_INCLUDED

// Gives an entity the ability to be consumed by the player for an upgrade.

class PowerupComponent
{
public:
    int id;
    int healthRestore; // Healing given by the power-up.
    int healthGain; // HP increase given by the power-up.
    int atkGain; // ATK increase given by the power-up.
    int defGain; // DEF increase given by the power-up.
    int psiRestore; // Mind Blast charges restored by the power-up.
    PowerupComponent();
    // Initializes the component for a particular entity.
    void setComponent(int i, int hr, int hg, int ag, int dg, int pr);
};

#endif // POWERUPCOMPONENT_H_INCLUDED
