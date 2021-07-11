//
//  GameView.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameView.hpp"

#include "Define.h"
#include "GameDefine.h"
#include "ContentManager.hpp"
#include "SceneManager.h"

#include "object/Bug.hpp"
#include "object/GameTimer.hpp"
#include "ui/BgBinary.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace cocos2d::ui;
using namespace std;

static const string SCHEDULER_BACKGROUND_BINARY = "SCHEDULER_BACKGROUND_BINARY";
static const float  SWATTER_MOVE_DURATION = 0.3f;

GameView::GameView() {
}

GameView::~GameView() {
}

bool GameView::init() {
    
    if( !Node::init() ) {
        return false;
    }
    
    setAnchorPoint(ANCHOR_M);
    setPosition(Vec2MC(0,0));
    setContentSize(SB_WIN_SIZE);
    
    initTouch();
    initBg();
    initObjects();
    initGameListener();
     
    return true;
}

void GameView::onEnter() {
    
    Log::i("GameView::onEnter");
    
    Node::onEnter();
}

void GameView::onEnterTransitionDidFinish() {
    
    Node::onEnterTransitionDidFinish();
}

void GameView::cleanup() {
    
    removeListeners(this);
    
    Node::cleanup();
}

#pragma mark- Game Event

/**
 * 게임 리셋
 */
void GameView::onGameReset() {
    
    life = 0;
    kills = 0;
    totalKills = 0;
}

/**
 * 게임 시작
 */
void GameView::onGameStart() {
    
    CCLOG("GameView::onGameStart");
    GameManager::onStageChanged();
}

/**
 * 게임 일시정지
 */
void GameView::onGamePause() {
    
    for( auto binary : bgBinarys ) {
        binary->pause();
    }
    
    for( auto bug : bugs ) {
        bug->pause();
    }
    
    timer->pause();
}

/**
 * 게임 재개
 */
void GameView::onGameResume() {
    
    for( auto binary : bgBinarys ) {
        binary->resume();
    }
    
    for( auto bug : bugs ) {
        bug->resume();
    }
    
    timer->resume();
}

/**
 * 게임 오버
 */
void GameView::onGameOver() {
    
    unschedule(SCHEDULER_BACKGROUND_BINARY);
    updateBackgroundBinary();
}

/**
 * 게임 결과
 */
void GameView::onGameResult() {
    
    removeChildByTag(Tag::BG_BINARY);
    bgBinarys.clear();
}

/**
 * 레벨 변경
 */
void GameView::onLevelChanged(const LevelData &level) {
    
    life = level.life;
    kills = 0;
    
    for( auto bug : bugs ) {
        bug->setEnabled(false);
        bug->setVisible(false);
    }
    
    for( auto life : lifes ) {
        life.setEnabled(false);
    }
    
    for( auto binary : bgBinarys ) {
        binary->switchMode(BgBinary::Mode::BINARY);
    }
    
    // 연출
    showLevelLabel([=]() {
        
        // 버그 업데이트
        for( int i = 0; i < level.bugs.size(); ++i ) {
            auto bug = bugs[i];
            bug->setData(level.bugs[i]);
            bug->setEnabled(true);
            bug->setVisible(true);
            bug->startTeleport();
        }
        
        // 라이프 업데이트
        for( int i = 0; i < level.life; ++i ) {
            auto life = lifes[i];
            life.setEnabled(true);
            life.setUsed(false);
        }
        
        // 타이머 재시작
        timer->restartTimer();
        this->schedule(CC_CALLBACK_1(GameView::updateBackgroundBinary, this), 1.0f, SCHEDULER_BACKGROUND_BINARY);
    });
}

/**
 * 스테이지 클리어
 */
void GameView::onLevelClear(const LevelData &stage) {
    
    unschedule(SCHEDULER_BACKGROUND_BINARY);
}

/**
 * 터치
 */
void GameView::onTouch(Touch *touch) {

    auto level = GAME_MANAGER->getLevelData();

    // 이미 레벨 클리어 or 게임 오버
    if( kills == level.bugs.size() || life == 0 ) {
        return;
    }
    
    // 버그 영역 판정
    vector<Bug*> killedBugs;
    
    for( int i = 0; i < level.bugs.size(); ++i ) {
        auto bug = bugs[i];
        
        if( bug->isEnabled() && bug->isTouchInside(touch) ) {
            killedBugs.push_back(bug);
        }
    }

    // 라이프 차감
    --life;
    
    int lifeIdx = level.life - life- 1;
    lifes[lifeIdx].setUsed(true);
    
    // 버그 퇴치!
    const int killBugCount = (int)killedBugs.size();
    Vec2 effectPos = touch->getLocation();
    
    if( killBugCount == 1 ) {
        auto box = killedBugs[0]->getBoundingBoxInWorld();
        effectPos.x = box.getMidX();
        effectPos.y = box.getMidY();
    }
    
    if( killBugCount > 0 ) {
        // disable bugs
        for( auto bug : killedBugs ) {
            bug->stopTeleport();
            bug->setEnabled(false);
        }
        
        // kill bugs
        killBugs(killedBugs, effectPos);
        
        kills += killBugCount;
        totalKills += killBugCount;
        
        auto killsLabel = getChildByTag<Label*>(Tag::KILLS);
        killsLabel->setString(STR_FORMAT("%d KILLS", totalKills));
        
        // 레벨 클리어
        if( kills == level.bugs.size() ) {
            // stop timer
            timer->stopTimer();
            
            auto touchNode = SBNodeUtils::createTouchNode();
            addChild(touchNode, SBZOrder::BOTTOM);
            
            SBDirector::postDelayed(this, [=]() {
                touchNode->removeFromParent();
                GameManager::onStageClear();
            }, (SWATTER_MOVE_DURATION*2) + 1.0f);
        }
        
        // + 연출
        SBDirector::postDelayed(this, [=]() {
            auto effectLabel = Label::createWithTTF(STR_FORMAT("+%d", killBugCount), FONT_SABO, 60,
                                                    Size::ZERO,
                                                    TextHAlignment::CENTER, TextVAlignment::CENTER);
            effectLabel->setAnchorPoint(ANCHOR_M);
            effectLabel->setPosition(effectPos + Vec2(0, 50));
            effectLabel->setTextColor(Color::HIGHLIGHT);
            // effectLabel->setTextColor(Color4B::WHITE);
            this->addChild(effectLabel, SBZOrder::BOTTOM);
            
            auto move = MoveBy::create(0.8f, Vec2(0, 100));
            auto remove = RemoveSelf::create();
            effectLabel->runAction(Sequence::create(move, remove, nullptr));
        }, SWATTER_MOVE_DURATION+0.1f);
    }
    // MISS
    else {
        CCLOG("MISS TOUCH");
        
        // 게임 오버
        if( life == 0 ) {
            GameManager::onGameOver(false);
        }
     
        // MISS 연출
        auto effectLabel = Label::createWithTTF("MISS", FONT_SABO, 45,
                                                Size::ZERO,
                                                TextHAlignment::CENTER, TextVAlignment::CENTER);
        effectLabel->setAnchorPoint(ANCHOR_M);
        effectLabel->setPosition(effectPos);
        effectLabel->setTextColor(Color::HIGHLIGHT);
        addChild(effectLabel, SBZOrder::BOTTOM);
        
        // fade
        effectLabel->setOpacity(0);
        
        auto fadeIn = FadeIn::create(0.1f);
        auto fadeOut = FadeOut::create(0.1f);
        auto callFunc = CallFunc::create([=]() {
            
            effectLabel->removeFromParent();
        });
        effectLabel->runAction(Sequence::create(fadeIn, DelayTime::create(0.3f),
                                                fadeOut, /*DelayTime::create(0.2f),*/
                                                callFunc, nullptr));
        // scale
        effectLabel->setScale(0.3f);
        
        auto scale1 = ScaleTo::create(0.1f, 1.1f);
        auto scale2 = ScaleTo::create(0.1f, 0.9f);
        auto scale3 = ScaleTo::create(0.1f, 1.0f);
        effectLabel->runAction(Sequence::create(scale1, scale2, scale3, nullptr));
    }
}

/**
 * 버그 퇴치!
 */
void GameView::killBugs(vector<Bug*> bugs, const Vec2 &effectPos) {
    
    CCASSERT(bugs.size() > 0, "GameView::killBugs error.");
    
    CCLOG("killBugs");
    
    // 파리채 연출
    const float swatterWidth = 210;
    
    auto swatter = Sprite::create(DIR_IMG_GAME + "fly_swatter.png");
    swatter->setAnchorPoint(ANCHOR_MB);
    swatter->setScale(swatterWidth / swatter->getContentSize().width);
    addChild(swatter, SBZOrder::BOTTOM);
    
    // 파리채 연출
    const float swatterScale = swatter->getScale();
    const float swatterHeight = swatter->getContentSize().height * swatterScale;
    const Vec2  swatterPos = Vec2(effectPos.x, -swatterHeight);
    
    swatter->setScaleY(swatterScale * 0.2f);
    swatter->setPosition(swatterPos);
    
    const float duration = SWATTER_MOVE_DURATION;
    
    // 화면 밖에서 버그까지 이동
    auto move1 = MoveTo::create(duration, Vec2(swatterPos.x,
                                               effectPos.y - swatterHeight + 110));
    auto scale1 = ScaleTo::create(duration, swatterScale);
    
    auto moveCompleted = CallFunc::create([=]() {
        
        // 버그 사망
        for( auto bug : bugs ) {
            bug->die();
        }
        
        SBAudioEngine::playEffect(DIR_SOUND + "snd_slap.mp3");
    });
    
    // 다시 화면 밖으로 퇴장
    auto move2 = MoveTo::create(duration, swatterPos);
    auto scale2 = ScaleTo::create(duration, swatterScale, swatterScale * 0.2f);
    auto actionCompleted = CallFunc::create([=]() {
    });
    
//    swatter->runAction(Sequence::create(Spawn::create(move1, scale1, nullptr),
//                                        moveCompleted,
//                                        nullptr));

    swatter->runAction(Sequence::create(Spawn::create(move1, scale1, nullptr),
                                        moveCompleted,
                                        DelayTime::create(0.05f),
                                        Spawn::create(move2, scale2, nullptr),
                                        actionCompleted,
                                        RemoveSelf::create(),
                                        nullptr));
    
    // 파리채 영역 확인
    /*
     auto swatterArea = LayerColor::create(Color4B(255, 0, 0, 255*0.4f));
     swatterArea->setIgnoreAnchorPointForPosition(false);
     swatterArea->setAnchorPoint(swatter->getAnchorPoint());
     swatterArea->setPosition(swatter->getPosition());
     swatterArea->setContentSize(swatter->getContentSize());
     swatterArea->setScale(swatter->getScale());
     addChild(swatterArea, SBZOrder::BOTTOM-1);
     */
}

/**
 * 배경 바이너리 업데이트
 */
void GameView::updateBackgroundBinary(float dt) {
    
    // 남은 바이너리 리스트
    auto remainBinarys = SBCollection::find(bgBinarys, [](BgBinary *b) -> bool {
        return b->getMode() == BgBinary::Mode::BINARY;
    });
    
    random_shuffle(remainBinarys.begin(), remainBinarys.end());
    
    // 버그 갯수 설정
    float timeRatio = timer->getTimeRatio();
    float f = 1 - timeRatio;
    int totalBugCount = bgBinarys.size() * (f*f);
    
    size_t newBugCount = totalBugCount - (bgBinarys.size() - remainBinarys.size());
    newBugCount = MIN(remainBinarys.size(), newBugCount);
    
    // 버그로 전환
    for( int i = 0; i < newBugCount; ++i ) {
        auto binary = remainBinarys[i];
        binary->switchMode(BgBinary::Mode::BUG);
    }
    
//    CCLOG("[LEVEL%d] updateBackgroundBinary ratio: %f totalBugCount: %d newBugCount: %d remainBinarys: %d",
//          GameManager::getLevel(),
//          timeRatio, totalBugCount, newBugCount, (int)remainBinarys.size());
}

/**
 * 레벨 표시
 */
void GameView::showLevelLabel(SBCallback onCompleted) {
    
    string str = STR_FORMAT("LEVEL %d", GAME_MANAGER->getLevel());
    
    // 11레벨 부터는 확률적으로 랜덤한 특수 문자를 섞는다
    /*
    if( level.level >= 11 && arc4random() % 2 == 0 ) {
        static vector<string> errorStr({ "!","@","#","$","%","^","&","*","?" });
        
        vector<string> arr = SBStringUtils::toArray(str);
        
        for( int i = 0; i < arr.size(); ++i ) {
            int ran = arc4random() % 3;
            
            if( ran == 0 ) {
                arr[i] = errorStr[arc4random() % errorStr.size()];
            }
        }
        
        str = SBStringUtils::toString(arr);
    }
    */
    
    SB_SAFE_HIDE(getChildByTag(Tag::BG_BINARY));
    
    // 연출 레이어
    auto effectLayer = SBNodeUtils::createTouchNode(Color::GAME_BG);
    addChild(effectLayer, SBZOrder::BOTTOM);
    
    // 레벨 Label
    auto levelLabel = Label::createWithTTF(str, FONT_SABO, 70);
    levelLabel->setAnchorPoint(ANCHOR_M);
    levelLabel->setPosition(Vec2MC(0, 0));
    levelLabel->setColor(Color3B(255, 255, 255));
    effectLayer->addChild(levelLabel);
    
    // fade
    levelLabel->setOpacity(0);
    
    auto fadeIn = FadeIn::create(0.15f);
    auto fadeOut = FadeOut::create(0.2f);
    auto callFunc = CallFunc::create([=]() {
        
        SB_SAFE_SHOW(getChildByTag(Tag::BG_BINARY));
        
        if( onCompleted ) {
            onCompleted();
        }
        
        effectLayer->removeFromParent();
    });
    levelLabel->runAction(Sequence::create(fadeIn, DelayTime::create(0.6f),
                                           fadeOut, /*DelayTime::create(0.2f),*/
                                           callFunc, nullptr));
    // scale
    levelLabel->setScale(0.3f);
    
    auto scale1 = ScaleTo::create(0.15f, 1.12f);
    auto scale2 = ScaleTo::create(0.1f, 0.9f);
    auto scale3 = ScaleTo::create(0.1f, 1.0f);
    levelLabel->runAction(Sequence::create(scale1, scale2, scale3, nullptr));
}

/**
 * 터치 이벤트 초기화
 */
void GameView::initTouch() {
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [=](Touch *touch, Event*) -> bool {
        
        // 멀티 터치 2개까지로 제한
        if( touch->getID() >= 2  ) {
            return true;
        }
        
        this->onTouch(touch);
        
        return true;
    };
    listener->onTouchEnded = [=](Touch *touch, Event*) {
        
    };
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

/**
 * 배경 초기화
 */
void GameView::initBg() {
    
    // 배경
    addChild(LayerColor::create(Color::GAME_BG));
    
    // 상단 배너 영역 확인
    /*
    auto banner = Sprite::create(DIR_IMG_GAME + "RSP_ad_top.png");
    banner->setAnchorPoint(ANCHOR_MT);
    banner->setPosition(Vec2TC(0,0));
    addChild(banner, 1);
     */
    
    // 이진수 배경
    auto layer = SBNodeUtils::createZeroSizeNode();
    layer->setTag(Tag::BG_BINARY);
    addChild(layer);
    
    const Vec2 ORIGIN(2,4);
    const float WIDTH = 34;
    const float HEIGHT = 34;
    const int   FONT_SIZE = HEIGHT / 1.1f;
    const int   ROWS = SB_WIN_SIZE.width / WIDTH;
    const int   COLUMN = SB_WIN_SIZE.height / HEIGHT;
    
    for( int x = 0; x < ROWS; ++x ) {
        for( int y = 0; y < COLUMN; ++y ) {
            // 중앙 타이머 표기를 위한 빈자리
//            if( x >= 5 && x <= 9 &&
//                y >= 12 && y <= 13 ) {
//                continue;
//            }
            
            auto pos = ORIGIN + Vec2(WIDTH*x, HEIGHT*y);

            if( x % 2 != 0 ) {
                pos.y += HEIGHT*0.5f;
            }
            
            auto binary = BgBinary::create(x, y, WIDTH, HEIGHT, FONT_SIZE);
            binary->setAnchorPoint(ANCHOR_BL);
            binary->setPosition(pos);
            layer->addChild(binary);
            
            bgBinarys.push_back(binary);
        }
    }
    
    // KILLS
    /*
     {
     auto n = LayerColor::create(Color4B(255,0,0,255*0.5f));
     n->setIgnoreAnchorPointForPosition(false);
     n->setAnchorPoint(ANCHOR_ML);
     n->setPosition(Vec2(13,30));
     n->setContentSize(Size(330, 65));
     addChild(n);
     }
     */
    
    auto killsLabel = Label::createWithTTF("0 KILLS", FONT_SABO, 42,
                                           Size(330, 0),
                                           TextHAlignment::LEFT, TextVAlignment::CENTER);
    killsLabel->setTag(Tag::KILLS);
    killsLabel->setAnchorPoint(ANCHOR_BL);
    killsLabel->setPosition(Vec2(13, 26));
    // killsLabel->setTextColor(Color::HIGHLIGHT);
    killsLabel->setTextColor(Color4B::WHITE);
//    killsLabel->enableOutline(Color4B::WHITE, 4);
    addChild(killsLabel);
}

/**
 * 오브젝트 초기화
 */
void GameView::initObjects() {
    
    // 버그
    for( int i = 0; i < GAME_CONFIG->getMaxBug(); ++i ) {
        auto bug = Bug::create();
        bug->setEnabled(false);
        addChild(bug, SBZOrder::BOTTOM);
        
        bugs.push_back(bug);
    }
    
    // 타이머
    timer = GameTimer::create();
    addChild(timer);
    
    timer->setOnTimeOverListener([=]() {
        GameManager::onGameOver(true);
    });
    
    // 라이프, 오른쪽 가장자리부터 생성
    const Vec2  POS = Vec2BR(-39, 51);
    const float PADDING_X = -52;
    
    for( int i = 0; i < GAME_CONFIG->getMaxLife(); ++i ) {
        Life life;
        
        // unused icon
        life.unusedIcon = Sprite::create(DIR_IMG_GAME + "fly_swatter_small.png");
        life.unusedIcon->setAnchorPoint(ANCHOR_M);
        life.unusedIcon->setPosition(POS + Vec2(PADDING_X*i, 0));
        addChild(life.unusedIcon);

        // used icon
        auto stencil = Sprite::create(DIR_IMG_GAME + "fly_swatter_small.png");
        stencil->setAnchorPoint(Vec2::ZERO);
        stencil->setPosition(Vec2::ZERO);
        
        auto clippingNode = ClippingNode::create(stencil);
        clippingNode->setContentSize(life.unusedIcon->getContentSize());
        clippingNode->setAlphaThreshold(0);
        
        clippingNode->addChild(SBNodeUtils::createBackgroundNode(clippingNode, Color4B(50,50,50,255)));
        
        life.usedIcon = clippingNode;
        life.usedIcon->setAnchorPoint(life.unusedIcon->getAnchorPoint());
        life.usedIcon->setPosition(life.unusedIcon->getPosition());
        addChild(life.usedIcon);
        
        life.setEnabled(false);
        lifes.push_back(life);
        
        /*
        auto spr = EffectSprite::create(DIR_IMG_GAME + "fly_swatter_small.png");
        spr->setAnchorPoint(ANCHOR_M);
        spr->setPosition(POS + Vec2(PADDING_X*i, 0));
        addChild(spr);
        
        spr->setEffect(Effect::create("shaders/example_GreyScale.fsh"));
        */
    }
}

/**
 * 게임 이벤트 리스너 초기화
 */
void GameView::initGameListener() {
    
    StringList events({
        GAME_EVENT_RESET,
        GAME_EVENT_START,
        GAME_EVENT_PAUSE,
        GAME_EVENT_RESUME,
        GAME_EVENT_OVER,
        GAME_EVENT_RESULT,
        GAME_EVENT_STAGE_CHANGED,
        GAME_EVENT_STAGE_CLEAR,
    });
    
    GameManager::addEventListener(events, [=](GameEvent event, void *userData) {
        
        switch( event ) {
            case GameEvent::RESET:     this->onGameReset();         break;
            case GameEvent::START:     this->onGameStart();         break;
            case GameEvent::PAUSE:     this->onGamePause();         break;
            case GameEvent::RESUME:    this->onGameResume();        break;
            case GameEvent::OVER:      this->onGameOver();          break;
            case GameEvent::RESULT:    this->onGameResult();        break;
                
            case GameEvent::STAGE_CHANGED: {
                auto level = (LevelData*)userData;
                this->onLevelChanged(*level);
            } break;
                
            case GameEvent::STAGE_CLEAR: {
                auto level = (LevelData*)userData;
                this->onLevelClear(*level);
            } break;
                
            default: break;
        }
    }, this);
}
