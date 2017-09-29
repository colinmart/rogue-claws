#ifndef STATUSCOMPONENT_H_INCLUDED
#define STATUSCOMPONENT_H_INCLUDED

// Gives an entity the statistics of a creature - player or enemy - and the ability to be interacted with as one.  This is what makes an entity 'alive'.

class StatusComponent
{
public:
    int id;
    bool alive; // Used to reference whether or not an entity is alive.
    std::string name; // Used when referring to the entity in messages.
    int maxhealth; // The most health an entity can have at any time, and the health it starts with.
    int health; // The amount of damage the entity can take and survive.
    int attack; // The damage the entity deals when it attacks another entity.
    int defense; // Reduces the damage the entity takes.
    int regentick; // The number of turns required for the entity to regenerate health
    int regen; // The entity's health rengerated per tick.
    int range; // How far the entity can deliver a blow to the player.
    StatusComponent();
    // Initializes the component for a particular entity.
    void setComponent(int i, std::string n, int hp, int atk, int def, int rgn, int rng);
};

#endif // STATUSCOMPONENT_H_INCLUDED
