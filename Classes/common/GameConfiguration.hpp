//
//  GameConfiguration.hpp
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#ifndef GameConfiguration_hpp
#define GameConfiguration_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"
#include "Define.h"

struct BugData {
    float teleportInterval;     // 순간 이동 시간 간격
};

struct LevelData {
    int level;
    std::vector<BugData> bugs;
    int life;
    float time;                 // 게임 제한 시간
    
    LevelData() {
        level = -1;
    }
};

#define GAME_CONFIG     GameConfiguration::getInstance()

class GameConfiguration : public cocos2d::Ref {
public:
    static GameConfiguration* getInstance();
    static void destroyInstance();
    ~GameConfiguration();
    
private:
    GameConfiguration();
    
public:
    void init();
    void parse(const std::string &json);
    
    LevelData getLevel(int level);
    LevelData getMaxLevel();
    
private:
    CC_SYNTHESIZE_READONLY(std::string, storeUrl, StoreUrl);
    CC_SYNTHESIZE_READONLY(std::string, moreGamesUrl, MoreGamesUrl);
    
    // 최대 스코어
    CC_SYNTHESIZE_READONLY(int, maxScore, MaxScore);
    
    // 최대 버그 수
    CC_SYNTHESIZE_READONLY(int, maxBug, MaxBug);
    
    // 최대 라이프
    CC_SYNTHESIZE_READONLY(int, maxLife, MaxLife);
    
    // 레벨 정보
    CC_SYNTHESIZE(std::vector<LevelData>, levels, Levels);
};

#endif /* GameConfiguration_hpp */
