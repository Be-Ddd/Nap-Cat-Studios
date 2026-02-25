//
//  Player.cpp
//  Demo
//
//  This is the implementation for the Player class.
//
#include "Player.h"

using namespace cugl;
using namespace cugl::graphics;

#pragma mark -
#pragma mark Static Variables

// Initialize static player ID counter
int Player::_playerID = -1;

#pragma mark -
#pragma mark Constructors

/**
 * Creates a player at the given world position.
 */
Player::Player(const Vec2& pos) :
    _pos(pos),
    _start(pos),
    _target(pos),
    _ID(++_playerID),
    _moving(false),
    _isCarrying(false),
    _moveTime(0.0f),
    _moveDuration(0.2f), // change later
    _facing(Direction::Down),
    _texture(nullptr),
    _carry(nullptr),
    _node(nullptr),
    _scale(0.15f),
    _radius(0.0f)
{
}

/**
 * Disposes all resources allocated to this player.
 */
void Player::dispose() {
    _texture = nullptr;
    if (_node != nullptr) {
        _node->dispose();
        _node = nullptr;
    }
}

/**
 * Initializes the player with a texture.
 */
bool Player::init(const Vec2& pos, const std::shared_ptr<Texture>& texture) {
    _texture = texture;
    
    if (_texture == nullptr) {
        return false;
    }
    
    // Create scene node for rendering
    _node = scene2::PolygonNode::allocWithTexture(_texture);
    if (_node == nullptr) {
        return false;
    }
    
    // Center the node's anchor
    _node->setAnchor(Vec2::ANCHOR_CENTER);
    _node->setPosition(_pos);
    
    return true;
}

#pragma mark -
#pragma mark Position

/**
 * Hard-sets the position (cancels any ongoing movement).
 */
void Player::setPosition(const Vec2& value) {
    _pos = value;
    _start = value;
    _target = value;
    _moving = false;
    _moveTime = 0.0f;
    
    // Update node position
    if (_node != nullptr) {
        _node->setPosition(_pos);
    }
}

void Player::setTexture(const std::shared_ptr<cugl::graphics::Texture>& texture){
    if (texture) {
        Size size = texture->getSize();
        _radius = std::max(size.width, size.height) / 2;
        _width = size.width / 2.0f;
        _height = size.height / 2.0f;
        _texture = texture;
    }else{
        _radius = 0.0f;
        _texture =nullptr;
    }
}

void Player::setCarry(const std::shared_ptr<cugl::graphics::Texture> &texture){
    if (texture) {

        _carry = texture;
    }else{
        _radius = 0.0f;
        _carry =nullptr;
    }
}

#pragma mark -
#pragma mark Movement

/**
 * Starts a single movement step in the given direction.
 */
void Player::startStep(Direction dir, const Vec2& delta) {
    if (_moving) {
        return;
    }
    
    // Update facing direction
    _facing = dir;
    
    // Set up movement interpolation
    _start = _pos;
    _target = _pos + delta;
    _moving = true;
    _moveTime = 0.0f;
}

/**
 * Updates smooth movement animation.
 */
void Player::update(float dt) {
    if (!_moving) {
        return;
    }
    
    _moveTime += dt;
    
    float t = _moveTime / _moveDuration;
    
    if (t >= 1.0f) {
        _pos = _target;
        _moving = false;
        _moveTime = 0.0f;
    } else {
        // linear movement
        _pos = _start + (_target - _start) * t;
        
        //smooth later
    }
    
    if (_node != nullptr) {
        _node->setPosition(_pos);
    }
}

void Player::move(Direction dir, float gridSize, int nRow, int nCol){
    cugl::Vec2 step = Vec2(0,0);
    switch (dir) {
        case Direction::Up:{
            step= cugl::Vec2(0,1);
            break;
        }
        case Direction::Down:{
            step= cugl::Vec2(0,-1);
            break;
        }
        case Direction::Left:{
            step= cugl::Vec2(-1,0);
            break;
        }
        case Direction::Right:{
            step = cugl::Vec2(1,0);
            break;
        }
        default:
            break;
    }
    float rightEdge = nCol*gridSize;
    float topEdge = nRow*gridSize;
    cugl::Vec2 next = _pos+step*gridSize;

    if (next.x < 0 || next.x >= rightEdge ||
            next.y < 0 || next.y >= topEdge) {
            return;   // block movement
        }
    _pos = next;
    
}

#pragma mark -
#pragma mark Rendering

/**
 * Draws this player to the sprite batch.
 */
void Player::draw(const std::shared_ptr<graphics::SpriteBatch>& batch) {
    if (_texture != nullptr && batch != nullptr &&!_isCarrying) {
        float scale = getScale();
        Vec2 pos = getPosition();
        // Vec2 origin(_radius,_radius);
        Vec2 origin(_width, _height);
        
        Affine2 trans;
        trans.scale(scale);
        trans.translate(pos);

        batch->draw(_texture, origin, trans);
    }else if (_carry!=nullptr && batch!=nullptr && _isCarrying){
        Vec2 pos = getPosition();
        Vec2 origin(_carry->getSize().width/2.0f, _carry->getSize().height/2.0f);
        
        Affine2 trans;
        trans.scale(1.0f);
        trans.translate(pos);

        batch->draw(_carry, origin, trans);
    }
}
