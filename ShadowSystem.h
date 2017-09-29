#ifndef SHADOWSYSTEM_H_INCLUDED
#define SHADOWSYSTEM_H_INCLUDED

// Deals with casting of light and shadow from an entity's position, determining what they can see.

class ShadowSystem
{
public:
    ShadowSystem();
    // Casts light and shadow from an entity, determining what they can see.
    void castShadow(int id);
    // Scans an octant for shadow cast FOV.
    void scanOctant(int depth, int maxDepth, float startSlope, float endSlope, int startX, int startY ,int xx, int xy, int yx, int yy);
};

#endif // SHADOWSYSTEM_H_INCLUDED
