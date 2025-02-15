//
//  GameScene.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameScene.hpp"
#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "BannerView.hpp"

#include "GameManager.hpp"
#include "GameView.hpp"

#include "CommonLoadingBar.hpp"
#include "SettingPopup.hpp"

#include "ui/ResultPopup.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

GameScene* GameScene::create() {
    
    auto scene = new GameScene();
    
    if( scene && scene->init() ) {
        scene->autorelease();
        return scene;
    }
    
    delete scene;
    return nullptr;
}

GameScene::GameScene() :
gameView(nullptr) {
}

GameScene::~GameScene() {
}

bool GameScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    SBAnalytics::setCurrentScreen(ANALYTICS_SCREEN_GAME);
    
    initBg();
    initGameView();
    initMenu();
    initGameListener();
    
    // 멀티 터치 방지
    addChild(SBNodeUtils::createSwallowMultiTouchNode(), INT_MAX);
    
    // 개발 버전 표기
    /*
    auto versionLabel = Label::createWithTTF(DEV_VERSION, FONT_ROBOTO_BLACK, 30, Size::ZERO,
                                             TextHAlignment::RIGHT, TextVAlignment::BOTTOM);
    versionLabel->setTextColor(Color4B::WHITE);
    versionLabel->setAnchorPoint(ANCHOR_BR);
    versionLabel->setPosition(Vec2BR(0,120));
    addChild(versionLabel, INT_MAX);
     */
    
    return true;
}

void GameScene::onEnter() {
    
    BaseScene::onEnter();
}

void GameScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // SBAudioEngine::playBGM(SOUND_BGM_GAME);
    
    GameManager::onGameEnter();
    GameManager::onGameStart();
}

void GameScene::onExit() {
    
    BaseScene::onExit();
}

void GameScene::cleanup() {
    
    BaseScene::cleanup();
}

bool GameScene::onApplicationEnterBackground() {
    
    if( !BaseScene::onApplicationEnterBackground() ) {
        return false;
    }
    
    GameManager::onGamePause();
    
    return true;
}

bool GameScene::onApplicationEnterForeground() {
    
    if( !BaseScene::onApplicationEnterForeground() ) {
        return false;
    }
    
    GameManager::onGameResume();
    
    return true;
}

bool GameScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    // 설정 팝업 생성
    /*
    if( PopupManager::getInstance()->getPopupCount() == 0 ) {
        onClick(getChildByTag(Tag::BTN_SETTING));
        return true;
    }
    */
    
    return false;
}

/**
 * 게임 리셋
 */
void GameScene::onGameReset() {
 
    setSceneTouchLocked(false);
}

/**
 * 게임 일시정지
 */
void GameScene::onGamePause() {
    
    setSceneTouchLocked(true);
    
    /*
    if( !GameManager::getInstance()->hasState(GameState::GAME_OVER) &&
        !GameManager::getInstance()->hasState(GameState::RESULT) ) {
        showPausePopup();
    }
    */
}

/**
 * 게임 재게
 */
void GameScene::onGameResume() {
    
    setSceneTouchLocked(false);
}

/**
 * 게임 오버
 */
void GameScene::onGameOver() {
    
    setSceneTouchLocked(true);
    
    SBDirector::postDelayed(this, [=]() {
        
        GameManager::onGameResult();
        
    }, GAME_RESULT_DELAY, true);
}

/**
 * 게임 결과
 */
void GameScene::onGameResult() {
    
    showResultPopup();
}

/**
 * 레벨 클리어
 */
void GameScene::onLevelClear(const LevelData &level) {
    
    if( GAME_MANAGER->getLevel() % 10 == 0 && AdsHelper::getInstance()->isInterstitialLoaded() ) {
        AdsHelper::getInstance()->showInterstitial();
    }
}

/**
 * Scene 전환
 */
void GameScene::replaceScene(SceneType type) {
    
    GameManager::onGameExit();
    GameManager::destroyInstance();
    removeListeners(this);
    
    BaseScene::replaceScene(type);
}

/**
 * 게임 결과 팝업 노출
 */
void GameScene::showResultPopup() {
 
    // 배너 숨김
    removeChildByTag(Tag::BANNER);
    AdsHelper::getInstance()->hideBanner();
    
    // 팝업
    auto popup = ResultPopup::create(GAME_MANAGER->getLevel());
    popup->setOnPopupEventListener([=](Node *sender, PopupEventType eventType) {
    });
    popup->setOnClickListener([=]() {
        this->replaceScene(SceneType::MAIN);
    });
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_BOTTOM);
}

/**
 * 리뷰 체크
 * 조건 충족 시 리뷰 작성 알림 팝업 노출
 */
void GameScene::checkReview() {
    
    if( /*리뷰 노출 조건*/ false ) {
        User::checkReview(0.5f);
    }
}

/**
 * 버튼 클릭
 */
void GameScene::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        case Tag::BTN_SETTING: {
            auto popup = SettingPopup::create();
            popup->setOnHomeListener([=]() {
                this->replaceScene(SceneType::MAIN);
            });
            SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
        } break;
    }
}

/**
 * 배경 초기화
 */
void GameScene::initBg() {
    
    addChild(LayerColor::create(Color4B(GAME_BG_COLOR)));
    
    // 배너
    if( !User::isRemovedAds() ) {
        auto bannerView = BannerView::create();
        bannerView->setTag(Tag::BANNER);
        addChild(bannerView, SBZOrder::TOP);
    }
}

/**
 * 게임뷰 초기화
 */
void GameScene::initGameView() {
    
    gameView = GameView::create();
    addChild(gameView);
    
    // GAME_MANAGER->setView(gameView);
}

/**
 * 메뉴 초기화
 */
void GameScene::initMenu() {
    
    // 설정 버튼
    /*
    auto settingBtn = SBButton::create(DIR_IMG_COMMON + "common_btn_more.png");
    settingBtn->setTag(Tag::BTN_SETTING);
    settingBtn->setZoomScale(ButtonZoomScale::NORMAL);
    settingBtn->setAnchorPoint(ANCHOR_M);
    settingBtn->setPosition(Vec2TR(-56, -54));
    addChild(settingBtn);
    
    settingBtn->setOnClickListener(CC_CALLBACK_1(GameScene::onClick, this));
     */
}

/**
 * 게임 이벤트 리스너 초기화
 */
void GameScene::initGameListener() {

    StringList events({
        GAME_EVENT_RESET,
        GAME_EVENT_PAUSE,
        GAME_EVENT_RESUME,
        GAME_EVENT_OVER,
        GAME_EVENT_RESULT,
        GAME_EVENT_STAGE_CLEAR,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::RESET:          this->onGameReset();         break;
            case GameEvent::PAUSE:          this->onGamePause();         break;
            case GameEvent::RESUME:         this->onGameResume();        break;
            case GameEvent::OVER:           this->onGameOver();          break;
            case GameEvent::RESULT:         this->onGameResult();        break;

            case GameEvent::STAGE_CLEAR: {
                auto level = (LevelData*)userData;
                this->onLevelClear(*level);
            } break;
                
            default: break;
        }
    }, this);
}
