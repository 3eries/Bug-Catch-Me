//
//  BgBinary.cpp
//  BugCatchMe-mobile
//
//  Created by seongmin hwang on 14/01/2019.
//

#include "BgBinary.hpp"
#include "Define.h"

USING_NS_CC;
using namespace std;

static const Color4B    COLOR = Color4B(61, 76, 110, 255);
static const StringList ERROR_STR({ "!","@","#","$","%","^","&","*","?" });

BgBinary* BgBinary::create(int x, int y, float w, float h, float fontSize) {
    
    auto b = new BgBinary();
    
    if( b && b->init(x, y, w, h, fontSize) ) {
        b->autorelease();
        return b;
    }
    
    CC_SAFE_DELETE(b);
    return nullptr;
}

BgBinary::BgBinary() :
mode(Mode::NONE) {
    
}

BgBinary::~BgBinary() {
    
}

bool BgBinary::init(int x, int y, float w, float h, float fontSize) {
    
    if( !Node::init() ) {
        return false;
    }
    
    setContentSize(Size(w,h));
    
    // binary
    binaryLabel = Label::createWithTTF("", FONT_COMMODORE, fontSize,
                                       getContentSize(),
                                       TextHAlignment::CENTER, TextVAlignment::CENTER);
    binaryLabel->setAnchorPoint(ANCHOR_M);
    binaryLabel->setPosition(Vec2MC(getContentSize(), 0, 0));
    binaryLabel->setTextColor(COLOR);
    addChild(binaryLabel);
    
    updateBinary();
    
    // bug
    /*
    string file = "bug1.png";
    
    if( x % 2 != 0 || y % 2 != 0 ) {
        file = "bug1_2.png";
    }
    
    auto stencil = Sprite::create(DIR_IMG_GAME + file);
    stencil->setAnchorPoint(Vec2::ZERO);
    stencil->setPosition(Vec2::ZERO);
    stencil->setScale((h / stencil->getContentSize().height) * 1.2f);
    
    auto clippingNode = ClippingNode::create(stencil);
    clippingNode->setContentSize(getContentSize());
    clippingNode->setAlphaThreshold(0);
    clippingNode->addChild(SBNodeUtils::createBackgroundNode(clippingNode, COLOR));
    
    bugImage = clippingNode;
    bugImage->setAnchorPoint(ANCHOR_M);
    bugImage->setPosition(Vec2MC(getContentSize(), 0, 0));
    addChild(bugImage);
    */
    
    switchMode(Mode::BINARY);
    
    return true;
}

void BgBinary::updateBinary() {
    
    binaryLabel->setString(TO_STRING(arc4random() % 2));
    binaryLabel->setOpacity(255 * random(0.2f, 0.9f));
}

void BgBinary::switchMode(Mode mode) {
    
    if( this->mode == mode ) {
        return;
    }
    
    this->mode = mode;
    
    binaryLabel->stopAllActions();
    binaryLabel->setVisible(false);
    // bugImage->setVisible(false);
    
    switch( mode ) {
        case Mode::BINARY: {
            binaryLabel->setVisible(true);
            updateBinary();
            
            auto delay = DelayTime::create(random(2.0f, 5.0f));
            auto callFunc = CallFunc::create([=]() {
                if( arc4random() % 8 == 0 ) {
                    this->updateBinary();
                }
            });
            binaryLabel->runAction(RepeatForever::create(Sequence::create(delay, callFunc, nullptr)));
            
        } break;
            
        case Mode::BUG: {
            // bugImage->setVisible(true);
            binaryLabel->setVisible(true);
            binaryLabel->setString(ERROR_STR[arc4random() % ERROR_STR.size()]);
            binaryLabel->setOpacity(255);
            
        } break;
            
        default: {
            CCASSERT(false, "BgBinary::switchMode error.");
        } break;
    }
}


