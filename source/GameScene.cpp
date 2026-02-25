//
//  SLGameScene.cpp
//  Ship Lab
//
//  This is the scene for running the game. Games are often separated into
//  mutliple scenes, each scene corresponding to a specific play interface.
//  Scenes correspond to the concept of player modes, which we talk about in
//  the introductory course.
//
//  Based on original GameX Ship Demo by Rama C. Hoetzlein, 2002
//
//  Author: Walker White
//  Version: 1/20/26
//
#include <cugl/cugl.h>
#include <iostream>
#include <sstream>

#include "GameScene.h"
//#include "SLCollisionController.h"
#include "AudioController.h"

using namespace cugl;
using namespace cugl::graphics;
using namespace cugl::audio;
using namespace std;

#pragma mark -
#pragma mark Level Layout

// Lock the screen size to fixed height regardless of aspect ratio
#define SCENE_HEIGHT 720

#pragma mark -
#pragma mark Helper

 

#pragma mark -
#pragma mark Constructors
/**
 * Initializes the controller contents, and starts the game
 *
 * The constructor does not allocate any objects or memory.  This allows
 * us to have a non-pointer reference to this controller, reducing our
 * memory allocation.  Instead, allocation happens in this method.
 *
 * @param assets    The (loaded) assets for this game mode
 *
 * @return true if the controller is initialized properly, false otherwise.
 */
bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked height
    if (assets == nullptr) {
        return false;
    } else if (!Scene2::initWithHint(Size(0,SCENE_HEIGHT))) {
        return false;
    }
    
    // Start up the input handler
    _assets = assets;
    Size dimen = getSize();
    
    // Get the background image and constant values
    _background = assets->get<Texture>("background");
    _miniBackground = assets->get<Texture>("miniGame_background 1");
    _constants = assets->get<JsonValue>("constants");

    // Initialize valuables
    _valuables.init(_constants->get("valuables"));
    _valuables.setTexture(assets->get<Texture>("valuable1"));
    
    auto pjson = _constants->get("player")->get("pos");
    cugl::Vec2 start(
        pjson->get(0)->asFloat(),
        pjson->get(1)->asFloat()
    );
    _player = std::make_shared<Player>(start);
    _player->setTexture(assets->get<Texture>("player"));
    
    _collisions.init(getSize());
    _gameState = GameState::PLAYING;
    
    // Play background music
    auto bgm = assets->get<Sound>("bgm");
    AudioEngine::get()->play("bgm", bgm, true);
    
    //hard coded, change later
    _interval = 60.0f/70.0f*1000 ;
    _step = 0.0f;
    _bang = assets->get<Sound>("bang");
    
    
    // Acquire the scene built by the asset loader and resize it
    std::shared_ptr<scene2::SceneNode> scene = _assets->get<scene2::SceneNode>("game");
    scene->setContentSize(dimen);
    scene->doLayout();

    addChild(scene);

    // Initialize buttons from the loaded scene
    _upButton    = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game.buttons.up"));
    _downButton  = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game.buttons.down"));
    _leftButton  = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game.buttons.left"));
    _rightButton = std::dynamic_pointer_cast<scene2::Button>(_assets->get<scene2::SceneNode>("game.buttons.right"));
    
    _upButton->addListener([this](const std::string& name, bool down) {
        if (down) {
            
        }
    });
    
    _downButton->addListener([this](const std::string& name, bool down) {
        if (down) {
        }
    });
    
    _leftButton->addListener([this](const std::string& name, bool down) {
        if (down) {
        }
    });
    
    _rightButton->addListener([this](const std::string& name, bool down) {
        if (down) {
        }
    });
    

    /* loading in the mini game scene -- START */

    _showOverlay = false;
    _inputStep = 0;
    _overlay = _assets->get<scene2::SceneNode>("miniGame");
    if (_overlay == nullptr) {
        std::cout << "[DEBUG] _overlay is NULL - scene2s not loaded" << endl;
    }
    else {
        std::cout << "[DEBUG] _overlay created OK" << endl;
        _overlay->setVisible(false);
        _overlay->setPosition(getSize().width / 2.0f, getSize().height / 2.0f);
        addChild(_overlay);
    }

    /* ~~~~~~~~~~~ MINI GAME SCENE END ~~~~~~ */
    
    /*addChild(_minigame);*/
    reset();
    return true;
}


/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _active = false;
        _assets = nullptr;
    }
}


#pragma mark -
#pragma mark Gameplay Handling
/**
 * Resets the status of the game so that we can play again.
 */
void GameScene::reset() {
    _gameState = GameState::PLAYING;
    _valuables.init(_constants->get("valuables"));
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not a graphics command
 *
 * @param dt    The amount of time (in seconds) since the last frame
 */
void GameScene::update(float dt) {
    // Read the keyboard for each controller.
    _step += dt * 1000;
    CULog("interval is %f", _interval);
    if (_step <= 0.3 * _interval || _step >= 0.7 * _interval) {
        CULog("in step");
        _input.readInput();
        if (_input.didPressReset()) {
            reset();
        }
        // Toggle mini-game overlay with M key
        cugl::Keyboard* keys = cugl::Input::get<cugl::Keyboard>();
        if (keys->keyPressed(cugl::KeyCode::M)) {
            _showOverlay = !_showOverlay;
            _inputStep = 0;
            if (_overlay) _overlay->setVisible(_showOverlay);
        }
        if (_input.didDrop()) {
            _valuables.set_val_dropped(_player->getCarried());
            _player->setCarrying(false, -1);

        }
        if (_input.didPickUp() and _collisions.hackyAttemptToPickUP(_player, _valuables)) {
            if (_showOverlay == false) {
                _showOverlay = true;
                _overlay->setVisible(true);
            }
        }
        std::vector<cugl::Vec2> player_pos;
        player_pos.push_back(_player->getPosition());
        _valuables.update(getSize(), player_pos);

        /**
        if (_collisions.resolveCollisions(_player, _valuables)) {
            std::cout<<"Collision between player and valuable"<<endl;
            */


        if (_gameState == GameState::PLAYING) {
            if (_showOverlay) {
                // Must enter Up, Left, Right, Down in order to dismiss
                static const Direction sequence[] = {
                    Direction::Up, Direction::Left, Direction::Right, Direction::Down
                };
                Direction dir = _input.getDirection();
                if (dir != Direction::None) {
                    if (dir == sequence[_inputStep]) {
                        _inputStep++;
                        if (_inputStep == 4) {
                            // Full sequence entered ¡ª dismiss overlay
                            _showOverlay = false;
                            _inputStep = 0;
                            if (_overlay) _overlay->setVisible(false);
                        }
                    }
                    else {
                        // Wrong input ¡ª reset sequence
                        _inputStep = 0;
                    }
                }
            }
            else {
                // the update loop
                if (_input.getDirection() != Direction::None) {
                    _player->move(_input.getDirection(), _gridSize, _nRow, _nCol);
                }
                std::vector<cugl::Vec2> player_pos;
                player_pos.push_back(_player->getPosition());
                _valuables.update(getSize(), player_pos);

            }
        }
    }
    if (_step >= _interval) {
        //BANG!!!
        //(plays Bang on beat)
        _step = 0.0f;
        AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
    }
}

/**
 * Draws all this scene to the scene's SpriteBatch.
 *
 * The default implementation of this method simply draws the scene graph
 * to the sprite batch. By overriding it, you can do custom drawing
 * in its place.
 */
void GameScene::render() {
    // For now we render 3152-style
    // DO NOT DO THIS IN YOUR FINAL GAME
    _batch->setPerspective(getCamera()->getCombined());
    _batch->begin();
    
    _batch->draw(_background,Rect(Vec2::ZERO,getSize()));
    //draw things here
    _valuables.draw(_batch, getSize());
    _player->draw(_batch);
    _batch->setColor(Color4::BLACK);
    
     
    _batch->end();

    // Draw scene graph children (overlay) on top via the proper Scene2 pipeline
    Scene2::render();
}

