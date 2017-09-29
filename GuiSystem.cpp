#include "GuiSystem.h"

#include "PositionComponent.h"
#include "StatusComponent.h"
#include "PsiComponent.h"
#include "TextureContainer.h"

extern std::unordered_map<int,PositionComponent> PositionList;
extern std::unordered_map<int,StatusComponent> StatusList;
extern std::unordered_map<int,PsiComponent> PsiList;

extern std::vector<std::string> messageLog;
extern sf::Text messageText;
extern sf::View mainView;
extern sf::Font gameFont;
extern std::vector<sf::Texture> globalTextures;
extern int textDisplace;

GuiSystem::GuiSystem()
{
    guiText.setFont(gameFont);
    guiText.setCharacterSize(15);
    guiText.setStyle(0);
    guiTexture.loadFromFile("sprites/guiSprites.png");
    atkSprite.setTexture(guiTexture);
    atkSprite.setTextureRect(sf::IntRect(83,0,19,19));
    defSprite.setTexture(guiTexture);
    defSprite.setTextureRect(sf::IntRect(101,0,19,19));
    rgnSprite.setTexture(guiTexture);
    rgnSprite.setTextureRect(sf::IntRect(65,0,19,19));
    psiBright.setTexture(guiTexture);
    psiBright.setTextureRect(sf::IntRect(0,0,32,32));
    psiDark.setTexture(guiTexture);
    psiDark.setTextureRect(sf::IntRect(33,0,32,32));
}

// Draws a health bar beneath all characters in the game with health.
void GuiSystem::drawHealthBars(std::vector<int> activeEntities, sf::RenderWindow* window)
{
    int entityX, entityY, entityID;
    float HPPer;
    // Assembles the healthbar.
    sf::RectangleShape healthback(sf::Vector2f(30,3));
    sf::RectangleShape healthbar(sf::Vector2f(30,2));
    healthback.setFillColor(sf::Color(0,0,0,255));
    healthbar.setFillColor(sf::Color(0,255,0,255));

    // For each active entity, their healthbar is drawn based on a percentage of their HP.
    for (auto it = StatusList.begin(); it != StatusList.end(); ++it)
    {
        entityID = it->second.id;

        if (std::find(activeEntities.begin(), activeEntities.end(), entityID) != activeEntities.end()) // If the item is active at the moment
        {
            entityX = PositionList[entityID].xCoord*32;
            entityY = PositionList[entityID].yCoord*32;
            HPPer = (float)it->second.health / (float)it->second.maxhealth;
            healthbar.setScale(HPPer,1);
            if (HPPer < 1)
            {
                healthback.setPosition(entityX+1,entityY+33);
                window->draw(healthback);
                healthbar.setPosition(entityX+1,entityY+33);
                window->draw(healthbar);
            }
        }
    }
}

// Draws the most recent messages in the message log.
void GuiSystem::writeMessages(sf::Vector2f winCoords)
{
    // The most recent six messages are displayed, or less if there are less in the log
    int messageNumber = std::min(7,int(messageLog.size()));
    int lowBound = messageLog.size() - messageNumber - 1;
    std::string messageContent = "";

    for (int i = messageLog.size()-1; i > lowBound; i--)
    {
        // The message displacement scrolling can be used to view earlier messages
        messageContent += messageLog[i-textDisplace];
        messageContent += "\n";
    }

    messageText.setPosition(5+winCoords.x,485+winCoords.y);
    messageText.setString(messageContent);
}

// Draws the HUD for the player.
void GuiSystem::drawHUD(sf::RenderWindow* window)
{
    std::stringstream ss;
    // Displacement of the view is obtained, allowing for proper drawing of the healthbar.
    // The top-left and bottom-right corner of the view, in terms of game tiles, are obtained; this tells us what tiles we need to consider for drawing.
    sf::Vector2f viewSize = mainView.getSize();
    sf::Vector2f viewCornerNW = mainView.getCenter();
    viewCornerNW.x = viewCornerNW.x - (viewSize.x/2); viewCornerNW.y = viewCornerNW.y - (viewSize.y/2);
    // Assembles the healthbar.
    float HPPer;
    sf::RectangleShape healthback(sf::Vector2f(300,30));
    healthback.setFillColor(sf::Color(20,20,20,255));
    healthback.setPosition(10+viewCornerNW.x,10+viewCornerNW.y);
    sf::RectangleShape healthbar(sf::Vector2f(296,25));
    healthbar.setPosition(12+viewCornerNW.x,11+viewCornerNW.y);
    healthbar.setFillColor(sf::Color(0,225,0,255));
    // The player's large healthbar is scaled and drawn.
    HPPer = (float)StatusList[0].health / (float)StatusList[0].maxhealth;
    healthbar.setScale(HPPer,1);
    window->draw(healthback);
    window->draw(healthbar);

    // The stat icons are drawn.
    atkSprite.setPosition(10+viewCornerNW.x,40+viewCornerNW.y);
    window->draw(atkSprite);
    defSprite.setPosition(9+viewCornerNW.x,62+viewCornerNW.y);
    window->draw(defSprite);
    rgnSprite.setPosition(11+viewCornerNW.x,84+viewCornerNW.y);
    window->draw(rgnSprite);

    // The stats are written out beside each icon..
    ss << StatusList[0].attack;
    guiText.setPosition(32+viewCornerNW.x,40+viewCornerNW.y);
    guiText.setString(ss.str());
    window->draw(guiText);
    ss.str(std::string());
    ss << StatusList[0].defense;
    guiText.setPosition(32+viewCornerNW.x,62+viewCornerNW.y);
    guiText.setString(ss.str());
    window->draw(guiText);
    ss.str(std::string());
    ss << StatusList[0].regen;
    guiText.setPosition(32+viewCornerNW.x,84+viewCornerNW.y);
    guiText.setString(ss.str());
    window->draw(guiText);
    ss.str(std::string());

    // The remaining psi charges are displayed in the right-hand corner.
    int psiDraw = PsiList[0].psiCharges;
    psiDark.setPosition((800+viewCornerNW.x)-42,viewCornerNW.y+10);
    window->draw(psiDark);
    // Bright charges are drawn for each psi charge possessed.
    if (psiDraw >= 1) { psiBright.setPosition((800+viewCornerNW.x)-42,viewCornerNW.y+10); window->draw(psiBright); }
    psiDark.setPosition((800+viewCornerNW.x)-78,viewCornerNW.y+10);
    window->draw(psiDark);
    if (psiDraw >= 2) { psiBright.setPosition((800+viewCornerNW.x)-78,viewCornerNW.y+10); window->draw(psiBright); }
    psiDark.setPosition((800+viewCornerNW.x)-114,viewCornerNW.y+10);
    window->draw(psiDark);
    if (psiDraw >= 3) { psiBright.setPosition((800+viewCornerNW.x)-114,viewCornerNW.y+10); window->draw(psiBright); }

    // Draws current health as a digit onto the healthbar.
    ss << StatusList[0].health;
    guiText.setPosition(15+viewCornerNW.x,2+viewCornerNW.y);
    guiText.setString(ss.str());
    guiText.setCharacterSize(30);
    window->draw(guiText);
    guiText.setCharacterSize(15);
}

// Draws the game help menu.  mainMenu determines if the main menu is up or not.
void GuiSystem::showHelp(sf::RenderWindow* window, bool mainMenu)
{
    // This string represents the game's help options.
    std::string helpString = "HOW TO PLAY >>>\n";
    helpString += "Escape from Abaddon Incorporated's secret facilities.\n";
    helpString += "If confronted with enemies, bump into and strike them, or blast them with psychic power.\n";
    helpString += "Absorb strong ambient energy to increase your abilities.\n";
    helpString += "Descend as deep into the facility's caverns as you can.\n";
    helpString += "\n";
    helpString += "CONTROLS >>>\n";
    helpString += "W: Move north.\n";
    helpString += "A: Move west.\n";
    helpString += "S: Move south.\n";
    helpString += "D: Move east.\n";
    helpString += "F: Charge Mind Blast.\n";
    helpString += "F > W/A/S/D: Fire Mind Blast (in that direction).\n";
    helpString += "E: Absorb strong ambient energy beneath you.\n";
    helpString += "R: Wait one turn.\n";
    helpString += "U: Ascend or descend a staircase beneath you.\n";
    helpString += "Z: Scroll the message log up.\n";
    helpString += "X: Scroll the message log down.\n";
    helpString += "Hold SHIFT: Accelerate time.  Cannot accelerate time while being hunted by enemies.\n";
    helpString += "Hold C: Display this help menu.\n";
    // The top-left and bottom-right corner of the view, in terms of game tiles, are obtained; this tells us what tiles we need to consider for drawing.
    // If the main menu is open, though, we won't need it.
    sf::Vector2i disp(0,0);
    if (!mainMenu)
    {
        sf::Vector2f viewSize = mainView.getSize();
        sf::Vector2f viewCornerNW = mainView.getCenter();
        disp.x = viewCornerNW.x - (viewSize.x/2); disp.y = viewCornerNW.y - (viewSize.y/2);
    }
    // Assembles the help box.
    sf::RectangleShape blackBack1(sf::Vector2f(706,506));
    sf::RectangleShape blackBack2(sf::Vector2f(700,500));
    sf::RectangleShape whiteBack(sf::Vector2f(704,504));
    blackBack1.setPosition(47+disp.x,47+disp.y);
    blackBack2.setPosition(50+disp.x,50+disp.y);
    whiteBack.setPosition(48+disp.x,48+disp.y);
    blackBack1.setFillColor(sf::Color(0,0,0,255));
    blackBack2.setFillColor(sf::Color(0,0,0,255));
    whiteBack.setFillColor(sf::Color(255,255,255,255));
    guiText.setPosition(54+disp.x,54+disp.y);
    guiText.setString(helpString);
    // The window and help text is drawn to the screen.
    window->draw(blackBack1);
    window->draw(whiteBack);
    window->draw(blackBack2);
    window->draw(guiText);
}
