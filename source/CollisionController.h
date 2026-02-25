//
//  SLCollisionController.h
//  ship
//
//  Created by user on 2026/2/22.
//


//
//  GLInputController.h
//  demo
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

#ifndef __COLLISION_CONTROLLER_H__
#define __COLLISION_CONTROLLER_H__
#include <cugl/cugl.h>
#include "ObjectModel.h"
#include "Player.h"
#include "ValuableSet.h"

/**
 * Device-independent input manager.
 *
 * This class currently only supports the keyboard for control.  As the
 * semester progresses, you will learn how to write an input controller
 * that functions differently on mobile than it does on the desktop.
 */
class CollisionController {
    private:
        /** The window size (to support wrap-around collisions) */
        cugl::Size _size;
        
    public:
        /**
         * Creates a new collision controller.
         *
         * You will notice this constructor does nothing.  That is because the
         * object is constructed the instance the game starts (main.cpp immediately
         * constructs ShipApp, which immediately constructs GameScene, which then
         * immediately constructs this class), before we know the window size.
         */
        CollisionController() {}
        
        /**
         * Deletes the collision controller.
         *
         * Not much to do here since there was no dynamic allocation.
         */
        ~CollisionController() {}
        
        /**
         * Initializes the collision controller with the given size.
         *
         * This initializer is where we can finally set the window size. This size
         * is used to manage screen wrap for collisions.
         *
         * The pattern we use in this class is that all initializers should return
         * a bool indicating whether initialization was successful (even for
         * initializers that always return true).
         *
         * @param size  The window size
         *
         * @return true if initialization was successful
         */
        bool init(cugl::Size size) {
            _size = size; 
            return true;
        }
        bool resolveCollisions(const std::shared_ptr<Player>& player, ValuableSet& vst);
};
#endif /*__COLLISION_CONTROLLER_H__*/
