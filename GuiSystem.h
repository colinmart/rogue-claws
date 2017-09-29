#ifndef GUISYSTEM_H_INCLUDED
#define GUISYSTEM_H_INCLUDED

// Deals with the drawing of health bars and messages to inform the user about what is currently happening.

class GuiSystem
{
private:
    sf::Text guiText;
    sf::Texture guiTexture;
    sf::Sprite atkSprite;
    sf::Sprite defSprite;
    sf::Sprite rgnSprite;
    sf::Sprite psiBright;
    sf::Sprite psiDark;
public:
    GuiSystem();
    // Draws character healthbars below their sprites.
    void drawHealthBars(std::vector<int> activeEntities, sf::RenderWindow* window);
    // Writes the currently visible messages from the message log to the screen.
    void writeMessages(sf::Vector2f winCoords);
    // Draws the HUD for the player.
    void drawHUD(sf::RenderWindow* window);
    // Draws the game help menu.  mainMenu determines if the main menu is up or not.
    void showHelp(sf::RenderWindow* window, bool mainMenu);
};

#endif // GUISYSTEM_H_INCLUDED
