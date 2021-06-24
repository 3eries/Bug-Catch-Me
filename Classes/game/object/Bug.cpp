//
//  Bug.cpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 13/01/2019.
//

#include "Bug.hpp"

#include "Define.h"
#include "UserDefaultKey.h"
#include "SceneManager.h"

#include "../GameDefine.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

static const string SCHEDULER_FIRST_TELEPORT        = "SCHEDULER_FIRST_TELEPORT";
static const string SCHEDULER_TELEPORT              = "SCHEDULER_TELEPORT";

static const float MIN_SCALE = 0.15f;
static const float MAX_SCALE = 0.5f;

static const float MIN_POS_X = 72;
static const float MAX_POS_X = 648;

static const float MIN_POS_Y = 120;
static const float MAX_POS_Y = 1124;

Bug::Bug() {
    
}

Bug::~Bug() {
    
}

bool Bug::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setCascadeOpacityEnabled(true);
    
    // 이미지 초기화
    image = SBAnimationSprite::create();
    image->setAnchorPoint(ANCHOR_M);
    addChild(image);
    
    setImage(ImageType::IDLE, true);
    
    auto size = image->getContentSize();
    setContentSize(size);
    image->setPosition(Vec2MC(size, 0, 0));
    
    // 리스너 초기화
    StringList events({
        GAME_EVENT_ENTER,
        GAME_EVENT_EXIT,
        GAME_EVENT_RESET,
        GAME_EVENT_START,
        GAME_EVENT_RESTART,
        GAME_EVENT_PAUSE,
        GAME_EVENT_RESUME,
        GAME_EVENT_OVER,
        GAME_EVENT_CONTINUE,
        GAME_EVENT_RESULT,
        GAME_EVENT_START_TIMER,
        GAME_EVENT_STAGE_CHANGED,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::ENTER:          this->onGameEnter();         break;
            case GameEvent::EXIT:           this->onGameExit();          break;
            case GameEvent::RESET:          this->onGameReset();         break;
            case GameEvent::START:          this->onGameStart();         break;
            case GameEvent::RESTART:        this->onGameRestart();       break;
            case GameEvent::PAUSE:          this->onGamePause();         break;
            case GameEvent::RESUME:         this->onGameResume();        break;
            case GameEvent::OVER:           this->onGameOver();          break;
            case GameEvent::CONTINUE:       this->onGameContinue();      break;
            case GameEvent::RESULT:         this->onGameResult();        break;
            case GameEvent::START_TIMER:    this->onStartTimer();        break;
                
            case GameEvent::STAGE_CHANGED: {
                auto level = (LevelData*)userData;
                this->onLevelChanged(*level);
            } break;
                
            default: break;
        }
    }, this);
    
    return true;
}

void Bug::onExit() {
    
    Node::onExit();
}

void Bug::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

void Bug::setEnabled(bool isEnabled) {
    
    this->enabled = isEnabled;
}

/**
 * 이미지 변경
 */
void Bug::setImage(ImageType type, bool isRunAnimation) {
    
    switch( type ) {
        case ImageType::IDLE: {
            vector<string> files({
                DIR_IMG_GAME + "bug1.png",
                DIR_IMG_GAME + "bug1_2.png",
            });
            
            auto anim = SBNodeUtils::createAnimation(files, 0.5f);
            image->setAnimation(anim);
        } break;
            
        case ImageType::DIE: {
            vector<string> files({
                DIR_IMG_GAME + "bug2.png",
            });
            
            auto anim = SBNodeUtils::createAnimation(files, 0);
            image->setAnimation(anim, 1);
        } break;
            
        default:
            CCASSERT(false, "Bug::setImage error.");
            break;
    }
    
    if( isRunAnimation ) {
        image->runAnimation();
    }
}

/**
 * 텔레포트 시작
 */
void Bug::startTeleport() {
    
    CCASSERT(enabled, "Bug::startTeleport error.");
    
    stopTeleport();
    teleport();
    
    const float INTERVAL = data.teleportInterval;
    
    scheduleOnce([=](float dt) {
        
        this->teleport();
        this->schedule([=](float dt) {
            
            this->teleport();
            
        }, INTERVAL, SCHEDULER_TELEPORT);
        
    }, INTERVAL*0.5f, SCHEDULER_FIRST_TELEPORT);
}

/**
 * 텔레포트 중지
 */
void Bug::stopTeleport() {
    
    unscheduleAllCallbacks();
}

/**
 * 텔레포트
 */
void Bug::teleport() {
    
    // 스케일
    setScale(random(MIN_SCALE, MAX_SCALE));
    
    // 각도
    setRotation(random(0, 360));
    
    // 좌표
    auto adjustment = [](float v, float min, float max) -> float {
        v = MAX(min, v);
        v = MIN(max, v);
        return v;
    };
    
    Vec2 pos;
    pos.x = random<int>(MIN_POS_X, MAX_POS_X);
    pos.y = random<int>(MIN_POS_Y, MAX_POS_Y);
    
    if( pos.getDistance(image->getPosition()) < 100 ) {
        Vec2 ran;
        ran.x = random<int>(300, 500) * ((arc4random() % 2) == 0 ? 1 : -1);
        ran.y = random<int>(400, 800) * ((arc4random() % 2) == 0 ? 1 : -1);
        
        pos = image->getPosition() + ran;
    }
    
    pos.x = adjustment(pos.x, MIN_POS_X, MAX_POS_X);
    pos.y = adjustment(pos.y, MIN_POS_Y, MAX_POS_Y);
    setPosition(pos);
}

/**
 * 리젠
 */
void Bug::regen() {
    
    setImage(ImageType::IDLE, true);
    setOpacity(255);
}

/**
 * 죽었당
 */
void Bug::die() {
    
    stopTeleport();
    setImage(ImageType::DIE, true);
    
    // 뿌직
    auto box = getBoundingBoxInWorld();
    
    auto dieEffect = SBAnimationSprite::create(SBNodeUtils::createAnimation(BUG_DIE_EFFECT_ANIMS, 1.0f/30), 1);
    dieEffect->setAnchorPoint(ANCHOR_M);
    dieEffect->setPosition(Vec2(box.getMidX(), box.getMidY()));
    dieEffect->setScale(box.size.height / dieEffect->getContentSize().height);
    SceneManager::getScene()->addChild(dieEffect, SBZOrder::BOTTOM);
    
    dieEffect->runAnimation([=](Node*) {
        dieEffect->removeFromParent();
    });
    
    // 파르르 떨리고 사라짐
    auto move1 = MoveBy::create(0.05f, Vec2(10, 0));
    auto move2 = MoveBy::create(0.05f, Vec2(-10, 0));
    auto fadeOut = FadeOut::create(0.8f);
    auto callFunc = CallFunc::create([=]() {
    });
    auto seq = Sequence::create(move1, move2, fadeOut, callFunc, nullptr);
    runAction(seq);
}

bool Bug::isTouchInside(Touch *touch) {
    
    return isEnabled() && getBoundingBoxInWorld().containsPoint(touch->getLocation());
}

Rect Bug::getBoundingBoxInWorld() {
    
    return SBNodeUtils::getBoundingBoxInWorld(this, getScaleX(), getScaleY());
}

/**
 * 게임 진입
 */
void Bug::onGameEnter() {
}

/**
 * 게임 퇴장
 */
void Bug::onGameExit() {
}

/**
 * 게임 리셋
 */
void Bug::onGameReset() {
    
}

/**
 * 게임 시작
 */
void Bug::onGameStart() {
}

/**
 * 게임 재시작
 */
void Bug::onGameRestart() {
}

/**
 * 게임 일시정지
 */
void Bug::onGamePause() {
}

/**
 * 게임 재게
 */
void Bug::onGameResume() {
}

/**
 * 게임 오버
 */
void Bug::onGameOver() {
}

/**
 * 게임 이어하기
 */
void Bug::onGameContinue() {
}

/**
 * 게임 결과
 */
void Bug::onGameResult() {
}

/**
 * 타이머 시작
 */
void Bug::onStartTimer() {
}

/**
 * 레벨 변경
 */
void Bug::onLevelChanged(const LevelData &level) {
    
    regen();
}


