//
//  GameScene.hpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BaseScene.hpp"
#include "GameManager.hpp"

class GameView;

class GameScene : public BaseScene {
public:
    static GameScene* create();
    ~GameScene();
    
private:
    enum Tag {
        BANNER           = 100,
        
        BTN_SETTING,
    };
    
private:
    GameScene();
    
    bool init() override;
    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExit() override;
    void cleanup() override;
    bool onApplicationEnterBackground() override;
    bool onApplicationEnterForeground() override;
    bool onBackKeyReleased() override;
    
    void initBg();
    void initGameView();
    void initMenu();
    void initGameListener();
    
// Game Event
private:
    void onGameReset();
    void onGamePause();
    void onGameResume();
    void onGameOver();
    void onGameResult();
    
    void onLevelClear(const LevelData &level);

public:
    void replaceScene(SceneType type) override;
    void showResultPopup();
    
    void checkReview();
    
    void onClick(cocos2d::Node *sender) override;
    
private:
    CC_SYNTHESIZE(GameView*, gameView, GameView);
};


#endif /* GameScene_hpp */
