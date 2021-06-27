//
//  GameTimer.hpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 13/01/2019.
//

#ifndef GameTimer_hpp
#define GameTimer_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "../GameManager.hpp"

class GameTimer : public cocos2d::Node {
public:
    CREATE_FUNC(GameTimer);
    ~GameTimer();
    
private:
    GameTimer();
    
    bool init() override;
    void cleanup() override;
    
public:
    void update(float dt) override;
    void updateTimeLabel(float time);
    
    void startTimer();
    void stopTimer();
    void restartTimer();
    
    float getTime();
    float getTimeRatio();
    
private:
    CC_SYNTHESIZE(SBCallback, onTimeOverListener, OnTimeOverListener);
    
    float elapsed;
    SB_SYNTHESIZE_READONLY_BOOL(started, Started);
    
    cocos2d::Label *timeLabel;
    
// Game Event
public:
    void onGameReset();
    void onGamePause();
    void onGameResume();
    void onGameOver();
};
    
#endif /* GameTimer_hpp */
