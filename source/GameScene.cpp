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
#include <filesystem>
#include "GameScene.h"
//#include "SLCollisionController.h"
#include "AudioController.h"
#include <vector>

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


struct HitLog {
    double timeMs;     // Absolute time since song start
    int beatIndex;     // Nearest beat number
    int errorMs;       // Signed timing error in milliseconds
    int direction;     // Direction enum as int
};

std::vector<HitLog> _hitLog;

double _songTimeMs = 3.429f*1000.0f;
float  bpm = 70.0f;

#include <fstream>
#include <iomanip>

void initHitLog() {
    std::string path =
        cugl::Application::get()->getSaveDirectory() + "hitlog.csv";

    // If file already exists, do nothing
    std::ifstream check(path);
    if (check.good()) {
        return;  // file already exists
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        CULog("Failed to create %s", path.c_str());
        return;
    }

    out << "time_ms,beat,error_ms,direction\n";
    out.close();

}

void appendHitLog(Direction dir, bool logOn){
    if (!logOn){
        return;
    }
    //things for the log
    double msPerBeat = 60000.0f / bpm;
    double beatPos = _songTimeMs / msPerBeat;
    int nearestBeat = (int)std::llround(beatPos);
    double errorMsDouble = (beatPos - nearestBeat) * msPerBeat;
    int errorMs = (int)std::round(errorMsDouble);
    _hitLog.push_back({
            _songTimeMs,
            nearestBeat,
            errorMs,
            (int)dir
        });
    std::string path =
            cugl::Application::get()->getSaveDirectory() + "hitlog.csv";

    CULog("Apending at hitlog.csv at %s", path.c_str());
    std::ofstream out(path, std::ios::app);

        if (!out.is_open()) {
            CULog("Failed to append to %s", path.c_str());
            return;
        }

        out << (int)_songTimeMs << ","
            << nearestBeat << ","
            << errorMs << ","
            << (int)dir << "\n";
}



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
    _player->setCarry(assets->get<Texture>("carry"));
    
    _collisions.init(getSize());
    _gameState = GameState::INPUT;
    
    // Play background music
    auto bgm = assets->get<Sound>("bgm2-2");
    AudioEngine::get()->play("bgm", bgm, true);
    global_start_stamp.mark();
    timestamp_by_beat = { global_start_stamp,global_start_stamp + 60000000 / bpm ,global_start_stamp ,global_start_stamp};
    inputs_by_beat = { 4,GameScene::InputType::NO_INPUT };
    //hard coded, change later in ms
    _interval = 1000*60.0f/bpm ;
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
    initHitLog();
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
    _gameState = GameState::INPUT;
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
    Timestamp current_time = Timestamp();
    Uint64 elapsedMs = Timestamp::ellapsedMillis(global_start_stamp, current_time);
    int updatedBeatNumber = (int) (elapsedMs / _interval) % 4;
    bool beat_change = false;
    if (global_beat != updatedBeatNumber) {
        beat_change = true;
        CULog("%d beat", global_beat);
        global_beat = updatedBeatNumber;
        timestamp_by_beat[global_beat].mark();
        timestamp_by_beat[(global_beat + 1) % 4] = current_time + _interval * 1000000;
        if (_gameState == GameState::OUTPUT || _gameState == GameState::INPUT) {
            _gameState = global_beat >= 2 ? GameState::OUTPUT : GameState::INPUT;
        }
        if (global_beat == 3 and _gameState == GameState::OUTPUT) {
            inputs_by_beat[0] = InputType::NO_INPUT;
            inputs_by_beat[1] = InputType::NO_INPUT;
        }
        //CULog("recorded actions: %d %d %d %d", inputs_by_beat[0], inputs_by_beat[1], inputs_by_beat[2], inputs_by_beat[3]);
        //CULog("global beat %d, actie time stamp %llu", global_beat, timestamp_by_beat[global_beat].getTime());
        //CULog("%llu",  timestamp_by_beat[global_beat].getTime());
    }
    
    //records for the log
    _songTimeMs += dt * 1000.0;
    
    //for reading proper input, we need to know when it was entered
    // when it was entered relative to the beat, on what beat it was entered on 
    _input.readInput();
    _gestureInputProcesserHelper();
    if (_input.didPressReset()) {
        reset();
    }
    //CULog("start %d", _input.queryStartEventReady());
    //CULog("end %d", _input.queryEndEventReady());
    if (_input.isLogOn()){
        initHitLog();
    }

    bool attempt_pickup = false;
    //CULog("press ready %d, output ready %d", _input.peekStartEvent().pressure, _input.peekEndEvent().pressure);
    if (_gameState == GameState::OUTPUT) {
        //CULog("recorded actions: %d %d %d %d", inputs_by_beat[0], inputs_by_beat[1], inputs_by_beat[2], inputs_by_beat[3]);
        if (inputs_by_beat[0] == inputs_by_beat[1]) {
            switch (inputs_by_beat[0])
            {
            case InputType::UP_SWIPE:
                _player->move(Direction::Up, _gridSize, _nRow, _nCol);
                break;
            case InputType::DOWN_SWIPE:
                _player->move(Direction::Down, _gridSize, _nRow, _nCol);
                break;
            case InputType::LEFT_SWIPE:
                _player->move(Direction::Left, _gridSize, _nRow, _nCol);
                break;
            case InputType::RIGHT_SWIPE:
                _player->move(Direction::Right, _gridSize, _nRow, _nCol);
                break;
            case InputType::TAP:
                if (_player->getCarried() != -1) {
                    _valuables.set_val_dropped(_player->getCarried());
                    _player->setCarrying(false, -1);
                }
                else {
                    attempt_pickup = true;
                }
                inputs_by_beat[2] = InputType::NO_INPUT;
                inputs_by_beat[3] = InputType::NO_INPUT;
                break;
            default:
                break;
                /**
                if (_input.didDrop() and _player->isCarrying()) {
                    _valuables.set_val_dropped(_player->getCarried());
                    _player->setCarrying(false, -1);
                } */
            }
            inputs_by_beat[0] = InputType::NO_INPUT;
            inputs_by_beat[1] = InputType::NO_INPUT;
        }

    }

    if (_input.didToggleOverlay()) {
        _showOverlay = !_showOverlay;
        _inputStep = 0;
        if (_overlay) _overlay->setVisible(_showOverlay);
        _gameState = GameState::MBS;
        attempt_pickup = true;
    }
    if (attempt_pickup || _gameState == GameState::MBS) {
        if (_gameState != GameState::MBS && _collisions.hackyAttemptToPickUP(_player, _valuables)) {
            _gameState = GameState::MBS;
            _countDownMini = -1;
        }
        _inWindow = true;
        if (_gameState == GameState::MBS) {
            if (_showOverlay == false) {
                _showOverlay = true;
                _overlay->setVisible(true);
            }
            if (_showOverlay) {
                // Must enter Up, Left, Right, Down in order to dismiss
                /*static const Direction sequence[] = {
                    Direction::Up, Direction::Left, Direction::Right, Direction::Down
                };*/
                if (directionSequence.empty()) { // Random generate directions
                    for (int i = 0; i < 4; i++) {
                        int r = rand() % 4;
                        Direction dir = static_cast<Direction>(r);
                        directionSequence.push_back(dir);
                        auto button = _overlay->getChild(i + 2);
                        if (button && button->getChildren().size() > 0) {
                            auto img = button->getChild(0);
                            if (img) {
                                float degree = 0.0f;
                                switch (dir) {
                                case Direction::Right: degree = -90; break;
                                case Direction::Up:    degree = 0; break;
                                case Direction::Left:  degree = 90; break;
                                case Direction::Down:  degree = 180; break;
                                }
                                img->setAngle(degree * M_PI / 180.0f);
                            }
                        }
                    }
                }
                //CULog("eval inputs");
                GameScene::InputType active_input = inputs_by_beat[global_beat];
                //CULog("inputs %d, %d, %d, %d", inputs_by_beat[0], inputs_by_beat[1], inputs_by_beat[2], inputs_by_beat[3]);
                //CULog("input %d", active_input);
                if (active_input != InputType::NO_INPUT && (_countDownMini >= 0 && _countDownMini < 4)) {
                    CULog("ENTERED----------------");
                    inputs_by_beat[global_beat] = InputType::NO_INPUT;
                    Direction dir = Direction::None;
                    if (active_input == InputType::UP_SWIPE) {
                        dir = Direction::Up;
                    }
                    else if (active_input == InputType::DOWN_SWIPE) {
                        dir = Direction::Down;
                    }
                    else if (active_input == InputType::LEFT_SWIPE) {
                        dir = Direction::Left;
                    }
                    else if (active_input == InputType::RIGHT_SWIPE) {
                        dir = Direction::Right;
                    }
                    appendHitLog(dir, _input.isLogOn());
                    CULog("identified with %d", active_input);
                    _inputOnBeat = true;
                    if (dir == directionSequence[_inputStep]) {
                        CULog("on beat");
                        _inputStep++;
                        if (_inputStep == 4) {
                            // Full sequence entered ¡ª dismiss overlay
                            _showOverlay = false;
                            _inputStep = 0;
                            if (_overlay) _overlay->setVisible(false);
                            _countDownMini = 5;
                            directionSequence.clear();
                            _gameState = GameState::INPUT;
                        }
                    }
                    else {
                        // Wrong input ¡ª fail the minigame
                        CULog("in fail block");
                        _inputStep = 0;
                        _showOverlay = false;
                        _valuables.set_val_dropped(_player->getCarried());
                        _player->setCarrying(false, -1);
                        if (_overlay) _overlay->setVisible(false);
                        _countDownMini = 5;
                        directionSequence.clear();
                        _gameState = GameState::INPUT;
                    }
                }
            }
        }
        _countDownMini = 0;
    }
    else {
        _inWindow = false;
        if (_gameState == GameState::INPUT || _gameState == GameState::OUTPUT) {
            if (!_inWindow && _wasInWindow) { // Exit an input window
                if (!_inputOnBeat && _countDownMini == 0 && _showOverlay) {
                    CULog("off beat");
                    _inputStep = 0;
                    _showOverlay = false;
                    _valuables.set_val_dropped(_player->getCarried());
                    _player->setCarrying(false, -1);
                    if (_overlay) _overlay->setVisible(false);
                    _countDownMini = 5;
                    directionSequence.clear();
                }
                if (_showOverlay) { // Check again
                    _countDownMini = max(_countDownMini - 1, 0);
                    CULog("%d", _countDownMini);
                }
                
            }
        }
    }

    std::vector<cugl::Vec2> player_pos;
    player_pos.push_back(_player->getPosition());
    _valuables.update(getSize(), player_pos);
    if (_inWindow && !_wasInWindow) { // Enter a new input window
        _inputOnBeat = false;
    }
}

void GameScene::_gestureInputProcesserHelper() {
    //need current beat filled to compare against
    //hard coded epsilon errors, should pull out later
    
    // the following ranges exist from percentages and are compared against the milli delta b/n expected and actual input:
    float poor = .35;     // poor * _interval = +-300 ms
    float ok = .25;       // ok * _interval = +-214.285714286 ms
    float good = .15;     // good * _interval = +-128.571428571  ms
    float perfect = 10; // perfect * _interval = +-85.7142857143 ms
    //pixel deadzone radius to prevent interpreting taps as slides
    if (_input.queryInputReady()) {
        //need to account for holding action
        std::pair<TouchEvent, TouchEvent> pairing = _input.peekCompletedEvent();
        TouchEvent first = pairing.first;
        TouchEvent second = pairing.second;
        Timestamp press = first.timestamp;
        Timestamp release = second.timestamp;

        //CULog("press time,   %llu", press.getTime());
        //CULog("release time, %llu", release.getTime());
        //CULog("delta %llu", Timestamp::ellapsedMillis(press,release));
        float smallest_delta = 10000;
        //press = press - global_start_stamp
        int smallest_beat_index = -1;
        for (int i = 0; i < 4; i++) {
            float delta1 = Timestamp::ellapsedMillis(timestamp_by_beat[i], press);
            float delta2 = Timestamp::ellapsedMillis(press, timestamp_by_beat[i]);
            float delta = delta1 < delta2 ? delta1 : delta2;
            CULog("delta: %f", delta);
            if (delta < smallest_delta) {
                smallest_delta = delta;
                smallest_beat_index = i;
            }
        }
        CULog("Closest delta: %f ms (beat index %d)", smallest_delta, smallest_beat_index);

        string beat_feedback = "";
        if (smallest_delta > _interval * poor) {
            beat_feedback = "miss";
        }
        else if (smallest_delta > _interval * ok) {
            beat_feedback = "poor";
        }
        else if (smallest_delta > _interval * good) {
            beat_feedback = "ok";
        }
        else if (smallest_delta > _interval * perfect) {
            beat_feedback = "good";
        }
        else {
            beat_feedback = "perfect";
        }
        //CULog("temp");
        if (inputs_by_beat[smallest_beat_index] == InputType::NO_INPUT and beat_feedback != "miss") {
            InputType interpreted_action = _interpretActionHelper(first, second);
            inputs_by_beat[smallest_beat_index] = interpreted_action;
        }
        //AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
        CULog(beat_feedback.c_str());
        _input.clearTouchEvents();

    }
}

GameScene::InputType GameScene::_interpretActionHelper(TouchEvent first, TouchEvent second ) {
    int input_deadzone = 225; //15^2
    if (first.position.distanceSquared(second.position) <= input_deadzone) {
        return InputType::TAP;
    }
    else {
        Vec2 displacment = second.position - first.position;
        if (abs(displacment.x) > abs(displacment.y)) {
            return displacment.x <= 0 ? GameScene::InputType::RIGHT_SWIPE : GameScene::InputType::LEFT_SWIPE;
        }
        else {
            return displacment.y <= 0 ? GameScene::InputType::UP_SWIPE : GameScene::InputType::DOWN_SWIPE;
        }
    }
    _wasInWindow = _inWindow;
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

    Scene2::render();
}

