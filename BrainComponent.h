#ifndef BRAINCOMPONENT_H_INCLUDED
#define BRAINCOMPONENT_H_INCLUDED

// Gives an entity the ability to automatically make decisions and oppose the player.  This is what makes an entity a functioning 'enemy'.

class BrainComponent
{
public:
    int id;
    int lastKnownX; // The X-component of the last position that the entity saw the player at.
    int lastKnownY; // The Y-component of the last position that the entity saw the player at.
    int aiType; // The type of AI function used by this brain (chase, guard, stumble, or psychic).
    // 0 = Reactive (won't move, attacks when player moves into its range).
    // 1 = Stumbler (moves to put self on same X and Y axis with player, easily blocked by walls).  Implies SightComponent.
    // 2 = Chaser (pursues player using A* algorithm, attacks when in range).  Implies SightComponent.
    // 3 = Psi Chaser (as 2, but attempts to use a Mind Blast if it's available).  Implies SightComponent and PsiComponent.
    BrainComponent();
    void setComponent(int i, int a);
};

#endif // BRAINCOMPONENT_H_INCLUDED
