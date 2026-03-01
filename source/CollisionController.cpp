//
//  SLCollisionController.cpp
//  demo
//
//  Created by user on 2026/2/22.
//

#include "CollisionController.h"
#include "TileModel.h"

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
    for (size_t i = 0; i < vset.current.size(); ++i) {
        std::shared_ptr<ValuableSet::Valuable> val = vset.current[i];
        // Calculate the normal of the (possible) point of collision
        // Calculate the grid the player is in
        int x_player = static_cast<int>((player->getPosition().x - 30.0f) / 100.0f);
        int y_player = static_cast<int>((player->getPosition().y) / 100.0f);
        // Calculate the grid the valuable is in
        int x_val = static_cast<int>((val->position.x - 30.0f) / 100.0f);
        int y_val = static_cast<int>((val->position.y) / 100.0f);
        // Pick up automatically when in the same grid, should add stealing process later
        if (x_player == x_val && y_player == y_val && !player->isCarrying()) {
            CULog("picking up");
            val->setState(ValuableSet::Valuable::CARRIED, player->getPlayerID());
            player->setCarrying(true, i);
            collision = true;
        }
    }
    return collision;
}
/**
* NEED TO FIX LATER
* 
* right now it is a copy paste of the above code, just specificially for returning T/F on specifically player pickup
* should be reintegrated and conencted to above
*/
bool CollisionController::hackyAttemptToPickUP(const std::shared_ptr<Player>& player, ValuableSet& vset, const std::shared_ptr<TileModel>& tiles) {
    bool collision = false;
    for (size_t i = 0; i < vset.current.size(); ++i) {
        std::shared_ptr<ValuableSet::Valuable> val = vset.current[i];
        // Calculate the normal of the (possible) point of collision
        // Calculate the grid the player is in
        // int x_player = static_cast<int>((player->getPosition().x - 30.0f) / 100.0f);
        // int y_player = static_cast<int>((player->getPosition().y) / 100.0f);
        auto [r_player, c_player] = tiles->worldToGrid(cugl::Vec2(player->getPosition().x, player->getPosition().y));
        CULog ("x_val of player %d y_val of player %d", r_player, c_player);
        // Calculate the grid the valuable is in
//        int x_val = static_cast<int>((val->position.x - 30.0f) / 100.0f);
//        int y_val = static_cast<int>((val->position.y) / 100.0f);
        auto [r_val, c_val] = tiles->worldToGrid(cugl::Vec2(val->position.x, val->position.y));
        CULog ("x_val of valuable %d y_val of valuable %d", r_val, c_val);
        // Pick up automatically when in the same grid, should add stealing process later
        if (r_player == r_val && c_player == c_val && !player->isCarrying()) {
            CULog("picking up");
            val->setState(ValuableSet::Valuable::CARRIED, player->getPlayerID());
            player->setCarrying(true, i);
            collision = true;
        }
    }
    return collision;
}
