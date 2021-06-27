//
//  GameTimer.cpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 13/01/2019.
//

#include "GameTimer.hpp"

#include "Define.h"
#include "../GameDefine.h"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

GameTimer::GameTimer() :
onTimeOverListener(nullptr),
elapsed(0),
started(false) {
}

GameTimer::~GameTimer() {
}

bool GameTimer::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(Vec2::ZERO);
    setPosition(Vec2::ZERO);
    setContentSize(SB_WIN_SIZE);
    
    // 타임 라벨 초기화
    timeLabel = Label::createWithTTF("00:00", FONT_SABO, 52, Size::ZERO,
                                     TextHAlignment::CENTER, TextVAlignment::CENTER);
    timeLabel->setAnchorPoint(ANCHOR_M);
    timeLabel->setPosition(Vec2MC(0, -10));
    timeLabel->setTextColor(Color4B(255, 255, 255, 255));
    // timeLabel->setTextColor(Color::HIGHLIGHT);
    addChild(timeLabel);
    
    // 리스너 초기화
    // enter, exit, start, restart, continue, result timer, levelchanged
    StringList events({
        GAME_EVENT_RESET,
        GAME_EVENT_PAUSE,
        GAME_EVENT_RESUME,
        GAME_EVENT_OVER,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        switch( event ) {
            case GameEvent::RESET:          this->onGameReset();         break;
            case GameEvent::PAUSE:          this->onGamePause();         break;
            case GameEvent::RESUME:         this->onGameResume();        break;
            case GameEvent::OVER:           this->onGameOver();          break;
            default: break;
        }
    }, this);
    
    return true;
}

void GameTimer::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

void GameTimer::update(float dt) {
    
    elapsed += dt;
    
    float time = getTime();
    updateTimeLabel(time);
    
    // 타임 오버
    if( time == 0 ) {
        if( onTimeOverListener ) {
            onTimeOverListener();
        }
    }
}

void GameTimer::updateTimeLabel(float time) {
    
    timeLabel->setString(STR_FORMAT("%02d:%02d", (int)(time / 60), (int)time % 60));
}

/**
 * 시작
 */
void GameTimer::startTimer() {
    
    if( started ) {
        return;
    }
    
    started = true;
    elapsed = 0;
    updateTimeLabel(GAME_MANAGER->getLevelData().time);
    
    scheduleUpdate();
}

/**
 * 중지
 */
void GameTimer::stopTimer() {
    
    started = false;
    
    unscheduleUpdate();
}

/**
 * 리셋 타이머
 */
void GameTimer::restartTimer() {
    
    stopTimer();
    startTimer();
}

float GameTimer::getTime() {
    
    float time = GAME_MANAGER->getLevelData().time - elapsed;
    time = MAX(0, time);
    
    return time;
}

float GameTimer::getTimeRatio() {
    
    return getTime() / GAME_MANAGER->getLevelData().time;
}

/**
 * 게임 리셋
 */
void GameTimer::onGameReset() {
    
    stopTimer();
}

/**
 * 게임 일시정지
 */
void GameTimer::onGamePause() {
}

/**
 * 게임 재게
 */
void GameTimer::onGameResume() {
}

/**
 * 게임 오버
 */
void GameTimer::onGameOver() {
    
    stopTimer();
}


