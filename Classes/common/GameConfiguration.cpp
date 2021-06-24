//
//  GameConfiguration.cpp
//
//  Created by seongmin hwang on 2018. 6. 3..
//

#include "GameConfiguration.hpp"

#include "Define.h"

USING_NS_CC;
using namespace std;

#define JSON_FILE       "config/game_config.json"

static GameConfiguration *instance = nullptr;
GameConfiguration* GameConfiguration::getInstance() {
    
    if( !instance ) {
        instance = new GameConfiguration();
    }
    
    return instance;
}

void GameConfiguration::destroyInstance() {
    
    CC_SAFE_DELETE(instance);
}

GameConfiguration::GameConfiguration() {
}

GameConfiguration::~GameConfiguration() {
}

void GameConfiguration::init() {

//    string jsonStr = SBStringUtils::readTextFile(JSON_FILE);
//    CCLOG("jsonStr: %s", jsonStr.c_str());
}

void GameConfiguration::parse(const string &json) {
    
    CCLOG("========== PARSE START (game_config.json)  ==========");
    CCLOG("%s", json.c_str());
    
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    auto platformObj = doc["ios"].GetObject();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    auto platformObj = doc["android"].GetObject();
#endif
    
    // store
    storeUrl             = platformObj["store"].GetString();
    moreGamesUrl         = platformObj["more_games"].GetString();
    
    // game value
    maxScore = doc["max_score"].GetInt();
    maxBug = doc["max_bug"].GetInt();
    maxLife = doc["max_life"].GetInt();
    
    // levels
    levels.clear();
    
    {
        auto levelList = doc["levels"].GetArray();
        LevelData prevLv;
        
        for( int i = 0; i < levelList.Size(); ++i ) {
            const rapidjson::Value &levelValue = levelList[i];
            
            LevelData lv = prevLv;
            lv.level = levelValue["level"].GetInt();
            
            // bugs
            if( levelValue.HasMember("bugs") ) {
                lv.bugs.clear();
                
                auto bugList = levelValue["bugs"].GetArray();
                
                for( int j = 0; j < bugList.Size(); ++j ) {
                    const auto &bugValue = bugList[j];
                    
                    BugData bug;
                    bug.teleportInterval = bugValue["teleport_interval"].GetFloat();
                    
                    lv.bugs.push_back(bug);
                }
            }
            
            if( levelValue.HasMember("life") )     lv.life = levelValue["life"].GetInt();
            if( levelValue.HasMember("time") )     lv.time = levelValue["time"].GetFloat();
            
            CCASSERT(lv.bugs.size() <= maxBug, "game_config.json parse error: invalid bugs size.");
            CCASSERT(lv.life <= maxLife, "game_config.json parse error: invalid life.");
            
            CCLOG("LEVEL %d\n\tbug count: %d, life: %d, time: %f",
                  lv.level, (int)lv.bugs.size(), lv.life, lv.time);
            
            for( auto bug : lv.bugs ) {
                CCLOG("\tteleportInterval: %f", bug.teleportInterval);
            }
            
            prevLv = lv;
            levels.push_back(lv);
        }
        
        // order by level asc
        sort(levels.begin(), levels.end(), [](const LevelData &l1, const LevelData &l2) {
            return l1.level < l2.level;
        });
    }
    
    CCLOG("========== PARSE END (game_config.json)  ==========");
}

LevelData GameConfiguration::getLevel(int level) {
    
    if( level > getMaxLevel().level ) {
        return LevelData();
    }
    
    return levels[level-1];
}

LevelData GameConfiguration::getMaxLevel() {
    
    if( levels.size() == 0 ) {
        return LevelData();
    }
    
    return levels[levels.size()-1];
}


