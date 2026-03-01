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
 * also predefines the start and end touch events to ensure no dirty data is used
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
    _dir = Direction::None;
    _didReset = false;
    _didDrop = false;
    _didPickUp = false;
    _toggleOverlay = false;

    // Lets use this space to understand what a TouchEvent is and how its used:
    // (there probably is a way to generate them via touch screen but i want to be a bit specific on usage/merge with keyboard
    // 
    // The idea is that we dont really care about how a finger moves, we only care about the start and end position/timing for our
    // actions of (tap, swipe, hold) atm
    // 
    // There are two stored events here, a start event and an end event for when a finger presses on a screen and releases, respectivly
    // each event has 4 parameters
    // position  - Vec2      used to store the touch/release location (start/end)
    // pressure  - int       currently used as a way to store if an event has been processed/ready to be read 0 = no 1 = true
    // touch     - int       used to identify the id of what is touching a screen, namely for multi fingers, but we only care about one atm, see logic below    
    // timestamp - Timestamp a timestamp marked when an input is read (see documentaiton its weird cant read time directly without getTIme())


#ifdef CU_TOUCH_SCREEN // if touch screen
    
    Touchscreen* touch_screen = Input::get<Touchscreen>();
    std::vector<TouchID> touch_ids = touch_screen->touchSet();
    if (!touch_ids.empty()) {
        TouchID focusedID = touch_ids[0]; //only read the first input cause thats all we care about ???? probably need to fix later
        if (touch_screen->touchDown(focusedID)) {
            _start_touch_event.position = touch_screen->touchPosition(focusedID);
            _end_touch_event.position.x *= -1; // reversed for touchscreens NEEDS TO BE DOUBLE CHECKED??
            _start_touch_event.pressure = 1;
            _start_touch_event.touch = focusedID;
            _start_touch_event.timestamp = Timestamp();
        }
        else if (touch_screen->touchReleased(focusedID) && _start_touch_event.pressure) {
            _end_touch_event.position = touch_screen->touchPosition(_start_touch_event.touch); //idk if this is right, probably just replace "_start_touch_event.touch" with focusedID 
            _start_touch_event.position.x *= -1;
            _end_touch_event.pressure = 1;
            _end_touch_event.touch = focusedID;
            _end_touch_event.timestamp = Timestamp();
        }
    }
    
#else

    // Convert keyboard state into game commands
    Keyboard* key_board = Input::get<Keyboard>();
    // This makes it easier to change the keys later
    KeyCode up = KeyCode::ARROW_UP;
    KeyCode down = KeyCode::ARROW_DOWN;
    KeyCode left = KeyCode::ARROW_LEFT;
    KeyCode right = KeyCode::ARROW_RIGHT;
    KeyCode tap = KeyCode::A;
    KeyCode log = KeyCode::L;
    KeyCode reset = KeyCode::R;
    int focusedID = -1; // id for mouse/keyboard is -1 as doesnt matter but we could store more information 

    Mouse* mouse = Input::get<Mouse>();
    if (mouse) { // super similar to touch screen above
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
    
    // tehcnically the input doesnt matter for the keypressed, the info is passed on release
    if (key_board->keyPressed(up) || key_board->keyPressed(down) || key_board->keyPressed(left) || key_board->keyPressed(right) || key_board->keyPressed(tap)) {
        //CULog("A");
        _start_touch_event.position = Vec2(0, 0);
        _start_touch_event.touch = focusedID;
        _start_touch_event.timestamp = Timestamp();
    }
    else if ((key_board->keyReleased(up) || key_board->keyReleased(down) || key_board->keyReleased(left) || key_board->keyReleased(right) || key_board->keyReleased(tap))) {
        //CULog("B");
        _start_touch_event.pressure = 1; // <-- !!BUG!! this start event flag was moved here because for some reason 
                                         // on keyboard only, the queryInputRead() function would return true and run 
                                         // before the second half was pressed, need timing so moved here
        _end_touch_event.pressure = 1;
        _end_touch_event.touch = focusedID;
        _end_touch_event.timestamp = Timestamp();

        Vec2 dir;

        // the difference between start and end tocuh event is what determines the action, tapping is 0 delta
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
    // turn on logging
    if (key_board->keyPressed(log)) {
        _logOn = !_logOn;
    }
    // toggle minigame overlay
    if (key_board->keyPressed(KeyCode::M)) {
        _toggleOverlay = true;
    }

#endif
}
