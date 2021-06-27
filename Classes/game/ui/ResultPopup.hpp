//
//  ResultPopup.hpp
//
//  Created by seongmin hwang on 2018. 5. 17..
//

#ifndef ResultPopup_hpp
#define ResultPopup_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "superbomb.h"

#include "BasePopup.hpp"

class ResultPopup : public BasePopup {
public:
    static ResultPopup* create(int level);
    ~ResultPopup();
    
private:
    ResultPopup(int level);
    
    bool init() override;
    void onEnter() override;
    bool onBackKeyReleased() override;
    
    void initBackgroundView() override;
    void initContentView() override;
    
public:
    void runEnterAction(float duration, SBCallback onFinished) override;
    void runEnterAction(SBCallback onFinished = nullptr) override;
    
    void runExitAction(float duration, SBCallback onFinished) override;
    void runExitAction(SBCallback onFinished = nullptr) override;
    
    void onEnterActionFinished() override;
    
private:
    int level;
    CC_SYNTHESIZE(SBCallback, onClickListener, OnClickListener);
    
    bool isTouchLocked;
};

#endif /* ResultPopup_hpp */
