//
//  GLInputController.h
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
#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__
#include "Direction.h"
using namespace cugl;

/**
 * Device-independent input manager.
 *
 * This class currently only supports the keyboard for control.  As the
 * semester progresses, you will learn how to write an input controller
 * that functions differently on mobile than it does on the desktop.
 */
class InputController {
private:
    
    Direction _dir;

    TouchEvent _start_touch_event;
    TouchEvent _end_touch_event;
    
    bool _didPress;
    

    /** Did we press the reset button? */
    bool _didReset;
    bool _didDrop;
    bool _didPickUp;
    bool _pressed = false;
    bool _logOn = false;
    bool _toggleOverlay = false;

    

public:
    
    Direction getDirection() const{
        return _dir;
    }
    bool didPress() const{
        return _didPress;
    }
    bool didDrop() const {
        return _didDrop;
    }
    bool didToggleOverlay() const {
        return _toggleOverlay;
    }

    bool queryInputReady() {
        return _start_touch_event.pressure == 1 && _end_touch_event.pressure == 1;
    }

    bool queryStartEventReady(){
        return _start_touch_event.pressure;
    }
    bool queryEndEventReady() {
        return _end_touch_event.pressure;
    }
    TouchEvent peekStartEvent() {
        return _start_touch_event;
    }
    TouchEvent peekEndEvent() {
        return _end_touch_event;
    }
    std::pair<TouchEvent, TouchEvent> peekCompletedEvent () {
        return std::pair(_start_touch_event, _end_touch_event);
    }
    void clearTouchEvents() {
        _start_touch_event = TouchEvent();
        _start_touch_event.pressure = 0;
        _end_touch_event = TouchEvent();
        _end_touch_event.pressure = 0;
    }

    bool didPickUp() const {
        return _didPickUp;
    }
    /**
     * Returns whether the reset button was pressed.
     *
     * @return whether the reset button was pressed.
     */
    bool didPressReset() const {
        return _didReset;
    }
    
    bool isLogOn() const{
        return _logOn;
    }

    /**
     * Creates a new input controller with the default settings
     *
     * This is a very simple class.  It only has the default settings and never
     * needs to attach any custom listeners at initialization.  Therefore, we
     * do not need an init method.  This constructor is sufficient.
     */
    InputController();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~InputController() {}
    
    /**
     * Reads the input for this player and converts the result into game logic.
     *
     * This is an example of polling input.  Instead of registering a listener,
     * we ask the controller about its current state.  When the game is running,
     * it is typically best to poll input instead of using listeners.  Listeners
     * are more appropriate for menus and buttons (like the loading screen).
     */
    void readInput();
};

#endif /*__INPUT_CONTROLLER_H__*/
