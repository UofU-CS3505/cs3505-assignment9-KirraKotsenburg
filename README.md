# Save Sick Grandma Game

## Overview

"Save Sick Grandma" is an educational game designed to teach players about Utah's native plants, with emphasis on distinguishing between medicinal herbs and dangerous/poisonous plants. The player embarks on a journey to collect healing herbs for their sick grandmother while avoiding poisonous plants that could be fatal if consumed. The game combines a simple driving mechanic with plant identification challenges, creating an engaging way to learn about local flora
  
## Educational Purpose

This game aims to:
- Educate players about beneficial medicinal herbs native to Utah
- Help players identify dangerous/poisonous plants to avoid
- Provide information about the properties and uses of various plants
- Create awareness about plant safety in wilderness settings

## Game Features

- **Three Progressive Levels**: Increasing difficulty with more plants to identify
- **Interactive Plant Encyclopedia**: Learn about various Utah plants and their properties
- **Realistic Plant Images**: Visual representations to aid in identification
- **Driving Physics**: Control a vehicle to navigate terrain and collect plants
- **Collection Tracking**: Monitor your progress in collecting the required herbs
- **Plant Popup Information**: Educational details about each plant you encounter

## Gameplay
1. **Main Objective**: Collect all the required medicinal herbs and reach your grandmother's house
2. **Controls**: Use the arrow keys to drive your vehicle left and right
3. **Plant Collection**: When you encounter a plant, choose whether to collect it or leave it
4. **Win Condition**: Successfully collect all required herbs and reach the grandmother's house
5. **Lose Condition**: Collect 3 poisonous plants or didn't collect all required herbs at the end.

## Technical Details
- **Language**: C++
- **Framework**: QT Framework(6.2+)
- **Physics Engine**: Box2D for vehicle and terrain physics

## Architecture
The game follows the Model-View-Controller pattern:
- **Model**: GameManager, PhysicsWorld, Hazard(Plants), Vehicle
- **View**: WorldRenderer, UI elements
- **Controller**: mainWindow, GameContactListener

## Build System
- QMake for build configuration
* Required dependencies:
  - QT 6.2 or higher
  - Box2D physics library

## User Stories
| User Stories  |
| ------------- |
| “As a Player, I want to learn about dangerous plants in Utah.” |
| “As a Player, I want to know what snakes to avoid on a hike.” |
| “As a Player, I want to simulate an exploratory hike.” |
|“As a Player, I want to save my Grandma. |
|“As a Player, I want to learn about medicinal/edible plants in Utah.” |  

## WARNING
This game is educational and should not replace proper training in plant identification.
Never consume wild plants without expert guidance.
The information provided in the game is for educational purposes only.

## Contributors
- Jason Chang - Developer
- Arthur Mo - Developer
- Kirra Kotsenburg - Developer
- DongJoo Lee - Developer

## Acknowledgements
- Utah Native Plant Society for information on local flora.
