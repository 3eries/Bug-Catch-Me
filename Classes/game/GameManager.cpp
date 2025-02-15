//
//  GameManager.cpp
//  Thirteen-mobile
//
//  Created by seongmin hwang on 19/08/2019.
//

#include "GameManager.hpp"

#include "UserDefaultKey.h"
#include "User.hpp"
#include "TestHelper.hpp"

#include "GameView.hpp"

USING_NS_CC;
USING_NS_SB;
using namespace std;

static GameManager *instance = nullptr;
GameManager* GameManager::getInstance() {
    
    if( !instance ) {
        instance = new GameManager();
    }
    
    return instance;
}

void GameManager::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

bool GameManager::isNullInstance() {
    return instance == nullptr;
}

GameManager::GameManager() {
}

GameManager::~GameManager() {
}


void GameManager::init() {
    
    reset();
}

void GameManager::reset() {
    
    CCLOG("GameManager::reset");
    
    state = GameState::NONE;
    score = 0;
    level = 1;
    resultCount = 0;
    continueCount = 0;
}

/**
 * 게임 상태를 변경합니다
 */
void GameManager::setState(GameState state) {
    this->state = state;
}

void GameManager::addState(GameState state) {
    this->state = (GameState)(this->state | state);
}

void GameManager::removeState(GameState state) {
    this->state = (GameState)(this->state & ~state);
}

bool GameManager::hasState(GameState state) {
    return (this->state & state) == state;
}

/**
 * 게임 플레이중인지 반환합니다
 */
bool GameManager::isPlaying() {
    
    return hasState(GameState::STARTED) &&
    !hasState(GameState::PAUSED) &&
    !hasState(GameState::GAME_OVER) &&
    !hasState(GameState::RESULT);
}

/**
 * 게임이 일시정지 되었는지 반환합니다
 */
bool GameManager::isPaused() {
    
    return hasState(GameState::PAUSED);
}

LevelData GameManager::getLevelData() {
    
    int level = MIN(GAME_CONFIG->getMaxLevel().level, getLevel());
    return GAME_CONFIG->getLevel(level);
}

/**
 * 스코어를 설정합니다
 */
void GameManager::setScore(int score) {
    
    score = MIN(GAME_CONFIG->getMaxScore(), score);
    instance->score = score;
    
    onScoreChanged();
}

void GameManager::addScore(int score) {
    
    setScore(instance->score + score);
}

int GameManager::getPlayCount() {
    return UserDefault::getInstance()->getIntegerForKey(UserDefaultKey::PLAY_COUNT, 0);
}

bool GameManager::isContinuable() {
    return instance->getContinueCount() == 0;
}

#pragma mark- Game Event

void GameManager::addEventListener(StringList events, GameEventListener gameEventListener,
                                   Node *target) {
    
    for( string eventName : events ) {
        auto listener = EventListenerCustom::create(eventName, [=](EventCustom *event) {
            gameEventListener(GAME_EVENT_ENUMS[eventName], event->getUserData());
        });
        getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, target);
    }
}

EventDispatcher* GameManager::getEventDispatcher() {
    
    return Director::getInstance()->getEventDispatcher();
}

/**
 * 게임 진입
 */
void GameManager::onGameEnter() {
    
    // log
    {
        string log;
        log += "\t" + STR_FORMAT("play count: %d", getPlayCount()) + "\n";
        
        Log::i("GameManager::onGameEnter\n{\n%s}", log.c_str());
    }
    
    instance->setState(GameState::ENTERED);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_ENTER);
}

/**
 * 게임 퇴장
 */
void GameManager::onGameExit() {
    
    Log::i("GameManager::onGameExit");
    
    onGamePause();
    
    instance->reset();
    instance->setState(GameState::EXITED);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_EXIT);
}

/**
 * 게임 리셋
 */
void GameManager::onGameReset() {
    
    Log::i("GameManager::onGameReset");
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_RESET);
}

/**
 * 게임 시작
 */
void GameManager::onGameStart() {
    
    Log::i("GameManager::onGameStart start");
    
    SBAnalytics::logEvent(ANALYTICS_EVENT_GAME_PLAY);
    
    onGameReset();
    
    UserDefault::getInstance()->setIntegerForKey(UserDefaultKey::PLAY_COUNT, getPlayCount()+1);
    UserDefault::getInstance()->flush();
    
    instance->setState(GameState::STARTED);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_START);
    
    Log::i("GameManager::onGameStart end");
}

/**
 * 게임 재시작
 */
void GameManager::onGameRestart() {
    
    CCLOG("GameManager::onGameRestart start");
    
    instance->reset();
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_RESTART);
    
    onGameStart();
    
    CCLOG("GameManager::onGameRestart end");
}

/**
 * 게임 일시정지
 */
void GameManager::onGamePause() {
    
    CCLOG("GameManager::onGamePause state is paused: %d", instance->hasState(GameState::PAUSED));
    
    if( instance->hasState(GameState::PAUSED) ) {
        return;
    }
    
    instance->addState(GameState::PAUSED);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_PAUSE);
}

/**
 * 게임 재개
 */
void GameManager::onGameResume() {
    
    CCLOG("GameManager::onGameResume state is paused: %d", instance->hasState(GameState::PAUSED));
    
    if( !instance->hasState(GameState::PAUSED) ) {
        return;
    }
    
    instance->removeState(GameState::PAUSED);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_RESUME);
}

/**
 * 게임 오버
 */
void GameManager::onGameOver(bool isTimeout) {
    
    Log::i("GameManager::onGameOver isTimeout: %d", isTimeout);
    
    CCASSERT(instance->hasState(GameState::STARTED), "GameManager::onGameOver invalid called.");
    
    instance->addState(GameState::GAME_OVER);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_OVER);
    
    instance->resultCount++;
}

/**
 * 게임 이어하기
 */
void GameManager::onGameContinue() {
    
    Log::i("GameManager::onGameContinue start");
    CCASSERT(instance->hasState(GameState::GAME_OVER), "GameManager::onGameContinue invalid called.");
    
    onGameReset();
    
    instance->continueCount++;
    
    instance->removeState(GameState::GAME_OVER);
    instance->addState(GameState::CONTINUE);
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_CONTINUE);
    
    Log::i("GameManager::onGameContinue end");
}

/**
 * 게임 결과
 */
static void logEventGameResult(int level) {
    
    if( level < 1 ) {
        return;
    }
    
    // firebase 이벤트 기록
    SBAnalytics::EventParams params;
    params[ANALYTICS_EVENT_PARAM_LEVEL] = SBAnalytics::EventParam(level);
    params[ANALYTICS_EVENT_PARAM_LEVEL_RANGE] = SBAnalytics::EventParam(SBAnalytics::getNumberRange(level, 1, 5, 5));

    SBAnalytics::logEvent(ANALYTICS_EVENT_GAME_RESULT, params);
}

void GameManager::onGameResult() {
    
    CCLOG("GameManager::onGameResult");
    
    CCASSERT(instance->hasState(GameState::GAME_OVER), "GameManager::onGameResult invalid called.");
    
    const int level = instance->getLevel();
    
    if( level > 0 ) {
        logEventGameResult(level);
    }
    
    instance->removeState(GameState::GAME_OVER);
    instance->addState(GameState::RESULT);
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_RESULT);
}

/**
 * 스테이지 변경
 */
void GameManager::onStageChanged() {
    
    auto level = instance->level;
    auto levelData = instance->getLevelData();
    Log::i("GameManager::onStageChanged stage: %d", level);
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_STAGE_CHANGED, &levelData);
}

/**
 * 스테이지 재시작
 */
void GameManager::onStageRestart() {
    
    auto level = instance->level;
    auto levelData = instance->getLevelData();
    Log::i("GameManager::onStageRestart stage: %d", level);
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_STAGE_RESTART, &levelData);
}

/**
 * 스테이지 클리어
 */
void GameManager::onStageClear() {
    
    auto level = instance->level;
    auto levelData = instance->getLevelData();
    Log::i("GameManager::onStageClear stage: %d", level);
    
    // superbomb::PluginPlay::submitScore(LEADER_BOARD_TOP_LEVEL, level);
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_STAGE_CLEAR, &levelData);
    
    instance->level++;
    onStageChanged();
}

/**
 * 타이머 시작
 */
void GameManager::onStartTimer() {
    
    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_START_TIMER);
}

/**
 * 다음 스테이지로 이동
 */
void GameManager::onMoveNextStage() {
    
//    // 마지막 레벨인 경우 가상 레벨 추가
//    if( instance->stage.stage == Database::getLastStage().stage ) {
//        Database::addVirtualLevel();
//    }
//
//    instance->setStage(instance->stage.stage+1);
//    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_MOVE_NEXT_STAGE, &instance->stage);
}

/**
 * 다음 스테이지로 이동 완료
 */
void GameManager::onMoveNextStageFinished() {
    
//    getEventDispatcher()->dispatchCustomEvent(GAME_EVENT_MOVE_NEXT_STAGE_FINISHED, &instance->stage);
//    onStageChanged();
}

/**
 * 스코어 변경
 */
void GameManager::onScoreChanged() {
    
    // getEventDispatcher()->dispatchOnScoreChanged(instance->score);
}

