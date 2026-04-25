# Congo Cash Casino
A assortment of five short games structured similarly to a casino hall. Beginning funds are fixed: exactly two hundred dollars, end goal is five thousand dollars. 
---
## Team Members
- Gabriel Carmona
- Leonidas Arellano
- Yoltic Alvarez
---

## Features
- Main menu with game selection screen
- Five playable casino games: Blackjack, Poker, Roulette, Slot Machine, Horse Racing
- Player balance system with a fixed starting amount ($200) and win goal ($5,000)
- Save / load functionality with 10 save slots
- Player stats panel
- Win/loss end screen
- Full audio: per-game background music tracks and sound effects
- Animated roulette wheel spin

---
## OOP Concepts Used
### Encapsulation
- Each game will store its data as private members and only allow controlled access through public methods. Preventing players or other parts of the program from directly changing game values incorrectly Examples: Blackjack: cards, player score, dealer score Poker: player hands, pot amount Roulette: bet amount, selected number/color Slot Machine: reel values, balance
### Inheritance
- Using a common parent class, and creating new classes that will inherit from it. Allowing common behavior features like "end game" or "start" while the games have their own definitions. `CasinoGame` is the abstract base class for all five games, providing a shared RNG (`m_rng`) and requiring each derived class to implement `getName()`. `CardGame` sits between `CasinoGame` and the card based games like `Blackjack` and `Poker` both inherit from it.
### Polymorphism
- We will use virtual functions in the base Game class, allowing us to treat all games the same way as they behave differently. Ex. `CasinoGame` declares `getName()` as a pure virtual function and `didPlayerWin()` as a virtual with a default.
### Composition
- A class may hold a reference or instance of another, forming a “has-a” relationship between them. This represents composition rather than inheritance, where one object relies on another to perform tasks. Instead of deriving behavior, it includes another object within its structure, gaining functionality through use rather than hierarchy.
---
## Team Contributions
### Member 1: [Gabriel Carmona]
- Main menu and game selection screen 
- UI layout and game flow
- Horse Racing minigame and UI
- UI for poker
- Replayability 
### Member 2: [Leonidas Arellano]
- Blackjack code and UI
- Poker UI
- Roulette UI
- Win/lose screen logic
### Member 3: [Yoltic Alvarez]
- Load / Save Functionality
- Poker logic
- Slot Machine logic and UI
- Statistics section
- Asset Creation
---

## How to Run
A prebuilt CasinoUI.exe and all required DLLs are included in the repository, just double-click Casino.exe to play. No installation needed.

## Build From Source

### Requirements
- MSYS2 with the MinGW-w64 toolchain
- SFML 3.0.0+

### Steps
1. Clone the repository:
   ```
   git clone https://github.com/zetsu2707/Final-Project.git
   ```
2. Edit any code as wanted
3. Run `build.bat` 
4. Run `CasinoUI.exe`

### Notes
- The `assets/` folder must remain in the same directory as the executable
- If audio does not play, verify OpenAL is installed on your system
- Save data is stored in the `data/` folder — do not delete it between sessions

---

### AI Tools
**Claude (Anthropic)**
Used as a debugging aid throughout development and as a co-author on select UI components.
- *Debugging:* Assisted with logic errors and integration issues across multiple source files throughout the project.
- *Roulette wheel spin animation* (`source/UIsource/routtleui.cpp`): The spin animation system was co-written with AI assistance, including the state variables `m_wheelAngle`, `m_spinStartAngle`, `m_spinTargetAngle`, `m_spinDurationSeconds`, and `m_spinClock` that drive the easing, deceleration, and landing on the winning number.
- *Slots UI* (`source/UIsource/SlotsUI.cpp`): The slots UI layout and interaction flow were co-written with AI assistance. 

**ChatGPT / OpenAI (image generation model)**
Used to generate several visual assets for non-commercial/educational purposes.
 
| Asset | Description |
|-------|-------------|
| `assets/roulette/roulette_wheel.png` | Roulette wheel graphic |
| `assets/roulette/roulette_table.png` | Roulette table layout |
| `assets/horse_racing/horses/` | Horse racer sprites |
| `assets/game_selection_background.jpg` | Game selection screen background |
| `assets/menu_background.jpg` | Main menu background |
| `assets/player_name_panel.png` | Player name input panel |
 
All assets were reviewed and integrated by our team.

### External Resources
<!-- List all non-AI resources used: -->
**General**
- [app.diagrams.net](https://app.diagrams.net/) — UML class diagram
- [w3schools C++ OOP](https://www.w3schools.com/cpp/cpp_oop.asp) — OOP concept reference
**SFML**
- [YouTube — SFML Tutorial Series](https://www.youtube.com/watch?v=lftcRWAIycg&list=PLz6j8tWRKzOHQPOL5gGY4Ev6DoMRB2gee)
- [YouTube — SFML Reference](https://www.youtube.com/watch?v=qvg8BXXWpCE)
- [YouTube — SFML Playlist](https://www.youtube.com/playlist?list=PL21OsoBLPpMOO6zyVlxZ4S4hwkY_SLRW9)

**Game-Specific Guides**
- Roulette: [YouTube](https://www.youtube.com/watch?v=wqvcf85Tujs&t=5s)
- Blackjack: [YouTube (1)](https://www.youtube.com/watch?v=TUXanmeigqE&t=189s), [YouTube (2)](https://www.youtube.com/watch?v=h4QlJ0yFRc4)
- Slots: [YouTube Playlist](https://www.youtube.com/playlist?list=PLEP4m_VmrAXd3QnQfQklU3CYhgIGHopct)
- Poker: [YouTube (1)](https://www.youtube.com/watch?v=f5vvgHX9coI), [YouTube (2)](https://www.youtube.com/watch?v=lpZbvsqHGbQ)
- Horse Race: [YouTube](https://www.youtube.com/watch?v=ND9UBKUTdRg)

**Assets**
- Deck of Cards: [OpenGameArt](https://opengameart.org/content/playing-cards-vector-png)
- Blackjack audio: [Pixabay](https://pixabay.com/music/search/blackjack/)
- Main menu music: [YouTube](https://www.youtube.com/watch?v=DCTggKrpMWs)
- Win sound effect: [YouTube](https://www.youtube.com/watch?v=EuLJv2qONRY)
- Horse race music: [YouTube](https://www.youtube.com/watch?v=8JCGpcFHuzU)
- Blackjack table image: [KindPNG](https://www.kindpng.com/imgv/iRimRoh_blackjack-table-png-black-jack-table-soft-16/)
- Poker background: [Vecteezy](https://www.vecteezy.com/vector-art/3520134-dark-poker-background-of-spades-and-clubs)
- Roulette background photo: [UConn Today](https://today.uconn.edu/wp-content/uploads/2025/08/Casino-Interior-Shot-I-6.17.2025--1024x576.png)
- All other sound effects: Non-copyrighted audio sourced from [Pixabay Sound Effects](https://pixabay.com/sound-effects/)

### Collaboration Policy Statement
All submitted work reflects our team’s understanding. Any external code
has been:
- Reviewed
- Modified as needed
- Integrated by our team
---

### UML
![UML Class Diagram](assets/UML%20Class%20Diagram.png)

## Additional Notes (Optional)
- Known issues or limitations
- Future improvements
