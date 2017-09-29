#ifndef SIGHTCOMPONENT_H_INCLUDED
#define SIGHTCOMPONENT_H_INCLUDED

// Gives an entity the ability to 'see' its surroundings as though looking realistically.

class SightComponent
{
public:
    int id;
    int visionRange; // How far the entity is capable of seeing.
    std::vector<sf::Vector2i> visionArea; // What tiles the entity can currently see.
    SightComponent();
    void setComponent(int i, int vr);
};

#endif // SIGHTCOMPONENT_H_INCLUDED
