// Description: Main driver file for the casino game UI application.
// Related Files: MainUI.h, MainUI.cpp
// Date Created: 3/29/2026
// Last Edited: 4/11/2026

#include <SFML/Graphics.hpp>
#include "UIheaders/MainUI.h"
#include "Audio.h"
#include <iostream>
int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Congo Clash Casino");
    window.setFramerateLimit(144);
    AudioManager audio;

    // Sound effects
    audio.loadSound("button", "assets/sound/effects/button.wav");
    audio.loadSound("horseracestart", "assets/sound/effects/horseracestart.wav");
	audio.loadSound("win", "assets/sound/effects/win.wav");
    //audio.loadSound("card", "assets/card_deal.wav");

	// Background music
    audio.playMusicFadeIn("assets/sound/background/main_track.wav", 3.0f, true, 15.f);


    MainUI ui(audio);
    ui.run(window);

    return 0;
}