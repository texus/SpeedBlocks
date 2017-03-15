#include "gui.h"
#include "optionSet.h"
#include "sounds.h"
#include "gamePlay.h"
#include "gameField.h"
#include "network.h"
#include "textures.h"
#include <iostream>
using namespace std;

UI::UI(sf::RenderWindow& rwindow, sf::Font& font1, sf::Font& font2, optionSet& opt, soundBank& soundy, gamePlay& gamey, network& _net, textures& _tex) : gui(rwindow), typewriter(font1), printFont(&font2), printFont2(font2) {
	training=false;
	playonline=false;
	quit=false;
	adjPieces=false;
	updPieces=false;
	chatFocused=false;
	inroom=false;
	startgame=false;
	startcount=false;
	disconnect=false;
	away=false;

	window=&rwindow;
	sounds=&soundy;
	options=&opt;
	game=&gamey;
	net=&_net;
	textureBase=&_tex;

	themeTG = tgui::Theme::create("TransparentGrey.txt");
	themeBB = tgui::Theme::create("BabyBlue.txt");

	initSprites();

	tgui::Panel::Ptr MainMenu = tgui::Panel::create();  // MAINMENU
	MainMenu->setSize(800, 600);
	MainMenu->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(MainMenu, "MainMenu");

	tgui::Button::Ptr PO = themeTG->load("Button");
	PO->setSize(480, 100);
	PO->setPosition(240, 100);
	PO->setText("Play Online");
	PO->setTextSize(72);
	PO->setFont(typewriter);
	PO->connect("pressed", &UI::playOnline, this);
	MainMenu->add(PO);

	tgui::Button::Ptr Tr = themeTG->load("Button");
	Tr->setSize(360, 100);
	Tr->setPosition(300, 220);
	Tr->setText("Training");
	Tr->setTextSize(72);
	Tr->setFont(typewriter);
	Tr->connect("pressed", &UI::setBool, this, std::ref(training));
	MainMenu->add(Tr);

	tgui::Button::Ptr Opt = themeTG->load("Button");
	Opt->setSize(310, 100);
	Opt->setPosition(325, 340);
	Opt->setText("Options");
	Opt->setTextSize(72);
	Opt->setFont(typewriter);
	Opt->connect("pressed", &UI::Options, this);
	MainMenu->add(Opt);

	tgui::Button::Ptr Quit = themeTG->load("Button");
	Quit->setSize(180, 100);
	Quit->setPosition(390, 460);
	Quit->setText("Quit");
	Quit->setTextSize(72);
	Quit->setFont(typewriter);
	Quit->connect("pressed", &UI::setBool, this, std::ref(quit));
	MainMenu->add(Quit);

	tgui::Panel::Ptr LiP = tgui::Panel::create(); // Login panel
	LiP->setSize(400, 300);
	LiP->setPosition(280, 150);
	LiP->setBackgroundColor(sf::Color(50,50,50,200));
	LiP->hide();
	gui.add(LiP, "Login");

	tgui::Label::Ptr LiL1 = themeTG->load("Label");
	LiL1->setText("Username/Email");
	LiL1->setPosition(20, 20);
	LiP->add(LiL1);

	tgui::Label::Ptr LiL2 = themeTG->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(220, 20);
	LiP->add(LiL2);

	tgui::EditBox::Ptr LiE1 = themeTG->load("EditBox");
	LiE1->setPosition(10, 60);
	LiE1->setSize(180, 30);
	LiP->add(LiE1, "Username");

	tgui::EditBox::Ptr LiE2 = themeTG->load("EditBox");
	LiE2->setPosition(210, 60);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	LiE2->connect("ReturnKeyPressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	LiP->add(LiE2);

	tgui::Button::Ptr LiB1 = themeBB->load("Button");
	LiB1->setPosition(50, 110);
	LiB1->setSize(100, 40);
	LiB1->setOpacity(0.7);
	LiB1->setText("Login");
	LiB1->connect("pressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	LiP->add(LiB1);

	tgui::Button::Ptr LiB2 = themeBB->load("Button");
	LiB2->setPosition(250, 110);
	LiB2->setSize(100, 40);
	LiB2->setOpacity(0.7);
	LiB2->setText("Cancel");
	LiB2->connect("Pressed", &UI::closeLogin, this);
	LiP->add(LiB2);

	tgui::Label::Ptr LiL3 = themeTG->load("Label");
	LiL3->setText("Guest name");
	LiL3->setPosition(145, 175);
	LiP->add(LiL3);

	tgui::EditBox::Ptr LiE3 = themeTG->load("EditBox");
	LiE3->setPosition(100, 210);
	LiE3->setSize(200, 30);
	LiE3->connect("ReturnKeyPressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	LiP->add(LiE3);

	tgui::Button::Ptr LiB3 = themeBB->load("Button");
	LiB3->setPosition(125, 250);
	LiB3->setSize(150, 40);
	LiB3->setOpacity(0.7);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &UI::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	LiP->add(LiB3);

	tgui::Tab::Ptr otab = themeBB->load("Tab"); // General Options
	otab->setTextSize(32);
	otab->setPosition(250, 30);
	otab->add("General");
	otab->add("Video");
	otab->add("Sound");
	otab->add("Back");
	otab->select(0);
	otab->hide();
	otab->connect("TabSelected", &UI::otabSelect, this);
	otab->setOpacity(0.7);
	gui.add(otab, "OptTab");

	tgui::Panel::Ptr GenOpt = tgui::Panel::create();
	GenOpt->setSize(960, 500);
	GenOpt->setPosition(0, 100);
	GenOpt->setBackgroundColor(sf::Color(255,255,255,0));
	GenOpt->hide();
	gui.add(GenOpt, "GenOpt");

	tgui::Label::Ptr NL = themeTG->load("Label");
	NL->setPosition(80, 13);
	NL->setText("Name");
	GenOpt->add(NL);
	tgui::EditBox::Ptr NTB = themeTG->load("EditBox");
	NTB->setText(options->name);
	NTB->setPosition(150, 10);
	NTB->setSize(400, 30);
	NTB->connect("TextChanged", &UI::changeName, this);
	GenOpt->add(NTB);

	char keyname[12]; // "Press any key" box

	tgui::Panel::Ptr CKP = themeTG->load("Panel");
	CKP->setSize(300, 100);
	CKP->setPosition(250, 150);
	CKP->hide();
	GenOpt->add(CKP, "SelectKey");
	tgui::Label::Ptr CKPL = themeTG->load("Label");
	CKPL->setTextSize(32);
	CKPL->setText("Press any key");
	CKPL->setPosition(40, 30);
	CKP->add(CKPL);

	tgui::Label::Ptr LeL = themeTG->load("Label"); // Binding Keys
	LeL->setPosition(0, 63);
	LeL->setSize(90, 30);
	LeL->setText("Left");
	LeL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(LeL);
	tgui::Button::Ptr LeB = themeTG->load("Button");
	LeB->setPosition(100, 60);
	LeB->connect("pressed", &UI::setKey, this, LeB, std::ref(options->left));
	SFKeyToString(options->left, keyname);
	LeB->setText(keyname);
	GenOpt->add(LeB);

	tgui::Label::Ptr RiL = themeTG->load("Label");
	RiL->setPosition(0, 103);
	RiL->setSize(90, 30);
	RiL->setText("Right");
	RiL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(RiL);
	tgui::Button::Ptr RiB = themeTG->load("Button");
	RiB->setPosition(100, 100);
	RiB->connect("pressed", &UI::setKey, this, RiB, std::ref(options->right));
	SFKeyToString(options->right, keyname);
	RiB->setText(keyname);
	GenOpt->add(RiB);

	tgui::Label::Ptr DoL = themeTG->load("Label");
	DoL->setPosition(0, 143);
	DoL->setSize(90, 30);
	DoL->setText("Down");
	DoL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(DoL);
	tgui::Button::Ptr DoB = themeTG->load("Button");
	DoB->setPosition(100, 140);
	DoB->connect("pressed", &UI::setKey, this, DoB, std::ref(options->down));
	SFKeyToString(options->down, keyname);
	DoB->setText(keyname);
	GenOpt->add(DoB);

	tgui::Label::Ptr CwL = themeTG->load("Label");
	CwL->setPosition(0, 183);
	CwL->setSize(90, 30);
	CwL->setText("Rotate >");
	CwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CwL);
	tgui::Button::Ptr CwB = themeTG->load("Button");
	CwB->setPosition(100, 180);
	CwB->connect("pressed", &UI::setKey, this, CwB, std::ref(options->rcw));
	SFKeyToString(options->rcw, keyname);
	CwB->setText(keyname);
	GenOpt->add(CwB);

	tgui::Label::Ptr CcL = themeTG->load("Label");
	CcL->setPosition(0, 223);
	CcL->setSize(90, 30);
	CcL->setText("Rotate <");
	CcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(CcL);
	tgui::Button::Ptr CcB = themeTG->load("Button");
	CcB->setPosition(100, 220);
	CcB->connect("pressed", &UI::setKey, this, CcB, std::ref(options->rccw));
	SFKeyToString(options->rccw, keyname);
	CcB->setText(keyname);
	GenOpt->add(CcB);

	tgui::Label::Ptr R1L = themeTG->load("Label");
	R1L->setPosition(350, 63);
	R1L->setSize(140, 30);
	R1L->setText("Rotate 180");
	R1L->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(R1L);
	tgui::Button::Ptr R1B = themeTG->load("Button");
	R1B->setPosition(500, 60);
	R1B->connect("pressed", &UI::setKey, this, R1B, std::ref(options->r180));
	SFKeyToString(options->r180, keyname);
	R1B->setText(keyname);
	GenOpt->add(R1B);

	tgui::Label::Ptr HdL = themeTG->load("Label");
	HdL->setPosition(350, 103);
	HdL->setSize(140, 30);
	HdL->setText("Hard Drop");
	HdL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(HdL);
	tgui::Button::Ptr HdB = themeTG->load("Button");
	HdB->setPosition(500, 100);
	HdB->connect("pressed", &UI::setKey, this, HdB, std::ref(options->hd));
	SFKeyToString(options->hd, keyname);
	HdB->setText(keyname);
	GenOpt->add(HdB);

	tgui::Label::Ptr ChL = themeTG->load("Label");
	ChL->setPosition(400, 143);
	ChL->setSize(90, 30);
	ChL->setText("Chat");
	ChL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ChL);
	tgui::Button::Ptr ChB = themeTG->load("Button");
	ChB->setPosition(500, 140);
	ChB->connect("pressed", &UI::setKey, this, ChB, std::ref(options->chat));
	SFKeyToString(options->chat, keyname);
	ChB->setText(keyname);
	GenOpt->add(ChB);

	tgui::Label::Ptr ScL = themeTG->load("Label");
	ScL->setPosition(400, 183);
	ScL->setSize(90, 30);
	ScL->setText("Score");
	ScL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(ScL);
	tgui::Button::Ptr ScB = themeTG->load("Button");
	ScB->setPosition(500, 180);
	ScB->connect("pressed", &UI::setKey, this, ScB, std::ref(options->score));
	SFKeyToString(options->score, keyname);
	ScB->setText(keyname);
	GenOpt->add(ScB);

	tgui::Label::Ptr AwL = themeTG->load("Label");
	AwL->setPosition(400, 223);
	AwL->setSize(90, 30);
	AwL->setText("Away");
	AwL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	GenOpt->add(AwL);
	tgui::Button::Ptr AwB = themeTG->load("Button");
	AwB->setPosition(500, 220);
	AwB->connect("pressed", &UI::setKey, this, AwB, std::ref(options->away));
	SFKeyToString(options->away, keyname);
	AwB->setText(keyname);
	GenOpt->add(AwB);

	tgui::Button::Ptr Rp[7]; // Align Pieces
	tgui::Button::Ptr Cc[7];
	for (int i=0; i<7; i++) {
		Rp[i] = themeBB->load("Button");
		Cc[i] = themeBB->load("Button");

		Rp[i]->setPosition(i*115+20, 365);
		Rp[i]->setSize(30,30);
		Rp[i]->setOpacity(0.7);
		Rp[i]->connect("pressed", &UI::rotPiece, this, i);

		Cc[i]->setPosition(i*115+60, 365);
		Cc[i]->setSize(30,30);
		Cc[i]->connect("pressed", &UI::colPiece, this, i);

		GenOpt->add(Rp[i]);
		GenOpt->add(Cc[i]);
	}

	tgui::Label::Ptr Rl1 = themeTG->load("Label"); // Repeat delay & speed
	Rl1->setPosition(750, 0);
	Rl1->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl1->setText("Left & Right Repeat\nDelay         Speed");
	GenOpt->add(Rl1);

	tgui::EditBox::Ptr Re1 = themeTG->load("EditBox");
	Re1->setPosition(750, 50);
	Re1->setSize(70, 30);
	Re1->setInputValidator(tgui::EditBox::Validator::UInt);
	Re1->setText(to_string(options->repeatDelay.asMilliseconds()));
	Re1->connect("TextChanged", &optionSet::setDelay, options, 1);
	GenOpt->add(Re1);

	tgui::EditBox::Ptr Re2 = themeTG->load("EditBox");
	Re2->setPosition(850, 50);
	Re2->setSize(70, 30);
	Re2->setInputValidator(tgui::EditBox::Validator::UInt);
	Re2->setText(to_string(options->repeatSpeed.asMilliseconds()));
	Re2->connect("TextChanged", &optionSet::setDelay, options, 2);
	GenOpt->add(Re2);

	tgui::Label::Ptr Rl2 = themeTG->load("Label");
	Rl2->setPosition(750, 100);
	Rl2->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	Rl2->setText("Down Repeat\n  Delay         Speed");
	GenOpt->add(Rl2);

	tgui::EditBox::Ptr Re3 = themeTG->load("EditBox");
	Re3->setPosition(750, 150);
	Re3->setSize(70, 30);
	Re3->setInputValidator(tgui::EditBox::Validator::UInt);
	Re3->setText(to_string(options->repeatDelayDown.asMilliseconds()));
	Re3->connect("TextChanged", &optionSet::setDelay, options, 3);
	GenOpt->add(Re3);

	tgui::EditBox::Ptr Re4 = themeTG->load("EditBox");
	Re4->setPosition(850, 150);
	Re4->setSize(70, 30);
	Re4->setInputValidator(tgui::EditBox::Validator::UInt);
	Re4->setText(to_string(options->repeatSpeedDown.asMilliseconds()));
	Re4->connect("TextChanged", &optionSet::setDelay, options, 4);
	GenOpt->add(Re4);

	tgui::Panel::Ptr VidOpt = tgui::Panel::create(); // Video Options
	VidOpt->setSize(800, 500);
	VidOpt->setPosition(0, 100);
	VidOpt->setBackgroundColor(sf::Color(255,255,255,0));
	VidOpt->hide();
	gui.add(VidOpt, "VidOpt");

	tgui::Label::Ptr ViL = themeTG->load("Label");
	ViL->setPosition(350, 0);
	ViL->setText("Video Mode");
	VidOpt->add(ViL);

	tgui::Slider::Ptr ViS = themeTG->load("Slider");
	ViS->setPosition(50, 50);
	ViS->setSize(700, 30);
	ViS->setMaximum(options->modes.size()-1);
	if (options->currentmode == -1)
		ViS->setValue(0);
	else
		ViS->setValue(options->currentmode);
	ViS->connect("ValueChanged", &UI::vidSlide, this);
	ViS->disable();
	VidOpt->add(ViS, "VMSlider");

	tgui::Label::Ptr SvL = themeTG->load("Label");
	SvL->setPosition(340, 90);
	sf::String cvmname;
	cvmname = to_string(options->modes[ViS->getValue()].width) + "x" + to_string(options->modes[ViS->getValue()].height);
	SvL->setText(cvmname);
	VidOpt->add(SvL, "VideoMode");

	tgui::CheckBox::Ptr FsC = themeTG->load("CheckBox");
	FsC->setPosition(120, 130);
	FsC->setText("Fullscreen");
	if (options->fullscreen) {
		FsC->check();
		ViS->enable();
	}
	FsC->connect("Checked Unchecked", &UI::fsChecked, this);
	VidOpt->add(FsC, "Fullscreen");

	tgui::Button::Ptr AvB = themeBB->load("Button");
	AvB->setText("Apply");
	AvB->setPosition(340, 200);
	AvB->setOpacity(0.7);
	AvB->connect("pressed", &UI::applyVideo, this);
	VidOpt->add(AvB);

	tgui::Panel::Ptr SndOpt = tgui::Panel::create(); // Sound Options
	SndOpt->setSize(800, 500);
	SndOpt->setPosition(0, 100);
	SndOpt->setBackgroundColor(sf::Color(255,255,255,0));
	SndOpt->hide();
	gui.add(SndOpt, "SndOpt");

	tgui::CheckBox::Ptr EsC = themeTG->load("CheckBox");
	EsC->setPosition(150, 0);
	EsC->setText("Sound Enabled");
	EsC->connect("Checked Unchecked", &UI::sndChecked, this);
	if (options->sound)
		EsC->check();
	SndOpt->add(EsC);

	tgui::Slider::Ptr MvS = themeTG->load("Slider");
	MvS->setPosition(50, 100);
	MvS->setSize(700,30);
	MvS->setMaximum(100);
	MvS->connect("ValueChanged", &UI::volSlide, this, 1);
	MvS->setValue(options->MusicVolume);
	SndOpt->add(MvS);

	tgui::Label::Ptr MvL = themeTG->load("Label");
	MvL->setText("Music Volume");
	MvL->setPosition(350, 50);
	SndOpt->add(MvL);

	tgui::Slider::Ptr EvS = themeTG->load("Slider");
	EvS->setPosition(50, 200);
	EvS->setSize(700,30);
	EvS->setMaximum(100);
	EvS->connect("ValueChanged", &UI::volSlide, this, 2);
	EvS->setValue(options->EffectVolume);
	SndOpt->add(EvS);

	tgui::Label::Ptr EvL = themeTG->load("Label");
	EvL->setText("Effect Volume");
	EvL->setPosition(350, 150);
	SndOpt->add(EvL);

	tgui::Slider::Ptr CvS = themeTG->load("Slider");
	CvS->setPosition(50, 300);
	CvS->setSize(700,30);
	CvS->setMaximum(100);
	CvS->connect("ValueChanged", &UI::volSlide, this, 3);
	CvS->setValue(options->ChatVolume);
	SndOpt->add(CvS);

	tgui::Label::Ptr CvL = themeTG->load("Label");
	CvL->setText("Chat Volume");
	CvL->setPosition(350, 250);
	SndOpt->add(CvL);

	// In-game UI starts here!

	tgui::Tab::Ptr IgT = themeBB->load("Tab");
	IgT->add("GameFields");
	IgT->add("Score");
	IgT->add("Chat");
	IgT->setPosition(465, 0);
	IgT->hide();
	IgT->select(0);
	IgT->connect("TabSelected", &UI::igtabSelect, this);
	IgT->setOpacity(0.7);
	gui.add(IgT, "InGameTab");

	tgui::Panel::Ptr GfP = tgui::Panel::create(); // GameFields panel
	GfP->setPosition(465, 40);
	GfP->setSize(490, 555);
	GfP->hide();
	GfP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(GfP, "GameFields");

	tgui::Panel::Ptr ScP = tgui::Panel::create(); // Score panel
	ScP->setPosition(465,40);
	ScP->setSize(490, 555);
	ScP->hide();
	ScP->setBackgroundColor(sf::Color(200,200,200, 50));
	gui.add(ScP, "Score");

	tgui::ListBox::Ptr ScT = themeBB->load("ListBox");
	ScT->setPosition(5, 35);
	ScT->setSize(480, 475);
	ScP->add(ScT, "ScoreBox");

	tgui::Label::Ptr SbL = themeTG->load("Label");
	SbL->setPosition(5, 5);
	SbL->setText("Name                 Score  BPM  Cmb Sent  SPM   Blocked");
	ScP->add(SbL, "ScoreBoxLab");

	tgui::Panel::Ptr ChP = tgui::Panel::create(); // Chat panel
	ChP->setPosition(465,40);
	ChP->setSize(490, 555);
	ChP->hide();
	ChP->setBackgroundColor(sf::Color(200,200,200, 50));
	gui.add(ChP, "Chat");

	tgui::Tab::Ptr ChT = themeBB->load("Tab");
	ChT->add("Room");
	ChT->add("Lobby");
	ChT->setTabHeight(30);
	ChT->select(0);
	ChT->connect("TabSelected", &UI::chattabSelect, this);
	ChT->setOpacity(0.7);
	ChP->add(ChT, "ChatTab");

	tgui::ChatBox::Ptr RoC = themeBB->load("ChatBox");
	RoC->setPosition(5, 35);
	RoC->setSize(480, 475);
	ChP->add(RoC, "Room");

	tgui::ChatBox::Ptr LoC = themeBB->load("ChatBox");
	LoC->setPosition(5, 35);
	LoC->setSize(480, 475);
	LoC->hide();
	ChP->add(LoC, "Lobby");

	tgui::EditBox::Ptr ChE = themeTG->load("EditBox");
	ChE->setPosition(5, 520);
	ChE->setSize(480, 30);
	ChE->connect("ReturnKeyPressed", &UI::sendMsg, this, std::bind(&tgui::Tab::getSelected, ChT));
	ChE->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, ChE));
	ChP->add(ChE, "ChatBox");

	tgui::Panel::Ptr WcP = tgui::Panel::create(); // Connecting... Panel
	WcP->setPosition(330,250);
	WcP->setSize(300, 100);
	WcP->hide();
	WcP->setBackgroundColor(sf::Color(50,50,50,150));
	gui.add(WcP, "Connecting");

	tgui::Label::Ptr WcL = themeTG->load("Label");
	WcL->setPosition(0, 40);
	WcL->setSize(300, 50);
	WcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	WcL->setText("Connecting to server...");
	WcP->add(WcL, "ConnText");

	// Online Play UI starts here

	tgui::Tab::Ptr LoT = themeBB->load("Tab"); // Online play Tab
	LoT->add("Rooms");
	LoT->add("Lobby");
	LoT->add("Back");
	LoT->setTabHeight(60);
	LoT->setPosition(50, 20);
	LoT->select(0);
	LoT->connect("TabSelected", &UI::opTabSelect, this);
	LoT->hide();
	LoT->setOpacity(0.7);
	gui.add(LoT, "opTab");

	tgui::Panel::Ptr RoP = tgui::Panel::create(); // Rooms Panel
	RoP->setPosition(0,100);
	RoP->setSize(960, 500);
	RoP->hide();
	RoP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(RoP, "Rooms");

	tgui::Scrollbar::Ptr RoS = themeTG->load("Scrollbar");
	RoS->setSize(30, 490);
	RoS->setPosition(400, 5);
	RoS->setMaximum(0);
	RoS->setLowValue(1);
	RoS->connect("ValueChanged", &UI::roomScrolled, this);
	RoP->add(RoS, "RoomScroll");

	tgui::Panel::Ptr LoP = tgui::Panel::create(); // Lobby Panel
	LoP->setPosition(0,100);
	LoP->setSize(960, 500);
	LoP->hide();
	LoP->setBackgroundColor(sf::Color(255,255,255,0));
	gui.add(LoP, "ServerLobby");

	tgui::ChatBox::Ptr SlC = themeBB->load("ChatBox");
	SlC->setPosition(5, 5);
	SlC->setSize(950, 455);
	LoP->add(SlC, "Lobby2");

	tgui::EditBox::Ptr SlE = themeTG->load("EditBox");
	SlE->setPosition(5, 465);
	SlE->setSize(950, 30);
	SlE->connect("ReturnKeyPressed", &UI::sendMsg, this, "Lobby");
	SlE->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, SlE));
	LoP->add(SlE, "slChatBox");

	tgui::Panel::Ptr AUS = tgui::Panel::create(); // Are you sure? Panel
	AUS->setPosition(330,250);
	AUS->setSize(300, 100);
	AUS->hide();
	AUS->setBackgroundColor(sf::Color(50,50,50, 200));
	gui.add(AUS, "AUS");

	tgui::Label::Ptr AUSL = themeTG->load("Label");
	AUSL->setPosition(0, 20);
	AUSL->setSize(300, 50);
	AUSL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	AUSL->setText("Are you sure?");
	AUS->add(AUSL, "AUSL");

	tgui::Button::Ptr AUSYB = themeBB->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &UI::ausY, this);
	AUS->add(AUSYB);

	tgui::Button::Ptr AUSNB = themeBB->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &UI::ausN, this);
	AUS->add(AUSNB);

	tgui::Label::Ptr QmL = themeTG->load("Label");
	QmL->setPosition(0,30);
	QmL->setSize(960,200);
	QmL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	QmL->setTextSize(32);
	QmL->setTextColor(sf::Color::Red);
	QmL->hide();
	gui.add(QmL, "QuickMsg");
}

void UI::addRoom(const sf::String& name, short curr, short max, short id) {
	playRoom newroom;
	playRooms.push_back(newroom);
	playRooms.back().name = name;
	playRooms.back().currentPlayers = curr;
	playRooms.back().maxPlayers = max;
	playRooms.back().button = themeTG->load("Button");
	playRooms.back().button->setText(name);
	playRooms.back().button->setSize(300, 100);
	playRooms.back().button->connect("Pressed", &UI::joinRoom, this, id);
	gui.get<tgui::Panel>("Rooms")->add(playRooms.back().button);

	playRooms.back().label = themeTG->load("Label");
	playRooms.back().label->setText(to_string(curr) + "/" + to_string(max) + " players");

	playRooms.back().id = id;

	setRoomPos();
}

void UI::joinRoom(sf::Uint16 id) { //0-Packet
	net->packet.clear();
	sf::Uint8 packetid = 0;
	net->packet << packetid << id;
	net->sendTCP();
	away=false;
	game->autoaway=false;
}

void GameFieldDrawer::leaveRoom() { //1-Packet
	net->packet.clear();
	sf::Uint8 packetid = 1;
	net->packet << packetid;
	net->sendTCP();
	inroom=false;
	quit=true;
	gui.get("opTab")->show();
	gui.get("Rooms")->show();
	gui.get<tgui::Tab>("opTab")->select(0);
	gui.get("InGameTab")->hide();
	gui.get("Chat")->hide();
	gui.get("Score")->hide();
	gui.get("GameFields")->hide();
	removeAllFields();
}

void UI::removeRoom(const sf::String& name) {
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		if (it->name == name) {
			gui.get<tgui::Panel>("Rooms")->remove(it->button);
			it = playRooms.erase(it);
			setRoomPos();
			return;
		}
	}
}

void UI::removeAllRooms() {
	while (playRooms.size() > 0)
		playRooms.pop_front();
}

void UI::setRoomPos() {
	short height = playRooms.size() * 120;
	if (height > 500) {
		height-=500;
		height/=30;
		height++;
		gui.get<tgui::Scrollbar>("RoomScroll", 1)->setMaximum(height);
	}
	else
		gui.get<tgui::Scrollbar>("RoomScroll", 1)->setMaximum(0);
	short scrollpos = gui.get<tgui::Scrollbar>("RoomScroll", 1)->getValue();
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		int i = std::distance(playRooms.begin(), it);
		it->button->setPosition(50, i*120 - scrollpos*30);
	}
}

void UI::roomScrolled(int c) {
	for (auto it = playRooms.begin(); it != playRooms.end(); it++) {
		int i = std::distance(playRooms.begin(), it);
		it->button->setPosition(50, i*120 - c*30);
	}
}

bool UI::login(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	gui.get("MainMenu")->hide();
	gui.get("Connecting")->show();
	if (net->connect() == sf::Socket::Done) {
		net->packet.clear();
		sf::Uint8 packetid = 2; //2-Packet
		sf::Uint16 port = net->localUdpPort;
		net->packet << packetid << port << guest << name << pass;
		net->sendTCP();
		playonline=true;
		gui.get("Login")->hide();
		if (guest)
			game->field.setName(name, *printFont);
	}
	else {
		quickMsg("Could not connect to server");
		gui.get("Connecting")->hide();
		gui.get("MainMenu")->show();
	}
}

void UI::closeLogin() {
	gui.get("MainMenu")->enable();
	gui.get("Login")->hide();
}

void UI::playOnline() {
	gui.get("MainMenu")->disable();
	gui.get("Login")->show();
	gui.get("Username", 1)->focus();
}

void UI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		gui.get("Rooms")->show();
		gui.get("ServerLobby")->hide();
	}
	else if (tab == "Lobby") {
		gui.get("ServerLobby")->show();
		gui.get("Rooms")->hide();
	}
	else if (tab == "Back") {
		gui.get("ServerLobby")->hide();
		gui.get("Rooms")->hide();
		gui.get("opTab")->hide();
		gui.get("MainMenu")->show();
		net->disconnect();
		playonline=false;
	}
}

void UI::ausY() {
	quit=true;
	gui.get("AUS")->hide();
}

void UI::ausN() {
	gui.get("AUS")->hide();
	gui.get("MainMenu")->enable();
}

void UI::quickMsg(const sf::String& msg) {
	gui.get<tgui::Label>("QuickMsg")->setText(msg);
	gui.get<tgui::Label>("QuickMsg")->show();
	gui.get<tgui::Label>("QuickMsg")->hideWithEffect(tgui::ShowAnimationType::Fade, sf::seconds(2));
}

void UI::chatFocus(bool i) {
	if (i)
		chatFocused=true;
	else
		chatFocused=false;
}

void UI::sendMsg(const sf::String& to, const sf::String& msg) {
	sf::Uint8 packetid = 10;
	if (msg[0]=='/' && msg[1]=='w' && msg[2]==' ') {
		short until = msg.find(' ', 3);
		sf::String privto = msg.substring(3, until-3);
		sf::String privmsg = msg.substring(until, sf::String::InvalidPos);
		net->packet.clear();
		net->packet << packetid;
		packetid = 3;
		net->packet << packetid << privto << privmsg;
		net->sendTCP();
		gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
		gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
		return;
	}
	sf::String postmsg = game->field.name + ": " + msg;
	gui.get<tgui::ChatBox>(to, 1)->addLine(postmsg, sf::Color(255, 0, 0));
	if (to == "Lobby")
		gui.get<tgui::ChatBox>("Lobby2", 1)->addLine(postmsg, sf::Color(255, 0, 0));
	gui.get<tgui::EditBox>("ChatBox", 1)->setText("");
	gui.get<tgui::EditBox>("slChatBox", 1)->setText("");
	net->packet.clear(); //10-Packet
	net->packet << packetid;
	if (to == "Room") {
		packetid = 1;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else if (to == "Lobby") {
		packetid = 2;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else {
		packetid = 3;
		net->packet << packetid << to << msg;
		net->sendTCP();
	}
}

void UI::chattabSelect(const std::string& tab) {
	for (size_t i = 0; i < privChats.size(); i++)
		privChats[i].chatBox->hide();
	gui.get("Lobby", 1)->hide();
	gui.get("Room", 1)->hide();
	gui.get(tab, 1)->show();
}

void UI::roomMsg(const sf::String& name, const sf::String& msg) {
	gui.get<tgui::ChatBox>("Room", 1)->addLine(name + ": " + msg);
}

void UI::lobbyMsg(const sf::String& name, const sf::String& msg) {
	gui.get<tgui::ChatBox>("Lobby", 1)->addLine(name + ": " + msg);
	gui.get<tgui::ChatBox>("Lobby2", 1)->addLine(name + ": " + msg);
}

void UI::privMsg(const sf::String& name, const sf::String& msg) {
	short create=-1;
	for (size_t i = 0; i < privChats.size(); i++)
		if (privChats[i].name == name) {
			create=i;
		}
	if (create == -1) {
		privChat newchat;
		newchat.name=name;
		privChats.push_back(move(newchat));
		create=privChats.size()-1;


		privChats[create].chatBox = themeBB->load("ChatBox");
		privChats[create].chatBox->setPosition(5, 35);
		privChats[create].chatBox->setSize(480, 475);
		privChats[create].chatBox->hide();
		gui.get<tgui::Panel>("Chat")->add(privChats[create].chatBox, name);
		int tmp = gui.get<tgui::Tab>("ChatTab", 1)->getSelectedIndex();
		gui.get<tgui::Tab>("ChatTab", 1)->add(name);
		gui.get<tgui::Tab>("ChatTab", 1)->select(tmp);
	}
	privChats[create].chatBox->addLine(name + ": " + msg);
}

void UI::clearScoreBox() {
	gui.get<tgui::ListBox>("ScoreBox", 1)->removeAllItems();
}
void UI::printScoreBox(sf::String&& name, sf::Uint16 score, sf::Uint8 rank, sf::Uint16 bpm, sf::Uint8 combo, sf::Uint16 sen, sf::Uint16 spm, sf::Uint16 received, sf::Uint16 blocked) {
	sf::String line, append;
	line = name; append = to_string(score); appendLine(line, append, 36);
	append = to_string(rank); appendLine(line, append, 40);
	append = to_string(bpm); appendLine(line, append, 46);
	append = to_string(combo); appendLine(line, append, 56);
	append = to_string(sen); appendLine(line, append, 66);
	append = to_string(spm); appendLine(line, append, 76);
	append = to_string(blocked) + "/" + to_string(received); appendLine(line, append, 96);
	gui.get<tgui::ListBox>("ScoreBox", 1)->addItem(line);
}

void UI::appendLine(sf::String& line, sf::String append, short to) {
	if (line.getSize() < 5)
		line+="\t";
	if (line.getSize() < 10)
		line+="\t";
	if (line.getSize() < 15)
		line+="\t";
	line+="\t";
	line+=append;
}

void UI::igtabSelect(const std::string& tab) {
	if (tab == "GameFields") {
		gui.get("GameFields")->show();
		gui.get("Score")->hide();
		gui.get("Chat")->hide();
	}
	else if (tab == "Score") {
		gui.get("GameFields")->hide();
		gui.get("Score")->show();
		gui.get("Chat")->hide();
	}
	else if (tab == "Chat") {
		gui.get("GameFields")->hide();
		gui.get("Score")->hide();
		gui.get("Chat")->show();
	}
}

void UI::Chat() {
	if (gui.get("Chat")->isVisible()) {
		gui.get("Chat")->hide();
		gui.get("GameFields")->show();
		gui.get<tgui::Tab>("InGameTab")->select(0);
	}
	else {
		gui.get("Chat")->show();
		gui.get("GameFields")->hide();
		gui.get("Score")->hide();
		gui.get<tgui::Tab>("InGameTab")->select(2);
	}
}

void UI::Score() {
	if (gui.get("Score")->isVisible()) {
		gui.get("Score")->hide();
		gui.get("GameFields")->show();
		gui.get<tgui::Tab>("InGameTab")->select(0);
	}
	else {
		gui.get("Chat")->hide();
		gui.get("GameFields")->hide();
		gui.get("Score")->show();
		gui.get<tgui::Tab>("InGameTab")->select(1);
	}
}

void UI::otabSelect(std::string tab) {
	if (tab == "General") {
		gui.get("GenOpt")->show();
		gui.get("VidOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=true;
	}
	else if (tab == "Video") {
		gui.get("VidOpt")->show();
		gui.get("GenOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=false;
		if (options->fullscreen)
			gui.get<tgui::CheckBox>("Fullscreen", 1)->check();
		else
			gui.get<tgui::CheckBox>("Fullscreen", 1)->uncheck();
	}
	else if (tab == "Sound") {
		gui.get("VidOpt")->hide();
		gui.get("GenOpt")->hide();
		gui.get("SndOpt")->show();
		adjPieces=false;
	}
	else if (tab == "Back") {
		gui.get("MainMenu")->show();
		gui.get("OptTab")->hide();
		gui.get("GenOpt")->hide();
		gui.get("VidOpt")->hide();
		gui.get("SndOpt")->hide();
		adjPieces=false;
	}
}

void UI::volSlide(short i, short vol) {
	if (i == 1) {
		options->MusicVolume = vol;
		sounds->setMusicVolume(vol);
	}
	else if (i == 2) {
		options->EffectVolume = vol;
		sounds->setEffectVolume(vol);
	}
	else if (i == 3) {
		options->ChatVolume = vol;
		sounds->setChatVolume(vol);
	}
}

void UI::sndChecked(bool i) {
	if (i)
		options->sound = true;
	else
		options->sound = false;
}

void UI::fsChecked(bool i) {
	if (i)
		gui.get("VMSlider", 1)->enable();
	else
		gui.get("VMSlider", 1)->disable();
}

void UI::applyVideo() {
	if (gui.get<tgui::CheckBox>("Fullscreen", 1)->isChecked()) {
		if (options->currentmode == -1 || options->currentmode != gui.get<tgui::Slider>("VMSlider", 1)->getValue()) {
			options->currentmode = gui.get<tgui::Slider>("VMSlider", 1)->getValue();
			window->close();
			window->create(options->modes[options->currentmode], "SpeedBlocks", sf::Style::Fullscreen);
			window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
			options->fullscreen=true;
		}
	}
	else if (options->currentmode != -1) {
		window->close();
		window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		options->currentmode = -1;
		window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		options->fullscreen=false;
	}
}

void UI::vidSlide(short i) {
	sf::String name;
	name = to_string(options->modes[i].width) + "x" + to_string(options->modes[i].height);
	gui.get<tgui::Label>("VideoMode", 1)->setText(name);
}

void UI::setBool(bool& var) {
	var=true;
}

void UI::Options() {
	gui.get("MainMenu")->hide();
	gui.get("OptTab")->show();
	gui.get("GenOpt")->show();
	gui.get<tgui::Tab>("OptTab")->select(0);
	adjPieces=true;
}

void UI::mainMenu() {
	gui.get("InGameTab")->hide();
	gui.get("Chat")->hide();
	gui.get("Score")->hide();
	gui.get("GameFields")->hide();
	gui.get("MainMenu")->show();
}

void UI::setKey(tgui::Button::Ptr butt, sf::Keyboard::Key& skey) {
	gui.get("SelectKey", true)->show();
	gui.get("GenOpt", true)->disable();
	setkey=true;
	key=&skey;
	button=butt;
}

void UI::putKey(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == -1 ) {
        	gui.get("SelectKey", true)->hide();
        	gui.get("GenOpt", true)->enable();
        	setkey=false;
        }
        else {
        	gui.get("SelectKey", true)->hide();
        	gui.get("GenOpt", true)->enable();
        	setkey=false;
        	*key = event.key.code;
        	char name[12];
        	SFKeyToString(event.key.code, name);
        	button->setText(name);
        }
    }
}

void UI::changeName(const sf::String& name) {
	options->name = name;
	game->field.setName(name, *printFont);
}

void UI::rotPiece(short i) {
	options->piecerotation[i]++;
	if (options->piecerotation[i]>3)
		options->piecerotation[i]=0;
	piece[i].setRotation(options->piecerotation[i]*90);
	game->updateBasePieces();
}

void UI::colPiece(short i) {
	options->basepiece[i].tile++;
	if (options->basepiece[i].tile>7)
		options->basepiece[i].tile=1;
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (options->basepiece[i].grid[y][x])
				options->basepiece[i].grid[y][x]=options->basepiece[i].tile;
	piece[i].setColor(pColor(options->basepiece[i].tile));
	game->updateBasePieces();
}

sf::Color pColor(short i) {
	sf::Color col;
	switch (i) {
		case 1: col.r=255; col.g=0; col.b=0; break;
		case 2: col.r=0; col.g=255; col.b=0; break;
		case 3: col.r=0; col.g=0; col.b=255; break;
		case 4: col.r=255; col.g=0; col.b=255; break;
		case 5: col.r=0; col.g=255; col.b=255; break;
		case 6: col.r=255; col.g=255; col.b=0; break;
		case 7: col.r=255; col.g=255; col.b=255; break;

		default: col.r=0; col.g=0; col.b=0; break;
	}
	return col;
}

void UI::initSprites() {
	sf::Texture tileT;
	sf::Sprite tile;
	tileT.loadFromFile("tile.png");
	tile.setTexture(tileT);

	sf::RenderTexture rendtex;
	rendtex.create(120,120);

	short value[112] = { 0, 4, 0, 0,
						 0, 4, 0, 0,
						 0, 4, 4, 0,
						 0, 0, 0, 0,

						 0, 3, 0, 0,
						 0, 3, 0, 0,
						 3, 3, 0, 0,
						 0, 0, 0, 0,

						 0, 5, 0, 0,
						 0, 5, 5, 0,
						 0, 0, 5, 0,
						 0, 0, 0, 0,

						 0, 7, 0, 0,
						 7, 7, 0, 0,
						 7, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 0,
						 0, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 6, 6, 0,
						 0, 6, 6, 0,
						 0, 0, 0, 0 };
	for (int p=0; p<7; p++) {
		rendtex.clear(sf::Color(255,255,255,0));
		if (p==4 || p==6) {
			for (int x=0; x<4; x++)
				for (int y=0; y<4; y++)
					if (value[16*p + 4*y + x]) {
						tile.setPosition(x*30, y*30);
						rendtex.draw(tile);
					}
			rendtex.display();
			texture[p] = rendtex.getTexture();
			piece[p].setTexture(texture[p]);
			piece[p].setScale(0.8, 0.8);
			piece[p].setPosition(115*p+50, 360+50);
			piece[p].setColor(pColor(options->basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options->piecerotation[p]*90);
		}
		else {
			for (int x=0; x<3; x++)
				for (int y=0; y<3; y++)
					if (value[16*p + 4*y + x]) {
						tile.setPosition(x*30+15, y*30+15);
						rendtex.draw(tile);
					}
			rendtex.display();
			texture[p] = rendtex.getTexture();
			piece[p].setTexture(texture[p]);
			piece[p].setScale(0.8, 0.8);
			piece[p].setPosition(115*p+50, 360+50);
			piece[p].setColor(pColor(options->basepiece[p].tile));
			piece[p].setOrigin(60,60);
			piece[p].setRotation(options->piecerotation[p]*90);
		}
	}
}

void SFKeyToString(unsigned int keycode, char *keyStr) {
    switch (keycode) {
    case sf::Keyboard::Key::Escape: sprintf(keyStr, "Escape"); break;
    case sf::Keyboard::Key::LControl: sprintf(keyStr, "LControl"); break;
    case sf::Keyboard::Key::LShift: sprintf(keyStr, "LShift"); break;
    case sf::Keyboard::Key::LAlt: sprintf(keyStr, "LAlt"); break;
    case sf::Keyboard::Key::LSystem: sprintf(keyStr, "LSystem"); break;
    case sf::Keyboard::Key::RControl: sprintf(keyStr, "RControl"); break;
    case sf::Keyboard::Key::RShift: sprintf(keyStr, "RShift"); break;
    case sf::Keyboard::Key::RAlt: sprintf(keyStr, "RAlt"); break;
    case sf::Keyboard::Key::RSystem: sprintf(keyStr, "RSystem"); break;
    case sf::Keyboard::Key::Menu: sprintf(keyStr, "Menu"); break;
    case sf::Keyboard::Key::LBracket: sprintf(keyStr, "LBracket"); break;
    case sf::Keyboard::Key::RBracket: sprintf(keyStr, "RBracket"); break;
    case sf::Keyboard::Key::SemiColon: sprintf(keyStr, ";"); break;
    case sf::Keyboard::Key::Comma: sprintf(keyStr, ","); break;
    case sf::Keyboard::Key::Period: sprintf(keyStr, "."); break;
    case sf::Keyboard::Key::Quote: sprintf(keyStr, "\'"); break;
    case sf::Keyboard::Key::Slash: sprintf(keyStr, "/"); break;
    case sf::Keyboard::Key::BackSlash: sprintf(keyStr, "\\"); break;
    case sf::Keyboard::Key::Tilde: sprintf(keyStr, "~"); break;
    case sf::Keyboard::Key::Equal: sprintf(keyStr, "="); break;
    case sf::Keyboard::Key::Dash: sprintf(keyStr, "-"); break;
    case sf::Keyboard::Key::Space: sprintf(keyStr, "Space"); break;
    case sf::Keyboard::Key::Return: sprintf(keyStr, "Return"); break;
    case sf::Keyboard::Key::Tab: sprintf(keyStr, "Tab"); break;
    case sf::Keyboard::Key::PageUp: sprintf(keyStr, "Page Up"); break;
    case sf::Keyboard::Key::PageDown: sprintf(keyStr, "Page Down"); break;
    case sf::Keyboard::Key::End: sprintf(keyStr, "End"); break;
    case sf::Keyboard::Key::Home: sprintf(keyStr, "Home"); break;
    case sf::Keyboard::Key::Insert: sprintf(keyStr, "Insert"); break;
    case sf::Keyboard::Key::Delete: sprintf(keyStr, "Delete"); break;
    case sf::Keyboard::Key::Add: sprintf(keyStr, "+"); break;
    case sf::Keyboard::Key::Subtract: sprintf(keyStr, "-"); break;
    case sf::Keyboard::Key::Multiply: sprintf(keyStr, "*"); break;
    case sf::Keyboard::Key::Divide: sprintf(keyStr, "/"); break;
    case sf::Keyboard::Key::Left: sprintf(keyStr, "Left"); break;
    case sf::Keyboard::Key::Right: sprintf(keyStr, "Right"); break;
    case sf::Keyboard::Key::Up: sprintf(keyStr, "Up"); break;
    case sf::Keyboard::Key::Down: sprintf(keyStr, "Down"); break;
    case sf::Keyboard::Key::BackSpace: sprintf(keyStr, "BackSpace"); break;
    case sf::Keyboard::Key::Numpad0: sprintf(keyStr, "NP 0"); break;
    case sf::Keyboard::Key::Numpad1: sprintf(keyStr, "NP 1"); break;
    case sf::Keyboard::Key::Numpad2: sprintf(keyStr, "NP 2"); break;
    case sf::Keyboard::Key::Numpad3: sprintf(keyStr, "NP 3"); break;
    case sf::Keyboard::Key::Numpad4: sprintf(keyStr, "NP 4"); break;
    case sf::Keyboard::Key::Numpad5: sprintf(keyStr, "NP 5"); break;
    case sf::Keyboard::Key::Numpad6: sprintf(keyStr, "NP 6"); break;
    case sf::Keyboard::Key::Numpad7: sprintf(keyStr, "NP 7"); break;
    case sf::Keyboard::Key::Numpad8: sprintf(keyStr, "NP 8"); break;
    case sf::Keyboard::Key::Numpad9: sprintf(keyStr, "NP 9"); break;
    case sf::Keyboard::Key::Num1: sprintf(keyStr, "1"); break;
    case sf::Keyboard::Key::Num2: sprintf(keyStr, "2"); break;
    case sf::Keyboard::Key::Num3: sprintf(keyStr, "3"); break;
    case sf::Keyboard::Key::Num4: sprintf(keyStr, "4"); break;
    case sf::Keyboard::Key::Num5: sprintf(keyStr, "5"); break;
    case sf::Keyboard::Key::Num6: sprintf(keyStr, "6"); break;
    case sf::Keyboard::Key::Num7: sprintf(keyStr, "7"); break;
    case sf::Keyboard::Key::Num8: sprintf(keyStr, "8"); break;
    case sf::Keyboard::Key::Num9: sprintf(keyStr, "9"); break;
    case sf::Keyboard::Key::Num0: sprintf(keyStr, "0"); break;
    case sf::Keyboard::Key::F1: sprintf(keyStr, "F1"); break;
    case sf::Keyboard::Key::F2: sprintf(keyStr, "F2"); break;
    case sf::Keyboard::Key::F3: sprintf(keyStr, "F3"); break;
    case sf::Keyboard::Key::F4: sprintf(keyStr, "F4"); break;
    case sf::Keyboard::Key::F5: sprintf(keyStr, "F5"); break;
    case sf::Keyboard::Key::F6: sprintf(keyStr, "F6"); break;
    case sf::Keyboard::Key::F7: sprintf(keyStr, "F7"); break;
    case sf::Keyboard::Key::F8: sprintf(keyStr, "F8"); break;
    case sf::Keyboard::Key::F9: sprintf(keyStr, "F9"); break;
    case sf::Keyboard::Key::F10: sprintf(keyStr, "F10"); break;
    case sf::Keyboard::Key::F11: sprintf(keyStr, "F11"); break;
    case sf::Keyboard::Key::F12: sprintf(keyStr, "F12"); break;
    case sf::Keyboard::Key::F13: sprintf(keyStr, "F13"); break;
    case sf::Keyboard::Key::F14: sprintf(keyStr, "F14"); break;
    case sf::Keyboard::Key::F15: sprintf(keyStr, "F15"); break;
    case sf::Keyboard::Key::Pause: sprintf(keyStr, "Pause"); break;
        
    default:
    if (keycode >= 0 && keycode < 26)
        sprintf(keyStr, "%c", keycode+65);
    }
}

void GameFieldDrawer::addField(obsField& field) {
	fields.push_back(field);
	calFieldPos();
	drawOppField(fields.back());
}

void GameFieldDrawer::removeField(short id) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == id) {
			it = fields.erase(it);
			break;
		}
	calFieldPos();
}

void GameFieldDrawer::removeAllFields() {
	while (fields.size())
		fields.pop_front();
}

void GameFieldDrawer::updateField(obsField& field) {
	for (auto it = fields.begin(); it != fields.end(); it++)
		if (it->id == field.id) {
			for (int y=0; y<22; y++)
				for (int x=0; x<10; x++)
					it->square[y][x] = field.square[y][x];
			it->nextpiece = field.nextpiece;
			it->nprot = field.nprot;
			it->npcol = field.npcol;
			drawOppField(*it);
		}
}

void GameFieldDrawer::calFieldPos() {
	float r = 600/440.0;
	short width = 490, height = 555, startx = 465, starty = 40;
	short total = fields.size(), placed = 0;

	short field_w = width;
	short field_h = r*field_w;
	if (field_h>height) {
		field_h=height;
		field_w=field_h/r;
	}

	bool done = false;
	short x, y, rows;

	while (!done) {
		x=0; y=0; rows=1;
		for (placed = 0; placed < total; placed++) {
			if (x+field_w > width) { x=0; y+=field_h; rows++; }
			if (y+field_h > height)
				break;
			x+=field_w;
		}
		if (placed >= total)
			done=true;
		else {
			field_w--;
			field_h=field_w*r;
		}
	}

	short perrow = (float)total/rows + 0.999999;

	r = field_w/440.0;
	short onrow=0, currow=0;

	short(*origin)[2] = new short[total][2];

	short c;
	for (c=0; c<total; c++) {
		origin[c][0] = startx + (onrow+1)*((width-field_w*perrow) / (perrow+1)) + onrow * field_w;
		origin[c][1] = starty + (currow+1)*((height-field_h*rows) / (rows+1)) + field_h*currow;
		onrow++;
		if (onrow==perrow) {
			onrow=0;
			currow++;
		}
	}

	c=0;
	for (auto &&it : fields) { //start 480, 0
		short originX = (480-origin[c][0]) / (r-1);
		short originY = (origin[c][1] / (r-1))*-1;

		it.sprite.setOrigin(originX, originY);
		it.sprite.setPosition(480+originX, originY);
		it.sprite.setScale(r, r);
		c++;
	}

	delete[] origin;
	origin = nullptr;

	currentR = r;
}

void GameFieldDrawer::resetOppFields() {
	for (auto&& field : fields) {
		field.datacount=250;
		field.nextpiece=game->nextpiece;
		field.posX=0;
		field.posY=0;
		field.clear();
		drawOppField(field);
	}
}

void GameFieldDrawer::drawOppField(obsField& field) {
	field.drawField();

	for (int rot=0; rot < field.nprot; rot++)
		options->basepiece[field.nextpiece].rcw();
	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (options->basepiece[field.nextpiece].grid[y][x] != 0) {
                	field.tile[field.npcol-1].setPosition(sf::Vector2f(-15*options->basepiece[field.nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[field.npcol-1]);
            	}
    for (int rot=0; rot < field.nprot; rot++)
		options->basepiece[field.nextpiece].rccw();
}

void GameFieldDrawer::drawFields() {
	if (!gui.get("GameFields")->isVisible())
		return;
	for (auto &&it : fields)
		window->draw(it.sprite);
	if (scaleup) {
		scaleup->scale += sclock.restart().asMilliseconds() / 5;
		if (scaleup->scale > 100)
			scaleup->scale = 100;
		float maxRat = 0.85, add;
		add = (maxRat-currentR)/100;
		if (add>0) {
			scaleup->sprite.setScale(currentR+add*scaleup->scale, currentR+add*scaleup->scale);
			window->draw(scaleup->sprite);
		}
	}
}

sf::String GameFieldDrawer::getName(sf::Uint16 id) {
	if (id == myId)
		return game->field.name;
	for (auto&& field : fields)
		if (field.id == id)
			return field.name;
	return "Unknown";
}

void GameFieldDrawer::goAway() {
	away=true;
	net->packet.clear();
	sf::Uint8 packetid = 8; //8-Packet
	net->packet << packetid;
	net->sendTCP();
	game->gameover=true;
	game->sendgameover=true;
	game->awayText.setString("Away");
	game->drawGameOver();
}

void GameFieldDrawer::unAway() {
	away=false;
	net->packet.clear();
	sf::Uint8 packetid = 9; //9-Packet
	net->packet << packetid;
	net->sendTCP();
	game->awayText.setString("");
	game->drawGameOver();
}

void GameFieldDrawer::handleEvent(sf::Event event) {
	gui.handleEvent(event);
	if (setkey)
		putKey(event);
	if (gui.get("GameFields")->isVisible()) {
		if (event.type == sf::Event::MouseMoved) {
			sf::Vector2f pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
			sf::FloatRect box;
			if (scaleup) {
				box = scaleup->sprite.getGlobalBounds();
				if (!box.contains(pos)) {
					scaleup->scale=0;
					scaleup->sprite.setScale(currentR, currentR);
					scaleup=0;
				}
			}
			for (auto &&it : fields) {
				box = it.sprite.getGlobalBounds();
				if (box.contains(pos)) {
					if (&it != scaleup) {
						if (scaleup) {
							scaleup->scale=0;
							scaleup->sprite.setScale(currentR, currentR);
						}
						scaleup=&it;
						scaleup->scale=0;
						sclock.restart();
						break;
					}
				}
			}
		}
		else if (event.type == sf::Event::MouseLeft && scaleup) {
			scaleup->scale=0;
			scaleup->sprite.setScale(currentR, currentR);
			scaleup=0;
		}
	}
	if (event.type == sf::Event::KeyPressed)
		if (event.key.code == sf::Keyboard::Escape) {
			if (chatFocused) {
				gui.get("ChatBox", 1)->unfocus();
				gui.get("slChatBox", 1)->unfocus();
			}
			else if (gui.get("Login")->isVisible()) {
				closeLogin();
			}
			else if (gui.get("MainMenu")->isVisible()) {
				if (gui.get("AUS")->isVisible()) {
					gui.get("AUS")->hide();
					gui.get("MainMenu")->enable();
				}
				else {
					gui.get<tgui::Label>("AUSL", 1)->setText("Do you want to quit?");
					gui.get("AUS")->show();
					gui.get("MainMenu")->disable();
				}
			}
			else if (gui.get("OptTab")->isVisible())
				gui.get<tgui::Tab>("OptTab")->select("Back");
			else if (gui.get("opTab")->isVisible())
				gui.get<tgui::Tab>("opTab")->select("Back");
			else if (gui.get("InGameTab")->isVisible()) {
				if (gui.get("AUS")->isVisible())
					gui.get("AUS")->hide();
				else {
					gui.get<tgui::Label>("AUSL", 1)->setText("Leave this game?");
					gui.get("AUS")->show();
				}
			}
		}
	if (gui.get("Rooms")->isVisible())
		if (event.type == sf::Event::MouseWheelScrolled)
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (!gui.get("RoomScroll", 1)->isFocused()) {
					short cur = gui.get<tgui::Scrollbar>("RoomScroll", 1)->getValue();
					cur-=event.mouseWheelScroll.delta;
					if (cur<0)
						cur=0;
					gui.get<tgui::Scrollbar>("RoomScroll", 1)->setValue(cur);
				}
}

void GameFieldDrawer::sendGameData() { //UDP-Packet
	sf::Time tmp = game->keyclock.getElapsedTime();
	if (tmp>gamedata) {
		gamedata=tmp+sf::milliseconds(100);
		compressor.compress();
		net->packet.clear();
		net->packet << myId << gamedatacount;
		gamedatacount++;
		for (int i=0; i<compressor.tmpcount; i++)
			net->packet << compressor.tmp[i];
		if (compressor.bitcount>0)
			net->packet << compressor.tmp[compressor.tmpcount];
		net->sendUDP();
	}

	if (game->linesSent > linesSent) { //5-Packet
		sf::Uint8 packetid=5;
		sf::Uint8 amount = game->linesSent-linesSent;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		linesSent = game->linesSent;
	}

	if (game->garbageCleared > garbageCleared) { //6-Packet
		sf::Uint8 packetid=6;
		sf::Uint8 amount = game->garbageCleared-garbageCleared;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		garbageCleared = game->garbageCleared;
	}

	if (game->linesBlocked > linesBlocked) { //7-Packet
		sf::Uint8 packetid=7;
		sf::Uint8 amount = game->linesBlocked-linesBlocked;
		net->packet.clear();
		net->packet << packetid << amount;
		net->sendTCP();
		linesBlocked = game->linesBlocked;
	}
}

void GameFieldDrawer::sendGameOver() { //3-Packet
	net->packet.clear();
	sf::Uint8 packetid = 3;
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
	game->sendgameover=false;
}

void GameFieldDrawer::sendGameWinner() { //4-Packet
	net->packet.clear();
	sf::Uint8 packetid = 4;
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
	game->winner=false;
}

void GameFieldDrawer::handlePacket() {
	if (net->packetid !=254)
		cout << "Packet id: " << (int)net->packetid << endl;
	switch ((int)net->packetid) {
		case 255: //Disconnect
			gui.get("opTab")->hide();
			gui.get("Rooms")->hide();
			gui.get("ServerLobby")->hide();
			gui.get("AUS")->hide();
			gui.get("Connecting")->hide();
			gui.get("MainMenu")->enable();
			mainMenu();
			disconnect=true;
			playonline=false;
		break;
		case 254: //UDP packet
		{
			sf::Uint16 clientid;
			sf::Uint8 datacount;
			net->packet >> clientid >> datacount;
			for (auto&& field : fields)
				if (field.id==clientid) {
					if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
						field.datacount=datacount;
						for (int c=0; net->packet >> compressor.tmp[c]; c++) {}
						compressor.field = &field;
						compressor.extract();
						drawOppField(field);
					}
					break;
				}
		}
		break;
		case 0: //Welcome packet
		{
			sf::String name;
			sf::Uint8 roomCount, maxPlayers, currentPlayers;
			sf::Uint16 id;

			net->packet >> myId >> name >> roomCount;

			lobbyMsg("Server", name);

			removeAllRooms();

			for (int i=0; i<roomCount; i++) {
				net->packet >> id >> name >> currentPlayers >> maxPlayers;
				addRoom(name, currentPlayers, maxPlayers, id);
			}
		}
		break;
		case 1://Start countdown
		{
			sf::Uint8 countdown;
			sf::Uint16 seed1, seed2;
			net->packet >> countdown >> seed1 >> seed2;
			srand(seed1);
			game->rander.seedHole(seed2);
			game->rander.reset();
			game->startCountdown();
			game->countDown(countdown);
			resetOppFields();
			startcount=true;
		}
		break;
		case 2://Countdown
		{
			sf::Uint8 countdown;
			net->packet >> countdown;
			game->countDown(countdown);
			if (!countdown) {
				startgame=true;
				gamedatacount=0;
				gamedata=sf::seconds(0);
			}
		}
		break;
		case 3://Join room ok/no
		{
			sf::Uint8 joinok;
			net->packet >> joinok;
			if (joinok) {
				sf::Uint8 playersinroom;
				sf::Uint16 playerid;
				net->packet >> playersinroom;
				obsField newfield(textureBase->tile, &textureBase->fieldBackground);
				newfield.clear();
				sf::String name;
				for (int c=0; c<playersinroom; c++) {
					net->packet >> playerid >> name;
					newfield.id = playerid;
					newfield.setName(name, *printFont);
					addField(newfield);
				}
				inroom=true;
				gui.get("InGameTab")->show();
				gui.get<tgui::Tab>("InGameTab")->select(0);
	            gui.get("GameFields")->show();
	            gui.get("opTab")->hide();
	            gui.get("Rooms")->hide();
	            game->makeNewPiece();
				game->field.clear();
				game->countdownText.setString("");
				game->preDraw();
			}
			else {} //Show "Couldn't join room" message
		}
		break;
		case 4: //Another player joined room
		{
			sf::String name;
			obsField newfield(textureBase->tile, &textureBase->fieldBackground);
			newfield.clear();
			net->packet >> newfield.id >> name;
			newfield.setName(name, *printFont);
			addField(newfield);
		}
		break;
		case 5: //Another player left the room
		{
			sf::Uint16 removeid;
			net->packet >> removeid;
			removeField(removeid);
		}
		break;
		case 6: //Round ended
			game->gameover=true;
		break;
		case 7: //Round ended and you where the winner
			game->gameover=true;
			game->winner=true;
		break;
		case 8: // Round score data
		{
			sf::Uint8 count, maxcombo, rank, position;
			sf::Uint16 id, linesSent, linesRecieved, linesBlocked, bpm, spm, score;
			net->packet >> count;
			clearScoreBox();
			for (int i=0; i<count; i++) {
				net->packet >> id >> maxcombo >> linesSent >> linesRecieved >> linesBlocked >> bpm >> spm >> rank >> position >> score;
				printScoreBox(getName(id), score, rank, bpm, maxcombo, linesSent, spm, linesRecieved, linesBlocked);
			}
		}
		break;
		case 9: // Auth result
		{
			sf::Uint8 success;
			net->packet >> success;
			if (success == 1) {
				sf::String name;
				net->packet >> name >> myId;
				game->field.setName(name, *printFont);
				gui.get("Connecting")->hide();
				gui.get("opTab")->show();
				gui.get("Rooms")->show();
				gui.get<tgui::Tab>("opTab")->select(0);
				gui.get("MainMenu")->enable();
			}
			else if (success == 2) {
				gui.get("Connecting")->hide();
				gui.get("opTab")->show();
				gui.get("Rooms")->show();
				gui.get<tgui::Tab>("opTab")->select(0);
				gui.get("MainMenu")->enable();
			}
			else {
				net->disconnect();
				playonline=false;
				quickMsg("Authentication failed");
				gui.get("Connecting")->hide();
				gui.get("MainMenu")->show();
				gui.get("Login")->show();
			}
		}
		break;
		case 10: //Garbage received
		{
			sf::Uint8 amount;
			net->packet >> amount;
			game->addGarbage(amount);
		}
		break;
		case 11: //Server telling me to reset my oppfields
			resetOppFields();
		break;
		case 12:
		{
			sf::String from, msg;
			sf::Uint8 type;
			net->packet >> type >> from >> msg;
			if (type == 3)
				privMsg(from, msg);
			else if (type == 2)
				lobbyMsg(from, msg);
			else
				roomMsg(from, msg);
		}
		break;
	}
}