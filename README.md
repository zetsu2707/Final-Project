# Congo Cash Casino
A assortment of five short games structured similarly to a casino hall. Beginning funds are fixed: exactly two hundred dollars, end goal is five thousand dollars. 
---
## Team Members
- Gabriel Carmona
- Leonidas Arellano
- Yoltic Alvarez
---

## Features
- Start Menu / Game Selection
- Load / Save Functionality
- User Interface and Game Flow
- Starting Narrative
- Replayability
- Blackjack
- Poker
- Roulette
- Slot Machine
- Horse Racing

---
## OOP Concepts Used
### Encapsulation
- Each game will store its data as private members and only allow controlled access through public methods. Preventing players or other parts of the program from directly changing game values incorrectly Examples: Blackjack: cards, player score, dealer score Poker: player hands, pot amount Roulette: bet amount, selected number/color Slot Machine: reel values, balance
### Inheritance
- Using a common parent class, and creating new classes that will inherit from it. Allowing common behavior features like "end game" or "start" while the games have their own definitions. Common base classes include shared data between games and base game functionality. 
### Polymorphism
- We will use virtual functions in the base Game class, allowing us to treat all games the same way as they behave differently. 
### Composition
- A class may hold a reference or instance of another, forming a “has-a” relationship between them. This represents composition rather than inheritance, where one object relies on another to perform tasks. Instead of deriving behavior, it includes another object within its structure, gaining functionality through use rather than hierarchy.
---
## Team Contributions
### Member 1: [Gabriel Carmona]
- Start Menu / Game Selection
- User Interface and Game Flow
- Starting Narrative
- Horse Racing
- Replayability
### Member 2: [Leonidas Arellano]
- Blackjack
- Roulette
- Replayability
### Member 3: [Yoltic Alvarez]
- Load / Save Functionality
- Poker
- Slot Machine(s)
- Replayability
---

<!--
### AI Tools
No AI Tools Used Yet
List any AI tools used (e.g., ChatGPT, Copilot, etc.) and describe:
- What you asked the AI to do
- What code or explanation it generated
- What you modified or learned from it
-->

### External Resources
<!-- List all non-AI resources used: -->
- https://app.diagrams.net/ 
- https://www.w3schools.com/cpp/cpp_oop.asp
- Documentation
- YouTube videos
- https://www.youtube.com/watch?v=lftcRWAIycg&list=PLz6j8tWRKzOHQPOL5gGY4Ev6DoMRB2gee https://www.youtube.com/watch?v=qvg8BXXWpCE
- Sample code

### Collaboration Policy Statement
All submitted work reflects our team’s understanding. Any external code
has been:
- Reviewed
- Modified as needed
- Integrated by our team
---
## Additional Notes (Optional)
- Known issues or limitations
- Future improvements
