// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// PROJECT 198 - CSCI
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

#include "PositionComponent.h"
#include "SpriteComponent.h"
#include "StatusComponent.h"
#include "BrainComponent.h"
#include "SightComponent.h"
#include "PowerupComponent.h"
#include "PsiComponent.h"

#include "MovementSystem.h"
#include "RenderSystem.h"
#include "RandomSystem.h"
#include "SpawnerSystem.h"
#include "CombatSystem.h"
#include "GuiSystem.h"
#include "AiSystem.h"
#include "ShadowSystem.h"
#include "CastingSystem.h"

#include "WorldMap.h"
#include "TextureContainer.h"

#include <time.h>

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// COMPONENTS
// Holds all components needed for running the game.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

// Holds the game's maps.
WorldMap dungeonMap;

// Holds all messages displayed on screen to inform the user, allowing them to be accessed.
std::vector<std::string> messageLog;

// Positional component.  Holds position in the world, and status on whether the entity blocks passing.
std::unordered_map<int,PositionComponent> PositionList;

// Image component.  Holds the entity's current sprite and any information on how animations have deformed it.
std::unordered_map<int,SpriteComponent> SpriteList;

// Status component.  Holds the entity's in-game stats, used for the game's combat system.
std::unordered_map<int,StatusComponent> StatusList;

// Brain component.  Holds the entity's knowledge about in-game events, used for the AI.
std::unordered_map<int,BrainComponent> BrainList;

// Sight component.  Holds the area the entity can see from their current position, and their range of vision.
std::unordered_map<int,SightComponent> SightList;

// Powerup component.  Holds entities that can be consumed for more power to the player.
std::unordered_map<int,PowerupComponent> PowerupList;

// Psi component.  Holds the entity's ability to use the Mind Blast power.
std::unordered_map<int,PsiComponent> PsiList;

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// SYSTEMS
// Holds all systems that operate on components.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

// Movement system.  Moves the entity to a different space on the map.
MovementSystem MoveSys;

// Rendering system.  Draws the map and entities on it.
RenderSystem RenderSys;

// RNG system.  Generates dungeon levels, etc.
RandomSystem RandomSys;

// Entity spawning system.  Generates entities to put into the game.
SpawnerSystem SpawnSys;

// Entity combat system.  Handles fights between entities.
CombatSystem CombatSys;

// Rendering system for game information.  Informs the player through visual feedback.
GuiSystem GuiSys;

// Enemy AI system.  Determines enemy actions.
AiSystem AiSys;

// Shadow system.  Determines what entities can see from their positions.
ShadowSystem ShadowSys;

// Casting system.  Handles use of entities' special powers.
CastingSystem CastingSys;

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// GAMEPLAY GLOBALS
// Holds all globals needed for the gameplay basics.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

bool gamePlaying = false; // Records whether the game is currently in an active state (the player has not gotten a game over, for instance).
int nextID = 1; // Holds the next ID to use.
TextureContainer globalTextures; // This container holds every texture used in the game to ensure they do not leave scope while still in use.
sf::View mainView(sf::FloatRect(0,0,800,600)); // The game's 'camera', this variable determines what part of the drawn area is shown on screen.
int tileSize = 32; // The size of each of the game's tiles, in pixels.
int mapWidth = 0; // Width of the map, for things that care about it.
sf::Vector2f viewPos;

bool playersTurn = true; // Holds whether it's currently the player's turn.
bool canTurbo = false; // Records whether the player can currently turbo keys.
sf::Vector2f playerPos; // Holds the player's current location for convenience.
bool playerCharging = false; // Holds whether the player is charging a power.

int score = 1; // Holds the current score.
int highScore = 1; // Holds the all-time high score.

sf::Font gameFont; // The font used by the game.
sf::Text messageText; // The text displayed in the message board.
int textDisplace = 0; // How far down the messages log is currently scrolled.
bool viewHelp = false; // Whether the player is currently looking at the help menu.

bool gameOver = false; // Whether the game has ended.
float transProgress = 0; // How far underway the end-game transition is.
bool retreated = false; // Whether the player retreated on their last attempt.
bool beatGame = false; // Whether the player beat the game on their last attempt.
bool gamePlayed = false; // Whether the game's been played yet.

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// CONTROL GLOBALS
// Holds all globals needed for the main loop.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

bool gameOpen = true; // Whether the player still wants the window to be open.
sf::Keyboard::Key lastKey = sf::Keyboard::Unknown; // Holds the last key pressed to prevent taking the same input too many times.
sf::Clock repeatClock; // Holds the time required to wait before you can double-input the same command.
#define REPEATTIME 0.4 // Holds the time in seconds before you can input a command twice in a row.
#define REPEATTURBO 0.05 // Holds the time in seconds before you can input a command twice in a row under turbo.

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// GAME FUNCTIONS
// Holds essential functions for making the game operate properly.
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

// This function eliminates an entity of a given ID.
void removeEntity(int id)
{
    // Trying to remove the player signals a game over.
    if (id == 0) { gameOver = true; }
    else
    {
        // The destroyed entity's position is removed from the map it's on.   (We're assuming an entity on another map can't be destroyed, for now.)
        dungeonMap.removePresence(id,PositionList[id].xCoord,PositionList[id].yCoord);
        // To finish it off, the entity is removed from all lists containing it.
        PositionList.erase(id);
        SpriteList.erase(id);
        StatusList.erase(id);
        BrainList.erase(id);
        SightList.erase(id);
        PowerupList.erase(id);
        PsiList.erase(id);
    }
}

// This function transitions the player from one level to the next, through the use of the 'staircases' located in each level.
void useStairs()
{
    int xc = PositionList[0].xCoord; int yc = PositionList[0].yCoord; // xc and yc represent the player's current position in the map.
    int floor = dungeonMap.getFloorType(xc,yc); // The floor type 2 is a staircase to the previous level; the floor type 3 is a staircase to the next level.
    // If the player is on an up staircase:
    if (floor == 2)
    {
        int level = dungeonMap.getCurrentLevel();
        if (level == 0) { gameOver = true; retreated = true; } // Going up the first floor's up staircase is one of the conditions that will end the game.
        else
        {
            dungeonMap.removePresence(0,xc,yc); // The player is removed from the current level.
            dungeonMap.changeCurrentLevel(level-1); // The current level becomes the next level.
            // Going up a staircase will put the player on the previous level's down staircase.
            sf::Vector2i newPos = dungeonMap.getExit();
            PositionList[0].xCoord = newPos.x;
            PositionList[0].yCoord = newPos.y;
            dungeonMap.addPresence(0,newPos.x,newPos.y);
        }
    }
    // If the player is on a down staircase:
    else if (floor == 3)
    {
        int level = dungeonMap.getCurrentLevel();
        // Passing level 20 wins the game.
        if (level == 19) { gameOver = true; beatGame = true; }
        else
        {
            // If the level that the player is trying to descend to does not yet exist, it will be created on the spot.
            if (level == dungeonMap.getMaxLevels())
            {
                RandomSys.makeDungeonLevel(50,50,10);
            }
            dungeonMap.removePresence(0,xc,yc);  // The player is removed from the current level.
            dungeonMap.changeCurrentLevel(level+1); // The current level becomes the next level.
            // Going down a staircase will put the player on the next level's up staircase.
            sf::Vector2i newPos = dungeonMap.getEntry();
            PositionList[0].xCoord = newPos.x;
            PositionList[0].yCoord = newPos.y;
            dungeonMap.addPresence(0,newPos.x,newPos.y);
            // If this is a new area, increase the score and print a message.
            if (level == score-1)
            {
                score++;
                std::string scoreLog = "Welcome to level ";
                std::stringstream ss;
                ss << score;
                scoreLog = scoreLog + ss.str() + ".";
                messageLog.push_back(scoreLog);
            }
        }
    }
}

// Allows a player to use a power-up on the ground beneath them.
void eatPowerup()
{
    std::vector<int> presenceHolder = dungeonMap.getPresence(playerPos.x,playerPos.y);
    for (unsigned int i = 0; i < presenceHolder.size(); ++i)
    {
        // For all non-solid objects on the player's tile;
        if (!PositionList[presenceHolder[i]].isSolid)
        {
            // If one of them is a power-up, eat it.
            if (PowerupList.find(presenceHolder[i]) != PowerupList.end())
            {
                int powerUp = presenceHolder[i];
                // The combat log will send a message to the player concerning the effects of the powerup.
                std::string gainsLog = "Cress absorbs the strong energy!";
                std::stringstream ss;
                int gainedStat = 0; // Holds the value of the stat currently being handled.
                bool gainedSomething = false; // Records whether the power-up had an effect.

                // The power-up's stat gains are given to the player.
                gainedStat = PowerupList[powerUp].healthGain;
                if (gainedStat > 0)
                {
                    StatusList[0].maxhealth += gainedStat;
                    ss << gainedStat;
                    gainsLog = gainsLog + "  HP increased by " + ss.str() + ".";
                    ss.str(std::string());
                    StatusList[0].regen = ceil(StatusList[0].maxhealth / 10); // Gaining maximum health will also allow for faster regeneration.
                    gainedSomething = true;
                }
                gainedStat = PowerupList[powerUp].atkGain;
                if (gainedStat > 0)
                {
                    StatusList[0].attack += gainedStat;
                    ss << gainedStat;
                    gainsLog = gainsLog + "  ATK increased by " + ss.str() + ".";
                    ss.str(std::string());
                    gainedSomething = true;
                }
                gainedStat = PowerupList[powerUp].defGain;
                if (gainedStat > 0)
                {
                    StatusList[0].defense += gainedStat;
                    ss << gainedStat;
                    gainsLog = gainsLog + "  DEF increased by " + ss.str() + ".";
                    ss.str(std::string());
                    gainedSomething = true;
                }

                // Health and psi charges may be restored if the power-up carried those properties.
                gainedStat = std::min(StatusList[0].health + PowerupList[powerUp].healthRestore,StatusList[0].maxhealth);
                if ((gainedStat - StatusList[0].health) > 0)
                {
                    ss << (gainedStat - StatusList[0].health);
                    StatusList[0].health = gainedStat;
                    gainsLog = gainsLog + "  Restored " + ss.str() + " health.";
                    ss.str(std::string());
                    gainedSomething = true;
                }
                gainedStat = std::min(PsiList[0].psiCharges + PowerupList[powerUp].psiRestore,PsiList[0].maxPsi);
                if ((gainedStat - PsiList[0].psiCharges) > 0)
                {
                    PsiList[0].psiCharges = gainedStat;
                    gainsLog = gainsLog + "  Psychic power replenished.";
                    ss.str(std::string());
                    gainedSomething = true;
                }

                // The power-up is consumed, and the message delivered, assuming it had an effect.  Otherwise, it's saved for later.
                if (gainedSomething) { removeEntity(powerUp); }
                else { gainsLog = "Cress tried to absorb the strong energy, but she's already at full strength."; }
                messageLog.push_back(gainsLog);
                break;
            }
        }
    }
}

// Causes the player to fire a Mind Blast, if they have one.
void castPsi(int dir)
{
    std::string psiLog = "";
    std::stringstream ss;
    // The player can only fire PSI if they have some charges left.
    if (PsiList[0].psiCharges < 1)
    {
        psiLog = "Psychic power is bottomed out.";
        // A message is delivered about what happened with the psychic ability's use.
        messageLog.push_back(psiLog);
    }
    else
    {
        // If the player is charging, the power can be cast.
        if (playerCharging)
        {
            playerCharging = false;
            CastingSys.mindBlast(0,dir);
        }
        // Otherwise, start charging.
        else
        {
            playerCharging = true;
            CastingSys.mindBlast(0,dir);
            psiLog = "Mind Blast is ready...";
            // A message is delivered about what happened with the psychic ability's use.
            messageLog.push_back(psiLog);
        }
    }
}

// Scrolls the message log up (negative) or down (positive).
void scrollMessages(int dir) { int s = messageLog.size(); if (std::min(textDisplace+dir,s-7) >= 0) { textDisplace = std::min(textDisplace+dir,s-7); } }

// Processes player input and takes in-game action accordingly.
void getControls()
{
    // To begin, the currently pressed key is obtained (out of all the keys that are relevant to the game).
    sf::Keyboard::Key currentKey = sf::Keyboard::Unknown;
    // WASD are movement keys; they move the player up, left, right, and down respectively.  There is currently no diagonal movement.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { currentKey = sf::Keyboard::W; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { currentKey = sf::Keyboard::A; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { currentKey = sf::Keyboard::S; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { currentKey = sf::Keyboard::D; }
    // E will eat a power-up if the player is currently standing on one.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) { currentKey = sf::Keyboard::E; }
    // F will charge Mind Blast, or use it if it's charged.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) { currentKey = sf::Keyboard::F; }
    // U will use a staircase to ascend or descend floors if the player is currently standing on one.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::U)) { currentKey = sf::Keyboard::U; }
    // R passes the turn without doing anything.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) { currentKey = sf::Keyboard::R; }
    // Z scrolls the message log up.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) { currentKey = sf::Keyboard::Z; }
    // X scrolls the message log down.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) { currentKey = sf::Keyboard::X; }

    // Certain 'special keys' operate outside the normal control scheme and are checked on their own.
    bool shiftPressed = false; // The Shift key is the 'turbo' key, and allows us to 'turbo' input commands (as long as danger isn't near).
    viewHelp = false; // The C key allows the player to view the controls.
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) && (canTurbo)) { shiftPressed = true; }
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::C))) { viewHelp = true; }
    // If the current key is the same key that was pressed the last time we checked;
    if (currentKey == lastKey)
    {
        float timeSinceLastPress = repeatClock.getElapsedTime().asSeconds(); // Get the time since last unique button press.
        // If it's been long enough to do the same button press again, reset the lastKey variable so that it won't stop the button press from occurring again.
        if ((timeSinceLastPress >= REPEATTIME) || (shiftPressed && (timeSinceLastPress >= REPEATTURBO))) { lastKey = sf::Keyboard::Unknown; }
    }
    // If it wasn't, or it's been long enough to do it again;
    if (currentKey != lastKey)
    {
        playersTurn = false; // If the player did anything, it is no longer their turn.
        // Player moveset changes based on if they are charging.
        if (!playerCharging)
        {
            // Run a switch statement for each possible input and take the appropriate action.
            switch (currentKey)
            {
            case sf::Keyboard::W:
                MoveSys.moveEntity(0,0,-1);
                break;
            case sf::Keyboard::A:
                MoveSys.moveEntity(0,-1,0);
                break;
            case sf::Keyboard::D:
                MoveSys.moveEntity(0,1,0);
                break;
            case sf::Keyboard::S:
                MoveSys.moveEntity(0,0,1);
                break;
            case sf::Keyboard::E:
                eatPowerup();
                break;
            case sf::Keyboard::R:
                if (shiftPressed) { currentKey = sf::Keyboard::Unknown; } // You can turbo R infinitely fast.
                break;
            case sf::Keyboard::F:
                castPsi(-1);
                break;
            case sf::Keyboard::U:
                useStairs();
                break;
            case sf::Keyboard::Z:
                scrollMessages(-1);
                playersTurn = true;
                break;
            case sf::Keyboard::X:
                scrollMessages(1);
                playersTurn = true;
                break;
            default:
                playersTurn = true;
                break;
            }
        }
        else
        {
            // Run a switch statement for each possible input and take the appropriate action.
            switch (currentKey)
            {
            case sf::Keyboard::W:
                castPsi(0);
                break;
            case sf::Keyboard::A:
                castPsi(3);
                break;
            case sf::Keyboard::D:
                castPsi(1);
                break;
            case sf::Keyboard::S:
                castPsi(2);
                break;
            case sf::Keyboard::Z:
                scrollMessages(-1);
                playersTurn = true;
                break;
            case sf::Keyboard::X:
                scrollMessages(1);
                playersTurn = true;
                break;
            default:
                playersTurn = true;
                break;
            }
        }
        repeatClock.restart(); // Reset the button-repeat clock.
        lastKey = currentKey; // Set the new previous key to what we just pressed.
    }
}

// Adjusts the game's camera to focus on wherever the player is currently standing.
void updateCamera()
{
    sf::Vector2f viewCenter = mainView.getCenter(); // The view is adjusted based on its center.
    sf::Vector2f relPos((PositionList[0].xCoord*tileSize)-viewCenter.x,(PositionList[0].yCoord*tileSize)-viewCenter.y); // By subtracting the center of the view, we get the distance in pixels between the player and the center.
    // The camera has a 200x200 'bounding box' of sorts around its center; if the player moves within this box, the camera will not move with them.
    // If the player moves out of the box, however, the camera will adjust itself until the player is exactly inside of the bounding box.
    // The required translation is performed by - if the player is out of the bounding box - multiplying the absolute value of their position - 100 * the sign of their position.
    // Since the bounding box is 100 in all directions from the center, this creates the required view translation to put the player inside the bounding box.
    if ((abs(relPos.x) - 100) > 0) { relPos.x = (abs(relPos.x) - 100) * ((relPos.x > 0) - (relPos.x < 0)); }
    else { relPos.x = 0; }
    if ((abs(relPos.y) - 100) > 0) { relPos.y = (abs(relPos.y) - 100) * ((relPos.y > 0) - (relPos.y < 0)); }
    else { relPos.y = 0; }
    // The translation is then applied to the camera.
    mainView.move(relPos);
}

// Checks if a tile or entity is currently within the game's camera and therefore needs to be drawn.
bool inCameraBounds(int x, int y)
{
    sf::Vector2f viewSize = mainView.getSize();
    sf::Vector2f viewCenter = mainView.getCenter();
    sf::FloatRect viewArea(viewCenter.x-(viewSize.x/2),viewCenter.y-(viewSize.y/2),viewSize.x,viewSize.y);
    sf::FloatRect checkedTile(x*tileSize,y*tileSize,tileSize,tileSize);
    return viewArea.intersects(checkedTile);
}

// Draws black zones over tiles not in the player's field of view.
void drawDarkness(sf::RenderWindow* window)
{
    sf::RectangleShape darkness(sf::Vector2f(32,32));
    darkness.setFillColor(sf::Color(0,0,0,255));
    // The top-left and bottom-right corner of the view, in terms of game tiles, are obtained; this tells us what tiles we need to consider for drawing.
    sf::Vector2f viewSize = mainView.getSize();
    sf::Vector2f viewCornerNW = mainView.getCenter();
    viewCornerNW.x = (viewCornerNW.x - (viewSize.x/2))/tileSize; viewCornerNW.y = (viewCornerNW.y - (viewSize.y/2))/tileSize;
    sf::Vector2f viewCornerSE = viewCornerNW;
    viewCornerSE.x += viewSize.x/32; viewCornerSE.y += viewSize.y/32;
    // The tiles are then iterated over; any not in the player's view are drawn over with darkness.
    sf::Vector2i currentTile;
    for (int w = viewCornerNW.y; w <= viewCornerSE.y; w++)
    {
        for (int z = viewCornerNW.x; z <= viewCornerSE.x; z++)
        {
            currentTile.x = z; currentTile.y = w;
            if(std::find(SightList[0].visionArea.begin(), SightList[0].visionArea.end(), currentTile) == SightList[0].visionArea.end())
            {
                darkness.setPosition(currentTile.x*32,currentTile.y*32);
                window->draw(darkness);
            }
        }
    }
}

// Fades out the screen in the event of a game over.
void fadeGameOut(sf::RenderWindow* window)
{
    transProgress += 2; // Every time fadeGameOut is called, the transition progresses.
    sf::RectangleShape darkFade(sf::Vector2f(900,700));
    // darkFade gets darker as time passes.
    darkFade.setFillColor(sf::Color(0,0,0,transProgress));
    // The top-left and bottom-right corner of the view, in terms of game tiles, are obtained; this tells us what tiles we need to consider for drawing.
    sf::Vector2f viewSize = mainView.getSize();
    sf::Vector2f viewCornerNW = mainView.getCenter();
    viewCornerNW.x = viewCornerNW.x - (viewSize.x/2); viewCornerNW.y = viewCornerNW.y - (viewSize.y/2);
    // The fade is aligned over and drawn to the window.
    darkFade.setPosition(viewCornerNW.x-50,viewCornerNW.y-50);
    window->draw(darkFade);
}

// Converts coordinates on the game map to an integer.
int coordToInt(int x, int y)
{
    return (x+(y*mapWidth));
}

// Converts an integer to coordinates on the game map.
sf::Vector2i intToCoord(int i)
{
    sf::Vector2i newCoord(i%mapWidth,i/mapWidth);
    return newCoord;
}

// Starts a new game.
void startGame()
{
    // Sets all variables to proper settings for the beginning of the game.
    score = 1;
    nextID = 1;
    transProgress = 0;
    lastKey = sf::Keyboard::Unknown;

    RandomSys.makeDungeonLevel(50,50,10); // Creates the first level.
    mapWidth = 50; // Initializes the map width.

    sf::Vector2i e = dungeonMap.getEntry();
    SpawnSys.spawnPlayer(e.x,e.y); // Spawns in the player's character.
    ShadowSys.castShadow(0); // Initializes shadowcasting.

    // The player's initial position is set.
    playerPos.x = (PositionList[0].xCoord);
    playerPos.y = (PositionList[0].yCoord);

    updateCamera(); // Adjusts the camera to show the area of the window most relevant to the viewer.

    messageText.setPosition(0,400); // Sets the text position for the message board.
    messageText.setString(""); // Initializes the message text.

    // Sets all conditional booleans to proper settings for the beginning of the game.
    gamePlaying = true;
    gameOver = false;
    retreated = false;
    beatGame = false;
    playersTurn = true;
    playerCharging = false;
    gamePlayed = true;
}

// Ends the current game.
void endGame(sf::RenderWindow* window)
{
    // All lists are cleared of entities.
    PositionList.clear();
    SpriteList.clear();
    StatusList.clear();
    SightList.clear();
    BrainList.clear();
    PowerupList.clear();
    PsiList.clear();

    // The message log is cleaned out.
    messageLog.clear();

    // All levels are cleaned out.
    dungeonMap.clear();

    // The window is cleaned.
    window->clear();

    window->setView(window->getDefaultView()); // Reset the view.
    if (highScore < score) { highScore = score; } // Update the high score.

    // Terminates the game loop.
    gamePlaying = false;
}

// Gets player's button presses during the menu, and acts on them if needed.
void getMenuControls()
{
    // Help can be viewed from the main menu.
    viewHelp = false;
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::C))) { viewHelp = true; }

    // ENTER starts a new game.
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) { startGame(); }
    // ESC closes the game.
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { gameOpen = false; }
}

// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
// MAIN GAME LOOP
// ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~

int main()
{
    srand(time(NULL)); // The random seed, before anything, is initialized.

    // The main window, 'window', is created.  Its view is assigned to the main camera.
    sf::RenderWindow window(sf::VideoMode(800, 600), "ROGUE CLAWS");
    window.setView(mainView);
    window.setFramerateLimit(60);

    // The game's font is loaded.
    gameFont.loadFromFile("fonts/8bitoperator_jve.ttf");

    // Initializes the in-game text.
    messageText.setFont(gameFont);
    messageText.setCharacterSize(15);
    messageText.setStyle(0);

    repeatClock.restart(); // The clock registering speed of input commands is set to zero before the loop begins.

    // The main game loop, which will operate while the game state is not over and the window has not been closed.
    while (window.isOpen() && gameOpen)
    {
        sf::Event event;
        // The while loop will ensure the window is closed if the close-window button is pressed.
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(); // Clears whatever was drawn on the window in the previous loop, preparing it for redrawing.

        // If the game is underway, run the game's processes;
        if (gamePlaying)
        {
            // If the game is not ending, process the player's movements and the AI.
            if (!gameOver)
            {
                if (playersTurn)
                {
                    getControls(); // Retrieves the player's button input and executes commands based on said input
                }
                else
                {
                    CombatSys.regen(); // At the end of each turn, living units regenerate some health.
                    // The player's position is updated each turn.
                    playerPos.x = (PositionList[0].xCoord);
                    playerPos.y = (PositionList[0].yCoord);
                    canTurbo = true; // If nobody can see the player, they can use turbo.
                    // All enemy units will act when it is not the player's turn.  Only enemies on the current floor can act.
                    std::vector<int> activeEntities = dungeonMap.entitiesInPlay();
                    int enemyID = 0;

                    // For each active entity;
                    for (unsigned int i = 0; i < activeEntities.size(); i++)
                    {
                        enemyID = activeEntities[i];
                        // All enemies' vision is updated in case their position changed.
                        if (SightList.find(enemyID) != SightList.end())
                        {
                            ShadowSys.castShadow(enemyID);
                        }
                        // If that entity has an AI component;
                        if (BrainList.find(enemyID) != BrainList.end())
                        {
                            // If the player is currently standing in the enemy's sight range, their last known player position is updated.
                            if(std::find(SightList[enemyID].visionArea.begin(), SightList[enemyID].visionArea.end(), sf::Vector2i(playerPos.x,playerPos.y)) != SightList[enemyID].visionArea.end())
                            {
                                canTurbo = false;
                                BrainList[enemyID].lastKnownX = playerPos.x;
                                BrainList[enemyID].lastKnownY = playerPos.y;
                            }
                            // The enemy units will act on their turn.
                            AiSys.enemyAction(enemyID);
                        }
                    }
                    playersTurn = true;
                }
            }
            updateCamera(); // Adjusts the camera to show the area of the window most relevant to the viewer.
            window.setView(mainView);
            // The player's field of vision is updated each turn.  (After the view changes, to prevent tearing.)
            ShadowSys.castShadow(0);
            // The block of functions below all deal with rendering the window.
            dungeonMap.drawFloors(&window); // Draws the level's background onto the window.
            // The rendering loop calls for each entity in play.
            std::vector<int> activeEntities = dungeonMap.entitiesInPlay();
            int currentID = 0;
            // All entities currently on the camera view are drawn.
            sf::Vector2i entityLocation;
            for (unsigned int i = 0; i < activeEntities.size(); i++)
            {
                currentID = activeEntities[i];
                entityLocation.x = PositionList[currentID].xCoord; entityLocation.y = PositionList[currentID].yCoord;
                RenderSys.updateSprite(currentID); // Updates the entity's sprite based on its current position and frame.
                // If the entity is in the current camera view,
                if (inCameraBounds(entityLocation.x,entityLocation.y))
                {
                    // If the entity is in the player's current view,
                    if(std::find(SightList[0].visionArea.begin(), SightList[0].visionArea.end(), entityLocation) != SightList[0].visionArea.end())
                    {
                        RenderSys.drawEntity(&window,currentID); // Draws the entity's sprite onto the window.
                    }
                }
            }
            // For the moment, black squares of varying shade are drawn over unseeable areas.
            drawDarkness(&window);
            GuiSys.drawHealthBars(activeEntities,&window); // Draws entities' health bars.
            GuiSys.writeMessages(window.mapPixelToCoords(sf::Vector2i(0,0))); window.draw(messageText); // Updates and then draws the in-game messages.
            GuiSys.drawHUD(&window); // Draws player HUD.
            if (viewHelp) { GuiSys.showHelp(&window,false); } // Draws the help screen.
            // If the game is ending;
            if (gameOver)
            {
                fadeGameOut(&window); // Fades out the game view.
                // If the transition progress is finished, end the game.
                if (transProgress >= 255) { endGame(&window); }
            }
            window.display(); // Draws the window's current view to the screen.
        }
        // If the game is not currently in an active state, the start menu is displayed.
        else
        {
            // The buttons pressed by the player are retrieved, and action taken on them if required.
            getMenuControls();

            // The menu text is drawn.
            // Conditional menu text;
            // If the game has just been launched, and no game has started yet.
            if (!gamePlayed)
            {
                messageText.setString("ROGUE CLAWS");
                messageText.setPosition(330,200);
                messageText.setCharacterSize(30);
                window.draw(messageText);
                messageText.setCharacterSize(15);
                messageText.setString("A student project");
                messageText.setPosition(345,240);
                window.draw(messageText);
            }
            // If the game was ended by going up from floor 1.
            else if (retreated)
            {
                messageText.setString("You retreated from the caverns.");
                messageText.setPosition(305,200);
                window.draw(messageText);
                messageText.setString("Your absorbed ambient energy has faded away.");
                messageText.setPosition(265,220);
                window.draw(messageText);
                messageText.setString("Steel your nerves and try again.");
                messageText.setPosition(305,240);
                window.draw(messageText);
            }
            // If the game was ended by going down from floor 20.
            else if (beatGame)
            {
                messageText.setString("You successfully escaped from the facility through the caverns.");
                messageText.setPosition(210,200);
                window.draw(messageText);
                messageText.setString("Abaddon Incorporated has been exposed as the enemy of all life.");
                messageText.setPosition(215,220);
                window.draw(messageText);
                messageText.setString("Well done.");
                messageText.setPosition(370,240);
                window.draw(messageText);
            }
            // Otherwise (if you died);
            else
            {
                std::stringstream ss;
                ss << score;
                messageText.setString("You perished in level " + ss.str() + " of the caverns.");
                messageText.setPosition(285,200);
                window.draw(messageText);
                messageText.setString("But your will to survive is overpoweringly strong.");
                messageText.setPosition(250,220);
                window.draw(messageText);
                messageText.setString("This death will not be permanent.");
                messageText.setPosition(305,240);
                window.draw(messageText);
            }

            // The high score is shown if it has been set.
            if (gamePlayed)
            {
                std::stringstream ss;
                ss << highScore;
                messageText.setString("Your high score is " + ss.str() + ".");
                messageText.setPosition(340,320);
                window.draw(messageText);
            }

            // This menu text always appears.
            messageText.setString("Press ENTER to start the game");
            messageText.setPosition(315,400);
            window.draw(messageText);
            messageText.setString("Press ESC to quit");
            messageText.setPosition(350,420);
            window.draw(messageText);
            messageText.setString("Hold C to view help");
            messageText.setPosition(345,440);
            window.draw(messageText);

            if (viewHelp) { GuiSys.showHelp(&window,true); } // Draws the help screen.  Works on the main menu.

            window.display(); // Draws the window's current view to the screen.
        }
    }

    return 0;
}
