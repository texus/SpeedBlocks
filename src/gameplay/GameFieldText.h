#ifndef GAMEFIELDTEXT_H
#define GAMEFIELDTEXT_H

#include <SFML/Graphics.hpp>

class Resources;

class GameFieldText {
    Resources& resources;

	sf::Text nameTag;
    sf::Text awayText;
    sf::Text positionText;
    sf::Text readyText;
    sf::Text comboText;
    sf::Text pendingText;
    sf::Text countdownText;
    sf::Text bpmText;
    sf::Text gameOverText;
    sf::Text sets;
    sf::Text rounds;
    sf::Text p1_sets;
    sf::Text p2_sets;
    sf::Text p1_rounds;
    sf::Text p2_rounds;
public:
    sf::RenderTexture* texture;
    
    sf::ConvexShape comboTimer;

    sf::Uint8 combo, pending;
    sf::Uint16 bpm;
    sf::Int8 position, countdown, gameover;
    bool away, ready, results;

    sf::String name;

    GameFieldText(Resources& _resources);
    GameFieldText(const GameFieldText& text);

    void setName(const sf::String& n);
    void setPosition(const sf::Int8 _position);
    void setCountdown(const sf::Int8 _countdown);
    void setGameover(const sf::Int8 _gameover);
    bool setComboTimer(sf::Uint8 count);
    void setBpm(const sf::Uint16 _bpm);
    void setCombo(const sf::Uint8 _combo);
    void setPending(const sf::Uint8 _pending);
    void setResults(sf::Uint16 myId);

    void clear();

    void drawText();
};

#endif