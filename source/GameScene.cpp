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
float _interval = 1000 * 60.0f / bpm;

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
    // stamp start of song and populate timestamp by beat and 0 + 1
    global_start_stamp.mark();
    // when modifying timestamps, recall that must be done in nanoseconds, hence _interval * 1000000
    timestamp_by_beat = { global_start_stamp,global_start_stamp + _interval * 1000000 ,global_start_stamp ,global_start_stamp};
    // prepopulate with no inputs
    inputs_by_beat = { 4,GameScene::InputType::NO_INPUT };

    //hard coded, change later, in ms
    _bang = assets->get<Sound>("bang");
    
    // Initialize tile model
    _tileModel = std::make_shared<TileModel>();
    _tileModel->init(_nRow, _nCol, _gridSize, _leftOffeset, _bottomOffeset);
    
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
    // get current timestamp and use for comparing beat processing
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

        // if in standerd gameplay, cycle between in and output
        if (_gameState == GameState::OUTPUT || _gameState == GameState::INPUT) {
            _gameState = global_beat >= 2 ? GameState::OUTPUT : GameState::INPUT;
        }
        // ensure clearing the beats on output (on beat 3 to prevent early clearing but mainly if FAILED_INPUT
        if (global_beat == 3 and _gameState == GameState::OUTPUT) {
            inputs_by_beat[0] = InputType::NO_INPUT;
            inputs_by_beat[1] = InputType::NO_INPUT;
        }
        /** debug block 
        //CULog("recorded actions: %d %d %d %d", inputs_by_beat[0], inputs_by_beat[1], inputs_by_beat[2], inputs_by_beat[3]);
        //CULog("global beat %d, actie time stamp %llu", global_beat, timestamp_by_beat[global_beat].getTime());
        //CULog("%llu",  timestamp_by_beat[global_beat].getTime());
        */
    }
    
    //records for the log
    _songTimeMs += dt * 1000.0;
    
    // update input, then processes gestures + external

    _input.readInput();
    _gestureInputProcesserHelper();

    if (_input.didPressReset()) {
        reset();
    }
//    CULog ("log is %d", _input.isLogOn());
    
    if (_step <= 0.2 * _interval || _step >= 0.8 * _interval) {
        // Toggle mini-game overlay with M key
        cugl::Keyboard* keys = cugl::Input::get<cugl::Keyboard>();
        if (keys->keyPressed(cugl::KeyCode::M)) {
            _showOverlay = !_showOverlay;
            _inputStep = 0;
            if (_overlay) _overlay->setVisible(_showOverlay);
        }
        if (_input.didDrop() and _player->isCarrying()) {
            _valuables.set_val_dropped(_player->getCarried());
            _player->setCarrying(false, -1);
        }
    }
    //CULog("start %d", _input.queryStartEventReady());
    //CULog("end %d", _input.queryEndEventReady());
    if (_input.isLogOn()){
        initHitLog();
    }

    bool attempt_pickup = false;
    // the block that actually processes InputTypes --> actions
    if (_gameState == GameState::OUTPUT) {
        // rn its if they are equal, doesnt have to be, just cleaner for the actions we implemented
        if (inputs_by_beat[0] == inputs_by_beat[1]) {
            switch (inputs_by_beat[0]) // again, they are equal so we can do this atm
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
                    // temp logic of if carrying --> drop if not pickup
                    if (_player->getCarried() != -1) {
                        _valuables.set_val_dropped(_player->getCarried());
                        _player->setCarrying(false, -1);
                    }
                    // try to pickup (may fail if no item or player fails mbs)
                    else {
                        attempt_pickup = true;
                    }
                    // atm idt anything else clears that input and may block mbs entered input
                    inputs_by_beat[2] = InputType::NO_INPUT;
                    inputs_by_beat[3] = InputType::NO_INPUT;
                    break;
                default:
                    break;
            }
            // regardless of outcome because two valid inputs entered --> flush current input
            inputs_by_beat[0] = InputType::NO_INPUT;
            inputs_by_beat[1] = InputType::NO_INPUT;
        }
    }

    // debug overlay logic
    if (_input.didToggleOverlay()) {
        _showOverlay = !_showOverlay;
        _inputStep = 0;
        if (_overlay) _overlay->setVisible(_showOverlay);
        _gameState = GameState::MBS;
        attempt_pickup = true;
    }
    // logic to run MBS
    if (attempt_pickup || _gameState == GameState::MBS) {
        if (_gameState != GameState::MBS && _collisions.hackyAttemptToPickUP(_player, _valuables, _tileModel)) {
            _gameState = GameState::MBS;
            _countDownMini = -1; // idt this line is necessary
        }
        _inWindow = true;
        // could probably simplify logic from here to "if (directionSequence.empty())" but didnt
        //  write it/ idk exactly what it does and could be edge cases
        if (_gameState == GameState::MBS) {
            if (_showOverlay == false) {
                _showOverlay = true;
                _overlay->setVisible(true);
            }
            if (_showOverlay) {
                // populate the directionSequence with a randomzied sequence of inputs
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

                // if there is a processed input and there still needs buttons to be pressed
                if (active_input != InputType::NO_INPUT && (_countDownMini >= 0 && _countDownMini < 4)) {
                    // regardeless purge the input on reading
                    inputs_by_beat[global_beat] = InputType::NO_INPUT;
                    Direction dir = Direction::None;

                    // conversion b/n types
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
                    //CULog("identified with %d", active_input);
                    _inputOnBeat = true;

                    // if valid input based on what it needs to be
                    if (dir == directionSequence[_inputStep]) {
                        //CULog("on beat");
                        _inputStep++;
                        // on completion of the 4 beat mbs
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
                        // Wrong input ¡ª fail the minigame *and fully shut down
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
        _countDownMini = 0; // again idk if this is needed?
    }
    else { // edge cases?? but constantly runs if the minigame is not up
        _inWindow = false;
        // idk logic fully but makes sure thing shuts down, might be able to be simplified 
        // and pull out repetative shut down code ^
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
                    if (_countDownMini == 1) AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
                    _countDownMini = max(_countDownMini - 1, 0);
                    CULog("%d", _countDownMini);
                }
                
            }
        }
    }

    // should be fixed when we move to proper grid based, but moves the paitning and player around
    std::vector<cugl::Vec2> player_pos;
    player_pos.push_back(_player->getPosition());
    _valuables.update(getSize(), player_pos);
    if (_inWindow && !_wasInWindow) { // Enter a new input window
        _inputOnBeat = false;
    }
}

void GameScene::_gestureInputProcesserHelper() {
    //TODO: ACCOUNT FOR HOLDING ACTIONS
    // the following ranges exist from percentages and are compared against the milli delta b/n expected and actual input:
    float poor = .35;     // poor * _interval = +-300 ms
    float ok = .25;       // ok * _interval = +-214.285714286 ms
    float good = .15;     // good * _interval = +-128.571428571  ms
    float perfect = 10;   // perfect * _interval = +-85.7142857143 ms

    // if valid input
    if (_input.queryInputReady()) {

        // need to account for holding action
        std::pair<TouchEvent, TouchEvent> pairing = _input.peekCompletedEvent();
        TouchEvent first = pairing.first;
        TouchEvent second = pairing.second;
        Timestamp press = first.timestamp;
        Timestamp release = second.timestamp;

        //CULog("press time,   %llu", press.getTime());
        //CULog("release time, %llu", release.getTime());
        //CULog("delta %llu", Timestamp::ellapsedMillis(press,release));

        // dummy values to compare against, *should* be over written
        float smallest_delta = 10000;
        int smallest_beat_index = -1;
        // cycle throguh all 4 beats and identify which is the closest (needed for late/early inputs
        for (int i = 0; i < 4; i++) {
            // the double delta is funky cause ellapsedMillis returns UInt64 so could hit roll over error
            float delta1 = Timestamp::ellapsedMillis(timestamp_by_beat[i], press);
            float delta2 = Timestamp::ellapsedMillis(press, timestamp_by_beat[i]);
            float delta = delta1 < delta2 ? delta1 : delta2;
            //CULog("delta: %f", delta);
            if (delta < smallest_delta) {
                smallest_delta = delta;
                smallest_beat_index = i;
            }
        }
        //CULog("Closest delta: %f ms (beat index %d)", smallest_delta, smallest_beat_index);

        // really just place holder code to eval later on player performance
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
        // CAN ONLY OVERRIDE NO_INPUT and didnt miss, then actually update action, dc afterwards
        if (inputs_by_beat[smallest_beat_index] == InputType::NO_INPUT and beat_feedback != "miss") {
            inputs_by_beat[smallest_beat_index] = _interpretActionHelper(first, second);
        }
        //debug audio BANG useful for iding delay
        //AudioEngine::get()->play("bang", _bang, false, _bang->getVolume(), true);
        //CULog(beat_feedback.c_str());

        // on processing start and end events, clear
        _input.clearTouchEvents();

    }
}

GameScene::InputType GameScene::_interpretActionHelper(TouchEvent first, TouchEvent second ) {
    //pixel deadzone radius to prevent interpreting taps as slides, could be too small
    int input_deadzone = 225; //15^2
    if (first.position.distanceSquared(second.position) <= input_deadzone) {
        return InputType::TAP;
    }
    else {
        Vec2 displacment = second.position - first.position;
        // again, sketchy, seems that we need to do diff math based on keyboard vs touch
        //  (should be touch oriented and change keyboard output in the vectors defined earlier)
        if (abs(displacment.x) > abs(displacment.y)) {
            return displacment.x <= 0 ? GameScene::InputType::RIGHT_SWIPE : GameScene::InputType::LEFT_SWIPE;
        }
        else {
            return displacment.y <= 0 ? GameScene::InputType::UP_SWIPE : GameScene::InputType::DOWN_SWIPE;
        }
    }
    
    _wasInWindow = _inWindow; // tbh idk why this is here, maybe a result of a merge
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

