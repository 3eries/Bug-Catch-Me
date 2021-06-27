//
//  GameDefine.h
//  Thirteen
//
//  Created by seongmin hwang on 19/08/2019.
//

#ifndef GameDefine_h
#define GameDefine_h

#include "cocos2d.h"
#include "superbomb.h"

static const std::string GAME_EVENT_ENTER                         = "GAME_EVENT_ENTER";
static const std::string GAME_EVENT_EXIT                          = "GAME_EVENT_EXIT";
static const std::string GAME_EVENT_RESET                         = "GAME_EVENT_RESET";
static const std::string GAME_EVENT_START                         = "GAME_EVENT_START";
static const std::string GAME_EVENT_RESTART                       = "GAME_EVENT_RESTART";
static const std::string GAME_EVENT_PAUSE                         = "GAME_EVENT_PAUSE";
static const std::string GAME_EVENT_RESUME                        = "GAME_EVENT_RESUME";
static const std::string GAME_EVENT_OVER                          = "GAME_EVENT_OVER";
static const std::string GAME_EVENT_CONTINUE                      = "GAME_EVENT_CONTINUE";
static const std::string GAME_EVENT_RESULT                        = "GAME_EVENT_RESULT";
static const std::string GAME_EVENT_STAGE_CHANGED                 = "GAME_EVENT_STAGE_CHANGED";
static const std::string GAME_EVENT_STAGE_RESTART                 = "GAME_EVENT_STAGE_RESTART";
static const std::string GAME_EVENT_STAGE_CLEAR                   = "GAME_EVENT_STAGE_CLEAR";
static const std::string GAME_EVENT_START_TIMER                   = "GAME_EVENT_START_TIMER";
static const std::string GAME_EVENT_MOVE_NEXT_STAGE               = "GAME_EVENT_MOVE_NEXT_STAGE";
static const std::string GAME_EVENT_MOVE_NEXT_STAGE_FINISHED      = "GAME_EVENT_MOVE_NEXT_STAGE_FINISHED";

enum class GameEvent {
    NONE = 0,
    ENTER = 1,
    EXIT,
    RESET,
    START,
    RESTART,
    PAUSE,
    RESUME,
    OVER,
    CONTINUE,
    RESULT,
    STAGE_CHANGED,
    STAGE_RESTART,
    STAGE_CLEAR,
    START_TIMER,
    MOVE_NEXT_STAGE,
    MOVE_NEXT_STAGE_FINISHED,
};

static std::map<std::string, GameEvent> GAME_EVENT_ENUMS = {
    { GAME_EVENT_ENTER, GameEvent::ENTER },
    { GAME_EVENT_EXIT, GameEvent::EXIT },
    { GAME_EVENT_RESET, GameEvent::RESET },
    { GAME_EVENT_START, GameEvent::START },
    { GAME_EVENT_RESTART, GameEvent::RESTART },
    { GAME_EVENT_PAUSE, GameEvent::PAUSE },
    { GAME_EVENT_RESUME, GameEvent::RESUME },
    { GAME_EVENT_OVER, GameEvent::OVER },
    { GAME_EVENT_CONTINUE, GameEvent::CONTINUE },
    { GAME_EVENT_RESULT, GameEvent::RESULT },
    { GAME_EVENT_STAGE_CHANGED, GameEvent::STAGE_CHANGED },
    { GAME_EVENT_STAGE_RESTART, GameEvent::STAGE_RESTART },
    { GAME_EVENT_STAGE_CLEAR, GameEvent::STAGE_CLEAR },
    { GAME_EVENT_START_TIMER, GameEvent::START_TIMER },
    { GAME_EVENT_MOVE_NEXT_STAGE, GameEvent::MOVE_NEXT_STAGE },
    { GAME_EVENT_MOVE_NEXT_STAGE_FINISHED, GameEvent::MOVE_NEXT_STAGE_FINISHED },
};

enum GameState {
    NONE            = (1 << 0),
    ENTERED         = (1 << 1),     // 게임 진입
    EXITED          = (1 << 2),     // 게임 퇴장
    STARTED         = (1 << 3),     // 게임 시작됨
    PAUSED          = (1 << 4),     // 일시정지
    IDLE            = (1 << 5),     // 대기
    GAME_OVER       = (1 << 6),     // 게임 오버
    CONTINUE        = (1 << 7),     // 이어하기
    RESULT          = (1 << 8),     // 결과 화면
};

#define                 GAME_BG_COLOR                   cocos2d::Color3B(0, 17, 17)
#define                 LEVEL_REFRESH_DURATION          1.7f
#define                 MOVE_NEXT_LEVEL_DURATION        0.5f

#define                 GAME_RESULT_DELAY               1.0f    // 게임 오버 -> 결과 노출까지의 지연 시간

#endif /* GameDefine_h */
