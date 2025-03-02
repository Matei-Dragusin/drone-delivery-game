# Drone Delivery Game

## Overview

This is a 3D drone delivery game implemented in C++ using a custom graphics framework. The player controls a drone to pick up packages from platforms and deliver them to designated delivery points within a procedurally generated terrain featuring trees, platforms, and varying heights.

## Game Mechanics

### Drone Controls
- **W/S**: Move forward/backward
- **A/D**: Strafe left/right
- **Q/E**: Descend/Ascend
- **Left/Right Arrow Keys**: Rotate drone
- **Right Mouse Button + Mouse Movement**: Alternative rotation control
- **Space**: Toggle wireframe view

### Gameplay
1. Locate and pick up packages (brown boxes) from red platforms
2. Once a package is picked up, follow the green arrow to the blue delivery platform
3. Position the drone above the delivery platform to complete the delivery
4. Repeat the process as new packages and delivery points spawn

## Technical Features

### Procedural Terrain Generation
- The ground is generated using multi-octave noise functions
- Terrain varies in height and texture based on elevation
- Flat zones are strategically placed for platform placement

### Camera System
- Third-person camera that follows the drone
- Minimap with top-down view in the corner of the screen
- Orthographic projection for the minimap

### Physics and Collision Detection
- Collision detection between:
  - Drone and platforms
  - Drone and trees (both trunks and foliage)
  - Package and environment when attached to drone
- Minimum height limit above terrain

### Object System
- Drone with rotating propellers
- Packages that can be picked up and attached to the drone
- Two types of platforms (pickup and delivery)
- Trees with trunks and two-tier conical foliage

### Visual Indicators
- Direction arrow pointing to objectives
- Minimap showing drone position, trees, and platforms
- Color-coded platforms (red for pickup, blue for delivery)

## Technical Implementation

### Core Components
- **Camera**: Implementation of a 3D camera with view matrix calculation
- **Mesh Generation**: Procedural generation of all game objects
- **Shader System**: Custom vertex and fragment shaders for terrain rendering
- **Input Handling**: Mouse and keyboard input processing
- **Collision System**: AABB collision detection for interaction with game objects

### Shaders
- **Vertex Shader**: Implements procedural terrain generation through noise functions
- **Fragment Shader**: Provides color gradients based on terrain height (grass, rock, snow)

### Game Objects
- **Drone**: Main player-controlled object with physics and collision
- **Packages**: Pickup items that attach to the drone
- **Platforms**: Spawn points for packages and delivery locations
- **Trees**: Obstacles with collision for both trunk and foliage

## Architecture

The game is structured around a `SimpleScene` base class with the main game logic in the `Tema2` class:

1. **Initialization**:
   - Create all necessary meshes for game objects
   - Set up cameras (main and minimap)
   - Generate terrain, trees, and platforms
   - Initialize package and delivery system

2. **Game Loop**:
   - Update drone position and rotation based on player input
   - Check collisions between game objects
   - Update package state (attached/detached)
   - Render main scene and minimap
   - Process delivery logic

3. **Input Handling**:
   - Process keyboard and mouse input for drone control
   - Implement collision-aware movement

## Visuals

The game features:
- Colorful low-poly style graphics
- Procedurally colored terrain based on height
- Rotating propellers on the drone
- Color-coded platforms and objects
- Directional indicator for objectives
- Real-time minimap

## Future Improvements

Potential enhancements for the game:
- Score system and timer
- Multiple difficulty levels
- More varied terrain with water and other obstacles
- Weather effects
- Additional drone customization
- Mission objectives and storyline

## Dependencies

The game uses:
- GLM for vector and matrix operations
- OpenGL for rendering
- Custom shader system for terrain and objects
- Vector-based physics for movement and collisions