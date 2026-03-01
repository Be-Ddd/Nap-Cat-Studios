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
    _didPress(false),
    _logOn(false){
    _start_touch_event = TouchEvent();
    _start_touch_event.pressure = 0;
    _end_touch_event = TouchEvent();
    _end_touch_event.pressure = 0;
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
    // Convert keyboard state into game commands
    _dir = Direction::None;
    _didReset = false;
    _didDrop = false;
    _didPickUp = false;
    _toggleOverlay = false  ;


    // Movement forward/backward
    Keyboard* key_board = Input::get<Keyboard>();
#ifdef CU_TOUCH_SCREEN
    
    Touchscreen* touch_screen = Input::get<Touchscreen>();
    std::vector<TouchID> touch_ids = touch_screen->touchSet();
    if (!touch_ids.empty()) {
        TouchID focusedID = touch_ids[0];
        if (touch_screen->touchDown(focusedID)) {
            _start_touch_event.position = touch_screen->touchPosition(focusedID);
            _end_touch_event.position.x *= -1;
            _start_touch_event.pressure = 1;
            _start_touch_event.touch = focusedID;
            _start_touch_event.timestamp = Timestamp();
        }
        else if (touch_screen->touchReleased(focusedID) && _start_touch_event.pressure) {
            _end_touch_event.position = touch_screen->touchPosition(_start_touch_event.touch);
            _start_touch_event.position.x *= -1;
            _end_touch_event.pressure = 1;
            _end_touch_event.touch = focusedID;
            _end_touch_event.timestamp = Timestamp();
        }
    }
    
#else

    // This makes it easier to change the keys later
    KeyCode up = KeyCode::ARROW_UP;
    KeyCode down = KeyCode::ARROW_DOWN;
    KeyCode left = KeyCode::ARROW_LEFT;
    KeyCode right = KeyCode::ARROW_RIGHT;
    KeyCode tap = KeyCode::A;
    KeyCode log = KeyCode::L;
    KeyCode reset = KeyCode::R;
    int focusedID = -1;

    Mouse* mouse = Input::get<Mouse>();
    if (mouse) {
        if (mouse->buttonDown().hasLeft()) {
            _start_touch_event.position = mouse->pointerPosition();
            _start_touch_event.position.x *= -1;
            _start_touch_event.pressure = 1;
            _start_touch_event.touch = focusedID;
            _start_touch_event.timestamp = Timestamp();
        }
        else if (mouse->buttonUp().hasLeft() && _start_touch_event.pressure) {
            _end_touch_event.position = mouse->pointerPosition();
            _end_touch_event.position.x *= -1;
            _end_touch_event.pressure = 1;
            _end_touch_event.touch = focusedID;
            _end_touch_event.timestamp = Timestamp();
        }
    }
    if (key_board->keyPressed(up) || key_board->keyPressed(down) || key_board->keyPressed(left) || key_board->keyPressed(right) || key_board->keyPressed(tap)) {
        CULog("A");
        _start_touch_event.position = Vec2(0, 0);
        _start_touch_event.touch = focusedID;
        _start_touch_event.timestamp = Timestamp();
    }
    else if ((key_board->keyReleased(up) || key_board->keyReleased(down) || key_board->keyReleased(left) || key_board->keyReleased(right) || key_board->keyReleased(tap))) {
        CULog("B");
        _start_touch_event.pressure = 1;
        _end_touch_event.pressure = 1;
        _end_touch_event.touch = focusedID;
        _end_touch_event.timestamp = Timestamp();

        Vec2 dir;

        if (key_board->keyReleased(up)) {
            dir = Vec2(0, -100);
        }
        else if (key_board->keyReleased(down)) {
            dir = Vec2(0, 100);
        }
        else if (key_board->keyReleased(left)) {
            dir = Vec2(100, 0);
        }
        else if (key_board->keyReleased(right)) {
            dir = Vec2(-100, 0);
        }
        else if (key_board->keyReleased(tap)) {
            dir = Vec2(0, 0);
        }

        _end_touch_event.position = dir;
    }
    // Reset the game
    if (key_board->keyDown(reset)) {
        _didReset = true;
    }
    if (key_board->keyPressed(log)) {
        _logOn = !_logOn;
    }
    if (key_board->keyPressed(KeyCode::M)) {
        _toggleOverlay = true;
    }

#endif
}
