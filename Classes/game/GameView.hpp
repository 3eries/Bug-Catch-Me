//
//  GameView.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameView_hpp
#define GameView_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

#include "Define.h"
#include "GameManager.hpp"

class Bug;
class GameTimer;
class BgBinary;

class GameView : public cocos2d::Node {
public:
    CREATE_FUNC(GameView);
    ~GameView();
    
private:
    enum Tag {
        BG_BINARY = 10,
        KILLS = 100,
    };
    
    enum class ZOrder {
    };
    
private:
    GameView();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void cleanup() override;
    
    void initTouch();
    void initBg();
    void initObjects();
    void initGameListener();
    
#pragma mark- Game Event
public:
    void onGameReset();
    void onGameStart();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameResult();
    
    void onLevelChanged(const LevelData &stage);
    void onLevelClear(const LevelData &stage);
    
private:
    void onTouch(cocos2d::Touch *touch);
    
    void killBugs(std::vector<Bug*> bugs, const cocos2d::Vec2 &effectPos);
    
    void updateBackgroundBinary(float dt = 0);
    void showLevelLabel(SBCallback onCompleted);
    
private:
    int life;           // 현재 레벨의 남은 라이프
    int kills;          // 현재 레벨의 버그 퇴치 수
    int totalKills;     // 누적 버그 퇴치 수
    
    struct Life {
        cocos2d::Node *unusedIcon;
        cocos2d::Node *usedIcon;
        
        void setEnabled(bool isEnabled) {
            unusedIcon->setVisible(isEnabled);
            usedIcon->setVisible(isEnabled);
        }
        
        void setUsed(bool used) {
            unusedIcon->setVisible(!used);
            usedIcon->setVisible(used);
        }
    };
    
    std::vector<Life> lifes;
    std::vector<Bug*> bugs;
    GameTimer *timer;

    std::vector<BgBinary*> bgBinarys;
};

#endif /* GameView_hpp */
