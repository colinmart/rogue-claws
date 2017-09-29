#ifndef TEXTURECONTAINER_H_INCLUDED
#define TEXTURECONTAINER_H_INCLUDED

// Holds all textures used in the game to prevent them from going out of scope.

class TextureContainer
{
public:
    std::vector<sf::Texture> charaTextures;
    TextureContainer()
    {
        for (int i = 0; i < 3; i++)
        {
            sf::Texture newTexture;
            charaTextures.push_back(newTexture);
        }
        charaTextures[0].loadFromFile("sprites/playerSprite.png");
        charaTextures[1].loadFromFile("sprites/enemySprites.png");
        charaTextures[2].loadFromFile("sprites/objectSprites.png");
    }
};

#endif // TEXTURECONTAINER_H_INCLUDED
