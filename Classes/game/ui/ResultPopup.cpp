//
//  ResultPopup.cpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#include "ResultPopup.hpp"

#include "Define.h"
#include "SceneManager.h"
#include "GameUIHelper.hpp"
#include "PopupManager.hpp"

#include "ExitAlertPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const float FIRST_TOUCH_LOCK_TIME                = 1.0f;

ResultPopup* ResultPopup::create(int level) {
    
    auto popup = new ResultPopup(level);
    
    if( popup && popup->init() ) {
        popup->autorelease();
        return popup;
    }
    
    CC_SAFE_DELETE(popup);
    return nullptr;
}

ResultPopup::ResultPopup(int level) : BasePopup(PopupType::GAME_RESULT),
level(level),
onClickListener(nullptr),
isTouchLocked(true) {
    
}

ResultPopup::~ResultPopup() {
    
}

bool ResultPopup::init() {
    
    if( !BasePopup::init() ) {
        return false;
    }
    
    return true;
}

void ResultPopup::onEnter() {
    
    BasePopup::onEnter();
    
    runEnterAction();
}

bool ResultPopup::onBackKeyReleased() {
    
    if( PopupManager::getPopupCount() > 1 || !PopupManager::exists(PopupType::GAME_RESULT) ) {
        return false;
    }
    
    // 광고가 열렸다 닫힌 경우 예외처리
    if( !Director::getInstance()->isValid() ) {
        return true;
    }
    
    // 화면 터치와 동일하게 처리
    if( !isTouchLocked ) {
        if( onClickListener ) {
            onClickListener();
        }
    }
    
    return true;
}

void ResultPopup::initBackgroundView() {
    
    BasePopup::initBackgroundView();
}

void ResultPopup::initContentView() {
    
    BasePopup::initContentView();
    
    // 진동
    Device::vibrate(0.1f);
    
    // 화면 터치 잠금
    isTouchLocked = true;
    
    SBDirector::postDelayed(this, [=]() {
        isTouchLocked = false;
    }, FIRST_TOUCH_LOCK_TIME, true);
    
    // 화면 터치 시 팝업 종료
    auto touchNode = SBNodeUtils::createTouchNode();
    addChild(touchNode);
    
    touchNode->addClickEventListener([=](Ref*) {
        // this->dismissWithAction();
        
        if( !isTouchLocked ) {
            if( onClickListener ) {
                onClickListener();
            }
        }
    });
    
    // UI
    const float TEXT_BOX_WIDTH = SB_WIN_SIZE.width * 0.9f;
    
    // 배경
    addChild(LayerColor::create(Color4B(20, 0, 255, 255)));
    
    // 타이틀
    // This Windows application has stopped responding th the system.
    {
        auto titleBg = LayerColor::create(Color4B(153, 154, 153, 255));
        titleBg->setIgnoreAnchorPointForPosition(false);
        titleBg->setAnchorPoint(ANCHOR_M);
        titleBg->setPosition(Vec2TC(0, -200));
        titleBg->setContentSize(Size(240, 60));
        addChild(titleBg);
        
        auto title = Label::createWithTTF("BERIES", FONT_COMMODORE, 40);
        title->setColor(Color3B(20, 0, 255));
        title->setAnchorPoint(ANCHOR_M);
        title->setPosition(Vec2MC(titleBg->getContentSize(), 0, 0));
        titleBg->addChild(title);
    }
    
    // desc1
    {
        string str[] = {
            "A problem has been detected\nand application has been\nshut down to prevent damage to your system.",
            "OUT_OF_MEMORY",
        };
        
        Vec2 pos = Vec2TC(0, -330);
        
        for( int i = 0; i < sizeof(str) / sizeof(string); ++i ) {
            auto desc = Label::createWithTTF(str[i], FONT_COMMODORE, 30, Size(TEXT_BOX_WIDTH, 0),
                                             TextHAlignment::LEFT, TextVAlignment::CENTER);
            desc->setColor(Color3B(255, 255, 255));
            desc->setAnchorPoint(ANCHOR_MT);
            desc->setPosition(pos);
            addChild(desc);
            
            pos.y -= desc->getContentSize().height;
            pos.y -= 60;
        }
    }
    
    //    auto desc1 = Label::createWithTTF("A problem has been detected\nand application has been\nshut down to prevent damage to your system.",
    //                                      FONT_RETRO, 30, Size(TEXT_BOX_WIDTH, 0),
    //                                      TextHAlignment::LEFT, TextVAlignment::CENTER);
    //    desc1->setColor(Color3B(255, 255, 255));
    //    desc1->setAnchorPoint(ANCHOR_M);
    //    desc1->setPosition(Vec2TC(0, -350));
    //    bg->addChild(desc1);
    
    // desc2
    {
        string str[] = {
            STR_FORMAT("Your Level is %d.", level),
            "",
            "Press any key to continue.",
        };
        
        // 시리즈 게임명 랜덤 설정
        StringList seriesGames = {
            "God of RPS",
            "Shaving My Head",
            "Pixel Puzzle: World Tour",
            "13 Number Puzzle",
        };
        
        str[1] = STR_FORMAT("Find the %s.", seriesGames[arc4random() % seriesGames.size()].c_str());
        
        Vec2 pos = Vec2MC(0, -20);
        
        for( int i = 0; i < sizeof(str) / sizeof(string); ++i ) {
            auto desc = Label::createWithTTF("* " + str[i], FONT_RETRO, 30, Size(TEXT_BOX_WIDTH, 40),
                                             TextHAlignment::LEFT, TextVAlignment::CENTER);
            desc->setColor(Color3B(255, 255, 255));
            desc->setAnchorPoint(ANCHOR_MT);
            desc->setPosition(pos);
            addChild(desc);
            
            desc->setOverflow(Label::Overflow::NONE);
            desc->setString(desc->getString());
            desc->setOverflow(Label::Overflow::SHRINK);
            
            pos.y -= desc->getContentSize().height;
            pos.y -= 10;
        }
    }
    
    // desc3
    {
        string str[] = {
            "Technical Information:",
            "*** STOP: 0x0419, 0x0820, 0x1004",
        };
        
        int fontSize[] = {
            30,
            26,
            30
        };
        
        Vec2 pos = Vec2BC(0, 350);
        
        for( int i = 0; i < sizeof(str) / sizeof(string); ++i ) {
            auto desc = Label::createWithTTF(str[i], FONT_RETRO, fontSize[i], Size(TEXT_BOX_WIDTH, 0),
                                             TextHAlignment::LEFT, TextVAlignment::CENTER);
            desc->setColor(Color3B(255, 255, 255));
            desc->setAnchorPoint(ANCHOR_MT);
            desc->setPosition(pos);
            addChild(desc);
            
            pos.y -= desc->getContentSize().height;
            pos.y -= 20;
        }
    }
    
    // desc4
    {
//        string str[] = {
//            "HAVE A NICE DAY! AMIGO!",
//        };
//
//        Vec2 pos = Vec2BC(0, 120);
//
//        for( int i = 0; i < sizeof(str) / sizeof(string); ++i ) {
//            auto desc = Label::createWithTTF(str[i], FONT_RETRO, 30, Size(TEXT_BOX_WIDTH, 0),
//                                             TextHAlignment::LEFT, TextVAlignment::CENTER);
//            desc->setColor(Color3B(255, 255, 255));
//            desc->setAnchorPoint(ANCHOR_MT);
//            desc->setPosition(pos);
//            addChild(desc);
//
//            pos.y -= desc->getContentSize().height;
//            pos.y -= 20;
//        }
    }
}

/**
 * 등장 연출
 */
void ResultPopup::runEnterAction(float duration, SBCallback onFinished) {
    
    BasePopup::runEnterAction(duration, onFinished);
    
    onEnterActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

void ResultPopup::runEnterAction(SBCallback onFinished) {
    
    runEnterAction(0, onFinished);
}

/**
 * 퇴장 연출
 */
void ResultPopup::runExitAction(float duration, SBCallback onFinished) {
    
    BasePopup::runExitAction(duration, onFinished);
    
    onExitActionFinished();
    SB_SAFE_PERFORM_LISTENER(this, onFinished);
}

void ResultPopup::runExitAction(SBCallback onFinished) {
    
    runExitAction(0, onFinished);
}

/**
 * 등장 연출 완료
 */
void ResultPopup::onEnterActionFinished() {
    
    BasePopup::onEnterActionFinished();
}
