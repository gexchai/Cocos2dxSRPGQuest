//
//  ActorSprite.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/05.
//
//

#ifndef __Cocos2dxSRPGQuest__ActorSprite__
#define __Cocos2dxSRPGQuest__ActorSprite__

#include "cocos2d.h"
#include "SRPGMapMacro.h"

USING_NS_CC;

class ActorSprite  : public cocos2d::Sprite
{
public:
    typedef struct _ActorDto {
        /** プレイヤーを一意に識別するID. */
        int playerId;
        /** キャライメージのID. */
        int imageResId;
        /** 顔画像のデフォルトID. */
        int faceImgId;
        /** キャラ名. */
        std::string name;
        /** 攻撃力. */
        int attackPoint;
        /** 防御力. */
        int defencePoint;
        /** レベル. */
        int lv;
        /** 経験値. */
        int exp;
        /** 次のレベルまでの経験値 */
        int nextExp;
        /** HP. */
        int hitPoint;
        /** HP最大値. */
        int hitPointLimit;
        /** MP. */
        int magicPoint;
        /** MP最大値. */
        int magicPointLimit;
        /** 移動力(ステータス表示用). */
        int movePoint;
        /** 攻撃範囲(ステータス表示用). */
        int attackRange;
        
        /** 装備. */
        //ActorPlayerEquipDto equipDto;
        /** スキルリスト. */
        //List<ActorPlayerSkillDto> skillDtoList;
    } ActorDto;

    // コンストラクタ、デストラクタ
    ActorSprite();
    ~ActorSprite();
    
    // 初期化とか
    virtual bool initWithActorDto(ActorDto pActorDto);
    static ActorSprite* createWithActorDto(ActorDto pActorDto);
    
    void setActorMapItem(ActorMapItem actorMapItem);
    ActorMapItem* getActorMapItem();
    ActorDto* getActorDto();
    
private:
    ActorMapItem m_actorMapItem;
    ActorDto m_actorDto;
    
    FiniteTimeAction* createBottomActorAnimate();
    FiniteTimeAction* createLeftActorAnimate();
    FiniteTimeAction* createRightActorAnimate();
    FiniteTimeAction* createTopActorAnimate();
    FiniteTimeAction* createActorAnimate(std::string frameName);
};

#endif /* defined(__Cocos2dxSRPGQuest__ActorSprite__) */

