#include <SFML/Graphics.hpp>
#include "gamePlay.h"
#include "textures.h"
#include "sounds.h"
#include "gui.h"
#include "network.h"
#include "packetcompress.h"
#include <iostream> // just here for quick and simple error testing, remove if you want
#include <TGUI/TGUI.hpp>
#include <sstream>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

using namespace std;

#define CLIENT_VERSION 1

bool loadError(sf::String error) {
    if (error == "OK")
        return false;
    sf::RenderWindow window;
    window.create(sf::VideoMode(500, 400), "SpeedBlocks");
    tgui::Gui gui(window);
    tgui::Label::Ptr errorMsg = tgui::Label::create();
    errorMsg->setText("Failed to load resources " + error + "\nSee that the file is there or reinstall the game.\n\nPress any key to quit.");
    errorMsg->setTextColor(sf::Color::White);
    errorMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    errorMsg->setTextSize(35);
    errorMsg->setSize(500,400);
    gui.add(errorMsg);
    gui.draw();
    window.display();

    sf::Event event;
    while (1) {
        window.waitEvent(event);
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::Closed)
            break;
    }
    return true;
}

int main()
{
    // Initializing classes and loading resources

    sf::Font typewriter, printFont;
    if (!typewriter.loadFromFile(resourcePath() + "media/Kingthings Trypewriter 2.ttf")) {
        loadError("media/Kingthings Trypewriter 2.ttf");
        return false;
    }
    if (!printFont.loadFromFile(resourcePath() + "media/F25_Bank_Printer.ttf")) {
        loadError("media/F25_Bank_Printer.ttf");
        return false;
    }

    textures textureBase;
    if (loadError(textureBase.loadTextures()))
        return false;

    network net;

    soundBank sounds;
    if (loadError(sounds.loadSounds()))
        return false;

    gamePlay game(&textureBase, &sounds, printFont, &typewriter);

    game.field.setName(game.options.name);

    sounds.setEffectVolume(game.options.EffectVolume);
    sounds.setMusicVolume(game.options.MusicVolume);
    sounds.setChatVolume(game.options.ChatVolume);

    sf::RenderWindow window;
    if (game.options.fullscreen)
        window.create(game.options.modes[game.options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
    if (!window.isOpen()) {
        window.create(sf::VideoMode(960, 600), "SpeedBlocks");
        //window.create(sf::VideoMode(544, 340), "SpeedBlocks");
        game.options.fullscreen=false;
        game.options.currentmode=-1;
    }
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (game.options.vSync)
        window.setVerticalSyncEnabled(true);

    UI gui(window, typewriter, printFont, game.options, sounds, game, net, textureBase);
    gui.clientVersion = CLIENT_VERSION;

    gui.gui.setView(view);

    sf::Clock frameClock;
    sf::Time current=sf::seconds(0), nextDraw=sf::seconds(0);

    sf::Time lastFrame=sf::seconds(0), longestFrame=sf::seconds(0), secCount=sf::seconds(0);
    int frameCount=0, frameRate=0;

    game.rander.seedPiece(time(NULL)); // Make sure the seed is random-ish in case the client never connects

    // The main-loop

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
            gui.handleEvent(event);

        if (gui.playonline)
            while (net.receiveData())
                gui.handlePacket();
        
        switch (gui.gamestate) {
            case CountDown:
                if (!gui.playonline)
                    if (game.countDown()) {
                        game.startGame();
                        gui.gamestate = Game;
                    }

                if (game.gameOver())
                    gui.setGameState(GameOver);
            break;

            case Game:
                game.delayCheck();

                if (gui.playonline)
                    gui.sendGameData();

                if (game.gameOver())
                    gui.setGameState(GameOver);
            break;

            case GameOver:
                if (gui.playonline)
                    if (game.winner)
                        gui.sendGameWinner();
            break;
        }

        // Drawing to the screen

        current = frameClock.getElapsedTime();
        if (current > nextDraw || game.options.vSync) {
            if (game.drawMe && gui.gamestate == Game) {
                game.draw();
                game.drawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            window.draw(textureBase.background);
            if (gui.gamestate == CountDown || gui.gamestate == Game || gui.gamestate == GameOver) {
                window.draw( game.field.sprite );
                gui.drawFields();
            }
            if (gui.adjPieces)
                for (int i=0; i<7; i++)
                    window.draw(gui.piece[i]);
            gui.gui.draw();
            window.display();
            frameRate++;
        }
        else if (!game.options.vSync)
            sf::sleep(game.options.inputDelay);
        if (nextDraw < current)
            nextDraw=current;

        // Performance output

        current = frameClock.getElapsedTime();
        if (current-lastFrame > longestFrame)
            longestFrame = current-lastFrame;
        frameCount++;

        if (current-secCount > sf::seconds(1)) {
            cout << "Framerate: " << frameRate << " Framecount: " << frameCount << " longest: " << longestFrame.asMilliseconds() << endl;
            frameRate=0;
            frameCount=0;
            longestFrame=sf::seconds(0);
            secCount=current;
        }
        lastFrame=current;
    }

    // Things to do before the game turns off

    game.options.saveOptions();

    return 0;
}