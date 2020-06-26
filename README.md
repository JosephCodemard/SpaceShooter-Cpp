# Space Shooter #

## This is a simple 2D, console based space shooter made in c++ ##
### NOTE: This is only windows compatible ###


## ConsoleGameEngine ##

This code manages the console using windows.h

It has functions
* OnUserCreate() - called at the start of the program;
* OnUserUpdate() - called every cycle;
* OnUserDestroy() - called to clean up at the end

It also has lots of utility functions that handle drawing.
The gameLoop is on a separate thread.

It uses WriteConsoleOutput() to write the buffer (an array of every character in the game) to the screen

## SpaceShooter ##

This code is where the game takes place

Every 'game object' has a class derived from the class 'entity'
ALl logic is implemented in the OnUserupdate() loop (called every frame)

![Space Shooter Image](https://github.com/JosephCodemard/SpaceShooter-Cpp/blob/master/Images/SpaceShooter.png?raw=true)

### Licensed under MIT ###