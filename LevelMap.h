#ifndef LEVELMAP_H_INCLUDED
#define LEVELMAP_H_INCLUDED

class LevelMap
{
private:
    int xLimit; // Maximum X width of the level.
    int yLimit; // Maximum Y width of the level.
    std::vector<int> areaFloors; // Holds the floor-type of each tile.
    std::unordered_map<int,std::vector<int>> areaPresence; // Holds the IDs of entities present on each tile.
    std::unordered_map<int,int> areaTileCode; // Holds the appropriate graphic for each tile.
    sf::Texture floorTileset; // The texture used to draw the level's graphic.
    sf::VertexArray floorGraphic; // This vertex array holds the level's graphic.
    int entryPoint; // This int holds the location of the stairs leading up.
    int exitPoint; // This int holds the location of the stairs leading down.
    int prevExit; // The exit of the previous level.
    int nextEntry; // The entrance to the next level.
public:
    LevelMap();
    // Initializes the level map by loading in pregenerated tiles and presences.
    void loadMap(int x, int y, std::vector<int> aF, std::unordered_map<int,std::vector<int>> aP);
    // Generates a graphic for the level map based on its tiles.
    void makeMapGraphic();
    // Returns the dimensions of the map.
    int getWidth();
    int getHeight();
    // Gets the type of tile located at (x,y).
    int getFloorType(int x, int y);
    // Gets all the entities located at (x,y).
    std::vector<int> getPresence(int x, int y);
    // Adds an entity's presence to (x,y).
    void addPresence(int id, int x, int y);
    // Removes an entity's presence from (x,y).
    void removePresence(int id, int x, int y);
    //int getPresenceCount(); // Returns the number of
    // Draws the graphic for the level onto window 'window'.
    void drawFloors(sf::RenderWindow* window);
    // Returns the location of the level's up stairs in the tile vector.
    int getEntry();
    // Returns the location of the level's down stairs in the tile vector.
    int getExit();
    // Sets the location of the previous level's down stairs.
    void setPrevExit(int exit);
    // Sets the location of the next level's up stairs.
    void setNextEntry(int entry);
    // Returns the level's x-limit.  Useful for dealing directly with the vectors in the level.
    int getLimits();
    // Returns every entity on the level.  Useful for iterating over them all at once.
    std::vector<int> entitiesInPlay();
};

#endif // LEVELMAP_H_INCLUDED
