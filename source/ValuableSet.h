//
//  ValuableSet.h
//
//  This class implements a collection of valuables.
//
#ifndef __VALUABLE_SET_H__
#define __VALUABLE_SET_H__
#include <cugl/cugl.h>
#include <unordered_set>

/**
 * Model class representing a collection of valuables.
 * 
 */
class ValuableSet {
#pragma mark Valuable
public:
    /**
     * An inner class that represents a single Valuable.
     *
     * Valuables come in two different sizes, represented by their types.
     */
    class Valuable {
        // It is okay for the user to access these directly
    public:
        /** Valuable position */
        cugl::Vec2 position;

        enum Status {
            /** The valuable is at the original position or dropped on floor */
            FREE,
            /** The valuable is carried by a player */
            CARRIED,
            /** The valuable is stored */
            STORED,
        };
        // But these need to be protected for invariant reasons
    private:
        /** The type of the valuable: 1 or 2 */
        int _type;
        /** The drawing scale of the valuable (to vary the size) */
        float _scale;
        /** Whether the valuable is picked up by a player */
        Status _state;
        /** The texture for the valuable sprite sheet. */
        std::shared_ptr<cugl::graphics::Texture> _texture;

    public:
        /**
         * Allocates an valuable by setting its position and type.
         *
         *
         * @param p     The position
         * @param type  The type (1 or 2)
         */
        Valuable(const cugl::Vec2 p, int type);

        /**
         * Allocates an valuable by setting its position, type and texture.
         *
         *
         * @param p     The position
         * @param type  The type (1 or 2)
         * @param texture The texture
         */
        Valuable(const cugl::Vec2 p, int type, std::shared_ptr<cugl::graphics::Texture> texture);

        /**
         * Returns the scale of this valuable.
         *
         * Multiply this scale by the standard valuable radius
         * ({@link ValuableSet#getRadius}) to get the "true" radius
         * of a valuable.
         *
         * @return the scale of this valuable.
         */
        float getScale() const { return _scale; }

        /**
         * Returns the type of this valuable.
         *
         * All valuables have types 1 or 2.  2 is the larger type of
         * valuable (statues, paintings) (scale 1.25), while 1 is the smaller (vase, jewelry) (scale of 0.5).
         *
         * @return the type of this valuable.
         */
        int getType() const { return _type; }

        /**
         * Returns the type of this valuable.
         *
         * All valuables have types 1 or 2.  2 is the larger type of
         * valuable (statues, paintings) (scale 1.25), while 1 is the smaller (vase, jewelry) (scale of 0.5).
         *
         * @param type  The type of this valuable.
         */
        void setType(int type);

        void setTexture(const std::shared_ptr<cugl::graphics::Texture>& value);

        const std::shared_ptr<cugl::graphics::Texture>& getTexture() const {
            return _texture;
        }

        /**
         * Updates the valuable one animation frame
         */
        void update(cugl::Size size);

    };

private:

    /** The radius of a general valuable */
    float _radius;

#pragma mark The Set
public:
    /** The collection of all ACTIVE valuables. Allow the user direct access */
    std::unordered_set<std::shared_ptr<Valuable>> current;

    /**
     * Creates a valuable set with the default values.
     *
     * To properly initialize the valuable set, you should call the init
     * method with the JSON value. We cannot do that in this constructor
     * because the JSON value does not exist at the time the constructor
     * is called (because we do not create this object dynamically).
     */
    ValuableSet();

    /**
     * Initializes valuable data with the given JSON
     *
     * This JSON contains all shared information. It also contains a list of valuables to
     * spawn initially.
     *
     * If this method is called a second time, it will reset all
     * valuable data.
     *
     * @param data  The data defining the valuable settings
     *
     * @return true if initialization was successful
     */
    bool init(std::shared_ptr<cugl::JsonValue> data);

    /**
     * Returns true if the valuable set is empty.
     *
     * @return true if the valuable set is empty.
     */
    bool isEmpty() const { return current.empty(); }

    /**
     * Returns the default radius of a valuable
     *
     * This value should be multiplied by the valuable scale to get the
     * true radius. This value is computed from the texture.  If there
     * is no valuable texture, this value is 0.
     */
    float getRadius() const { return _radius; }

    /**
     * Adds a valuable to the active queue.
     *
     * @param p     The valuable position.
     * @param type  The valuable type.
     */
    void spawnValuable(cugl::Vec2 p, int type);

    /**
     * Updates all the valuables in the active set.
     *
     */
    void update(cugl::Size size);

    /**
     * Draws all active valuables to the sprite batch within the given bounds.
     *
     * Stored asteroids are not drawn.
     *
     * @param batch     The sprite batch to draw to
     * @param size      The size of the window (for wrap around)
     */
    void draw(const std::shared_ptr<cugl::graphics::SpriteBatch>& batch,
        cugl::Size size);
};

#endif /* __VALUABLE_SET_H__ */
