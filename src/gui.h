#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"

class optionSet;
class soundBank;
class gamePlay;
class network;
class PacketCompress;
class textures;
class Resources;

class Menu;
class LoginBox;
class GameOptions;
class Connecting;
class GameplayUI;
class OnlineplayUI;
class AreYouSure;
class PerformanceOutput;
class BugReport;

class obsField;

enum GameStates { MainMenu, CountDown, Game, GameOver, Replay, Practice };

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);
	~UI();

	tgui::Gui tGui;
	tgui::Theme::Ptr themeTG;
	tgui::Theme::Ptr themeBB;

	Menu* mainMenu;
	LoginBox* loginBox;
	GameOptions* gameOptions;
	Connecting* connectingScreen;
	GameplayUI* gameplayUI;
	OnlineplayUI* onlineplayUI;
	AreYouSure* areYouSure;
	PerformanceOutput* performanceOutput;
	BugReport* bugReport;
	tgui::Label::Ptr QuickMsg;

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	network& net;

	sf::RenderWindow* window;

	bool playonline;
	bool chatFocused;
	bool inroom;
	bool away;

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Uint16 clientVersion;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Time spamTime;
	sf::Clock delayClock;

	bool udpConfirmed;

	GameStates gamestate;

	short spamCount;

	void setGameState(GameStates state);

	void Chat();
	void Score();

	void joinRoom(sf::Uint16);
	void leaveRoom();

	void getMsg();
	void sendMsg(const sf::String& to, const sf::String& msg);
	void chatFocus(bool i);

	void quickMsg(const sf::String& msg);

	void disconnect();

	//GameFIeldDrawer stuff

	std::list<obsField> fields;

	sf::Clock sclock;
	obsField* scaleup;

	float currentR;

	PacketCompress compressor;
	sf::Time gamedata;
	sf::Uint8 gamedatacount;
	sf::Uint16 myId;

	void addField(obsField& field);
	void removeField(sf::Uint16 id);
	void updateField(obsField& field);
	void calFieldPos();

	void removeAllFields();

	void resetOppFields();

	void drawOppField(obsField& field);
	void drawFields();

	void handleEvent(sf::Event& event);

	void gameInput(sf::Event& event);
	void windowEvents(sf::Event& event);
	void resizeWindow(sf::Event& event);
	
	void enlargePlayfield(sf::Event& event);
	void keyEvents(sf::Event& event);
	void scrollBar(sf::Event& event);

	void handlePacket();
	void handleSignal();

	void sendGameData();
	void sendGameOver();
	void sendGameWinner();

	void goAway();
	void unAway();

	void ready();

	void delayCheck();

	//Send packet functions
	void sendPacket0(sf::Uint16 id);
	void sendPacket1();
	void sendPacket2(const sf::String& hashorname, sf::Uint8 guest);
	void sendPacket3();
	void sendPacket4();
	void sendPacket5(sf::Uint8 amount);
	void sendPacket6(sf::Uint8 amount);
	void sendPacket7(sf::Uint8 amount);
	void sendPacket8();
	void sendPacket9();
	void sendPacket10(const sf::String& to, const sf::String& msg);
	void sendPacket11(const sf::String& name, sf::Uint8 maxPlayers);
	void sendPacket12();
	void sendPacket13();
	void sendPacket14(sf::Uint16 id);
	void sendPacket15(sf::Uint16 id);
	void sendPacket16(sf::Uint16 id);
	void sendPacket17(sf::Uint16 id);
	void sendPacket18(sf::Uint16 tournamentId, sf::Uint16 gameId);
	void sendPacket19(sf::Uint16 tournamentId);
	void sendPacket20();
	void sendPacket99();
	void sendPacket100();
	void sendPacket102(sf::Uint8 pingId);
};

#endif