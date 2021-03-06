//
//  TitleSceneLoader.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/09/29.
//
//

#ifndef Cocos2dxSRPGQuest_TitleSceneLoader_h
#define Cocos2dxSRPGQuest_TitleSceneLoader_h

#include "TitleScene.h"

class TitleSceneLoader : public cocos2d::extension::LayerLoader
{
public:
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(TitleSceneLoader, loader);
    
    static Scene* createScene();
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(TitleScene);
};

#endif
