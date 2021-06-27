//
//  MainScene.cpp
//
//  Created by seongmin hwang on 2018. 5. 14..
//

#include "MainScene.hpp"

#include "Define.h"
#include "User.hpp"
#include "SceneManager.h"
#include "PopupManager.hpp"
#include "GameUIHelper.hpp"

#include "../test/TestHelper.hpp"
#include "../game/GameManager.hpp"
#include "../game/GameDefine.h"

#include "CommonLoadingBar.hpp"
#include "ExitAlertPopup.hpp"
#include "SettingPopup.hpp"
#include "BannerView.hpp"

#include "../game/object/Bug.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

MainScene::MainScene() {
    
}

MainScene::~MainScene() {
    
}

bool MainScene::init() {
    
    if( !BaseScene::init() ) {
        return false;
    }
    
    SBAnalytics::setCurrentScreen(ANALYTICS_SCREEN_MAIN);
    
    initBg();
    initMenu();
    
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

void MainScene::onEnter() {
    
    BaseScene::onEnter();
    
    // bgm
    scheduleOnce([=](float) {
        // SBAudioEngine::playBGM(SOUND_BGM_MAIN);
    }, SceneManager::REPLACE_DURATION_MAIN, "MAIN_SCENE_BGM_DELAY");
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
    }
}

void MainScene::onEnterTransitionDidFinish() {
    
    BaseScene::onEnterTransitionDidFinish();
    
    // 개인 정보 처리 방침 안내 팝업
    if( !ConsentManager::isPrivacyPolicyChecked() ) {
        SBDirector::getInstance()->setScreenTouchLocked(false);
        
        ConsentForm::create()
        ->setTitle("Important")
        ->setMessage("We don't collect your personal data,\nhowever our ads suppliers may use\nthem to personalise ads for you. You'll\nfind links to our partner's privacy\npolicies in our Privacy Policy.\n\nYou can opt-out ads tracking from\ngame & device settings.")
        ->setPolicyUrl("http://www.3eries.com/privacy_policy")
        ->show();
    }
}

void MainScene::onExit() {
    
    BaseScene::onExit();
}

bool MainScene::onBackKeyReleased() {
    
    if( !BaseScene::onBackKeyReleased() ) {
        return false;
    }
    
    // 앱 종료 알림 팝업 생성
    if( PopupManager::getInstance()->getPopupCount() == 0 ) {
        SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
        
        auto popup = (ExitAlertPopup*)PopupManager::show(PopupType::EXIT_APP);
        popup->setOnExitAppListener([=]() {
            SBSystemUtils::exitApp();
        });
        
        return true;
    }
    
    return false;
}

/**
 * 버튼 클릭
 */
void MainScene::onClick(Node *sender) {
    
    SBAudioEngine::playEffect(SOUND_BUTTON_CLICK);
    
    switch( sender->getTag() ) {
        // 크레딧
        case Tag::BTN_CREDIT: {
            SBAnalytics::logEvent(ANALYTICS_EVENT_CREDIT);
            
            auto popup = PopupManager::show(PopupType::CREDIT);
            popup->setLocalZOrder(ZOrder::POPUP_TOP);
            popup->setOnDismissListener([=](Node*) {
            });
        } break;
            
        // 게임 시작
        case Tag::BTN_START: {
            replaceGameScene();
        } break;

        // 리더 보드
        case Tag::BTN_LEADER_BOARD: {
            if( superbomb::PluginPlay::isSignedIn() ) {
                superbomb::PluginPlay::showAllLeaderboards();
            } else {
                superbomb::PluginPlay::signIn();
            }
        } break;
            
        // 설정
        case Tag::BTN_SETTING: {
            showSettingPopup();
        } break;
            
        // test
        case Tag::BTN_TEST: {
        } break;
            
        default:
            break;
    }
}

/**
 * 게임씬으로 전환
 */
void MainScene::replaceGameScene() {
    
    SB_SAFE_HIDE(getChildByTag(Tag::BTN_SETTING));
    
    auto onAdClosed = [=]() {
        GAME_MANAGER->init();
        replaceScene(SceneType::GAME);
    };
    
    // 최초 실행이 아닐때 전면 광고 노출
    if( !SBDirector::isFirstRun() &&
        !User::isRemovedAds() && AdsHelper::isInterstitialLoaded() ) {
        SBDirector::getInstance()->setScreenTouchLocked(true);
        
        auto listener = AdListener::create(AdType::INTERSTITIAL);
        listener->setTarget(this);
        listener->onAdClosed = onAdClosed;
        AdsHelper::showInterstitial(listener);
        
    } else {
        onAdClosed();
    }
}

/**
 * 설정 팝업 노출
 */
void MainScene::showSettingPopup() {
    
    auto popup = SettingPopup::create();
    SceneManager::getScene()->addChild(popup, ZOrder::POPUP_MIDDLE);
}

void MainScene::initBg() {
    
    addChild(LayerColor::create(Color::GAME_BG));
    
    auto title = Sprite::create(DIR_IMG_MAIN + "title.png");
    title->setAnchorPoint(ANCHOR_M);
    title->setPosition(Vec2TC(0, -369));
    addChild(title);
    
    // Bug
    auto bug = Bug::create();
    bug->setAnchorPoint(ANCHOR_M);
    bug->setPosition(Vec2BC(0, 226));
    bug->setScale(0.25f);
    addChild(bug);
    
    // Balloon
    auto balloon = Sprite::create(DIR_IMG_MAIN + "balloon.png");
    balloon->setAnchorPoint(ANCHOR_M);
    balloon->setPosition(Vec2BC(0, 365));
    addChild(balloon);
    
    // 배너
    if( !User::isRemovedAds() ) {
        auto bannerView = BannerView::create();
        addChild(bannerView, SBZOrder::TOP);
    }
    
    // 크레딧
    auto creditBtn = SBNodeUtils::createTouchNode();
    creditBtn->setTag(Tag::BTN_CREDIT);
    creditBtn->setAnchorPoint(ANCHOR_M);
    creditBtn->setPosition(Vec2TC(0, -369 + 40));
    creditBtn->setContentSize(Size(title->getContentSize().width, 200));
    addChild(creditBtn, SBZOrder::BOTTOM);
    
    creditBtn->addClickEventListener([=](Ref*) {
        this->onClick(creditBtn);
    });
}

/**
 * 메뉴 초기화
 */
void MainScene::initMenu() {
    
    auto btn = SBNodeUtils::createTouchNode();
    btn->setTag(Tag::BTN_START);
    btn->setAnchorPoint(ANCHOR_M);
    btn->setPosition(Vec2MC(0,0));
    btn->setContentSize(Size(SB_WIN_SIZE.width*0.8f, SB_WIN_SIZE.height*0.8f));
    addChild(btn);
    
    btn->addClickEventListener([=](Ref*) {
        this->onClick(btn);
    });
}
