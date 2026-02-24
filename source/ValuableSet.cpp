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
        _scale = 0.14;
        break;
    case 1:
        _scale = 0.11;
        break;
    default:
        _scale = 0.0f;
        break;
    }
}

void ValuableSet::Valuable::setState(Status state, int id) {
    _state = state;
    carrierID = id;
}

/**
 * Updates the valuable one animation frame
 *
 * This method performs no
 * collision detection. Collisions are resolved afterwards.
 */
void ValuableSet::Valuable::update(Size size, cugl::Vec2 pos) {
    // position += velocity;
    position = pos;
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
                type = entry->get(1)->get(0)->asInt(0);
                spawnValuable(pos, type);
            }
        }

        return true;
    }
    return false;
}

/**
 * Sets the image for a single valuable; reused by all valuables.
 *
 * This value should be loaded by the GameScene and set there. However,
 * we have to be prepared for this to be null at all times.
 *
 * @param value the image for a single photon; reused by all photons.
 */
void ValuableSet::setTexture(const std::shared_ptr<Texture>& value) {
    if (value > 0) {
        Size size = value->getSize();

        _radius = std::max(size.width, size.height) / 2;
        _width = size.width / 2.0f;
        _height = size.height / 2.0f;
        _texture = value;
    }
    else {
        _radius = 0;
        _texture = nullptr;
    }
}

/**
 * Adds a valuable to the active queue.
 *
 * @param p     The valuable position.
 * @param type  The valuable type.
 */
void ValuableSet::spawnValuable(Vec2 p, int t) {
    std::shared_ptr<Valuable> rock = std::make_shared<Valuable>(p, t);
    rock->setState(Valuable::FREE, -1);
    current.emplace(rock);
}

/**
 * Updates all the valuables in the active set.
 *
 * This method performs no collision detection. Collisions
 * are resolved afterwards.
 */
void ValuableSet::update(Size size, std::vector<cugl::Vec2> pos) {
    // Move asteroids, updating the animation frame
    for (auto it = current.begin(); it != current.end(); ++it) {
        if ((*it)->getCarrier() != -1 && (*it)->position != pos[(*it)->getCarrier()]) {
            (*it)->update(size, pos[(*it)->getCarrier()]);
        }
        (*it)->update(size, (*it)->position);
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
    if (_texture) {
        for (auto it = current.begin(); it != current.end(); ++it) {
            float scale = (*it)->getScale();
            Vec2 pos = (*it)->position;
            // Vec2 origin(_radius, _radius);
            Vec2 origin(_width, _height);

            Affine2 trans;
            trans.scale(scale);
            trans.translate(pos);

            float r = _radius * scale;
            batch->draw(_texture, origin, trans);
        }
    }
}
