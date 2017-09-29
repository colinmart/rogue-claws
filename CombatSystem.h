#ifndef COMBATSYSTEM_H_INCLUDED
#define COMBATSYSTEM_H_INCLUDED

// Deals with combat between two living entities.

class CombatSystem
{
public:
    CombatSystem();
    // Causes the entity with ID 'idA' to attack the entity with id 'idD'.
    void fight(int idA, int idD);
    // Causes the entity with ID 'idA' to Mind Blast the entity with id 'idD'.
    void blast(int idA, int idD);
    // Causes all entities with health to regenerate some of it per turn.
    void regen();
};

#endif // COMBATSYSTEM_H_INCLUDED
