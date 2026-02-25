//
//  SLCollisionController.cpp
//  demo
//
//  Created by user on 2026/2/22.
//

#include "CollisionController.h"

using namespace cugl;

/**
 * Returns true if there is a player-as collision
 *
 * In addition to checking for the collision, this method also resolves it.
 * That means it applies damage to the ship for EACH asteroid encountered.
 * It does not, however, play the sound. That happens in the main controller
 *
 * Note that this method must take wrap into consideration as well. If the
 * asteroid/ship can be drawn at multiple points on the screen, then it can
 * collide at multiple places as well.
 *
 * @param ship  The players ship
 * @param aset  The asteroid set
 *
 * @return true if there is a ship-asteroid collision
 */
bool CollisionController::resolveCollisions(const std::shared_ptr<Player>& player, ValuableSet& vset) {
    bool collision = false;
    std::cout<<"enter resolveCollisions"<<std::endl;
    for (auto it = vset.current.begin(); it != vset.current.end(); ++it) {
        // Calculate the normal of the (possible) point of collision
        std::shared_ptr<ValuableSet::Valuable> val = *it;
        // Calculate the grid the player is in
        int x_player = static_cast<int>((player->getPosition().x - 30.0f) / 100.0f);
        int y_player = static_cast<int>((player->getPosition().y) / 100.0f);
        // Calculate the grid the valuable is in
        int x_val = static_cast<int>(((*it)->position.x - 30.0f) / 100.0f);
        int y_val = static_cast<int>(((*it)->position.y) / 100.0f);
        // Pick up automatically when in the same grid, should add stealing process later
        if (x_player == x_val && y_player == y_val && !player->isCarrying()) {
            (*it)->setState(ValuableSet::Valuable::CARRIED, player->getPlayerID());
            player->setCarrying(true);
            collision = true;
        }
    }
    return collision;
}
