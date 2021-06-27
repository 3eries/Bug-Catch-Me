//
//  BgBinary.hpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 14/01/2019.
//

#ifndef BgBinary_hpp
#define BgBinary_hpp

#include <stdio.h>

#include "cocos2d.h"
#include "superbomb.h"

class BgBinary : public cocos2d::Node {
public:
    static BgBinary* create(int x, int y, float w, float h, float fontSize);
    ~BgBinary();
    
private:
    BgBinary();
    
    bool init(int x, int y, float w, float h, float fontSize);
    
    void updateBinary();
    
public:
    enum class Mode {
        NONE,
        BINARY,
        BUG,
    };
    
    void switchMode(Mode mode);
    
private:
    CC_SYNTHESIZE(Mode, mode, Mode);
    
    cocos2d::Label *binaryLabel;
    // cocos2d::Node *bugImage;
};

#endif /* BgBinary_hpp */
