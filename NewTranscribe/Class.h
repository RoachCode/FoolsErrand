#pragma once
#include "Sprite.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <Windows.h>
#include <WinUser.h>
#include <stdint.h>
#include <vector>
#include <set>

#include <fstream> // for tilemap array loading

std::string filepath = "C:/LastLegend/Resources/";

enum SpriteColorizer
{
    Default,
    Transparent,
    Black,
    Inverted,
    Blue,
    Dark,
};
enum Area
{
    WestKagar,
    WestKagarInn,
    KagarCaverns,
};
enum Direction
{
    Up,
    Down,
    Left,
    Right
};

class Window : public sf::RenderWindow, public sf::View
{
private:
    unsigned int screenWidth{ sf::VideoMode::getDesktopMode().width };
    unsigned int screenHeight{ sf::VideoMode::getDesktopMode().height };
    int numberOfTilemapsX{ 0 };
    int numberOfTilemapsY{ 0 };
    void getHz()
    {
        DEVMODE dm;
        memset(&dm, 0, sizeof(DEVMODE));
        dm.dmSize = sizeof(DEVMODE);
        EnumDisplaySettingsEx(NULL, ENUM_CURRENT_SETTINGS, &dm, 0);
        FPS = dm.dmDisplayFrequency;

    }
    void setSize()
    {
        if (screenHeight > 4 * 448 && screenWidth > 4 * 768)
        {
            zoomFloat = 0.25f;
        }
        else
        {
            zoomFloat = 0.5f;
        }
        gameWidthInGrids = static_cast<int>(screenWidth / (32 / zoomFloat));
        gameHeightInGrids = static_cast<int>(screenHeight / (32 / zoomFloat));
        extraPixelsX = (screenWidth - (gameWidthInGrids * (32 / zoomFloat))) * zoomFloat;
        extraPixelsY = (screenHeight - (gameHeightInGrids * (32 / zoomFloat))) * zoomFloat;

        sf::View view1(sf::FloatRect(0, 0, screenWidth * zoomFloat, screenHeight * zoomFloat));

        view1.setCenter(screenWidth / (2 / zoomFloat) + extraPixelsX * 0.5, screenHeight / (2 / zoomFloat) + extraPixelsY * 0.5);


        //view1.zoom(zoomFloat);
        viewGlobal = view1;
        window.setView(viewGlobal);

        initialViewX = screenWidth / (2 / zoomFloat) + extraPixelsX * 0.5;
        initialViewY = screenHeight / (2 / zoomFloat) + extraPixelsY * 0.5;
    }

    sf::RectangleShape fullScreenOverlay;
public:
    uint8_t fadeToBlackAlpha{ 0 };

    short FPS{ 60 };
    float zoomFloat{ 0.5f };
    int viewX{ 0 };
    int viewY{ 0 };
    int gameWidthInGrids{ 24 };
    int gameHeightInGrids{ 14 };
    int extraPixelsX{ 0 };
    int extraPixelsY{ 0 };
    int initialViewX{ 0 };
    int initialViewY{ 0 };
    int sizeOfVector{ 0 };
    uint8_t scanLineOpacity{ 5 };
    sf::RenderWindow window;
    sf::View viewGlobal;
    void showScanLines()
    {
        if (scanLineOpacity > 8)
        {
            scanLineOpacity = 1;
        }
        else if (scanLineOpacity < 1)
        {
            scanLineOpacity = 8;
        }
        sf::VertexArray line(sf::Lines, 2);


        line[0].color = sf::Color(5, 55, 155, 3 * scanLineOpacity);
        line[1].color = sf::Color(5, 55, 155, 3 * scanLineOpacity);
        for (int i = 0; i < numberOfTilemapsY * 448; i += 2)
        {
            //horiz
            line[0].position = sf::Vector2f(0.f, i);
            line[1].position = sf::Vector2f(numberOfTilemapsX * 768, i);
            window.draw(line);
        }
        line[0].color = sf::Color(0, 0, 15, 30 * scanLineOpacity);
        line[1].color = sf::Color(0, 0, 15, 30 * scanLineOpacity);

        for (int i = 0; i < numberOfTilemapsX * 768; i++)
        {
            //vert
            line[0].position = sf::Vector2f(i, 0.f);
            line[1].position = sf::Vector2f(i, numberOfTilemapsY * 448);
            window.draw(line);
        }
        

    }
    void init()
    {
        getHz();
        window.create(sf::VideoMode(screenWidth, screenHeight), "DEFAULT", sf::Style::Fullscreen);
        setSize();
        window.setVerticalSyncEnabled(true);
        window.setMouseCursorVisible(false);
        fullScreenOverlay.setSize(sf::Vector2f(getScreenSize()));
    }
    void setTilemapCount(int x, int y)
    {
        if (x > 0 && x < 5 && y > 0 && y < 5)
        {
            numberOfTilemapsX = x;
            numberOfTilemapsY = y;
        }
    }
    sf::Vector2i getTilemapCount()
    {
        return sf::Vector2i(numberOfTilemapsX, numberOfTilemapsY);
    }
    sf::Vector2u getScreenSize()
    {
        return sf::Vector2u(screenWidth, screenHeight);
    }
    void crash()
    {
        window.close();
    }
    // While returning true this function is busy fading to or from black.
    bool pollFadeToBlack(bool fadeTo)
    {
        fullScreenOverlay.setFillColor(sf::Color(0, 0, 0, fadeToBlackAlpha));
        if (fadeToBlackAlpha > 0)
        {
            window.draw(fullScreenOverlay);
        }
        if (fadeTo)
        {
            if (fadeToBlackAlpha < 255)
            {
                fadeToBlackAlpha += 15;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if (fadeToBlackAlpha > 0)
            {
                fadeToBlackAlpha -= 15;
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    sf::Vector2f zeroCoords()
    {
        return sf::Vector2f(viewGlobal.getCenter().x - getScreenSize().x / (2 / zoomFloat), 
                            viewGlobal.getCenter().y - getScreenSize().y / (2 / zoomFloat));
    }
};
Window W;

class TextClass
{
public:
    sf::Font font;
    sf::Text text;
    void init()
    {
        font.loadFromFile(filepath + "lucid.ttf");
        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);
    }
    bool bannerBool;
    std::string string;
    void display(float x, float y, std::string string, int size=16)
    {
        
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + 1, y + 1);
        text.setString(string);
        W.window.draw(text);
        text.setFillColor(sf::Color::White);
        text.setPosition(x, y);
        text.setString(string);
        W.window.draw(text);
    }
    void textBanner(std::string s)
    {
        string = s;
        bannerBool = true;
    }

    void textBox()
    {

    }
    void pollText()
    {
        sf::Clock static bannerClock;
        if (bannerBool)
        {
            sf::Time t = bannerClock.getElapsedTime();
            if (t.asMilliseconds() > 3000)
            {
                bannerBool = false;
            }
            string = "WASABI";
            display(W.viewGlobal.getCenter().x - (string.length() * 48) / 4, W.zeroCoords().y + 8, string, 48);
        }
        else
        {
            bannerClock.restart();
        }
    }

};
TextClass Text;

class AudioClass
{
public:
    void changeMusic()
    {

    }
};
AudioClass Audio;

class TileMap : public sf::Drawable, public sf::Transformable
{
public:

    bool load(const sf::Image& tileImage, sf::Vector2u tileSize, const unsigned short* tiles, unsigned int width, unsigned int height)
    {
        // load the tileset texture
        if (!m_tileset.loadFromImage(tileImage))
            return false;

        // resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::Quads);
        m_vertices.resize(width * height * 4);

        // populate the vertex array, with one quad per tile
        for (unsigned int i = 0; i < width; ++i)
        {
            for (unsigned int j = 0; j < height; ++j)
            {
                // get the current tile number
                int tileNumber = tiles[i + j * width];

                // find its position in the tileset texture
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices[(i + j * width) * 4];

                // define its 4 corners
                quad[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                quad[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                quad[2].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
                quad[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

                // define its 4 texture coordinates
                quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
                quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);

            }
        }
        return true;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};

class Character
{
public:
    // BOOLS
#pragma region
    bool downABool{ false };
    bool downBBool{ false };
    bool downCBool{ false };
    bool upABool{ false };
    bool upBBool{ false };
    bool upCBool{ false };
    bool leftABool{ false };
    bool leftBBool{ false };
    bool leftCBool{ false };
    bool rightABool{ false };
    bool rightBBool{ false };
    bool rightCBool{ false };
    bool frontBool{ false };
    bool backBool{ false };
    bool idleRBool{ false };
    bool idleLBool{ false };
    bool shrug1RBool{ false };
    bool shrug1LBool{ false };
    bool shrug2RBool{ false };
    bool shrug2LBool{ false };
    bool shrug2mouthRBool{ false };
    bool shrug2mouthLBool{ false };
    bool crawlRBool{ false };
    bool crawlLBool{ false };
    bool pointRBool{ false };
    bool pointLBool{ false };
    bool handsUpRBool{ false };
    bool handsUpLBool{ false };
    bool pushRBool{ false };
    bool pushLBool{ false };
    bool deadRBool{ false };
    bool deadLBool{ false };
    bool hitRBool{ false };
    bool hitLBool{ false };
#pragma endregion
    int order{ 0 };
    int id{ 0 };
    sf::Texture spriteTexture;
    sf::Texture portraitTexture;
    sf::Sprite sprite;
    sf::RectangleShape portrait;
    // TRANSPARENT, BLACK, INVERTED, DEFAULT
    int spriteColour{};
    const uint8_t spriteW{ 32 };
    const uint8_t spriteH{ 32 };
    std::vector<uint8_t> temp{};
    void clearBools()
    {

        downABool = false;
        downBBool = false;
        downCBool = false;
        upABool = false;
        upBBool = false;
        upCBool = false;
        leftABool = false;
        leftBBool = false;
        leftCBool = false;
        rightABool = false;
        rightBBool = false;
        rightCBool = false;
        frontBool = false;
        backBool = false;
        idleRBool = false;
        idleLBool = false;
        shrug1RBool = false;
        shrug1LBool = false;
        shrug2RBool = false;
        shrug2LBool = false;
        shrug2mouthRBool = false;
        shrug2mouthLBool = false;
        crawlRBool = false;
        crawlLBool = false;
        pointRBool = false;
        pointLBool = false;
        handsUpRBool = false;
        handsUpLBool = false;
        pushRBool = false;
        pushLBool = false;
        deadRBool = false;
        deadLBool = false;
        hitRBool = false;
        hitLBool = false;
    }
    void pickArray()
    {
        switch (id)
        {
        case 0:
            break;
        case 1:
            if (downABool == true)
            {
                for (int i = 0; i < arsonDownA.size(); i++)
                {
                    temp.push_back(arsonDownA[i]);
                }
            }
            else if (downBBool == true)
            {
                for (int i = 0; i < arsonDownB.size(); i++)
                {
                    temp.push_back(arsonDownB[i]);
                }
            }
            else if (downCBool == true)
            {
                for (int i = 0; i < arsonDownC.size(); i++)
                {
                    temp.push_back(arsonDownC[i]);
                }
            }
            else if (upABool == true)
            {
                for (int i = 0; i < arsonUpA.size(); i++)
                {
                    temp.push_back(arsonUpA[i]);
                }
            }
            else if (upBBool == true)
            {
                for (int i = 0; i < arsonUpB.size(); i++)
                {
                    temp.push_back(arsonUpB[i]);
                }
            }
            else if (upCBool == true)
            {
                for (int i = 0; i < arsonUpC.size(); i++)
                {
                    temp.push_back(arsonUpC[i]);
                }
            }
            else if (leftABool == true)
            {
                for (int i = 0; i < arsonLeftA.size(); i++)
                {
                    temp.push_back(arsonLeftA[i]);
                }
            }
            else if (leftBBool == true)
            {
                for (int i = 0; i < arsonLeftB.size(); i++)
                {
                    temp.push_back(arsonLeftB[i]);
                }
            }
            else if (leftCBool == true)
            {
                for (int i = 0; i < arsonLeftC.size(); i++)
                {
                    temp.push_back(arsonLeftC[i]);
                }
            }
            else if (rightABool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonLeftA[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightBBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonLeftB[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightCBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonLeftC[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (backBool == true)
            {
                for (int i = 0; i < arsonBack.size(); i++)
                {
                    temp.push_back(arsonBack[i]);
                }
            }
            else if (frontBool == true)
            {
                for (int i = 0; i < arsonFront.size(); i++)
                {
                    temp.push_back(arsonFront[i]);
                }
            }
            else if (idleRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonIdleL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (idleLBool == true)
            {
                for (int i = 0; i < arsonIdleL.size(); i++)
                {
                    temp.push_back(arsonIdleL[i]);
                }
            }
            else if (pointRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonPointL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pointLBool == true)
            {
                for (int i = 0; i < arsonPointL.size(); i++)
                {
                    temp.push_back(arsonPointL[i]);
                }
            }
            else if (crawlRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (crawlLBool == true)
            {
                for (int i = 0; i < arsonCrawlL.size(); i++)
                {
                    temp.push_back(arsonCrawlL[i]);
                }
            }
            else if (shrug1RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug1LBool == true)
            {
                for (int i = 0; i < arsonShrug1L.size(); i++)
                {
                    temp.push_back(arsonShrug1L[i]);
                }
            }
            else if (shrug2RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2LBool == true)
            {
                for (int i = 0; i < arsonShrug2L.size(); i++)
                {
                    temp.push_back(arsonShrug2L[i]);
                }
            }
            else if (shrug2mouthRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2mouthLBool == true)
            {
                for (int i = 0; i < arsonShrug2MouthL.size(); i++)
                {
                    temp.push_back(arsonShrug2MouthL[i]);
                }
            }
            else if (deadRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonDeadL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (deadLBool == true)
            {
                for (int i = 0; i < arsonDeadL.size(); i++)
                {
                    temp.push_back(arsonDeadL[i]);
                }
            }
            else if (pushRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonPushL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pushLBool == true)
            {
                for (int i = 0; i < arsonPushL.size(); i++)
                {
                    temp.push_back(arsonPushL[i]);
                }
            }
            else if (handsUpRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (handsUpLBool == true)
            {
                for (int i = 0; i < arsonHandsUpL.size(); i++)
                {
                    temp.push_back(arsonHandsUpL[i]);
                }
            }
            else if (hitRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(arsonHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(arsonHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(arsonHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(arsonHitL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (hitLBool == true)
            {
                for (int i = 0; i < arsonHitL.size(); i++)
                {
                    temp.push_back(arsonHitL[i]);
                }
            }
            else
            {
                for (int i = 0; i < arsonUpA.size(); i++)
                {
                    temp.push_back(255);
                }
            }
        break;
        case 2:
            if (downABool == true)
                {
                    for (int i = 0; i < gaiaDownA.size(); i++)
                    {
                        temp.push_back(gaiaDownA[i]);
                    }
                }
            else if (downBBool == true)
                {
                    for (int i = 0; i < gaiaDownB.size(); i++)
                    {
                        temp.push_back(gaiaDownB[i]);
                    }
                }
            else if (downCBool == true)
                {
                    for (int i = 0; i < gaiaDownC.size(); i++)
                    {
                        temp.push_back(gaiaDownC[i]);
                    }
                }
            else if (upABool == true)
                {
                    for (int i = 0; i < gaiaUpA.size(); i++)
                    {
                        temp.push_back(gaiaUpA[i]);
                    }
                }
            else if (upBBool == true)
                {
                    for (int i = 0; i < gaiaUpB.size(); i++)
                    {
                        temp.push_back(gaiaUpB[i]);
                    }
                }
            else if (upCBool == true)
                {
                    for (int i = 0; i < gaiaUpC.size(); i++)
                    {
                        temp.push_back(gaiaUpC[i]);
                    }
                }
            else if (leftABool == true)
                {
                    for (int i = 0; i < gaiaLeftA.size(); i++)
                    {
                        temp.push_back(gaiaLeftA[i]);
                    }
                }
            else if (leftBBool == true)
                {
                    for (int i = 0; i < gaiaLeftB.size(); i++)
                    {
                        temp.push_back(gaiaLeftB[i]);
                    }
                }
            else if (leftCBool == true)
                {
                    for (int i = 0; i < gaiaLeftC.size(); i++)
                    {
                        temp.push_back(gaiaLeftC[i]);
                    }
                }
            else if (rightABool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (rightBBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (rightCBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (backBool == true)
                {
                    for (int i = 0; i < gaiaBack.size(); i++)
                    {
                        temp.push_back(gaiaBack[i]);
                    }
                }
            else if (frontBool == true)
                {
                    for (int i = 0; i < gaiaFront.size(); i++)
                    {
                        temp.push_back(gaiaFront[i]);
                    }
                }
            else if (idleRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (idleLBool == true)
                {
                    for (int i = 0; i < gaiaIdleL.size(); i++)
                    {
                        temp.push_back(gaiaIdleL[i]);
                    }
                }
            else if (pointRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (pointLBool == true)
                {
                    for (int i = 0; i < gaiaPointL.size(); i++)
                    {
                        temp.push_back(gaiaPointL[i]);
                    }
                }
            else if (crawlRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (crawlLBool == true)
                {
                    for (int i = 0; i < gaiaCrawlL.size(); i++)
                    {
                        temp.push_back(gaiaCrawlL[i]);
                    }
                }
            else if (shrug1RBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (shrug1LBool == true)
                {
                    for (int i = 0; i < gaiaShrug1L.size(); i++)
                    {
                        temp.push_back(gaiaShrug1L[i]);
                    }
                }
            else if (shrug2RBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (shrug2LBool == true)
                {
                    for (int i = 0; i < gaiaShrug2L.size(); i++)
                    {
                        temp.push_back(gaiaShrug2L[i]);
                    }
                }
            else if (shrug2mouthRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (shrug2mouthLBool == true)
                {
                    for (int i = 0; i < gaiaShrug2MouthL.size(); i++)
                    {
                        temp.push_back(gaiaShrug2MouthL[i]);
                    }
                }
            else if (deadRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (deadLBool == true)
                {
                    for (int i = 0; i < gaiaDeadL.size(); i++)
                    {
                        temp.push_back(gaiaDeadL[i]);
                    }
                }
            else if (pushRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (pushLBool == true)
                {
                    for (int i = 0; i < gaiaPushL.size(); i++)
                    {
                        temp.push_back(gaiaPushL[i]);
                    }
                }
            else if (handsUpRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (handsUpLBool == true)
                {
                    for (int i = 0; i < gaiaHandsUpL.size(); i++)
                    {
                        temp.push_back(gaiaHandsUpL[i]);
                    }
                }
            else if (hitRBool == true)
                {
                    for (int k = 0; k < 32; k++) // vertical count
                    {
                        for (int j = 0; j < 4 * 32; j += 4)
                        {
                            temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                            temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                            temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                            temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                        }
                    }
                }
            else if (hitLBool == true)
                {
                    for (int i = 0; i < gaiaHitL.size(); i++)
                    {
                        temp.push_back(gaiaHitL[i]);
                    }
                }
            else
                {
                    for (int i = 0; i < gaiaUpA.size(); i++)
                    {
                        temp.push_back(255);
                    }
                }
        break;
        case 3:
            if (downABool == true)
                    {
                        for (int i = 0; i < coleDownA.size(); i++)
                        {
                            temp.push_back(coleDownA[i]);
                        }
                    }
            else if (downBBool == true)
                    {
                        for (int i = 0; i < coleDownB.size(); i++)
                        {
                            temp.push_back(coleDownB[i]);
                        }
                    }
            else if (downCBool == true)
                    {
                        for (int i = 0; i < coleDownC.size(); i++)
                        {
                            temp.push_back(coleDownC[i]);
                        }
                    }
            else if (upABool == true)
                    {
                        for (int i = 0; i < coleUpA.size(); i++)
                        {
                            temp.push_back(coleUpA[i]);
                        }
                    }
            else if (upBBool == true)
                    {
                        for (int i = 0; i < coleUpB.size(); i++)
                        {
                            temp.push_back(coleUpB[i]);
                        }
                    }
            else if (upCBool == true)
                    {
                        for (int i = 0; i < coleUpC.size(); i++)
                        {
                            temp.push_back(coleUpC[i]);
                        }
                    }
            else if (leftABool == true)
                    {
                        for (int i = 0; i < coleLeftA.size(); i++)
                        {
                            temp.push_back(coleLeftA[i]);
                        }
                    }
            else if (leftBBool == true)
                    {
                        for (int i = 0; i < coleLeftB.size(); i++)
                        {
                            temp.push_back(coleLeftB[i]);
                        }
                    }
            else if (leftCBool == true)
                    {
                        for (int i = 0; i < coleLeftC.size(); i++)
                        {
                            temp.push_back(coleLeftC[i]);
                        }
                    }
            else if (rightABool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleLeftA[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (rightBBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleLeftB[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (rightCBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleLeftC[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (backBool == true)
                    {
                        for (int i = 0; i < coleBack.size(); i++)
                        {
                            temp.push_back(coleBack[i]);
                        }
                    }
            else if (frontBool == true)
                    {
                        for (int i = 0; i < coleFront.size(); i++)
                        {
                            temp.push_back(coleFront[i]);
                        }
                    }
            else if (idleRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleIdleL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (idleLBool == true)
                    {
                        for (int i = 0; i < coleIdleL.size(); i++)
                        {
                            temp.push_back(coleIdleL[i]);
                        }
                    }
            else if (pointRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(colePointL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(colePointL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(colePointL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(colePointL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (pointLBool == true)
                    {
                        for (int i = 0; i < colePointL.size(); i++)
                        {
                            temp.push_back(colePointL[i]);
                        }
                    }
            else if (crawlRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (crawlLBool == true)
                    {
                        for (int i = 0; i < coleCrawlL.size(); i++)
                        {
                            temp.push_back(coleCrawlL[i]);
                        }
                    }
            else if (shrug1RBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (shrug1LBool == true)
                    {
                        for (int i = 0; i < coleShrug1L.size(); i++)
                        {
                            temp.push_back(coleShrug1L[i]);
                        }
                    }
            else if (shrug2RBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (shrug2LBool == true)
                    {
                        for (int i = 0; i < coleShrug2L.size(); i++)
                        {
                            temp.push_back(coleShrug2L[i]);
                        }
                    }
            else if (shrug2mouthRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (shrug2mouthLBool == true)
                    {
                        for (int i = 0; i < coleShrug2MouthL.size(); i++)
                        {
                            temp.push_back(coleShrug2MouthL[i]);
                        }
                    }
            else if (deadRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleDeadL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (deadLBool == true)
                    {
                        for (int i = 0; i < coleDeadL.size(); i++)
                        {
                            temp.push_back(coleDeadL[i]);
                        }
                    }
            else if (pushRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(colePushL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(colePushL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(colePushL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(colePushL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (pushLBool == true)
                    {
                        for (int i = 0; i < colePushL.size(); i++)
                        {
                            temp.push_back(colePushL[i]);
                        }
                    }
            else if (handsUpRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (handsUpLBool == true)
                    {
                        for (int i = 0; i < coleHandsUpL.size(); i++)
                        {
                            temp.push_back(coleHandsUpL[i]);
                        }
                    }
            else if (hitRBool == true)
                    {
                        for (int k = 0; k < 32; k++) // vertical count
                        {
                            for (int j = 0; j < 4 * 32; j += 4)
                            {
                                temp.push_back(coleHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                                temp.push_back(coleHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                                temp.push_back(coleHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                                temp.push_back(coleHitL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                            }
                        }
                    }
            else if (hitLBool == true)
                    {
                        for (int i = 0; i < coleHitL.size(); i++)
                        {
                            temp.push_back(coleHitL[i]);
                        }
                    }
            else
                    {
                        for (int i = 0; i < coleUpA.size(); i++)
                        {
                            temp.push_back(255);
                        }
                    }
        break;
        case 4:
            if (downABool == true)
            {
                for (int i = 0; i < neekoDownA.size(); i++)
                {
                    temp.push_back(neekoDownA[i]);
                }
            }
            else if (downBBool == true)
            {
                for (int i = 0; i < neekoDownB.size(); i++)
                {
                    temp.push_back(neekoDownB[i]);
                }
            }
            else if (downCBool == true)
            {
                for (int i = 0; i < neekoDownC.size(); i++)
                {
                    temp.push_back(neekoDownC[i]);
                }
            }
            else if (upABool == true)
            {
                for (int i = 0; i < neekoUpA.size(); i++)
                {
                    temp.push_back(neekoUpA[i]);
                }
            }
            else if (upBBool == true)
            {
                for (int i = 0; i < neekoUpB.size(); i++)
                {
                    temp.push_back(neekoUpB[i]);
                }
            }
            else if (upCBool == true)
            {
                for (int i = 0; i < neekoUpC.size(); i++)
                {
                    temp.push_back(neekoUpC[i]);
                }
            }
            else if (leftABool == true)
            {
                for (int i = 0; i < neekoLeftA.size(); i++)
                {
                    temp.push_back(neekoLeftA[i]);
                }
            }
            else if (leftBBool == true)
            {
                for (int i = 0; i < neekoLeftB.size(); i++)
                {
                    temp.push_back(neekoLeftB[i]);
                }
            }
            else if (leftCBool == true)
            {
                for (int i = 0; i < neekoLeftC.size(); i++)
                {
                    temp.push_back(neekoLeftC[i]);
                }
            }
            else if (rightABool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoLeftA[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightBBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoLeftB[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightCBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoLeftC[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (backBool == true)
            {
                for (int i = 0; i < neekoBack.size(); i++)
                {
                    temp.push_back(neekoBack[i]);
                }
            }
            else if (frontBool == true)
            {
                for (int i = 0; i < neekoFront.size(); i++)
                {
                    temp.push_back(neekoFront[i]);
                }
            }
            else if (idleRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoIdleL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (idleLBool == true)
            {
                for (int i = 0; i < neekoIdleL.size(); i++)
                {
                    temp.push_back(neekoIdleL[i]);
                }
            }
            else if (pointRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoPointL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pointLBool == true)
            {
                for (int i = 0; i < neekoPointL.size(); i++)
                {
                    temp.push_back(neekoPointL[i]);
                }
            }
            else if (crawlRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (crawlLBool == true)
            {
                for (int i = 0; i < neekoCrawlL.size(); i++)
                {
                    temp.push_back(neekoCrawlL[i]);
                }
            }
            else if (shrug1RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug1LBool == true)
            {
                for (int i = 0; i < neekoShrug1L.size(); i++)
                {
                    temp.push_back(neekoShrug1L[i]);
                }
            }
            else if (shrug2RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2LBool == true)
            {
                for (int i = 0; i < neekoShrug2L.size(); i++)
                {
                    temp.push_back(neekoShrug2L[i]);
                }
            }
            else if (shrug2mouthRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2mouthLBool == true)
            {
                for (int i = 0; i < neekoShrug2MouthL.size(); i++)
                {
                    temp.push_back(neekoShrug2MouthL[i]);
                }
            }
            else if (deadRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoDeadL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (deadLBool == true)
            {
                for (int i = 0; i < neekoDeadL.size(); i++)
                {
                    temp.push_back(neekoDeadL[i]);
                }
            }
            else if (pushRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoPushL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pushLBool == true)
            {
                for (int i = 0; i < neekoPushL.size(); i++)
                {
                    temp.push_back(neekoPushL[i]);
                }
            }
            else if (handsUpRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (handsUpLBool == true)
            {
                for (int i = 0; i < neekoHandsUpL.size(); i++)
                {
                    temp.push_back(neekoHandsUpL[i]);
                }
            }
            else if (hitRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(neekoHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(neekoHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(neekoHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(neekoHitL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (hitLBool == true)
            {
                for (int i = 0; i < neekoHitL.size(); i++)
                {
                    temp.push_back(neekoHitL[i]);
                }
            }
            else
            {
                for (int i = 0; i < neekoUpA.size(); i++)
                {
                    temp.push_back(255);
                }
            }
            break;
        default:
            if (downABool == true)
            {
                for (int i = 0; i < gaiaDownA.size(); i++)
                {
                    temp.push_back(gaiaDownA[i]);
                }
            }
            else if (downBBool == true)
            {
                for (int i = 0; i < gaiaDownB.size(); i++)
                {
                    temp.push_back(gaiaDownB[i]);
                }
            }
            else if (downCBool == true)
            {
                for (int i = 0; i < gaiaDownC.size(); i++)
                {
                    temp.push_back(gaiaDownC[i]);
                }
            }
            else if (upABool == true)
            {
                for (int i = 0; i < gaiaUpA.size(); i++)
                {
                    temp.push_back(gaiaUpA[i]);
                }
            }
            else if (upBBool == true)
            {
                for (int i = 0; i < gaiaUpB.size(); i++)
                {
                    temp.push_back(gaiaUpB[i]);
                }
            }
            else if (upCBool == true)
            {
                for (int i = 0; i < gaiaUpC.size(); i++)
                {
                    temp.push_back(gaiaUpC[i]);
                }
            }
            else if (leftABool == true)
            {
                for (int i = 0; i < gaiaLeftA.size(); i++)
                {
                    temp.push_back(gaiaLeftA[i]);
                }
            }
            else if (leftBBool == true)
            {
                for (int i = 0; i < gaiaLeftB.size(); i++)
                {
                    temp.push_back(gaiaLeftB[i]);
                }
            }
            else if (leftCBool == true)
            {
                for (int i = 0; i < gaiaLeftC.size(); i++)
                {
                    temp.push_back(gaiaLeftC[i]);
                }
            }
            else if (rightABool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaLeftA[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightBBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaLeftB[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (rightCBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaLeftC[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (backBool == true)
            {
                for (int i = 0; i < gaiaBack.size(); i++)
                {
                    temp.push_back(gaiaBack[i]);
                }
            }
            else if (frontBool == true)
            {
                for (int i = 0; i < gaiaFront.size(); i++)
                {
                    temp.push_back(gaiaFront[i]);
                }
            }
            else if (idleRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaIdleL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (idleLBool == true)
            {
                for (int i = 0; i < gaiaIdleL.size(); i++)
                {
                    temp.push_back(gaiaIdleL[i]);
                }
            }
            else if (pointRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaPointL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pointLBool == true)
            {
                for (int i = 0; i < gaiaPointL.size(); i++)
                {
                    temp.push_back(gaiaPointL[i]);
                }
            }
            else if (crawlRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaCrawlL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (crawlLBool == true)
            {
                for (int i = 0; i < gaiaCrawlL.size(); i++)
                {
                    temp.push_back(gaiaCrawlL[i]);
                }
            }
            else if (shrug1RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaShrug1L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug1LBool == true)
            {
                for (int i = 0; i < gaiaShrug1L.size(); i++)
                {
                    temp.push_back(gaiaShrug1L[i]);
                }
            }
            else if (shrug2RBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaShrug2L[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2LBool == true)
            {
                for (int i = 0; i < gaiaShrug2L.size(); i++)
                {
                    temp.push_back(gaiaShrug2L[i]);
                }
            }
            else if (shrug2mouthRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaShrug2MouthL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (shrug2mouthLBool == true)
            {
                for (int i = 0; i < gaiaShrug2MouthL.size(); i++)
                {
                    temp.push_back(gaiaShrug2MouthL[i]);
                }
            }
            else if (deadRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaDeadL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (deadLBool == true)
            {
                for (int i = 0; i < gaiaDeadL.size(); i++)
                {
                    temp.push_back(gaiaDeadL[i]);
                }
            }
            else if (pushRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaPushL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (pushLBool == true)
            {
                for (int i = 0; i < gaiaPushL.size(); i++)
                {
                    temp.push_back(gaiaPushL[i]);
                }
            }
            else if (handsUpRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaHandsUpL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (handsUpLBool == true)
            {
                for (int i = 0; i < gaiaHandsUpL.size(); i++)
                {
                    temp.push_back(gaiaHandsUpL[i]);
                }
            }
            else if (hitRBool == true)
            {
                for (int k = 0; k < 32; k++) // vertical count
                {
                    for (int j = 0; j < 4 * 32; j += 4)
                    {
                        temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 3]);
                        temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 2]);
                        temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j - 1]);
                        temp.push_back(gaiaHitL[(k * 32 * 4) + (4 * 32) - 1 - j]);
                    }
                }
            }
            else if (hitLBool == true)
            {
                for (int i = 0; i < gaiaHitL.size(); i++)
                {
                    temp.push_back(gaiaHitL[i]);
                }
            }
            else
            {
                for (int i = 0; i < gaiaUpA.size(); i++)
                {
                    temp.push_back(255);
                }
            }
            break;
        }
    }
    void textureUpdate(bool& textureBool)
    {
        clearBools();
        textureBool = true;
        spriteTexture.create(spriteW, spriteH);
        sf::Uint8* pixels = new sf::Uint8[spriteW * spriteH * 4];
        static bool noTexture{ false };
        uint8_t rw{ 255 };
        uint8_t gw{ 0 };
        uint8_t bw{ 0 };
        uint8_t aw{ 255 };

        pickArray();

        for (int i = 0; i < spriteW * spriteH * 4; i += 4)
        {
            //every iteration will call in order every pixel until done.
            rw = temp[0 + i];
            gw = temp[1 + i];
            bw = temp[2 + i];
            aw = temp[3 + i];

            switch (spriteColour)
            {
            case Default:
                pixels[i + 0] = rw;
                pixels[i + 1] = gw;
                pixels[i + 2] = bw;
                pixels[i + 3] = aw;
                break;
            case Inverted:
                if (rw <= 34 && gw <= 34 && bw <= 34)
                {
                    pixels[i + 0] = rw; // assigns the value at r to the pixels object
                    pixels[i + 1] = gw;
                    pixels[i + 2] = bw;
                }
                else
                {
                    pixels[i + 0] = 255 - rw; // assigns the value at r to the pixels object
                    pixels[i + 1] = 255 - gw;
                    pixels[i + 2] = 255 - bw;
                }
                pixels[i + 3] = aw;
                break;
            case Black:
                if (rw > 25 || gw > 25 || bw > 25)
                {
                    aw = 0;
                }
                pixels[i + 0] = rw; // assigns the value at r to the pixels object
                pixels[i + 1] = gw;
                pixels[i + 2] = bw;
                pixels[i + 3] = aw;
                break;
            case Transparent:
                if (aw > 250)
                {
                    aw = 127;
                }
                pixels[i + 0] = rw; // assigns the value at r to the pixels object
                pixels[i + 1] = gw;
                pixels[i + 2] = bw;
                pixels[i + 3] = aw;
                break;
            case Blue:
                if (rw + gw + bw > 255)
                {
                    bw = 255;
                }
                else
                {
                    bw = rw + gw + bw;
                    rw = 0;
                    gw = 0;
                }
                pixels[i + 0] = rw; // assigns the value at r to the pixels object
                pixels[i + 1] = gw;
                pixels[i + 2] = bw;
                pixels[i + 3] = aw;
                break;
            case Dark:
                pixels[i + 0] = rw / 2; // assigns the value at r to the pixels object
                pixels[i + 1] = gw / 2;
                pixels[i + 2] = bw / 2;
                pixels[i + 3] = aw;
                break;
            default:
                pixels[i + 0] = rw;
                pixels[i + 1] = gw;
                pixels[i + 2] = bw;
                pixels[i + 3] = aw;
                break;
            }


        }
        spriteTexture.update(pixels);
        sprite.setTexture(spriteTexture);
        delete[] pixels;
        temp.clear();
    }
    void swapOrder(Character &otherCharacter)
    {
        std::swap(order, otherCharacter.order);
    }
    unsigned short x{ 0 };
    unsigned short y{ 0 };
    uint8_t animCode{ 0 };
    sf::Clock movementClock;
    void loadPortrait()
    {
        portrait.setSize(sf::Vector2f(64, 64));
        portraitTexture.create(64, 64);
        sf::Uint8* pixels2 = new sf::Uint8[64 * 64 * 4];
        switch (id)
        {
        case 1:
            //Arson
            for (int i = 0; i < 64 * 64 * 4; i += 4)
            {
                pixels2[i + 0] = arsonPortrait[0 + i];
                pixels2[i + 1] = arsonPortrait[1 + i];
                pixels2[i + 2] = arsonPortrait[2 + i];
                pixels2[i + 3] = arsonPortrait[3 + i];
            }
            break;
        case 2:
            //Gaia
            for (int i = 0; i < 64 * 64 * 4; i += 4)
            {
                pixels2[i + 0] = gaiaPortrait[0 + i];
                pixels2[i + 1] = gaiaPortrait[1 + i];
                pixels2[i + 2] = gaiaPortrait[2 + i];
                pixels2[i + 3] = gaiaPortrait[3 + i];
            }
            break;
        case 3:
            //Cole
            for (int i = 0; i < 64 * 64 * 4; i += 4)
            {
                pixels2[i + 0] = colePortrait[0 + i];
                pixels2[i + 1] = colePortrait[1 + i];
                pixels2[i + 2] = colePortrait[2 + i];
                pixels2[i + 3] = colePortrait[3 + i];
            }
            break;
        case 4:
            //Neeko
            for (int i = 0; i < 64 * 64 * 4; i += 4)
            {
                pixels2[i + 0] = neekoPortrait[0 + i];
                pixels2[i + 1] = neekoPortrait[1 + i];
                pixels2[i + 2] = neekoPortrait[2 + i];
                pixels2[i + 3] = neekoPortrait[3 + i];
            }
            break;
        default:
            //Default
            for (int i = 0; i < 64 * 64 * 4; i += 4)
            {
                pixels2[i + 0] = defaultPortrait[0 + i];
                pixels2[i + 1] = defaultPortrait[1 + i];
                pixels2[i + 2] = defaultPortrait[2 + i];
                pixels2[i + 3] = defaultPortrait[3 + i];
            }
            break;
        }

        portraitTexture.update(pixels2);
        portrait.setTexture(&portraitTexture);
        delete[] pixels2;
        //portrait.setSize(sf::Vector2f(128, 128));
        portrait.setScale(sf::Vector2f(2, 2));
    }

    std::vector<short> coordVector;

    void move(short deltaX, short deltaY)
    {
        
        x += deltaX;
        y += deltaY;

        if (order == 1)
        {
            if (W.viewGlobal.getCenter().y - y > 32 * 3) // up
            {
                if (W.viewGlobal.getCenter().y > W.getScreenSize().y / 4 + W.extraPixelsY / 8)
                {
                    W.viewGlobal.move(0, deltaY);
                    W.window.setView(W.viewGlobal);
                }

            }
            else if (y - W.viewGlobal.getCenter().y > 32 * 3) // down
            {
                if (W.viewGlobal.getCenter().y < 448 * W.getTilemapCount().y - W.getScreenSize().y / 4 - W.extraPixelsY / 8)
                {
                    W.viewGlobal.move(0, deltaY);
                    W.window.setView(W.viewGlobal);
                }
            }
            if (W.viewGlobal.getCenter().x - x > 32 * 6 - 16) // left
            {
                if (W.viewGlobal.getCenter().x > W.getScreenSize().x / 4 + W.extraPixelsX / 8)
                {
                    W.viewGlobal.move(deltaX, 0);
                    W.window.setView(W.viewGlobal);
                }

            }
            else if (x - W.viewGlobal.getCenter().x > 32 * 6 + 16) // right
            {
                if (W.viewGlobal.getCenter().x < 768 * W.getTilemapCount().x - W.getScreenSize().x / 4 - W.extraPixelsX / 8)
                {
                    W.viewGlobal.move(deltaX, 0);
                    W.window.setView(W.viewGlobal);
                }
            }


        }
        coordVector.push_back(deltaX);
        coordVector.push_back(deltaY);

        sf::Time animateMovement = movementClock.getElapsedTime();
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y - 10));
        if (animateMovement.asMilliseconds() >= 120)
        {
            if (deltaY < 0)
            {
                switch (animCode)
                {
                case 0:
                    textureUpdate(upABool);
                    animCode += 1;
                    break;
                case 1:
                    textureUpdate(upBBool);
                    animCode += 1;
                    break;
                case 2:
                    textureUpdate(upABool);
                    animCode += 1;
                    break;
                case 3:
                    textureUpdate(upCBool);
                    animCode = 0;
                    break;
                default:
                    break;
                }
            }
            else if (deltaY > 0)
            {
                switch (animCode)
                {
                case 0:
                    textureUpdate(downABool);
                    animCode += 1;
                    break;
                case 1:
                    textureUpdate(downBBool);
                    animCode += 1;
                    break;
                case 2:
                    textureUpdate(downABool);
                    animCode += 1;
                    break;
                case 3:
                    textureUpdate(downCBool);
                    animCode = 0;
                    break;
                default:
                    break;
                }
            }
            else if (deltaX < 0)
            {
                switch (animCode)
                {
                case 0:
                    textureUpdate(leftABool);
                    animCode += 1;
                    break;
                case 1:
                    textureUpdate(leftBBool);
                    animCode += 1;
                    break;
                case 2:
                    textureUpdate(leftABool);
                    animCode += 1;
                    break;
                case 3:
                    textureUpdate(leftCBool);
                    animCode = 0;
                    break;
                default:
                    break;
                }
            }
            else if (deltaX > 0)
            {
                switch (animCode)
                {
                case 0:
                    textureUpdate(rightABool);
                    animCode += 1;
                    break;
                case 1:
                    textureUpdate(rightBBool);
                    animCode += 1;
                    break;
                case 2:
                    textureUpdate(rightABool);
                    animCode += 1;
                    break;
                case 3:
                    textureUpdate(rightCBool);
                    animCode = 0;
                    break;
                default:
                    break;
                }
            }
            movementClock.restart();
        }

    }

    void place(short deltaX, short deltaY)
    {
        if (deltaX % 32 != 0)
        {
            deltaX = (deltaX / 32) * 32;
        }
        if (deltaY % 32 != 0)
        {
            deltaY = (deltaY / 32) * 32;
        }
        if (deltaX >= 0 && deltaY >= 0 && deltaX <= W.getTilemapCount().x * 768 && deltaY <= W.getTilemapCount().y * 448)
        {
            x = deltaX;
            y = deltaY;
        }


        sprite.setPosition(static_cast<float>(x), static_cast<float>(y - 10));
        if (order == 1)
        {


            W.viewGlobal.setCenter(x + 16, y);

            if (W.viewGlobal.getCenter().y < W.getScreenSize().y / 4 + W.extraPixelsY / 8)
            {
                W.viewGlobal.move(0, (W.getScreenSize().y / 4 + W.extraPixelsY / 8) - W.viewGlobal.getCenter().y);
            }
            else if (W.viewGlobal.getCenter().y > 448 * W.getTilemapCount().y - 448)
            {
                W.viewGlobal.setCenter(W.viewGlobal.getCenter().x, 448 * W.getTilemapCount().y - W.getScreenSize().y / 4 - W.extraPixelsY / 8);
                //w.window.close();
            }
            if (W.viewGlobal.getCenter().x < W.getScreenSize().x / 4 + W.extraPixelsX / 8)
            {
                W.viewGlobal.move((W.getScreenSize().x / 4 + W.extraPixelsX / 8) - W.viewGlobal.getCenter().x, 0);
            }
            else if (W.viewGlobal.getCenter().x > 768 * W.getTilemapCount().x - 768)
            {
                W.viewGlobal.setCenter(768 * W.getTilemapCount().x - W.getScreenSize().x / 4 - W.extraPixelsX / 8, W.viewGlobal.getCenter().y);
            }

            W.window.setView(W.viewGlobal);
        }
    }

    void follow(Character &character)
    {

        if (character.order == order - 1)
        {
            
            if (character.coordVector.size() == W.sizeOfVector)
            {
                move(character.coordVector.front(), character.coordVector[1]);
                character.coordVector.erase(character.coordVector.begin());
                character.coordVector.erase(character.coordVector.begin());
            }
            else if (character.coordVector.size() > 1)
            {
                if (character.coordVector.front() < 0)
                {
                    textureUpdate(idleLBool);
                }
                else if (character.coordVector.front() > 0)
                {
                    textureUpdate(idleRBool);
                }
                else if (character.coordVector[1] < 0)
                {
                    textureUpdate(upABool);
                }
                else if (character.coordVector[1] > 0)
                {
                    textureUpdate(downABool);
                }
            }
        }
    }
};
Character Arson;
Character Gaia;
Character Cole;
Character Neeko;
Character Rylla;

class TileImages : public sf::Image
{
public:
    sf::Image tileImage;
    // TILEMAP DECLARATIONS
#pragma region
    TileMap map00;
    TileMap map01;
    TileMap map02;
    TileMap map03;
    TileMap map10;
    TileMap map11;
    TileMap map12;
    TileMap map13;
    TileMap map20;
    TileMap map21;
    TileMap map22;
    TileMap map23;
    TileMap map30;
    TileMap map31;
    TileMap map32;
    TileMap map33;

    TileMap map00a;
    TileMap map01a;
    TileMap map02a;
    TileMap map03a;
    TileMap map10a;
    TileMap map11a;
    TileMap map12a;
    TileMap map13a;
    TileMap map20a;
    TileMap map21a;
    TileMap map22a;
    TileMap map23a;
    TileMap map30a;
    TileMap map31a;
    TileMap map32a;
    TileMap map33a;

    TileMap map00b;
    TileMap map01b;
    TileMap map02b;
    TileMap map03b;
    TileMap map10b;
    TileMap map11b;
    TileMap map12b;
    TileMap map13b;
    TileMap map20b;
    TileMap map21b;
    TileMap map22b;
    TileMap map23b;
    TileMap map30b;
    TileMap map31b;
    TileMap map32b;
    TileMap map33b;

    TileMap map00c;
    TileMap map01c;
    TileMap map02c;
    TileMap map03c;
    TileMap map10c;
    TileMap map11c;
    TileMap map12c;
    TileMap map13c;
    TileMap map20c;
    TileMap map21c;
    TileMap map22c;
    TileMap map23c;
    TileMap map30c;
    TileMap map31c;
    TileMap map32c;
    TileMap map33c;

    TileMap map00d;
    TileMap map01d;
    TileMap map02d;
    TileMap map03d;
    TileMap map10d;
    TileMap map11d;
    TileMap map12d;
    TileMap map13d;
    TileMap map20d;
    TileMap map21d;
    TileMap map22d;
    TileMap map23d;
    TileMap map30d;
    TileMap map31d;
    TileMap map32d;
    TileMap map33d;
    // sprite
    TileMap map00e;
    TileMap map01e;
    TileMap map02e;
    TileMap map03e;
    TileMap map10e;
    TileMap map11e;
    TileMap map12e;
    TileMap map13e;
    TileMap map20e;
    TileMap map21e;
    TileMap map22e;
    TileMap map23e;
    TileMap map30e;
    TileMap map31e;
    TileMap map32e;
    TileMap map33e;

    TileMap map00f;
    TileMap map01f;
    TileMap map02f;
    TileMap map03f;
    TileMap map10f;
    TileMap map11f;
    TileMap map12f;
    TileMap map13f;
    TileMap map20f;
    TileMap map21f;
    TileMap map22f;
    TileMap map23f;
    TileMap map30f;
    TileMap map31f;
    TileMap map32f;
    TileMap map33f;

    TileMap map00g;
    TileMap map01g;
    TileMap map02g;
    TileMap map03g;
    TileMap map10g;
    TileMap map11g;
    TileMap map12g;
    TileMap map13g;
    TileMap map20g;
    TileMap map21g;
    TileMap map22g;
    TileMap map23g;
    TileMap map30g;
    TileMap map31g;
    TileMap map32g;
    TileMap map33g;

    TileMap map00h;
    TileMap map01h;
    TileMap map02h;
    TileMap map03h;
    TileMap map10h;
    TileMap map11h;
    TileMap map12h;
    TileMap map13h;
    TileMap map20h;
    TileMap map21h;
    TileMap map22h;
    TileMap map23h;
    TileMap map30h;
    TileMap map31h;
    TileMap map32h;
    TileMap map33h;

    TileMap map00i;
    TileMap map01i;
    TileMap map02i;
    TileMap map03i;
    TileMap map10i;
    TileMap map11i;
    TileMap map12i;
    TileMap map13i;
    TileMap map20i;
    TileMap map21i;
    TileMap map22i;
    TileMap map23i;
    TileMap map30i;
    TileMap map31i;
    TileMap map32i;
    TileMap map33i;
#pragma endregion

    int tile{ 0 };
    int directionX{ 0 };
    int directionY{ 0 };
    std::set<int> allowed
    {   
        0, 5, 13, 20, 41, 80, 100, 105, 132,
    };
    sf::RectangleShape singleTile;
    // returns false if movement allowed
    bool walls(int location, Character& character, int direction)
    {
        switch (direction)
        {
        case Up:
            directionX = 0;
            directionY = -1;
            break;
        case Down:
            directionX = 0;
            directionY = 1;
            break;
        case Left:
            directionX = -1;
            directionY = 0;
            break;
        case Right:
            directionX = 1;
            directionY = 0;
            break;
        default:
            directionX = 0;
            directionY = 0;
            break;
        }

        switch (location)
        {
        case WestKagar:
            switch (((character.x / 32) + directionX) / 24)
            {
            case 0:
                switch (((character.y / 32) + directionY) / 14) // checks y position
                {
                case 0:
                    tile = westKagar00[(character.x / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = westKagar01[(character.x / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = westKagar02[(character.x / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            case 1:
                switch (((character.y / 32) + directionY) / 14)
                {
                case 0:
                    tile = westKagar10[((character.x - 768) / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = westKagar11[((character.x - 768) / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = westKagar12[((character.x - 768) / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            case 2:
                switch (((character.y / 32) + directionY) / 14)
                {
                case 0:
                    tile = westKagar20[((character.x - 1536) / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = westKagar21[((character.x - 1536) / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = westKagar22[((character.x - 1536) / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        case KagarCaverns:
            switch (((character.x / 32) + directionX) / 24)
            {
            case 0:
                switch (((character.y / 32) + directionY) / 14) // checks y position
                {
                case 0:
                    tile = kagarCaverns00[(character.x / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = kagarCaverns01[(character.x / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = kagarCaverns02[(character.x / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                case 3:
                    tile = kagarCaverns03[(character.x / 32) + directionX + ((((character.y - 1344) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            case 1:
                switch (((character.y / 32) + directionY) / 14)
                {
                case 0:
                    tile = kagarCaverns10[((character.x - 768) / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = kagarCaverns11[((character.x - 768) / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = kagarCaverns12[((character.x - 768) / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                case 3:
                    tile = kagarCaverns13[((character.x - 768) / 32) + directionX + ((((character.y - 1344) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            case 2:
                switch (((character.y / 32) + directionY) / 14)
                {
                case 0:
                    tile = kagarCaverns20[((character.x - 1536) / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = kagarCaverns21[((character.x - 1536) / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = kagarCaverns22[((character.x - 1536) / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                case 3:
                    tile = kagarCaverns23[((character.x - 1536) / 32) + directionX + ((((character.y - 1344) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            case 3:
                switch (((character.y / 32) + directionY) / 14)
                {
                case 0:
                    tile = kagarCaverns30[((character.x - 2304) / 32) + directionX + (((character.y / 32) + directionY) * 24)];
                    break;
                case 1:
                    tile = kagarCaverns31[((character.x - 2304) / 32) + directionX + ((((character.y - 448) / 32) + directionY) * 24)];
                    break;
                case 2:
                    tile = kagarCaverns32[((character.x - 2304) / 32) + directionX + ((((character.y - 896) / 32) + directionY) * 24)];
                    break;
                case 3:
                    tile = kagarCaverns33[((character.x - 2304) / 32) + directionX + ((((character.y - 1344) / 32) + directionY) * 24)];
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        return (allowed.contains(tile)) ? false : true;

    }
    
    void loadSceneFromArray(int location)
    {
        switch (location)
        {
        case WestKagar:
            W.setTilemapCount(3, 3);
            map00.load(tileImage, sf::Vector2u(32, 32), westKagar00, 24, 14);
            map00.setPosition(0.f, 0.f);
            map00a.load(tileImage, sf::Vector2u(32, 32), westKagar00a, 24, 14);
            map00a.setPosition(0.f, 0.f);
            map00b.load(tileImage, sf::Vector2u(32, 32), westKagar00b, 24, 14);
            map00b.setPosition(0.f, 0.f);
            map00c.load(tileImage, sf::Vector2u(32, 32), westKagar00c, 24, 14);
            map00c.setPosition(0.f, 0.f);
            map00d.load(tileImage, sf::Vector2u(32, 32), westKagar00d, 24, 14);
            map00d.setPosition(0.f, 0.f);
            map00e.load(tileImage, sf::Vector2u(32, 32), westKagar00e, 24, 14);
            map00e.setPosition(0.f, 0.f);
            map00f.load(tileImage, sf::Vector2u(32, 32), westKagar00f, 24, 14);
            map00f.setPosition(0.f, 0.f);
            map00g.load(tileImage, sf::Vector2u(32, 32), westKagar00g, 24, 14);
            map00g.setPosition(0.f, 0.f);
            map00h.load(tileImage, sf::Vector2u(32, 32), westKagar00h, 24, 14);
            map00h.setPosition(0.f, 0.f);
            map00i.load(tileImage, sf::Vector2u(32, 32), westKagar00i, 24, 14);
            map00i.setPosition(0.f, 0.f);

            map01.load(tileImage, sf::Vector2u(32, 32), westKagar01, 24, 14);
            map01.setPosition(0.f, 448.f);
            map01a.load(tileImage, sf::Vector2u(32, 32), westKagar01a, 24, 14);
            map01a.setPosition(0.f, 448.f);
            map01b.load(tileImage, sf::Vector2u(32, 32), westKagar01b, 24, 14);
            map01b.setPosition(0.f, 448.f);
            map01c.load(tileImage, sf::Vector2u(32, 32), westKagar01c, 24, 14);
            map01c.setPosition(0.f, 448.f);
            map01d.load(tileImage, sf::Vector2u(32, 32), westKagar01d, 24, 14);
            map01d.setPosition(0.f, 448.f);
            map01e.load(tileImage, sf::Vector2u(32, 32), westKagar01e, 24, 14);
            map01e.setPosition(0.f, 448.f);
            map01f.load(tileImage, sf::Vector2u(32, 32), westKagar01f, 24, 14);
            map01f.setPosition(0.f, 448.f);
            map01g.load(tileImage, sf::Vector2u(32, 32), westKagar01g, 24, 14);
            map01g.setPosition(0.f, 448.f);
            map01h.load(tileImage, sf::Vector2u(32, 32), westKagar01h, 24, 14);
            map01h.setPosition(0.f, 448.f);
            map01i.load(tileImage, sf::Vector2u(32, 32), westKagar01i, 24, 14);
            map01i.setPosition(0.f, 448.f);

            map02.load(tileImage, sf::Vector2u(32, 32), westKagar02, 24, 14);
            map02.setPosition(0.f, 896.f);
            map02a.load(tileImage, sf::Vector2u(32, 32), westKagar02a, 24, 14);
            map02a.setPosition(0.f, 896.f);
            map02b.load(tileImage, sf::Vector2u(32, 32), westKagar02b, 24, 14);
            map02b.setPosition(0.f, 896.f);
            map02c.load(tileImage, sf::Vector2u(32, 32), westKagar02c, 24, 14);
            map02c.setPosition(0.f, 896.f);
            map02d.load(tileImage, sf::Vector2u(32, 32), westKagar02d, 24, 14);
            map02d.setPosition(0.f, 896.f);
            map02e.load(tileImage, sf::Vector2u(32, 32), westKagar02e, 24, 14);
            map02e.setPosition(0.f, 896.f);
            map02f.load(tileImage, sf::Vector2u(32, 32), westKagar02f, 24, 14);
            map02f.setPosition(0.f, 896.f);
            map02g.load(tileImage, sf::Vector2u(32, 32), westKagar02g, 24, 14);
            map02g.setPosition(0.f, 896.f);
            map02h.load(tileImage, sf::Vector2u(32, 32), westKagar02h, 24, 14);
            map02h.setPosition(0.f, 896.f);
            map02i.load(tileImage, sf::Vector2u(32, 32), westKagar02i, 24, 14);
            map02i.setPosition(0.f, 896.f);

            map10.load(tileImage, sf::Vector2u(32, 32), westKagar10, 24, 14);
            map10.setPosition(768.f, 0.f);
            map10a.load(tileImage, sf::Vector2u(32, 32), westKagar10a, 24, 14);
            map10a.setPosition(768.f, 0.f);
            map10b.load(tileImage, sf::Vector2u(32, 32), westKagar10b, 24, 14);
            map10b.setPosition(768.f, 0.f);
            map10c.load(tileImage, sf::Vector2u(32, 32), westKagar10c, 24, 14);
            map10c.setPosition(768.f, 0.f);
            map10d.load(tileImage, sf::Vector2u(32, 32), westKagar10d, 24, 14);
            map10d.setPosition(768.f, 0.f);
            map10e.load(tileImage, sf::Vector2u(32, 32), westKagar10e, 24, 14);
            map10e.setPosition(768.f, 0.f);
            map10f.load(tileImage, sf::Vector2u(32, 32), westKagar10f, 24, 14);
            map10f.setPosition(768.f, 0.f);
            map10g.load(tileImage, sf::Vector2u(32, 32), westKagar10g, 24, 14);
            map10g.setPosition(768.f, 0.f);
            map10h.load(tileImage, sf::Vector2u(32, 32), westKagar10h, 24, 14);
            map10h.setPosition(768.f, 0.f);
            map10i.load(tileImage, sf::Vector2u(32, 32), westKagar10i, 24, 14);
            map10i.setPosition(768.f, 0.f);

            map11.load(tileImage, sf::Vector2u(32, 32), westKagar11, 24, 14);
            map11.setPosition(768.f, 448.f);
            map11a.load(tileImage, sf::Vector2u(32, 32), westKagar11a, 24, 14);
            map11a.setPosition(768.f, 448.f);
            map11b.load(tileImage, sf::Vector2u(32, 32), westKagar11b, 24, 14);
            map11b.setPosition(768.f, 448.f);
            map11c.load(tileImage, sf::Vector2u(32, 32), westKagar11c, 24, 14);
            map11c.setPosition(768.f, 448.f);
            map11d.load(tileImage, sf::Vector2u(32, 32), westKagar11d, 24, 14);
            map11d.setPosition(768.f, 448.f);
            map11e.load(tileImage, sf::Vector2u(32, 32), westKagar11e, 24, 14);
            map11e.setPosition(768.f, 448.f);
            map11f.load(tileImage, sf::Vector2u(32, 32), westKagar11f, 24, 14);
            map11f.setPosition(768.f, 448.f);
            map11g.load(tileImage, sf::Vector2u(32, 32), westKagar11g, 24, 14);
            map11g.setPosition(768.f, 448.f);
            map11h.load(tileImage, sf::Vector2u(32, 32), westKagar11h, 24, 14);
            map11h.setPosition(768.f, 448.f);
            map11i.load(tileImage, sf::Vector2u(32, 32), westKagar11i, 24, 14);
            map11i.setPosition(768.f, 448.f);

            map12.load(tileImage, sf::Vector2u(32, 32), westKagar12, 24, 14);
            map12.setPosition(768.f, 896.f);
            map12a.load(tileImage, sf::Vector2u(32, 32), westKagar12a, 24, 14);
            map12a.setPosition(768.f, 896.f);
            map12b.load(tileImage, sf::Vector2u(32, 32), westKagar12b, 24, 14);
            map12b.setPosition(768.f, 896.f);
            map12c.load(tileImage, sf::Vector2u(32, 32), westKagar12c, 24, 14);
            map12c.setPosition(768.f, 896.f);
            map12d.load(tileImage, sf::Vector2u(32, 32), westKagar12d, 24, 14);
            map12d.setPosition(768.f, 896.f);
            map12e.load(tileImage, sf::Vector2u(32, 32), westKagar12e, 24, 14);
            map12e.setPosition(768.f, 896.f);
            map12f.load(tileImage, sf::Vector2u(32, 32), westKagar12f, 24, 14);
            map12f.setPosition(768.f, 896.f);
            map12g.load(tileImage, sf::Vector2u(32, 32), westKagar12g, 24, 14);
            map12g.setPosition(768.f, 896.f);
            map12h.load(tileImage, sf::Vector2u(32, 32), westKagar12h, 24, 14);
            map12h.setPosition(768.f, 896.f);
            map12i.load(tileImage, sf::Vector2u(32, 32), westKagar12i, 24, 14);
            map12i.setPosition(768.f, 896.f);

            map20.load(tileImage, sf::Vector2u(32, 32), westKagar20, 24, 14);
            map20.setPosition(1536.f, 0.f);
            map20a.load(tileImage, sf::Vector2u(32, 32), westKagar20a, 24, 14);
            map20a.setPosition(1536.f, 0.f);
            map20b.load(tileImage, sf::Vector2u(32, 32), westKagar20b, 24, 14);
            map20b.setPosition(1536.f, 0.f);
            map20c.load(tileImage, sf::Vector2u(32, 32), westKagar20c, 24, 14);
            map20c.setPosition(1536.f, 0.f);
            map20d.load(tileImage, sf::Vector2u(32, 32), westKagar20d, 24, 14);
            map20d.setPosition(1536.f, 0.f);
            map20e.load(tileImage, sf::Vector2u(32, 32), westKagar20e, 24, 14);
            map20e.setPosition(1536.f, 0.f);
            map20f.load(tileImage, sf::Vector2u(32, 32), westKagar20f, 24, 14);
            map20f.setPosition(1536.f, 0.f);
            map20g.load(tileImage, sf::Vector2u(32, 32), westKagar20g, 24, 14);
            map20g.setPosition(1536.f, 0.f);
            map20h.load(tileImage, sf::Vector2u(32, 32), westKagar20h, 24, 14);
            map20h.setPosition(1536.f, 0.f);
            map20i.load(tileImage, sf::Vector2u(32, 32), westKagar20i, 24, 14);
            map20i.setPosition(1536.f, 0.f);

            map21.load(tileImage, sf::Vector2u(32, 32), westKagar21, 24, 14);
            map21.setPosition(1536.f, 448.f);
            map21a.load(tileImage, sf::Vector2u(32, 32), westKagar21a, 24, 14);
            map21a.setPosition(1536.f, 448.f);
            map21b.load(tileImage, sf::Vector2u(32, 32), westKagar21b, 24, 14);
            map21b.setPosition(1536.f, 448.f);
            map21c.load(tileImage, sf::Vector2u(32, 32), westKagar21c, 24, 14);
            map21c.setPosition(1536.f, 448.f);
            map21d.load(tileImage, sf::Vector2u(32, 32), westKagar21d, 24, 14);
            map21d.setPosition(1536.f, 448.f);
            map21e.load(tileImage, sf::Vector2u(32, 32), westKagar21e, 24, 14);
            map21e.setPosition(1536.f, 448.f);
            map21f.load(tileImage, sf::Vector2u(32, 32), westKagar21f, 24, 14);
            map21f.setPosition(1536.f, 448.f);
            map21g.load(tileImage, sf::Vector2u(32, 32), westKagar21g, 24, 14);
            map21g.setPosition(1536.f, 448.f);
            map21h.load(tileImage, sf::Vector2u(32, 32), westKagar21h, 24, 14);
            map21h.setPosition(1536.f, 448.f);
            map21i.load(tileImage, sf::Vector2u(32, 32), westKagar21i, 24, 14);
            map21i.setPosition(1536.f, 448.f);

            map22.load(tileImage, sf::Vector2u(32, 32), westKagar22, 24, 14);
            map22.setPosition(1536.f, 896.f);
            map22a.load(tileImage, sf::Vector2u(32, 32), westKagar22a, 24, 14);
            map22a.setPosition(1536.f, 896.f);
            map22b.load(tileImage, sf::Vector2u(32, 32), westKagar22b, 24, 14);
            map22b.setPosition(1536.f, 896.f);
            map22c.load(tileImage, sf::Vector2u(32, 32), westKagar22c, 24, 14);
            map22c.setPosition(1536.f, 896.f);
            map22d.load(tileImage, sf::Vector2u(32, 32), westKagar22d, 24, 14);
            map22d.setPosition(1536.f, 896.f);
            map22e.load(tileImage, sf::Vector2u(32, 32), westKagar22e, 24, 14);
            map22e.setPosition(1536.f, 896.f);
            map22f.load(tileImage, sf::Vector2u(32, 32), westKagar22f, 24, 14);
            map22f.setPosition(1536.f, 896.f);
            map22g.load(tileImage, sf::Vector2u(32, 32), westKagar22g, 24, 14);
            map22g.setPosition(1536.f, 896.f);
            map22h.load(tileImage, sf::Vector2u(32, 32), westKagar22h, 24, 14);
            map22h.setPosition(1536.f, 896.f);
            map22i.load(tileImage, sf::Vector2u(32, 32), westKagar22i, 24, 14);
            map22i.setPosition(1536.f, 896.f);
            break;
        case KagarCaverns:
            W.setTilemapCount(4, 4);
            map00.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00, 24, 14);
            map00.setPosition(0.f, 0.f);
            map00a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00a, 24, 14);
            map00a.setPosition(0.f, 0.f);
            map00b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00b, 24, 14);
            map00b.setPosition(0.f, 0.f);
            map00c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00c, 24, 14);
            map00c.setPosition(0.f, 0.f);
            map00d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00d, 24, 14);
            map00d.setPosition(0.f, 0.f);
            map00e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00e, 24, 14);
            map00e.setPosition(0.f, 0.f);
            map00f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00f, 24, 14);
            map00f.setPosition(0.f, 0.f);
            map00g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00g, 24, 14);
            map00g.setPosition(0.f, 0.f);
            map00h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00h, 24, 14);
            map00h.setPosition(0.f, 0.f);
            map00i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00i, 24, 14);
            map00i.setPosition(0.f, 0.f);

            map01.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01, 24, 14);
            map01.setPosition(0.f, 448.f);
            map01a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01a, 24, 14);
            map01a.setPosition(0.f, 448.f);
            map01b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01b, 24, 14);
            map01b.setPosition(0.f, 448.f);
            map01c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01c, 24, 14);
            map01c.setPosition(0.f, 448.f);
            map01d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01d, 24, 14);
            map01d.setPosition(0.f, 448.f);
            map01e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01e, 24, 14);
            map01e.setPosition(0.f, 448.f);
            map01f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01f, 24, 14);
            map01f.setPosition(0.f, 448.f);
            map01g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01g, 24, 14);
            map01g.setPosition(0.f, 448.f);
            map01h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01h, 24, 14);
            map01h.setPosition(0.f, 448.f);
            map01i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01i, 24, 14);
            map01i.setPosition(0.f, 448.f);

            map02.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02, 24, 14);
            map02.setPosition(0.f, 896.f);
            map02a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02a, 24, 14);
            map02a.setPosition(0.f, 896.f);
            map02b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02b, 24, 14);
            map02b.setPosition(0.f, 896.f);
            map02c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02c, 24, 14);
            map02c.setPosition(0.f, 896.f);
            map02d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02d, 24, 14);
            map02d.setPosition(0.f, 896.f);
            map02e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02e, 24, 14);
            map02e.setPosition(0.f, 896.f);
            map02f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02f, 24, 14);
            map02f.setPosition(0.f, 896.f);
            map02g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02g, 24, 14);
            map02g.setPosition(0.f, 896.f);
            map02h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02h, 24, 14);
            map02h.setPosition(0.f, 896.f);
            map02i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02i, 24, 14);
            map02i.setPosition(0.f, 896.f);

            map03.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03, 24, 14);
            map03.setPosition(0.f, 1344.f);
            map03a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03a, 24, 14);
            map03a.setPosition(0.f, 1344.f);
            map03b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03b, 24, 14);
            map03b.setPosition(0.f, 1344.f);
            map03c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03c, 24, 14);
            map03c.setPosition(0.f, 1344.f);
            map03d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03d, 24, 14);
            map03d.setPosition(0.f, 1344.f);
            map03e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03e, 24, 14);
            map03e.setPosition(0.f, 1344.f);
            map03f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03f, 24, 14);
            map03f.setPosition(0.f, 1344.f);
            map03g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03g, 24, 14);
            map03g.setPosition(0.f, 1344.f);
            map03h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03h, 24, 14);
            map03h.setPosition(0.f, 1344.f);
            map03i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03i, 24, 14);
            map03i.setPosition(0.f, 1344.f);

            map10.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10, 24, 14);
            map10.setPosition(768.f, 0.f);
            map10a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10a, 24, 14);
            map10a.setPosition(768.f, 0.f);
            map10b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10b, 24, 14);
            map10b.setPosition(768.f, 0.f);
            map10c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10c, 24, 14);
            map10c.setPosition(768.f, 0.f);
            map10d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10d, 24, 14);
            map10d.setPosition(768.f, 0.f);
            map10e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10e, 24, 14);
            map10e.setPosition(768.f, 0.f);
            map10f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10f, 24, 14);
            map10f.setPosition(768.f, 0.f);
            map10g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10g, 24, 14);
            map10g.setPosition(768.f, 0.f);
            map10h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10h, 24, 14);
            map10h.setPosition(768.f, 0.f);
            map10i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10i, 24, 14);
            map10i.setPosition(768.f, 0.f);

            map11.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11, 24, 14);
            map11.setPosition(768.f, 448.f);
            map11a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11a, 24, 14);
            map11a.setPosition(768.f, 448.f);
            map11b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11b, 24, 14);
            map11b.setPosition(768.f, 448.f);
            map11c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11c, 24, 14);
            map11c.setPosition(768.f, 448.f);
            map11d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11d, 24, 14);
            map11d.setPosition(768.f, 448.f);
            map11e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11e, 24, 14);
            map11e.setPosition(768.f, 448.f);
            map11f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11f, 24, 14);
            map11f.setPosition(768.f, 448.f);
            map11g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11g, 24, 14);
            map11g.setPosition(768.f, 448.f);
            map11h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11h, 24, 14);
            map11h.setPosition(768.f, 448.f);
            map11i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11i, 24, 14);
            map11i.setPosition(768.f, 448.f);

            map12.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12, 24, 14);
            map12.setPosition(768.f, 896.f);
            map12a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12a, 24, 14);
            map12a.setPosition(768.f, 896.f);
            map12b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12b, 24, 14);
            map12b.setPosition(768.f, 896.f);
            map12c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12c, 24, 14);
            map12c.setPosition(768.f, 896.f);
            map12d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12d, 24, 14);
            map12d.setPosition(768.f, 896.f);
            map12e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12e, 24, 14);
            map12e.setPosition(768.f, 896.f);
            map12f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12f, 24, 14);
            map12f.setPosition(768.f, 896.f);
            map12g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12g, 24, 14);
            map12g.setPosition(768.f, 896.f);
            map12h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12h, 24, 14);
            map12h.setPosition(768.f, 896.f);
            map12i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12i, 24, 14);
            map12i.setPosition(768.f, 896.f);

            map13.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13, 24, 14);
            map13.setPosition(768.f, 1344.f);
            map13a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13a, 24, 14);
            map13a.setPosition(768.f, 1344.f);
            map13b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13b, 24, 14);
            map13b.setPosition(768.f, 1344.f);
            map13c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13c, 24, 14);
            map13c.setPosition(768.f, 1344.f);
            map13d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13d, 24, 14);
            map13d.setPosition(768.f, 1344.f);
            map13e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13e, 24, 14);
            map13e.setPosition(768.f, 1344.f);
            map13f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13f, 24, 14);
            map13f.setPosition(768.f, 1344.f);
            map13g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13g, 24, 14);
            map13g.setPosition(768.f, 896.f);
            map13h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13h, 24, 14);
            map13h.setPosition(768.f, 896.f);
            map13i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13i, 24, 14);
            map13i.setPosition(768.f, 896.f);
         
            map20.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20, 24, 14);
            map20.setPosition(1536.f, 0.f);
            map20a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20a, 24, 14);
            map20a.setPosition(1536.f, 0.f);
            map20b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20b, 24, 14);
            map20b.setPosition(1536.f, 0.f);
            map20c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20c, 24, 14);
            map20c.setPosition(1536.f, 0.f);
            map20d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20d, 24, 14);
            map20d.setPosition(1536.f, 0.f);
            map20e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20e, 24, 14);
            map20e.setPosition(1536.f, 0.f);
            map20f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20f, 24, 14);
            map20f.setPosition(1536.f, 0.f);
            map20g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20g, 24, 14);
            map20g.setPosition(1536.f, 0.f);
            map20h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20h, 24, 14);
            map20h.setPosition(1536.f, 0.f);
            map20i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20i, 24, 14);
            map20i.setPosition(1536.f, 0.f);

            map21.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21, 24, 14);
            map21.setPosition(1536.f, 448.f);
            map21a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21a, 24, 14);
            map21a.setPosition(1536.f, 448.f);
            map21b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21b, 24, 14);
            map21b.setPosition(1536.f, 448.f);
            map21c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21c, 24, 14);
            map21c.setPosition(1536.f, 448.f);
            map21d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21d, 24, 14);
            map21d.setPosition(1536.f, 448.f);
            map21e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21e, 24, 14);
            map21e.setPosition(1536.f, 448.f);
            map21f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21f, 24, 14);
            map21f.setPosition(1536.f, 448.f);
            map21g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21g, 24, 14);
            map21g.setPosition(1536.f, 448.f);
            map21h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21h, 24, 14);
            map21h.setPosition(1536.f, 448.f);
            map21i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21i, 24, 14);
            map21i.setPosition(1536.f, 448.f);

            map22.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22, 24, 14);
            map22.setPosition(1536.f, 896.f);
            map22a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22a, 24, 14);
            map22a.setPosition(1536.f, 896.f);
            map22b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22b, 24, 14);
            map22b.setPosition(1536.f, 896.f);
            map22c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22c, 24, 14);
            map22c.setPosition(1536.f, 896.f);
            map22d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22d, 24, 14);
            map22d.setPosition(1536.f, 896.f);
            map22e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22e, 24, 14);
            map22e.setPosition(1536.f, 896.f);
            map22f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22f, 24, 14);
            map22f.setPosition(1536.f, 896.f);
            map22g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22g, 24, 14);
            map22g.setPosition(1536.f, 896.f);
            map22h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22h, 24, 14);
            map22h.setPosition(1536.f, 896.f);
            map22i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22i, 24, 14);
            map22i.setPosition(1536.f, 896.f);

            map23.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23, 24, 14);
            map23.setPosition(1536.f, 1344.f);
            map23a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23a, 24, 14);
            map23a.setPosition(1536.f, 1344.f);
            map23b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23b, 24, 14);
            map23b.setPosition(1536.f, 1344.f);
            map23c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23c, 24, 14);
            map23c.setPosition(1536.f, 1344.f);
            map23d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23d, 24, 14);
            map23d.setPosition(1536.f, 1344.f);
            map23e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23e, 24, 14);
            map23e.setPosition(1536.f, 1344.f);
            map23f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23f, 24, 14);
            map23f.setPosition(1536.f, 1344.f);
            map23g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23g, 24, 14);
            map23g.setPosition(1536.f, 1344.f);
            map23h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23h, 24, 14);
            map23h.setPosition(1536.f, 1344.f);
            map23i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23i, 24, 14);
            map23i.setPosition(1536.f, 1344.f);

            map30.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30, 24, 14);
            map30.setPosition(2304.f, 0.f);
            map30a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30a, 24, 14);
            map30a.setPosition(2304.f, 0.f);
            map30b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30b, 24, 14);
            map30b.setPosition(2304.f, 0.f);
            map30c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30c, 24, 14);
            map30c.setPosition(2304.f, 0.f);
            map30d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30d, 24, 14);
            map30d.setPosition(2304.f, 0.f);
            map30e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30e, 24, 14);
            map30e.setPosition(2304.f, 0.f);
            map30f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30f, 24, 14);
            map30f.setPosition(2304.f, 0.f);
            map30g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30g, 24, 14);
            map30g.setPosition(2304.f, 0.f);
            map30h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30h, 24, 14);
            map30h.setPosition(2304.f, 0.f);
            map30i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30i, 24, 14);
            map30i.setPosition(2304.f, 0.f);

            map31.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31, 24, 14);
            map31.setPosition(2304.f, 448.f);
            map31a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31a, 24, 14);
            map31a.setPosition(2304.f, 448.f);
            map31b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31b, 24, 14);
            map31b.setPosition(2304.f, 448.f);
            map31c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31c, 24, 14);
            map31c.setPosition(2304.f, 448.f);
            map31d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31d, 24, 14);
            map31d.setPosition(2304.f, 448.f);
            map31e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31e, 24, 14);
            map31e.setPosition(2304.f, 448.f);
            map31f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31f, 24, 14);
            map31f.setPosition(2304.f, 448.f);
            map31g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31g, 24, 14);
            map31g.setPosition(2304.f, 448.f);
            map31h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31h, 24, 14);
            map31h.setPosition(2304.f, 448.f);
            map31i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31i, 24, 14);
            map31i.setPosition(2304.f, 448.f);

            map32.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32, 24, 14);
            map32.setPosition(2304.f, 896.f);
            map32a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32a, 24, 14);
            map32a.setPosition(2304.f, 896.f);
            map32b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32b, 24, 14);
            map32b.setPosition(2304.f, 896.f);
            map32c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32c, 24, 14);
            map32c.setPosition(2304.f, 896.f);
            map32d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32d, 24, 14);
            map32d.setPosition(2304.f, 896.f);
            map32e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32e, 24, 14);
            map32e.setPosition(2304.f, 896.f);
            map32f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32f, 24, 14);
            map32f.setPosition(2304.f, 896.f);
            map32g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32g, 24, 14);
            map32g.setPosition(2304.f, 896.f);
            map32h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32h, 24, 14);
            map32h.setPosition(2304.f, 896.f);
            map32i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32i, 24, 14);
            map32i.setPosition(2304.f, 896.f);

            map33.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33, 24, 14);
            map33.setPosition(2304.f, 1344.f);
            map33a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33a, 24, 14);
            map33a.setPosition(2304.f, 1344.f);
            map33b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33b, 24, 14);
            map33b.setPosition(2304.f, 1344.f);
            map33c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33c, 24, 14);
            map33c.setPosition(2304.f, 1344.f);
            map33d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33d, 24, 14);
            map33d.setPosition(2304.f, 1344.f);
            map33e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33e, 24, 14);
            map33e.setPosition(2304.f, 1344.f);
            map33f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33f, 24, 14);
            map33f.setPosition(2304.f, 1344.f);
            map33g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33g, 24, 14);
            map33g.setPosition(2304.f, 1344.f);
            map33h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33h, 24, 14);
            map33h.setPosition(2304.f, 1344.f);
            map33i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33i, 24, 14);
            map33i.setPosition(2304.f, 1344.f);
        default:
            break;
        }

    }
    
    void loadSceneFromTxt(int location)
    {
        std::ifstream loadArray;
        switch (location)
        {
        
        case WestKagar:
            // 00
#pragma region
            
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray00i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar00i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 01
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray01i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar01i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 02
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray02i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar02i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 10
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray10i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar10i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 11
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray11i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar11i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 12
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray12i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar12i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 20
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray20i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar20i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 21
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray21i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar21i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 22
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-WestKagar/saveArray22i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> westKagar22i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // Load Maps
#pragma region
            map00.load(tileImage, sf::Vector2u(32, 32), westKagar00, 24, 14);
            map00.setPosition(0.f, 0.f);
            map00a.load(tileImage, sf::Vector2u(32, 32), westKagar00a, 24, 14);
            map00a.setPosition(0.f, 0.f);
            map00b.load(tileImage, sf::Vector2u(32, 32), westKagar00b, 24, 14);
            map00b.setPosition(0.f, 0.f);
            map00c.load(tileImage, sf::Vector2u(32, 32), westKagar00c, 24, 14);
            map00c.setPosition(0.f, 0.f);
            map00d.load(tileImage, sf::Vector2u(32, 32), westKagar00d, 24, 14);
            map00d.setPosition(0.f, 0.f);
            map00e.load(tileImage, sf::Vector2u(32, 32), westKagar00e, 24, 14);
            map00e.setPosition(0.f, 0.f);
            map00f.load(tileImage, sf::Vector2u(32, 32), westKagar00f, 24, 14);
            map00f.setPosition(0.f, 0.f);
            map00g.load(tileImage, sf::Vector2u(32, 32), westKagar00g, 24, 14);
            map00g.setPosition(0.f, 0.f);
            map00h.load(tileImage, sf::Vector2u(32, 32), westKagar00h, 24, 14);
            map00h.setPosition(0.f, 0.f);
            map00i.load(tileImage, sf::Vector2u(32, 32), westKagar00i, 24, 14);
            map00i.setPosition(0.f, 0.f);

            map01.load(tileImage, sf::Vector2u(32, 32), westKagar01, 24, 14);
            map01.setPosition(0.f, 448.f);
            map01a.load(tileImage, sf::Vector2u(32, 32), westKagar01a, 24, 14);
            map01a.setPosition(0.f, 448.f);
            map01b.load(tileImage, sf::Vector2u(32, 32), westKagar01b, 24, 14);
            map01b.setPosition(0.f, 448.f);
            map01c.load(tileImage, sf::Vector2u(32, 32), westKagar01c, 24, 14);
            map01c.setPosition(0.f, 448.f);
            map01d.load(tileImage, sf::Vector2u(32, 32), westKagar01d, 24, 14);
            map01d.setPosition(0.f, 448.f);
            map01e.load(tileImage, sf::Vector2u(32, 32), westKagar01e, 24, 14);
            map01e.setPosition(0.f, 448.f);
            map01f.load(tileImage, sf::Vector2u(32, 32), westKagar01f, 24, 14);
            map01f.setPosition(0.f, 448.f);
            map01g.load(tileImage, sf::Vector2u(32, 32), westKagar01g, 24, 14);
            map01g.setPosition(0.f, 448.f);
            map01h.load(tileImage, sf::Vector2u(32, 32), westKagar01h, 24, 14);
            map01h.setPosition(0.f, 448.f);
            map01i.load(tileImage, sf::Vector2u(32, 32), westKagar01i, 24, 14);
            map01i.setPosition(0.f, 448.f);

            map02.load(tileImage, sf::Vector2u(32, 32), westKagar02, 24, 14);
            map02.setPosition(0.f, 896.f);
            map02a.load(tileImage, sf::Vector2u(32, 32), westKagar02a, 24, 14);
            map02a.setPosition(0.f, 896.f);
            map02b.load(tileImage, sf::Vector2u(32, 32), westKagar02b, 24, 14);
            map02b.setPosition(0.f, 896.f);
            map02c.load(tileImage, sf::Vector2u(32, 32), westKagar02c, 24, 14);
            map02c.setPosition(0.f, 896.f);
            map02d.load(tileImage, sf::Vector2u(32, 32), westKagar02d, 24, 14);
            map02d.setPosition(0.f, 896.f);
            map02e.load(tileImage, sf::Vector2u(32, 32), westKagar02e, 24, 14);
            map02e.setPosition(0.f, 896.f);
            map02f.load(tileImage, sf::Vector2u(32, 32), westKagar02f, 24, 14);
            map02f.setPosition(0.f, 896.f);
            map02g.load(tileImage, sf::Vector2u(32, 32), westKagar02g, 24, 14);
            map02g.setPosition(0.f, 896.f);
            map02h.load(tileImage, sf::Vector2u(32, 32), westKagar02h, 24, 14);
            map02h.setPosition(0.f, 896.f);
            map02i.load(tileImage, sf::Vector2u(32, 32), westKagar02i, 24, 14);
            map02i.setPosition(0.f, 896.f);

            map10.load(tileImage, sf::Vector2u(32, 32), westKagar10, 24, 14);
            map10.setPosition(768.f, 0.f);
            map10a.load(tileImage, sf::Vector2u(32, 32), westKagar10a, 24, 14);
            map10a.setPosition(768.f, 0.f);
            map10b.load(tileImage, sf::Vector2u(32, 32), westKagar10b, 24, 14);
            map10b.setPosition(768.f, 0.f);
            map10c.load(tileImage, sf::Vector2u(32, 32), westKagar10c, 24, 14);
            map10c.setPosition(768.f, 0.f);
            map10d.load(tileImage, sf::Vector2u(32, 32), westKagar10d, 24, 14);
            map10d.setPosition(768.f, 0.f);
            map10e.load(tileImage, sf::Vector2u(32, 32), westKagar10e, 24, 14);
            map10e.setPosition(768.f, 0.f);
            map10f.load(tileImage, sf::Vector2u(32, 32), westKagar10f, 24, 14);
            map10f.setPosition(768.f, 0.f);
            map10g.load(tileImage, sf::Vector2u(32, 32), westKagar10g, 24, 14);
            map10g.setPosition(768.f, 0.f);
            map10h.load(tileImage, sf::Vector2u(32, 32), westKagar10h, 24, 14);
            map10h.setPosition(768.f, 0.f);
            map10i.load(tileImage, sf::Vector2u(32, 32), westKagar10i, 24, 14);
            map10i.setPosition(768.f, 0.f);

            map11.load(tileImage, sf::Vector2u(32, 32), westKagar11, 24, 14);
            map11.setPosition(768.f, 448.f);
            map11a.load(tileImage, sf::Vector2u(32, 32), westKagar11a, 24, 14);
            map11a.setPosition(768.f, 448.f);
            map11b.load(tileImage, sf::Vector2u(32, 32), westKagar11b, 24, 14);
            map11b.setPosition(768.f, 448.f);
            map11c.load(tileImage, sf::Vector2u(32, 32), westKagar11c, 24, 14);
            map11c.setPosition(768.f, 448.f);
            map11d.load(tileImage, sf::Vector2u(32, 32), westKagar11d, 24, 14);
            map11d.setPosition(768.f, 448.f);
            map11e.load(tileImage, sf::Vector2u(32, 32), westKagar11e, 24, 14);
            map11e.setPosition(768.f, 448.f);
            map11f.load(tileImage, sf::Vector2u(32, 32), westKagar11f, 24, 14);
            map11f.setPosition(768.f, 448.f);
            map11g.load(tileImage, sf::Vector2u(32, 32), westKagar11g, 24, 14);
            map11g.setPosition(768.f, 448.f);
            map11h.load(tileImage, sf::Vector2u(32, 32), westKagar11h, 24, 14);
            map11h.setPosition(768.f, 448.f);
            map11i.load(tileImage, sf::Vector2u(32, 32), westKagar11i, 24, 14);
            map11i.setPosition(768.f, 448.f);

            map12.load(tileImage, sf::Vector2u(32, 32), westKagar12, 24, 14);
            map12.setPosition(768.f, 896.f);
            map12a.load(tileImage, sf::Vector2u(32, 32), westKagar12a, 24, 14);
            map12a.setPosition(768.f, 896.f);
            map12b.load(tileImage, sf::Vector2u(32, 32), westKagar12b, 24, 14);
            map12b.setPosition(768.f, 896.f);
            map12c.load(tileImage, sf::Vector2u(32, 32), westKagar12c, 24, 14);
            map12c.setPosition(768.f, 896.f);
            map12d.load(tileImage, sf::Vector2u(32, 32), westKagar12d, 24, 14);
            map12d.setPosition(768.f, 896.f);
            map12e.load(tileImage, sf::Vector2u(32, 32), westKagar12e, 24, 14);
            map12e.setPosition(768.f, 896.f);
            map12f.load(tileImage, sf::Vector2u(32, 32), westKagar12f, 24, 14);
            map12f.setPosition(768.f, 896.f);
            map12g.load(tileImage, sf::Vector2u(32, 32), westKagar12g, 24, 14);
            map12g.setPosition(768.f, 896.f);
            map12h.load(tileImage, sf::Vector2u(32, 32), westKagar12h, 24, 14);
            map12h.setPosition(768.f, 896.f);
            map12i.load(tileImage, sf::Vector2u(32, 32), westKagar12i, 24, 14);
            map12i.setPosition(768.f, 896.f);

            map20.load(tileImage, sf::Vector2u(32, 32), westKagar20, 24, 14);
            map20.setPosition(1536.f, 0.f);
            map20a.load(tileImage, sf::Vector2u(32, 32), westKagar20a, 24, 14);
            map20a.setPosition(1536.f, 0.f);
            map20b.load(tileImage, sf::Vector2u(32, 32), westKagar20b, 24, 14);
            map20b.setPosition(1536.f, 0.f);
            map20c.load(tileImage, sf::Vector2u(32, 32), westKagar20c, 24, 14);
            map20c.setPosition(1536.f, 0.f);
            map20d.load(tileImage, sf::Vector2u(32, 32), westKagar20d, 24, 14);
            map20d.setPosition(1536.f, 0.f);
            map20e.load(tileImage, sf::Vector2u(32, 32), westKagar20e, 24, 14);
            map20e.setPosition(1536.f, 0.f);
            map20f.load(tileImage, sf::Vector2u(32, 32), westKagar20f, 24, 14);
            map20f.setPosition(1536.f, 0.f);
            map20g.load(tileImage, sf::Vector2u(32, 32), westKagar20g, 24, 14);
            map20g.setPosition(1536.f, 0.f);
            map20h.load(tileImage, sf::Vector2u(32, 32), westKagar20h, 24, 14);
            map20h.setPosition(1536.f, 0.f);
            map20i.load(tileImage, sf::Vector2u(32, 32), westKagar20i, 24, 14);
            map20i.setPosition(1536.f, 0.f);

            map21.load(tileImage, sf::Vector2u(32, 32), westKagar21, 24, 14);
            map21.setPosition(1536.f, 448.f);
            map21a.load(tileImage, sf::Vector2u(32, 32), westKagar21a, 24, 14);
            map21a.setPosition(1536.f, 448.f);
            map21b.load(tileImage, sf::Vector2u(32, 32), westKagar21b, 24, 14);
            map21b.setPosition(1536.f, 448.f);
            map21c.load(tileImage, sf::Vector2u(32, 32), westKagar21c, 24, 14);
            map21c.setPosition(1536.f, 448.f);
            map21d.load(tileImage, sf::Vector2u(32, 32), westKagar21d, 24, 14);
            map21d.setPosition(1536.f, 448.f);
            map21e.load(tileImage, sf::Vector2u(32, 32), westKagar21e, 24, 14);
            map21e.setPosition(1536.f, 448.f);
            map21f.load(tileImage, sf::Vector2u(32, 32), westKagar21f, 24, 14);
            map21f.setPosition(1536.f, 448.f);
            map21g.load(tileImage, sf::Vector2u(32, 32), westKagar21g, 24, 14);
            map21g.setPosition(1536.f, 448.f);
            map21h.load(tileImage, sf::Vector2u(32, 32), westKagar21h, 24, 14);
            map21h.setPosition(1536.f, 448.f);
            map21i.load(tileImage, sf::Vector2u(32, 32), westKagar21i, 24, 14);
            map21i.setPosition(1536.f, 448.f);

            map22.load(tileImage, sf::Vector2u(32, 32), westKagar22, 24, 14);
            map22.setPosition(1536.f, 896.f);
            map22a.load(tileImage, sf::Vector2u(32, 32), westKagar22a, 24, 14);
            map22a.setPosition(1536.f, 896.f);
            map22b.load(tileImage, sf::Vector2u(32, 32), westKagar22b, 24, 14);
            map22b.setPosition(1536.f, 896.f);
            map22c.load(tileImage, sf::Vector2u(32, 32), westKagar22c, 24, 14);
            map22c.setPosition(1536.f, 896.f);
            map22d.load(tileImage, sf::Vector2u(32, 32), westKagar22d, 24, 14);
            map22d.setPosition(1536.f, 896.f);
            map22e.load(tileImage, sf::Vector2u(32, 32), westKagar22e, 24, 14);
            map22e.setPosition(1536.f, 896.f);
            map22f.load(tileImage, sf::Vector2u(32, 32), westKagar22f, 24, 14);
            map22f.setPosition(1536.f, 896.f);
            map22g.load(tileImage, sf::Vector2u(32, 32), westKagar22g, 24, 14);
            map22g.setPosition(1536.f, 896.f);
            map22h.load(tileImage, sf::Vector2u(32, 32), westKagar22h, 24, 14);
            map22h.setPosition(1536.f, 896.f);
            map22i.load(tileImage, sf::Vector2u(32, 32), westKagar22i, 24, 14);
            map22i.setPosition(1536.f, 896.f);
#pragma endregion
            break;
        case KagarCaverns:
            // 00
#pragma region
            //  
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray00i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns00i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 01
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray01i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns01i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 02
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray02i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns02i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 03
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray03i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns03i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 10
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray10i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns10i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 11
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray11i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns11i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 12
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray12i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns12i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 13
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray13i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns13i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 20
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray20i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns20i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 21
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray21i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns21i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 22
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray22i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns22i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 23
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray23i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns23i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 30
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray30i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns30i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 31
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray31i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns31i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 32
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray32i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns32i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // 33
#pragma region
              
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33a.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33a[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33b.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33b[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33c.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33c[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33d.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33d[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33e.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33e[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33f.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33f[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33g.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33g[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33h.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33h[i];
            }
            loadArray.close();
            loadArray.open("C:/TileMaster/Arrays/Save-KagarCaverns/saveArray33i.txt", std::ifstream::in);
            for (int i = 0; i < 336; i++)
            {
                if (loadArray.peek() == ',')
                {
                    loadArray.ignore();
                }

                loadArray >> kagarCaverns33i[i];
            }
            loadArray.close();
            // wait
#pragma endregion
            // Load Maps
#pragma region
            map00.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00, 24, 14);
            map00.setPosition(0.f, 0.f);
            map00a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00a, 24, 14);
            map00a.setPosition(0.f, 0.f);
            map00b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00b, 24, 14);
            map00b.setPosition(0.f, 0.f);
            map00c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00c, 24, 14);
            map00c.setPosition(0.f, 0.f);
            map00d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00d, 24, 14);
            map00d.setPosition(0.f, 0.f);
            map00e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00e, 24, 14);
            map00e.setPosition(0.f, 0.f);
            map00f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00f, 24, 14);
            map00f.setPosition(0.f, 0.f);
            map00g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00g, 24, 14);
            map00g.setPosition(0.f, 0.f);
            map00h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00h, 24, 14);
            map00h.setPosition(0.f, 0.f);
            map00i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns00i, 24, 14);
            map00i.setPosition(0.f, 0.f);

            map01.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01, 24, 14);
            map01.setPosition(0.f, 448.f);
            map01a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01a, 24, 14);
            map01a.setPosition(0.f, 448.f);
            map01b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01b, 24, 14);
            map01b.setPosition(0.f, 448.f);
            map01c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01c, 24, 14);
            map01c.setPosition(0.f, 448.f);
            map01d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01d, 24, 14);
            map01d.setPosition(0.f, 448.f);
            map01e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01e, 24, 14);
            map01e.setPosition(0.f, 448.f);
            map01f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01f, 24, 14);
            map01f.setPosition(0.f, 448.f);
            map01g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01g, 24, 14);
            map01g.setPosition(0.f, 448.f);
            map01h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01h, 24, 14);
            map01h.setPosition(0.f, 448.f);
            map01i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns01i, 24, 14);
            map01i.setPosition(0.f, 448.f);

            map02.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02, 24, 14);
            map02.setPosition(0.f, 896.f);
            map02a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02a, 24, 14);
            map02a.setPosition(0.f, 896.f);
            map02b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02b, 24, 14);
            map02b.setPosition(0.f, 896.f);
            map02c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02c, 24, 14);
            map02c.setPosition(0.f, 896.f);
            map02d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02d, 24, 14);
            map02d.setPosition(0.f, 896.f);
            map02e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02e, 24, 14);
            map02e.setPosition(0.f, 896.f);
            map02f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02f, 24, 14);
            map02f.setPosition(0.f, 896.f);
            map02g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02g, 24, 14);
            map02g.setPosition(0.f, 896.f);
            map02h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02h, 24, 14);
            map02h.setPosition(0.f, 896.f);
            map02i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns02i, 24, 14);
            map02i.setPosition(0.f, 896.f);

            map03.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03, 24, 14);
            map03.setPosition(0.f, 1344.f);
            map03a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03a, 24, 14);
            map03a.setPosition(0.f, 1344.f);
            map03b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03b, 24, 14);
            map03b.setPosition(0.f, 1344.f);
            map03c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03c, 24, 14);
            map03c.setPosition(0.f, 1344.f);
            map03d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03d, 24, 14);
            map03d.setPosition(0.f, 1344.f);
            map03e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03e, 24, 14);
            map03e.setPosition(0.f, 1344.f);
            map03f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03f, 24, 14);
            map03f.setPosition(0.f, 1344.f);
            map03g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03g, 24, 14);
            map03g.setPosition(0.f, 1344.f);
            map03h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03h, 24, 14);
            map03h.setPosition(0.f, 1344.f);
            map03i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns03i, 24, 14);
            map03i.setPosition(0.f, 1344.f);

            map10.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10, 24, 14);
            map10.setPosition(768.f, 0.f);
            map10a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10a, 24, 14);
            map10a.setPosition(768.f, 0.f);
            map10b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10b, 24, 14);
            map10b.setPosition(768.f, 0.f);
            map10c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10c, 24, 14);
            map10c.setPosition(768.f, 0.f);
            map10d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10d, 24, 14);
            map10d.setPosition(768.f, 0.f);
            map10e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10e, 24, 14);
            map10e.setPosition(768.f, 0.f);
            map10f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10f, 24, 14);
            map10f.setPosition(768.f, 0.f);
            map10g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10g, 24, 14);
            map10g.setPosition(768.f, 0.f);
            map10h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10h, 24, 14);
            map10h.setPosition(768.f, 0.f);
            map10i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns10i, 24, 14);
            map10i.setPosition(768.f, 0.f);

            map11.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11, 24, 14);
            map11.setPosition(768.f, 448.f);
            map11a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11a, 24, 14);
            map11a.setPosition(768.f, 448.f);
            map11b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11b, 24, 14);
            map11b.setPosition(768.f, 448.f);
            map11c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11c, 24, 14);
            map11c.setPosition(768.f, 448.f);
            map11d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11d, 24, 14);
            map11d.setPosition(768.f, 448.f);
            map11e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11e, 24, 14);
            map11e.setPosition(768.f, 448.f);
            map11f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11f, 24, 14);
            map11f.setPosition(768.f, 448.f);
            map11g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11g, 24, 14);
            map11g.setPosition(768.f, 448.f);
            map11h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11h, 24, 14);
            map11h.setPosition(768.f, 448.f);
            map11i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns11i, 24, 14);
            map11i.setPosition(768.f, 448.f);

            map12.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12, 24, 14);
            map12.setPosition(768.f, 896.f);
            map12a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12a, 24, 14);
            map12a.setPosition(768.f, 896.f);
            map12b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12b, 24, 14);
            map12b.setPosition(768.f, 896.f);
            map12c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12c, 24, 14);
            map12c.setPosition(768.f, 896.f);
            map12d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12d, 24, 14);
            map12d.setPosition(768.f, 896.f);
            map12e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12e, 24, 14);
            map12e.setPosition(768.f, 896.f);
            map12f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12f, 24, 14);
            map12f.setPosition(768.f, 896.f);
            map12g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12g, 24, 14);
            map12g.setPosition(768.f, 896.f);
            map12h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12h, 24, 14);
            map12h.setPosition(768.f, 896.f);
            map12i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns12i, 24, 14);
            map12i.setPosition(768.f, 896.f);

            map13.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13, 24, 14);
            map13.setPosition(768.f, 1344.f);
            map13a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13a, 24, 14);
            map13a.setPosition(768.f, 1344.f);
            map13b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13b, 24, 14);
            map13b.setPosition(768.f, 1344.f);
            map13c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13c, 24, 14);
            map13c.setPosition(768.f, 1344.f);
            map13d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13d, 24, 14);
            map13d.setPosition(768.f, 1344.f);
            map13e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13e, 24, 14);
            map13e.setPosition(768.f, 1344.f);
            map13f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13f, 24, 14);
            map13f.setPosition(768.f, 1344.f);
            map13g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13g, 24, 14);
            map13g.setPosition(768.f, 1344.f);
            map13h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13h, 24, 14);
            map13h.setPosition(768.f, 1344.f);
            map13i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns13i, 24, 14);
            map13i.setPosition(768.f, 1344.f);

            map20.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20, 24, 14);
            map20.setPosition(1536.f, 0.f);
            map20a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20a, 24, 14);
            map20a.setPosition(1536.f, 0.f);
            map20b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20b, 24, 14);
            map20b.setPosition(1536.f, 0.f);
            map20c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20c, 24, 14);
            map20c.setPosition(1536.f, 0.f);
            map20d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20d, 24, 14);
            map20d.setPosition(1536.f, 0.f);
            map20e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20e, 24, 14);
            map20e.setPosition(1536.f, 0.f);
            map20f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20f, 24, 14);
            map20f.setPosition(1536.f, 0.f);
            map20g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20g, 24, 14);
            map20g.setPosition(1536.f, 0.f);
            map20h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20h, 24, 14);
            map20h.setPosition(1536.f, 0.f);
            map20i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns20i, 24, 14);
            map20i.setPosition(1536.f, 0.f);

            map21.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21, 24, 14);
            map21.setPosition(1536.f, 448.f);
            map21a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21a, 24, 14);
            map21a.setPosition(1536.f, 448.f);
            map21b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21b, 24, 14);
            map21b.setPosition(1536.f, 448.f);
            map21c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21c, 24, 14);
            map21c.setPosition(1536.f, 448.f);
            map21d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21d, 24, 14);
            map21d.setPosition(1536.f, 448.f);
            map21e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21e, 24, 14);
            map21e.setPosition(1536.f, 448.f);
            map21f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21f, 24, 14);
            map21f.setPosition(1536.f, 448.f);
            map21g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21g, 24, 14);
            map21g.setPosition(1536.f, 448.f);
            map21h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21h, 24, 14);
            map21h.setPosition(1536.f, 448.f);
            map21i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns21i, 24, 14);
            map21i.setPosition(1536.f, 448.f);

            map22.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22, 24, 14);
            map22.setPosition(1536.f, 896.f);
            map22a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22a, 24, 14);
            map22a.setPosition(1536.f, 896.f);
            map22b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22b, 24, 14);
            map22b.setPosition(1536.f, 896.f);
            map22c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22c, 24, 14);
            map22c.setPosition(1536.f, 896.f);
            map22d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22d, 24, 14);
            map22d.setPosition(1536.f, 896.f);
            map22e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22e, 24, 14);
            map22e.setPosition(1536.f, 896.f);
            map22f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22f, 24, 14);
            map22f.setPosition(1536.f, 896.f);
            map22g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22g, 24, 14);
            map22g.setPosition(1536.f, 896.f);
            map22h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22h, 24, 14);
            map22h.setPosition(1536.f, 896.f);
            map22i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns22i, 24, 14);
            map22i.setPosition(1536.f, 896.f);

            map23.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23, 24, 14);
            map23.setPosition(1536.f, 1344.f);
            map23a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23a, 24, 14);
            map23a.setPosition(1536.f, 1344.f);
            map23b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23b, 24, 14);
            map23b.setPosition(1536.f, 1344.f);
            map23c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23c, 24, 14);
            map23c.setPosition(1536.f, 1344.f);
            map23d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23d, 24, 14);
            map23d.setPosition(1536.f, 1344.f);
            map23e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23e, 24, 14);
            map23e.setPosition(1536.f, 1344.f);
            map23f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23f, 24, 14);
            map23f.setPosition(1536.f, 1344.f);
            map23g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23g, 24, 14);
            map23g.setPosition(1536.f, 1344.f);
            map23h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23h, 24, 14);
            map23h.setPosition(1536.f, 1344.f);
            map23i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns23i, 24, 14);
            map23i.setPosition(1536.f, 1344.f);

            map30.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30, 24, 14);
            map30.setPosition(2304.f, 0.f);
            map30a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30a, 24, 14);
            map30a.setPosition(2304.f, 0.f);
            map30b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30b, 24, 14);
            map30b.setPosition(2304.f, 0.f);
            map30c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30c, 24, 14);
            map30c.setPosition(2304.f, 0.f);
            map30d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30d, 24, 14);
            map30d.setPosition(2304.f, 0.f);
            map30e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30e, 24, 14);
            map30e.setPosition(2304.f, 0.f);
            map30f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30f, 24, 14);
            map30f.setPosition(2304.f, 0.f);
            map30g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30g, 24, 14);
            map30g.setPosition(2304.f, 0.f);
            map30h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30h, 24, 14);
            map30h.setPosition(2304.f, 0.f);
            map30i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns30i, 24, 14);
            map30i.setPosition(2304.f, 0.f);

            map31.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31, 24, 14);
            map31.setPosition(2304.f, 448.f);
            map31a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31a, 24, 14);
            map31a.setPosition(2304.f, 448.f);
            map31b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31b, 24, 14);
            map31b.setPosition(2304.f, 448.f);
            map31c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31c, 24, 14);
            map31c.setPosition(2304.f, 448.f);
            map31d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31d, 24, 14);
            map31d.setPosition(2304.f, 448.f);
            map31e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31e, 24, 14);
            map31e.setPosition(2304.f, 448.f);
            map31f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31f, 24, 14);
            map31f.setPosition(2304.f, 448.f);
            map31g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31g, 24, 14);
            map31g.setPosition(2304.f, 448.f);
            map31h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31h, 24, 14);
            map31h.setPosition(2304.f, 448.f);
            map31i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns31i, 24, 14);
            map31i.setPosition(2304.f, 448.f);

            map32.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32, 24, 14);
            map32.setPosition(2304.f, 896.f);
            map32a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32a, 24, 14);
            map32a.setPosition(2304.f, 896.f);
            map32b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32b, 24, 14);
            map32b.setPosition(2304.f, 896.f);
            map32c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32c, 24, 14);
            map32c.setPosition(2304.f, 896.f);
            map32d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32d, 24, 14);
            map32d.setPosition(2304.f, 896.f);
            map32e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32e, 24, 14);
            map32e.setPosition(2304.f, 896.f);
            map32f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32f, 24, 14);
            map32f.setPosition(2304.f, 896.f);
            map32g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32g, 24, 14);
            map32g.setPosition(2304.f, 896.f);
            map32h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32h, 24, 14);
            map32h.setPosition(2304.f, 896.f);
            map32i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns32i, 24, 14);
            map32i.setPosition(2304.f, 896.f);

            map33.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33, 24, 14);
            map33.setPosition(2304.f, 1344.f);
            map33a.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33a, 24, 14);
            map33a.setPosition(2304.f, 1344.f);
            map33b.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33b, 24, 14);
            map33b.setPosition(2304.f, 1344.f);
            map33c.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33c, 24, 14);
            map33c.setPosition(2304.f, 1344.f);
            map33d.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33d, 24, 14);
            map33d.setPosition(2304.f, 1344.f);
            map33e.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33e, 24, 14);
            map33e.setPosition(2304.f, 1344.f);
            map33f.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33f, 24, 14);
            map33f.setPosition(2304.f, 1344.f);
            map33g.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33g, 24, 14);
            map33g.setPosition(2304.f, 1344.f);
            map33h.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33h, 24, 14);
            map33h.setPosition(2304.f, 1344.f);
            map33i.load(tileImage, sf::Vector2u(32, 32), kagarCaverns33i, 24, 14);
            map33i.setPosition(2304.f, 1344.f);
#pragma endregion
            break;
        default:
            break;
        }

    }
    void drawScene(int location)
    {
        switch (location)
        {
        case WestKagar:
            W.window.draw(map00);
            W.window.draw(map01);
            W.window.draw(map02);
            W.window.draw(map10);
            W.window.draw(map11);
            W.window.draw(map12);
            W.window.draw(map20);
            W.window.draw(map21);
            W.window.draw(map22);

            W.window.draw(map00a);
            W.window.draw(map01a);
            W.window.draw(map02a);
            W.window.draw(map10a);
            W.window.draw(map11a);
            W.window.draw(map12a);
            W.window.draw(map20a);
            W.window.draw(map21a);
            W.window.draw(map22a);

            W.window.draw(map00b);
            W.window.draw(map01b);
            W.window.draw(map02b);
            W.window.draw(map10b);
            W.window.draw(map11b);
            W.window.draw(map12b);
            W.window.draw(map20b);
            W.window.draw(map21b);
            W.window.draw(map22b);

            W.window.draw(map00c);
            W.window.draw(map01c);
            W.window.draw(map02c);
            W.window.draw(map10c);
            W.window.draw(map11c);
            W.window.draw(map12c);
            W.window.draw(map20c);
            W.window.draw(map21c);
            W.window.draw(map22c);

            W.window.draw(map00d);
            W.window.draw(map01d);
            W.window.draw(map02d);
            W.window.draw(map10d);
            W.window.draw(map11d);
            W.window.draw(map12d);
            W.window.draw(map20d);
            W.window.draw(map21d);
            W.window.draw(map22d);
            break;
        case KagarCaverns:
            W.window.draw(map00);
            W.window.draw(map01);
            W.window.draw(map02);
            W.window.draw(map03);
            W.window.draw(map10);
            W.window.draw(map11);
            W.window.draw(map12);
            W.window.draw(map13);
            W.window.draw(map20);
            W.window.draw(map21);
            W.window.draw(map22);
            W.window.draw(map23);
            W.window.draw(map30);
            W.window.draw(map31);
            W.window.draw(map32);
            W.window.draw(map33);

            W.window.draw(map00a);
            W.window.draw(map01a);
            W.window.draw(map02a);
            W.window.draw(map03a);
            W.window.draw(map10a);
            W.window.draw(map11a);
            W.window.draw(map12a);
            W.window.draw(map13a);
            W.window.draw(map20a);
            W.window.draw(map21a);
            W.window.draw(map22a);
            W.window.draw(map23a);
            W.window.draw(map30a);
            W.window.draw(map31a);
            W.window.draw(map32a);
            W.window.draw(map33a);

            W.window.draw(map00b);
            W.window.draw(map01b);
            W.window.draw(map02b);
            W.window.draw(map03b);
            W.window.draw(map10b);
            W.window.draw(map11b);
            W.window.draw(map12b);
            W.window.draw(map13b);
            W.window.draw(map20b);
            W.window.draw(map21b);
            W.window.draw(map22b);
            W.window.draw(map23b);
            W.window.draw(map30b);
            W.window.draw(map31b);
            W.window.draw(map32b);
            W.window.draw(map33b);

            W.window.draw(map00c);
            W.window.draw(map01c);
            W.window.draw(map02c);
            W.window.draw(map03c);
            W.window.draw(map10c);
            W.window.draw(map11c);
            W.window.draw(map12c);
            W.window.draw(map13c);
            W.window.draw(map20c);
            W.window.draw(map21c);
            W.window.draw(map22c);
            W.window.draw(map23c);
            W.window.draw(map30c);
            W.window.draw(map31c);
            W.window.draw(map32c);
            W.window.draw(map33c);

            W.window.draw(map00d);
            W.window.draw(map01d);
            W.window.draw(map02d);
            W.window.draw(map03d);
            W.window.draw(map10d);
            W.window.draw(map11d);
            W.window.draw(map12d);
            W.window.draw(map13d);
            W.window.draw(map20d);
            W.window.draw(map21d);
            W.window.draw(map22d);
            W.window.draw(map23d);
            W.window.draw(map30d);
            W.window.draw(map31d);
            W.window.draw(map32d);
            W.window.draw(map33d);
            break;
        default:
            break;
        }

    }
    void drawOverScene(int location)
    {
        switch (location)
        {
        case WestKagar:

            W.window.draw(map00e);
            W.window.draw(map01e);
            W.window.draw(map02e);
            W.window.draw(map10e);
            W.window.draw(map11e);
            W.window.draw(map12e);
            W.window.draw(map20e);
            W.window.draw(map21e);
            W.window.draw(map22e);

            W.window.draw(map00f);
            W.window.draw(map01f);
            W.window.draw(map02f);
            W.window.draw(map10f);
            W.window.draw(map11f);
            W.window.draw(map12f);
            W.window.draw(map20f);
            W.window.draw(map21f);
            W.window.draw(map22f);

            W.window.draw(map00g);
            W.window.draw(map01g);
            W.window.draw(map02g);
            W.window.draw(map10g);
            W.window.draw(map11g);
            W.window.draw(map12g);
            W.window.draw(map20g);
            W.window.draw(map21g);
            W.window.draw(map22g);

            W.window.draw(map00h);
            W.window.draw(map01h);
            W.window.draw(map02h);
            W.window.draw(map10h);
            W.window.draw(map11h);
            W.window.draw(map12h);
            W.window.draw(map20h);
            W.window.draw(map21h);
            W.window.draw(map22h);
        
            W.window.draw(map00i);
            W.window.draw(map01i);
            W.window.draw(map02i);
            W.window.draw(map10i);
            W.window.draw(map11i);
            W.window.draw(map12i);
            W.window.draw(map20i);
            W.window.draw(map21i);
            W.window.draw(map22i);
            break;
        case KagarCaverns:
            W.window.draw(map00e);
            W.window.draw(map01e);
            W.window.draw(map02e);
            W.window.draw(map03e);
            W.window.draw(map10e);
            W.window.draw(map11e);
            W.window.draw(map12e);
            W.window.draw(map13e);
            W.window.draw(map20e);
            W.window.draw(map21e);
            W.window.draw(map22e);
            W.window.draw(map23e);
            W.window.draw(map30e);
            W.window.draw(map31e);
            W.window.draw(map32e);
            W.window.draw(map33e);

            W.window.draw(map00f);
            W.window.draw(map01f);
            W.window.draw(map02f);
            W.window.draw(map03f);
            W.window.draw(map10f);
            W.window.draw(map11f);
            W.window.draw(map12f);
            W.window.draw(map13f);
            W.window.draw(map20f);
            W.window.draw(map21f);
            W.window.draw(map22f);
            W.window.draw(map23f);
            W.window.draw(map30f);
            W.window.draw(map31f);
            W.window.draw(map32f);
            W.window.draw(map33f);

            W.window.draw(map00g);
            W.window.draw(map01g);
            W.window.draw(map02g);
            W.window.draw(map03g);
            W.window.draw(map10g);
            W.window.draw(map11g);
            W.window.draw(map12g);
            W.window.draw(map13g);
            W.window.draw(map20g);
            W.window.draw(map21g);
            W.window.draw(map22g);
            W.window.draw(map23g);
            W.window.draw(map30g);
            W.window.draw(map31g);
            W.window.draw(map32g);
            W.window.draw(map33g);

            W.window.draw(map00h);
            W.window.draw(map01h);
            W.window.draw(map02h);
            W.window.draw(map03h);
            W.window.draw(map10h);
            W.window.draw(map11h);
            W.window.draw(map12h);
            W.window.draw(map13h);
            W.window.draw(map20h);
            W.window.draw(map21h);
            W.window.draw(map22h);
            W.window.draw(map23h);
            W.window.draw(map30h);
            W.window.draw(map31h);
            W.window.draw(map32h);
            W.window.draw(map33h);

            W.window.draw(map00i);
            W.window.draw(map01i);
            W.window.draw(map02i);
            W.window.draw(map03i);
            W.window.draw(map10i);
            W.window.draw(map11i);
            W.window.draw(map12i);
            W.window.draw(map13i);
            W.window.draw(map20i);
            W.window.draw(map21i);
            W.window.draw(map22i);
            W.window.draw(map23i);
            W.window.draw(map30i);
            W.window.draw(map31i);
            W.window.draw(map32i);
            W.window.draw(map33i);
            break;
        default:
            break;
        }
    }

    void drawSingleTile(int tile, short x, short y)
    {
        //todo
    }
};
TileImages TileImage;

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
    
    ParticleSystem(unsigned int count) :
        m_particles(count),
        m_vertices(sf::Quads, count * 4),
        m_lifetime(sf::seconds(3.f)),
        m_emitter(0.f, 0.f)
    {
    }

    void setEmitter(sf::Vector2f position)
    {
        m_emitter = position;
    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if (p.lifetime <= sf::Time::Zero)
            {
                resetParticle(i);
            }
            // update the position and color of the corresponding vertex

            m_vertices[i * 4].position += p.velocity * elapsed.asSeconds();
            m_vertices[i * 4 + 1].position = m_vertices[i * 4].position + sf::Vector2f(0, 5);
            m_vertices[i * 4 + 2].position = m_vertices[i * 4].position + sf::Vector2f(5, 5);
            m_vertices[i * 4 + 3].position = m_vertices[i * 4].position + sf::Vector2f(5, 0);
            //m_vertices[i].color = sf::Color(0, 255, 255, 255);
            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i * 4].color.a = static_cast<sf::Uint8>(ratio * 255);
            m_vertices[i * 4 + 1].color.a = static_cast<sf::Uint8>(ratio * 255);
            m_vertices[i * 4 + 2].color.a = static_cast<sf::Uint8>(ratio * 255);
            m_vertices[i * 4 + 3].color.a = static_cast<sf::Uint8>(ratio * 255);
        }
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;
        // draw the vertex array
        target.draw(m_vertices, states);
    }

private:

    struct Particle
    {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    void resetParticle(std::size_t index)
    {
        // give a random velocity and lifetime to the particle
        float angle = (std::rand() % 360) * 3.14f / 180.f;
        float speed = 50.f;
        sf::Time life{ sf::milliseconds((std::rand() % 2000) + 1000) };
        sf::Vector2f groupVel(sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed));
        m_particles[index].velocity = groupVel;
        m_particles[index].lifetime = life;

        // reset the position of the corresponding vertex
        m_vertices[index * 4].position = m_emitter;
        m_vertices[index * 4 + 1].position = m_emitter + sf::Vector2f(0, 5);
        m_vertices[index * 4 + 2].position = m_emitter + sf::Vector2f(5, 5);
        m_vertices[index * 4 + 3].position = m_emitter + sf::Vector2f(5, 0);
    }

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime;
    sf::Vector2f m_emitter;
};

class Animations
{
public:

private:

};
Animations Animation;

class Controls
{
public:
    

    bool movementAllowed{ true };
    bool lastKeyUp{ false };
    bool lastKeyDown{ false };
    bool lastKeyLeft{ false };
    bool lastKeyRight{ false };
    short movementSpeed{ 4 };
    int location{ 0 };

    Controls()
    {
        location = WestKagar;
    }

    void refreshBools()
    {
        lastKeyUp = false;
        lastKeyDown = false;
        lastKeyLeft = false;
        lastKeyRight = false;
    }

    bool checkBounds(Character& character, int direction)
    {
        switch (location)
        {
        case WestKagar:
            switch (direction)
            {
            case Up:
                if (!TileImage.walls(location, character, Up))
                {
                    return
                        (character.y <= 0) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Down:
                if (!TileImage.walls(location, character, Down))
                {
                    return
                        (character.y >= W.getTilemapCount().y * 448 - 32) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Left:
                if (!TileImage.walls(location, character, Left))
                {
                    return
                        (character.x <= 0) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Right:
                if (!TileImage.walls(location, character, Right))
                {
                    return
                        (character.x > W.getTilemapCount().x * 768 - 64) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            default:
                return false;
                break;
            }
            break;
        case KagarCaverns:
            switch (direction)
            {
            case Up:
                if (!TileImage.walls(location, character, Up))
                {
                    return
                        (character.y <= 0) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Down:
                if (!TileImage.walls(location, character, Down))
                {
                    return
                        (character.y >= W.getTilemapCount().y * 448 - 32) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Left:
                if (!TileImage.walls(location, character, Left))
                {
                    return
                        (character.x <= 0) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            case Right:
                if (!TileImage.walls(location, character, Right))
                {
                    return
                        (character.x > W.getTilemapCount().x * 768 - 64) ? false :
                        true;
                }
                else
                {
                    return false;
                }
                break;
            default:
                return false;
                break;
            }
            break;
        default:
            break;
        }
        
    }

    void pollMovement(Character& currentCharacter) 
    {

        if (currentCharacter.x % 32 == 0 && currentCharacter.y % 32 == 0 && currentCharacter.order == 1)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                if (checkBounds(currentCharacter, Up))
                {
                    currentCharacter.move(0, -movementSpeed);
                }
                else
                {
                    currentCharacter.textureUpdate(currentCharacter.upABool);
                }
                if (!lastKeyUp)
                {
                    refreshBools();
                    lastKeyUp = true;
                }

            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                if (checkBounds(currentCharacter, Down))
                {
                    currentCharacter.move(0, movementSpeed);
                }
                else
                {
                    currentCharacter.textureUpdate(currentCharacter.downABool);
                }
                if (!lastKeyDown)
                {
                    refreshBools();
                    lastKeyDown = true;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                if (checkBounds(currentCharacter, Left))
                {
                    currentCharacter.move(-movementSpeed, 0);
                }
                else
                {
                    currentCharacter.textureUpdate(currentCharacter.idleLBool);
                }
                if (!lastKeyLeft)
                {
                    refreshBools();
                    lastKeyLeft = true;
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                if (checkBounds(currentCharacter, Right))
                {
                    currentCharacter.move(movementSpeed, 0);
                }
                else
                {
                    currentCharacter.textureUpdate(currentCharacter.idleRBool);
                }
                if (!lastKeyRight)
                {
                    refreshBools();
                    lastKeyRight = true;
                }
            }
            else
            {
                if (lastKeyUp)
                {
                    currentCharacter.textureUpdate(currentCharacter.upABool);

                }
                else if (lastKeyDown)
                {
                    currentCharacter.textureUpdate(currentCharacter.downABool);
                }
                else if (lastKeyRight)
                {
                    currentCharacter.textureUpdate(currentCharacter.idleRBool);
                }
                else if (lastKeyLeft)
                {
                    currentCharacter.textureUpdate(currentCharacter.idleLBool);
                }
                currentCharacter.animCode = 0;
            }
        }
        else
        {
            if (currentCharacter.x % 32 != 0)
            {
                if (lastKeyRight)
                {
                    currentCharacter.move(movementSpeed, 0);
                }
                else if (lastKeyLeft)
                {
                    currentCharacter.move(-movementSpeed, 0);
                }
            }
            else if (currentCharacter.y % 32 != 0)
            {
                if (lastKeyUp)
                {
                    currentCharacter.move(0, -movementSpeed);
                }
                else if (lastKeyDown)
                {
                    currentCharacter.move(0, movementSpeed);
                }
            }
        }



    }

    void setSpeed()
    {

        if (W.FPS < 100)
        {
            movementSpeed = 4;
            W.sizeOfVector = 16;
        }
        else if (W.FPS < 150)
        {
            movementSpeed = 2;
            W.sizeOfVector = 32;
        }
        else
        {
            movementSpeed = 1;
            W.sizeOfVector = 64;
        }
        //movementSpeed = W.FPS / 30;
    }

    Character& getOrder(int order)
    {

        if (Arson.order == order)
        {
            return Arson;
        }
        else if (Gaia.order == order)
        {
            return Gaia;
        }
        else if (Cole.order == order)
        {
            return Cole;
        }
        else if (Neeko.order == order)
        {
            return Neeko;
        }
        else if (Rylla.order == order)
        {
            return Rylla;
        }

    }
};
Controls Controller;

class Menus
{
public:
    int playerBoxWidth{ 0 };
    int playerBoxHeight{ 0 };
    sf::ConvexShape corner;
    sf::RectangleShape border;
    //sf::VertexBuffer fill;
    uint8_t red1{ 10 };
    uint8_t red2{ 22 };
    uint8_t red3{ 17 };
    uint8_t red4{ 20 };
    uint8_t green1{ 20 };
    uint8_t green2{ 30 };
    uint8_t green3{ 20 };
    uint8_t green4{ 30 };
    uint8_t blue1{ 0 };
    uint8_t blue2{ 0 };
    uint8_t blue3{ 0 };
    uint8_t blue4{ 0 };
    uint8_t alpha1{ 155 };
    uint8_t alpha2{ 235 };
    uint8_t alpha3{ 255 };
    uint8_t alpha4{ 235 };
    int offsetFactor{ 2 };
    float const magicRatio87{ 0.9f };
    bool menuActive{ false };

    bool mainMenu{ false };
    bool itemMenu{ false };
    bool equipMenu{ false };
    bool magicMenu{ false };
    bool statsMenu{ false };
    bool configMenu{ false };
    bool saveMenu{ false };
    void updateVertices(int x, int y, int width, int height)
    {
        x += 6;
        y += 6;
        width -= 12;
        height -= 12;
        sf::VertexArray background(sf::Quads, 4);

        background[0].position = sf::Vector2f(x, y);
        background[1].position = sf::Vector2f(x + width, y);
        background[2].position = sf::Vector2f(x + width, y + height);
        background[3].position = sf::Vector2f(x, y + height);

        background[0].color = sf::Color(red1, green1, blue1, alpha1);
        background[1].color = sf::Color(red2, green2, blue2, alpha2);
        background[2].color = sf::Color(red3, green3, blue3, alpha3);
        background[3].color = sf::Color(red4, green4, blue4, alpha4);

        W.window.draw(background);
    }
    // Set the texture width to negative values to switch the image (flip horizontol or vert)
    // Calls default constructor
    Menus()
    {
        offsetFactor = 2 / W.zoomFloat;
        border.setSize(sf::Vector2f(5, 5));
        corner.setPointCount(20);
        corner.setPoint(0, sf::Vector2f(0, 0));
        corner.setPoint(1, sf::Vector2f(1, 0));
        corner.setPoint(2, sf::Vector2f(1, 1));
        corner.setPoint(3, sf::Vector2f(4, 1));
        corner.setPoint(4, sf::Vector2f(4, 2));
        corner.setPoint(5, sf::Vector2f(6, 2));
        corner.setPoint(6, sf::Vector2f(12, 8));
        corner.setPoint(7, sf::Vector2f(12, 10));
        corner.setPoint(8, sf::Vector2f(13, 10));
        corner.setPoint(9, sf::Vector2f(13, 13));
        corner.setPoint(10, sf::Vector2f(14, 13));
        corner.setPoint(11, sf::Vector2f(14, 14));
        corner.setPoint(12, sf::Vector2f(9, 14));
        corner.setPoint(13, sf::Vector2f(9, 12));
        corner.setPoint(14, sf::Vector2f(8, 12));
        corner.setPoint(15, sf::Vector2f(8, 11));
        corner.setPoint(16, sf::Vector2f(3, 6));
        corner.setPoint(17, sf::Vector2f(2, 6));
        corner.setPoint(18, sf::Vector2f(2, 5));
        corner.setPoint(19, sf::Vector2f(0, 5));

        updateVertices(0, 0, 0, 0);
        playerBoxWidth = static_cast<int>((W.getScreenSize().x / (2 / W.zoomFloat)) / 3 * 3.5);
        playerBoxHeight = static_cast<int>(((W.getScreenSize().y / (2 / W.zoomFloat)) / 2) - 8);
    }
    void drawFullBox(bool colourFill)
    {
        float zeroX{ W.zeroCoords().x };
        float zeroY{ W.zeroCoords().y };

        if (colourFill)
        {
            updateVertices(zeroX + 2, zeroY + 2, W.getScreenSize().x * W.zoomFloat - 4, W.getScreenSize().y * W.zoomFloat - 4);
        }
        //w.window.draw(fill);

        border.setFillColor(sf::Color::Black);
        border.setSize(sf::Vector2f(W.getScreenSize().x * W.zoomFloat - 30 - 4, 5));
        border.setPosition(zeroX + 15 + 3, zeroY + 2 + 1);
        W.window.draw(border);
        border.setPosition(zeroX + 15 + 3, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 2 - 5 + 1);
        W.window.draw(border);
        border.setSize(sf::Vector2f(5, W.getScreenSize().y * W.zoomFloat - 30 - 4));
        border.setPosition(zeroX + 2 + 1, zeroY + 17 + 1);
        W.window.draw(border);
        border.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 7 + 1, zeroY + 17 + 1);
        W.window.draw(border);

        corner.setFillColor(sf::Color::Black);
        corner.setRotation(270.f);
        corner.setPosition(zeroX + 4, zeroY + 18);
        W.window.draw(corner);
        corner.setRotation(0.f);
        corner.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 16, zeroY + 4);
        W.window.draw(corner);
        corner.setRotation(90.f);
        corner.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 2, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 16);
        W.window.draw(corner);
        corner.setRotation(180.f);
        corner.setPosition(zeroX + 18, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 2);
        W.window.draw(corner);

        border.setFillColor(sf::Color::White);
        border.setSize(sf::Vector2f(W.getScreenSize().x * W.zoomFloat - 30 - 4, 5));
        border.setPosition(zeroX + 15 + 1, zeroY + 2 - 1);
        W.window.draw(border);
        border.setPosition(zeroX + 15 + 1, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 2 - 5 - 1);
        W.window.draw(border);
        border.setSize(sf::Vector2f(5, W.getScreenSize().y * W.zoomFloat - 30 - 4));
        border.setPosition(zeroX + 2 - 1, zeroY + 17 - 1);
        W.window.draw(border);
        border.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 7 - 1, zeroY + 17 - 1);
        W.window.draw(border);

        corner.setFillColor(sf::Color::White);
        corner.setRotation(270.f);
        corner.setPosition(zeroX + 2, zeroY + 16);
        W.window.draw(corner);
        corner.setRotation(0.f);
        corner.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 18, zeroY + 2);
        W.window.draw(corner);
        corner.setRotation(90.f);
        corner.setPosition(W.viewGlobal.getCenter().x + W.getScreenSize().x / (2 / W.zoomFloat) - 4, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 18);
        W.window.draw(corner);
        corner.setRotation(180.f);
        corner.setPosition(zeroX + 16, W.viewGlobal.getCenter().y + W.getScreenSize().y / (2 / W.zoomFloat) - 4);
        W.window.draw(corner);
    }
    void drawBox(int x, int y, int width, int height, bool colourFill)
    {
        float zeroX{ W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) + 11 };
        float zeroY{ W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) + 11 };


        if (width < 32)
        {
            width = 32;
        }

        if (height < 32)
        {
            height = 32;
        }

        if (colourFill)
        {
            updateVertices(zeroX, zeroY, width, height);
        }


        x += zeroX + 1;
        y += zeroY + 1;
        border.setFillColor(sf::Color::Black);
        border.setSize(sf::Vector2f(width - 30, 5));
        border.setPosition(x + 15, y);
        W.window.draw(border);
        border.setPosition(x + 15, y + height - 5);
        W.window.draw(border);
        border.setSize(sf::Vector2f(5, height - 30));
        border.setPosition(x, y + 15);
        W.window.draw(border);
        border.setPosition(x + width - 5, y + 15);
        W.window.draw(border);

        corner.setFillColor(sf::Color::Black);
        corner.setRotation(270.f);
        corner.setPosition(sf::Vector2f(x + 1, y + 15));
        W.window.draw(corner);
        corner.setRotation(0.f);
        corner.setPosition(sf::Vector2f(x + width - 15, y + 1));
        W.window.draw(corner);
        corner.setRotation(90.f);
        corner.setPosition(sf::Vector2f(x + width - 1, y + height - 15));
        W.window.draw(corner);
        corner.setRotation(180.f);
        corner.setPosition(sf::Vector2f(x + 15, y + height - 1));
        W.window.draw(corner);

        x -= 2;
        y -= 2;

        border.setFillColor(sf::Color::White);
        border.setSize(sf::Vector2f(width - 30, 5));
        border.setPosition(x + 15, y);
        W.window.draw(border);
        border.setPosition(x + 15, y + height - 5);
        W.window.draw(border);
        border.setSize(sf::Vector2f(5, height - 30));
        border.setPosition(x, y + 15);
        W.window.draw(border);
        border.setPosition(x + width - 5, y + 15);
        W.window.draw(border);

        corner.setFillColor(sf::Color::White);
        corner.setRotation(270.f);
        corner.setPosition(sf::Vector2f(x + 1, y + 15));
        W.window.draw(corner);
        corner.setRotation(0.f);
        corner.setPosition(sf::Vector2f(x + width - 15, y + 1));
        W.window.draw(corner);
        corner.setRotation(90.f);
        corner.setPosition(sf::Vector2f(x + width - 1, y + height - 15));
        W.window.draw(corner);
        corner.setRotation(180.f);
        corner.setPosition(sf::Vector2f(x + 15, y + height - 1));
        W.window.draw(corner);
    }
    void drawBanner(bool colourFill)
    {
        float zeroX{ W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) };
        float zeroY{ W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) };
        drawBox(0, 0, W.getScreenSize().x * W.zoomFloat - 22, 64, colourFill);
    }
    void drawMainMenu()
    {
        Controller.getOrder(1).portrait.setPosition(sf::Vector2f(W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) + 17, W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) + 17));
        Controller.getOrder(2).portrait.setPosition(sf::Vector2f(W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) + 17, W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) + 17 + (playerBoxHeight + 3) * 1));
        Controller.getOrder(3).portrait.setPosition(sf::Vector2f(W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) + 17, W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) + 18 + (playerBoxHeight + 3) * 2));
        Controller.getOrder(4).portrait.setPosition(sf::Vector2f(W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat) + 17, W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat) + 18 + (playerBoxHeight + 3) * 3));
        
        if (playerBoxHeight < Controller.getOrder(1).portrait.getSize().x * 2)
        {
            Controller.getOrder(1).portrait.setSize(sf::Vector2f((playerBoxHeight - 11) / 2, (playerBoxHeight - 11) / 2));
            Controller.getOrder(2).portrait.setSize(sf::Vector2f((playerBoxHeight - 11) / 2, (playerBoxHeight - 11) / 2));
            Controller.getOrder(3).portrait.setSize(sf::Vector2f((playerBoxHeight - 11) / 2, (playerBoxHeight - 11) / 2));
            Controller.getOrder(4).portrait.setSize(sf::Vector2f((playerBoxHeight - 11) / 2, (playerBoxHeight - 11) / 2));
        }
        W.window.draw(Controller.getOrder(1).portrait);
        W.window.draw(Controller.getOrder(2).portrait);
        W.window.draw(Controller.getOrder(3).portrait);
        W.window.draw(Controller.getOrder(4).portrait);
        drawBox(0, 0, playerBoxHeight, playerBoxHeight, false);
        drawBox(0, (playerBoxHeight + 3) * 1, playerBoxHeight, playerBoxHeight, false);
        drawBox(0, (playerBoxHeight + 3) * 2 + 1, playerBoxHeight, playerBoxHeight, false);
        drawBox(0, (playerBoxHeight + 3) * 3 + 1, playerBoxHeight, playerBoxHeight, false);

        drawBox(playerBoxHeight + 3, 0, playerBoxWidth, playerBoxHeight, false);
        drawBox(playerBoxHeight + 3, (playerBoxHeight + 3) * 1, playerBoxWidth, playerBoxHeight, false);
        drawBox(playerBoxHeight + 3, (playerBoxHeight + 3) * 2 + 1, playerBoxWidth, playerBoxHeight, false);
        drawBox(playerBoxHeight + 3, (playerBoxHeight + 3) * 3 + 1, playerBoxWidth, playerBoxHeight, false);

        drawBox(playerBoxHeight + playerBoxWidth + 6, 0, (W.getScreenSize().x / 2) - (playerBoxHeight + playerBoxWidth + 6) - 22, W.getScreenSize().y / 2 - 22, false);
    }

    void changeMenu(bool& currentMenu, bool& nextMenu)
    {
        if (!currentMenu)
        {
            activateMenu();
        }
        else
        {
            currentMenu = false;
        }
        if (nextMenu)
        {
            //EXIT: nextMenu must be menuActive
            activateMenu();
        }
        else
        {
            nextMenu = true;
        }
    }
    void clearAllMenus()
    {
        menuActive = false;
        mainMenu = false;
        itemMenu = false;
        equipMenu = false;
        magicMenu = false;
        statsMenu = false;
        configMenu = false;
        saveMenu = false;
        Controller.movementAllowed = true;
    }
    void activateMenu()
    {
        if (!menuActive)
        {
            mainMenu = true;
            menuActive = true;
        }
        else
        {
            clearAllMenus();
        }
    }
    bool activateMenuBool{ false };
    bool deactivateMenuBool{ false };
    void pollMenu()
    {
        if (menuActive)
        {
            drawFullBox(true);
            if (mainMenu)
            {
                drawMainMenu();
            }

        }

        if (activateMenuBool)
        {
            if (!menuActive)
            {
                Controller.movementAllowed = false;
                if (!W.pollFadeToBlack(true))
                {
                    activateMenu();
                }
            }
            else
            {
                if (!W.pollFadeToBlack(false))
                {
                    activateMenuBool = false;
                }
            }
        }
        else if (deactivateMenuBool)
        {
            if (!menuActive)
            {
                if (!W.pollFadeToBlack(false))
                {
                    deactivateMenuBool = false;
                }
            }
            else
            {
                if (!W.pollFadeToBlack(true))
                {
                    activateMenu();
                }
            }
        }

        if (Text.bannerBool)
        {
            drawBanner(true);
        }
    }
};
Menus Menu;

class Devtool : public Controls, public Window
{
public:
    int counter{ 0 };
    sf::Texture printScreenTexture;
    void printScreen()
    {
        printScreenTexture.create(W.getScreenSize().x, W.getScreenSize().y);
        printScreenTexture.update(W.window);
        printScreenTexture.copyToImage().saveToFile("C:/Users/Windows/Documents/ScreenshotCannon/screen" + std::to_string(counter) + ".png");
        ++counter;
    }
    void showCoords()
    {
        std::string a = std::to_string((getOrder(1).x / 32) / 24) + ", " + std::to_string((getOrder(1).y / 32) / 14);
        std::string b = std::to_string(getOrder(1).x / 32) + ", " + std::to_string(getOrder(1).y / 32);
        Text.display(W.viewGlobal.getCenter().x, W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat), a);
        Text.display(W.viewGlobal.getCenter().x, 32 + W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat), b);
    }
    sf::Clock loopClock;
    void showLoopClock()
    {
        W.window.setVerticalSyncEnabled(false);
        sf::Time loopTime = loopClock.restart();
        Text.display(W.viewGlobal.getCenter().x - W.getScreenSize().x / (2 / W.zoomFloat), W.viewGlobal.getCenter().y - W.getScreenSize().y / (2 / W.zoomFloat), std::to_string(loopTime.asMilliseconds()));
    }
    void showTileAhead()
    {
        Text.display(getOrder(1).x, getOrder(1).y, std::to_string(TileImage.tile));
    }
};
Devtool DEVTOOLS;