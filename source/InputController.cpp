//
//  GLInputController.cpp
//  Ship Lab
//
//  This class buffers in input from the devices and converts it into its
//  semantic meaning. If your game had an option that allows the player to
//  remap the control keys, you would store this information in this class.
//  That way, the main game scene does not have to keep track of the current
//  key mapping.
//
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//
//  Author: Walker M. White
//  Version: 1/20/25
//
#include <cugl/cugl.h>
#include "InputController.h"

using namespace cugl;

/**
 * Creates a new input controller with the default settings
 *
 * This is a very simple class.  It only has the default settings and never
 * needs to attach any custom listeners at initialization.  Therefore, we
 * do not need an init method.  This constructor is sufficient.
 */
InputController::InputController() :
_dir(Direction::None),
_didPress(false){
}



/**
 * Reads the input for this player and converts the result into game logic.
 *
 * This is an example of polling input.  Instead of registering a listener,
 * we ask the controller about its current state.  When the game is running,
 * it is typically best to poll input instead of using listeners.  Listeners
 * are more appropriate for menus and buttons (like the loading screen).
 */
void InputController::readInput() {
    // This makes it easier to change the keys later
    KeyCode up    = KeyCode::ARROW_UP;
    KeyCode down  = KeyCode::ARROW_DOWN;
    KeyCode left  = KeyCode::ARROW_LEFT;
    KeyCode right = KeyCode::ARROW_RIGHT;
    KeyCode shoot = KeyCode::SPACE;
    KeyCode reset = KeyCode::R;

    // Convert keyboard state into game commands
    _dir = Direction::None;
    _didReset = false;
    
    // Movement forward/backward
    Keyboard* keys = Input::get<Keyboard>();
    if (keys->keyPressed(up)) {
        _dir = Direction::Up;
    } else if (keys->keyPressed(down)) {
        _dir = Direction::Down;
    }
    // Movement left/right
    else if (keys->keyPressed(left)) {
        _dir =Direction::Left;
    } else if (keys->keyPressed(right)) {
        _dir = Direction::Right;
    }
    
    // Reset the game
    if (keys->keyDown(reset)) {
        _didReset = true;
    }
}
