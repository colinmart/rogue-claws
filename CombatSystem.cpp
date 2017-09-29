#include "CombatSystem.h"

#include "StatusComponent.h"

extern std::unordered_map<int,StatusComponent> StatusList;

extern std::vector<std::string> messageLog;

extern void removeEntity(int id);

CombatSystem::CombatSystem() {}

// Causes the entity with ID 'idA' to attack the entity with id 'idD'.
void CombatSystem::fight(int idA, int idD)
{
    // The combat log will send a message to the player concerning the events of combat.
    std::string combatLog = "";
    std::stringstream ss;
    int damage = StatusList[idA].attack - StatusList[idD].defense + ((rand() % 10) + 1);
    damage = std::max(damage,0);
    // The defender's health is reduced by damage taken.
    StatusList[idD].health = std::max(StatusList[idD].health-damage,0);
    // Getting hit stops regeneration.
    StatusList[idD].regentick = 0;
    ss << damage;
    combatLog = combatLog + StatusList[idA].name + " strikes " + StatusList[idD].name + " and deals " + ss.str() + " damage!";
    // If health falls to 0 or below as a result, the entity is defeated and removed from the game.
    if (StatusList[idD].health <= 0) { combatLog = combatLog + "  " + StatusList[idD].name + " is destroyed!"; removeEntity(idD); }
    messageLog.push_back(combatLog);
}

// Causes the entity with ID 'idA' to Mind Blast the entity with id 'idD'.
void CombatSystem::blast(int idA, int idD)
{
    // The combat log will send a message to the player concerning the events of combat.
    std::string combatLog = "";
    std::stringstream ss;
    int damage = (StatusList[idA].attack * 2); // Defense does nothing against Mind Blast.
    damage = std::max(damage,0);
    // The defender's health is reduced by damage taken.
    StatusList[idD].health = std::max(StatusList[idD].health-damage,0);
    ss << damage;
    combatLog = combatLog + StatusList[idA].name + " blasts " + StatusList[idD].name + " and deals " + ss.str() + " damage!";
    // If health falls to 0 or below as a result, the entity is defeated and removed from the game.
    if (StatusList[idD].health <= 0) { combatLog = combatLog + "  " + StatusList[idD].name + " is destroyed!"; removeEntity(idD); }
    messageLog.push_back(combatLog);
}

void CombatSystem::regen()
{
    for (auto it = StatusList.begin(); it != StatusList.end(); ++it)
    {
        if (it->second.health < it->second.maxhealth)
        {
            it->second.regentick += 1;
            if (it->second.regentick >= 5)
            {
                it->second.regentick -= 5;
                it->second.health = std::min(it->second.health + it->second.regen, it->second.maxhealth);
            }
        }
    }
}
