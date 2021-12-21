#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <chrono>
#include <thread>
#include "Sprite.h"
#include "Levels.h"
#include "Class.h"

// GLOBALS
// This is temporary, and will eventually call the containing folder.



void wait(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
void init()
{
    W.init();
    Controller.setSpeed();
    //TileImage.tileImage.loadFromFile(filepath + "TILE.bmp");
    TileImage.tileImage.loadFromFile("C:/TileMaster/TILE.bmp");
    TileImage.tileImage.createMaskFromColor(sf::Color(237, 28, 36, 255), 0);
    TileImage.tileImage.createMaskFromColor(sf::Color(13, 103, 148, 255), 150);
    TileImage.tileImage.createMaskFromColor(sf::Color(26, 98, 138, 255), 200);
    Arson.id = 1;
    Gaia.id = 2;
    Cole.id = 3;
    Neeko.id = 4;
    Arson.order = 1;
    Gaia.order = 4;
    Cole.order = 3;
    Neeko.order = 2;
    
    Text.init();

}
void drawSprites()
{
    std::set<unsigned short> spriteSet{ Controller.getOrder(1).y, Controller.getOrder(2).y, Controller.getOrder(3).y, Controller.getOrder(4).y };
    for (std::set<unsigned short>::iterator it = spriteSet.begin(); it != spriteSet.end(); it++)
    {
        for (int i = 1; i < 5; i++)
        {
            if (*it == Controller.getOrder(i).y)
            {
                W.window.draw(Controller.getOrder(i).sprite);
            }
        }

    }
}
void loadScene(int location)
{
    //fade to black
    //fade out music

    Controller.location = location;
    switch (location)
    {
    case WestKagar:
        W.setTilemapCount(3, 3);
        Text.textBanner("West Kagar");
        break;
    case WestKagarInn:
        W.setTilemapCount(1, 1);
        break;
    case KagarCaverns:
        W.setTilemapCount(4, 4);
        Text.textBanner("KAGAR CAVERNS");
        break;
    default:
        break;
    }
    //TileImage.loadSceneFromArray(location);
    TileImage.loadSceneFromTxt(location);

    //start fade in
    //start music
}

int main()
{
    init();
    loadScene(WestKagar);
    ///////////////////////////////////////////////////////////////////

    Arson.place(284, 320);
    Gaia.place(284, 320);
    Cole.place(284, 320);
    Neeko.place(284, 320);
    Arson.textureUpdate(Arson.idleLBool);

    Controller.getOrder(1).loadPortrait();
    Controller.getOrder(2).loadPortrait();
    Controller.getOrder(3).loadPortrait();
    Controller.getOrder(4).loadPortrait();

    //ParticleSystem particles(1000);
    sf::Clock clock;
    ///////////////////////////////////////////////////////////////////
    while (W.window.isOpen())
    {
        W.window.clear();
        sf::Event event;
        while (W.window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::KeyReleased:
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    if (Menu.menuActive)
                    {
                        Menu.deactivateMenuBool = true;
                    }
                    else { W.window.close(); }
                    break;
                case sf::Keyboard::Insert:
                    std::cout << '\n';
                    TileImage.allowed.insert(TileImage.tile);
                    for (int i : TileImage.allowed)
                    {
                        std::cout << i << ", ";
                    }
                    break;
                case sf::Keyboard::Space:
                    if (W.fadeToBlackAlpha < 50)
                    {
                        if (!Menu.menuActive)
                        {
                            Menu.activateMenuBool = true;
                        }
                        else
                        {
                            Menu.deactivateMenuBool = true;
                        }
                    }
                    break;
                default:
                    break;
                }
                break;
            default:
                break;

            }
        }
        if (Controller.movementAllowed)
        {
            Controller.pollMovement(Controller.getOrder(1));
            Controller.getOrder(2).follow(Controller.getOrder(1));
            Controller.getOrder(3).follow(Controller.getOrder(2));
            Controller.getOrder(4).follow(Controller.getOrder(3));
        }

        TileImage.drawScene(Controller.location);
        drawSprites();
        TileImage.drawOverScene(Controller.location); 

        Menu.pollMenu();
        Text.pollText();
  
        ////// TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        {
            W.showScanLines();
        }
        //Menu.drawBanner(true);
        ///////////////////////////////////////////////////////
        //DEVTOOLS.showCoords();
        //DEVTOOLS.showLoopClock();
        //DEVTOOLS.showTileAhead();
        //DEVTOOLS.printScreen();
        ///////////////////////////////////////////////////////

        //sf::Vector2i mouse = sf::Mouse::getPosition(W.window);
        //particles.setEmitter(W.window.mapPixelToCoords(mouse));
        //sf::Time elapsed = clock.restart();
        //particles.update(elapsed);
        //W.window.draw(particles);
        
        W.window.display();
    }
    return 0;
}