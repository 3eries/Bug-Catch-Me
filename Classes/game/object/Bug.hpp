//
//  Bug.hpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 13/01/2019.
//

#ifndef Bug_hpp
#define Bug_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class Bug : public cocos2d::Node {
public:
    enum class ImageType {
        IDLE,
        DIE,
    };
    
public:
    CREATE_FUNC(Bug);
    ~Bug();
    
private:
    Bug();
    
    bool init() override;
    void onExit() override;
    void cleanup() override;
    
public:
    void setEnabled(bool isEnabled);
    void setImage(ImageType type, bool isRunAnimation);
    
    void startTeleport();
    void stopTeleport();
    void teleport();
    
    void regen();
    void die();
    
    bool          isTouchInside(cocos2d::Touch *touch);
    cocos2d::Rect getBoundingBoxInWorld();
    
private:
    CC_SYNTHESIZE(BugData, data, Data);
    SB_SYNTHESIZE_READONLY_BOOL(enabled, Enabled);
    
    SBAnimationSprite *image;
    
// Game Event
public:
    void onGameEnter();
    void onGameExit();
    void onGameReset();
    void onGameStart();
    void onGameRestart();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameContinue();
    void onGameResult();
    
    void onStartTimer();
    void onLevelChanged(const LevelData &level);
};

#endif /* Bug_hpp */
