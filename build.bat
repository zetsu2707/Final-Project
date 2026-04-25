@echo off
set GPP=C:\msys64\mingw64\bin\g++.exe
set SFML_INC=C:\msys64\mingw64\include
set SFML_LIB=C:\msys64\mingw64\lib
set ROOT=%~dp0

"%GPP%" -std=c++20 ^
  -I"%ROOT%include" ^
  -I"%SFML_INC%" ^
  "%ROOT%source\main.cpp" ^
  "%ROOT%source\CasinoGame.cpp" ^
  "%ROOT%source\CardGame.cpp" ^
  "%ROOT%source\Player.cpp" ^
  "%ROOT%source\Blackjack.cpp" ^
  "%ROOT%source\Poker.cpp" ^
  "%ROOT%source\Roulette.cpp" ^
  "%ROOT%source\Slots.cpp" ^
  "%ROOT%source\HorseRace.cpp" ^
  "%ROOT%source\Audio.cpp" ^
  "%ROOT%source\UIsource\mainUI.cpp" ^
  "%ROOT%source\UIsource\casinogameUI.CPP" ^
  "%ROOT%source\UIsource\blackjackUI.cpp" ^
  "%ROOT%source\UIsource\PokerUI.cpp" ^
  "%ROOT%source\UIsource\rouletteUI.cpp" ^
  "%ROOT%source\UIsource\slotsUI.cpp" ^
  "%ROOT%source\UIsource\HorseraceUI.cpp" ^
  -L"%SFML_LIB%" ^
  -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system ^
  -o "%ROOT%CasinoUI.exe"

if %ERRORLEVEL% == 0 (
    echo Build successful: CasinoUI.exe
) else (
    echo Build failed.
)
