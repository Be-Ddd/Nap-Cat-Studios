#include "ValuableSet.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;

#pragma mark Valuable
/**
 * Allocates a valuable by setting its position and velocity.
 *
 * @param p     The position
 * @param v     The velocity
 */
ValuableSet::Valuable::Valuable(const cugl::Vec2 p, int type) {
    position = p;
    setType(type);
}

/**
 * Allocates an asteroid by setting its position, type, and texture
 *
 * @param p     The position
 * @param v     The velocity
 * @param type  The type (1, 2, or 3)
 */
ValuableSet::Valuable::Valuable(const cugl::Vec2 p, int type, std::shared_ptr<cugl::graphics::Texture> texture) {
    position = p;
    setType(type);
    setTexture(texture);
}

/**
 * Returns the type of this valuable.
 *
 * All valuables have types 1 or 2.  2 is the larger type of
 * valuable (statues, paintings) (scale 1.25), while 1 is the smaller (vase, jewelry) (scale of 0.5).
 *
 * @param type  The type of this valuable.
 */
void ValuableSet::Valuable::setType(int type) {
    CUAssertLog(type > 0 && type <= 2, "type must be 1 or 2");
    _type = type;
    switch (type) {
    case 2:
        _scale = 1.25;
        break;
    case 1:
        _scale = .5;
        break;
    default:
        _scale = 0.0f;
        break;
    }
}

/**
 * Updates the valuable one animation frame
 *
 * This method performs no
 * collision detection. Collisions are resolved afterwards.
 */
void ValuableSet::Valuable::update(Size size) {
    // position += velocity;
}

#pragma mark Valuable Set
/**
 * Creates a valuable set with the default values.
 *
 * To properly initialize the valuable set, you should call the init
 * method with the JSON value. We cannot do that in this constructor
 * because the JSON value does not exist at the time the constructor
 * is called (because we do not create this object dynamically).
 */
ValuableSet::ValuableSet() :
    _radius(0) {
}

/**
 * Initializes valuable data with the given JSON
 *
 * This JSON contains all shared information. 
 * It also contains a list of valuables to
 * spawn initially.
 *
 * If this method is called a second time, it will reset all
 * valuable data.
 *
 * @param data  The data defining the valuable settings
 *
 * @return true if initialization was successful
 */
bool ValuableSet::init(std::shared_ptr<cugl::JsonValue> data) {
    if (data) {
        // Reset all data
        current.clear();

        // This is an iterator over all of the elements of rocks
        if (data->get("start")) {
            auto rocks = data->get("start")->children();
            for (auto it = rocks.begin(); it != rocks.end(); ++it) {
                std::shared_ptr<JsonValue> entry = (*it);
                Vec2 pos;
                pos.x = entry->get(0)->get(0)->asFloat(0);
                pos.y = entry->get(0)->get(1)->asFloat(0);
                int type;
                type = entry->get(1)->asInt(0);
                spawnValuable(pos, type);
            }
        }

        return true;
    }
    return false;
}

/**
 * Adds a valuable to the active queue.
 *
 * @param p     The valuable position.
 * @param type  The valuable type.
 */
void ValuableSet::spawnValuable(Vec2 p, int t) {
    std::shared_ptr<Valuable> rock = std::make_shared<Valuable>(p, t);
    
    current.emplace(rock);
}

/**
 * Updates all the valuables in the active set.
 *
 * This method performs no collision detection. Collisions
 * are resolved afterwards.
 */
void ValuableSet::update(Size size) {
    // Move asteroids, updating the animation frame
    for (auto it = current.begin(); it != current.end(); ++it) {
        (*it)->update(size);
    }
}

/**
 * Draws all active valuables to the sprite batch within the given bounds.
 *
 * Stored valuables are not drawn.
 *
 * @param batch     The sprite batch to draw to
 * @param size      The size of the window (for wrap around)
 */
void ValuableSet::draw(const std::shared_ptr<SpriteBatch>& batch, Size size) {
    for (auto it = current.begin(); it != current.end(); ++it) {
        if ((*it)->getTexture()) {
            float scale = (*it)->getScale();
            Vec2 pos = (*it)->position;
            Vec2 origin(_radius, _radius);

            Affine2 trans;
            trans.scale(scale);
            trans.translate(pos);

            float r = _radius * scale;
            batch->draw((*it)->getTexture(), origin, trans);
        }
    }
}