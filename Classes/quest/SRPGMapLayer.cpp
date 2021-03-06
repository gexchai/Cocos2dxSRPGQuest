//
//  SRPGMapLayer.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/05.
//
//

#include "SRPGMapLayer.h"

USING_NS_CC;

SRPGMapLayer::SRPGMapLayer()
:m_baseMapSize(Size::ZERO),
m_baseTileSize(Size::ZERO),
m_baseContentSize(Size::ZERO),
m_touchStartPoint(Point::ZERO)
{
}

SRPGMapLayer::~SRPGMapLayer()
{
}

bool SRPGMapLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size winSize = Director::getInstance()->getWinSize();

    // ---------------------
    // タイルマップを生成
    // ---------------------
    //    TMXTiledMap* pTiledMap = TMXTiledMap::create("japan_map.tmx");
    TMXTiledMap* pTiledMap = TMXTiledMap::create("tmx/desert.tmx");
//    pTiledMap->setPosition(pTiledMap->getTileSize().width * -0.5, pTiledMap->getTileSize().height * -0.5);
    pTiledMap->setPosition(Point::ZERO);
    this->addChild(pTiledMap, SRPGMapLayer::zTiledMapIndex, SRPGMapLayer::kTiledMapTag);
    
    m_baseMapSize = pTiledMap->getMapSize();
    m_baseTileSize = pTiledMap->getTileSize();
    m_baseContentSize = pTiledMap->getContentSize();
    
    // マップマネージャを生成
    m_mapManager.init(0, (int)m_baseMapSize.height, 0, (int)m_baseMapSize.width);
    
    // ---------------------
    // グリッド線を生成
    // ---------------------
    DrawNode* draw = DrawNode::create();
    draw->setPosition(Point::ZERO);
    
    // 線の太さと色
    float lineSize = 1 * 0.5;
    Color4F color = Color4F::BLUE;
    
    // 縦線を引く
    for (int x = 1; x < m_baseMapSize.width; x++)
    {
        float xPoint = x * m_baseTileSize.width;
        draw->drawSegment(Point(xPoint, 0), Point(xPoint, m_baseContentSize.height), lineSize, color);
    }
    // 横線を引く
    for (int y = 1; y < m_baseMapSize.height; y++)
    {
        float yPoint = y * m_baseTileSize.height;
        draw->drawSegment(Point(0, yPoint), Point(m_baseContentSize.width, yPoint), lineSize, color);
    }
    this->addChild(draw, SRPGMapLayer::zGridLineIndex, SRPGMapLayer::kGridLineTag);

    // ---------------------
    // アクターを生成
    // ---------------------
    ActorSprite::ActorDto actorDto;
    actorDto.attackRange = 1;
    actorDto.movePoint = 5;
    actorDto.playerId = 4;
    ActorMapItem* pActorMapItem = addActor(MapDataType::PLAYER, 1, 5, 5, actorDto);
    
    // 移動可能範囲のリストを作成
    std::list<MapIndex> moveList = m_mapManager.createActorFindDist(pActorMapItem->mapIndex, pActorMapItem->moveDist);
    // 移動可能範囲を表示
    addMapCursor(MapDataType::MOVE_DIST, moveList);
    
    return true;
}

#pragma mark
#pragma mark マップ上オブジェクト制御

void SRPGMapLayer::addMapCursor(MapDataType pMapDataType, std::list<MapIndex> moveMapPointList)
{
    // バッチノードを登録しレイヤに追加
    auto* pBatchNode = (SpriteBatchNode*) this->getChildByTag(SRPGMapLayer::kCursorBaseTag);
    if (!pBatchNode)
    {
        // 白い32x32の四角い画像を使う
        pBatchNode = SpriteBatchNode::create("grid32.png");
        this->addChild(pBatchNode, SRPGMapLayer::zCursorBaseIndex, SRPGMapLayer::kCursorBaseTag);
    }
    
    for (MapIndex mapIndex : moveMapPointList)
    {
        // 緑半透明50%
        auto* sprite = Sprite::createWithTexture(pBatchNode->getTexture());
        sprite->setPosition(indexToPoint(mapIndex.x, mapIndex.y));
        sprite->setOpacity(128);
        
        if (pMapDataType == MapDataType::MOVE_DIST)
        {
            sprite->setColor(Color3B::GREEN);
            sprite->setZOrder(SRPGMapLayer::zCursorMoveFindIndex);
        }
        else if (pMapDataType == MapDataType::MOVE_STEP_DIST)
        {
            sprite->setColor(Color3B::ORANGE);
            sprite->setZOrder(SRPGMapLayer::zCursorMoveStepIndex);
        }
        else if (pMapDataType == MapDataType::SELECTED_DIST)
        {
            sprite->setColor(Color3B::BLUE);
            sprite->setZOrder(SRPGMapLayer::zCursorSelectedIndex);
        }
        // バッチノードに登録
        pBatchNode->addChild(sprite);
        
        // 1.0秒でフェードイン、フェードアウトを繰り返すように設定
        FadeTo* fadeIn = FadeTo::create(0.5f, 128);
        FadeTo* fadeOut = FadeTo::create(0.5f, 0);
        Sequence* sequence = Sequence::create(fadeOut, fadeIn, NULL);
        RepeatForever* repeat = RepeatForever::create(sequence);
        sprite->runAction(repeat);
    }
    
//    MapItem mapItem;
//    mapItem.attackDist = 0;
//    mapItem.moveDist = 0;
//    mapItem.mapPointX = pMapPointX;
//    mapItem.mapPointY = pMapPointY;
//    mapItem.mapDataType = pMapDataType;
}

/**
 * マップカーソル削除
 */
void SRPGMapLayer::clearAllMapCursor()
{
    // バッチノード取得
    auto* pBatchNode = (SpriteBatchNode*) this->getChildByTag(SRPGMapLayer::kCursorBaseTag);
    if (pBatchNode)
    {
        pBatchNode->removeAllChildren();
    }
}

bool SRPGMapLayer::isMapCursor(MapDataType mapDataType)
{
    bool isShow = false;
    
    // バッチノード取得
    auto* pBatchNode = (SpriteBatchNode*) this->getChildByTag(SRPGMapLayer::kCursorBaseTag);
    if (pBatchNode)
    {
        Object* object = NULL;
        CCARRAY_FOREACH(pBatchNode->getChildren(), object)
        {
            if ((mapDataType == MapDataType::MOVE_DIST && ((Sprite*)object)->getZOrder() == zCursorMoveFindIndex)
            || (mapDataType == MapDataType::MOVE_STEP_DIST && ((Sprite*)object)->getZOrder() == zCursorMoveStepIndex))
            {
                if(((Sprite*)object)->isVisible()) isShow = true;
                break;
            }
        }
    }
    return isShow;
}

void SRPGMapLayer::showMapCursor(MapDataType mapDataType)
{
    visibleMapCursor(mapDataType, true);
}

void SRPGMapLayer::hideMapCursor(MapDataType mapDataType)
{
    visibleMapCursor(mapDataType, false);
}

void SRPGMapLayer::visibleMapCursor(MapDataType mapDataType, bool visible)
{
    // バッチノード取得
    auto* pBatchNode = (SpriteBatchNode*) this->getChildByTag(SRPGMapLayer::kCursorBaseTag);
    if (pBatchNode)
    {
        Object* object = NULL;
        CCARRAY_FOREACH(pBatchNode->getChildren(), object)
        {
            if (mapDataType == MapDataType::MOVE_DIST && ((Sprite*)object)->getZOrder() == zCursorMoveFindIndex)
            {
                ((Sprite*)object)->setVisible(visible);
            }
            else if (mapDataType == MapDataType::MOVE_STEP_DIST && ((Sprite*)object)->getZOrder() == zCursorMoveStepIndex)
            {
                ((Sprite*)object)->setVisible(visible);
            }
        }
    }
}

/**
 * アクター追加.
 */
ActorMapItem* SRPGMapLayer::addActor(MapDataType pMapDataType, int pSeqNo, int pMapPointX, int pMapPointY, ActorSprite::ActorDto pActorDto)
{
    Size winSize = Director::getInstance()->getWinSize();
    
    // Spriteを配置
    ActorSprite* pActorSprite = ActorSprite::createWithActorDto(pActorDto);
    pActorSprite->setPosition(indexToPoint(pMapPointX, pMapPointY));
    this->addChild(pActorSprite, SRPGMapLayer::zActorBaseIndex, (SRPGMapLayer::kActorBaseTag + pSeqNo));

    // 座標変換テスト用
    CCLOG("x = %f, y = %f", pActorSprite->getPositionX(), pActorSprite->getPositionY());
    MapIndex mapIndex = pointToIndex(pActorSprite->getPosition());
    CCLOG("index_x = %d, index_y = %d", mapIndex.x, mapIndex.y);
    
    // マップデータを作成
    ActorMapItem actorMapItem;
    actorMapItem.seqNo = pSeqNo;
    actorMapItem.attackDist = pActorDto.attackRange;
    actorMapItem.moveDist = pActorDto.movePoint;
    
    actorMapItem.mapIndex = mapIndex;
    actorMapItem.mapDataType = pMapDataType;
    
    actorMapItem.moveDone = false;
    actorMapItem.attackDone = false;
    pActorSprite->setActorMapItem(actorMapItem);
    
    m_mapManager.addActor(pActorSprite->getActorMapItem());
    
    return pActorSprite->getActorMapItem();
//    mMapItemManager.setObject(mapPointX, mapPointY, playerMapItem);
}

ActorSprite* SRPGMapLayer::findActorSprite(int seqNo)
{
    auto* actorSprite = this->getChildByTag((SRPGMapLayer::kActorBaseTag + seqNo));
    if (actorSprite && typeid(*actorSprite) == typeid(ActorSprite))
    {
        return (ActorSprite*)actorSprite;
    }
    return NULL;
}

#pragma mark
#pragma mark タッチイベント制御

bool SRPGMapLayer::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    // タッチした位置をマップ座標に変換
    Point locationPoint = convertToSRPGMapPoint(touch);
    MapIndex locationMapIndex = touchPointToIndex(locationPoint);
    CCLOG("onTouchBegan location mapIdx x = %d y = %d", locationMapIndex.x, locationMapIndex.y);
    m_touchStartPoint = locationPoint;
    return true;
}

void SRPGMapLayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}


void SRPGMapLayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    // タッチした位置をマップ座標に変換
    MapIndex startSRPGMapIndex = touchPointToIndex(m_touchStartPoint);
    MapIndex endSRPGMapIndex = touchPointToIndex(convertToSRPGMapPoint(touch));
    // タッチの開始位置と終了位置のグリッドが一致したらグリッド選択とする
    if (MAP_INDEX_DIFF(startSRPGMapIndex, endSRPGMapIndex))
    {
        // 表示前のカーソルクリア
        clearAllMapCursor();
        
        // 選択カーソル表示
        std::list<MapIndex> moveMapPointList;
        moveMapPointList.push_back(endSRPGMapIndex);
        addMapCursor(MapDataType::SELECTED_DIST, moveMapPointList);
        // 選択位置の処理実行
        executeMapIndex(&endSRPGMapIndex);
    }
}

void SRPGMapLayer::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

Point SRPGMapLayer::convertToSRPGMapPoint(Touch *pTouch)
{
    return this->convertToWorldSpace(this->convertTouchToNodeSpace(pTouch)) - this->getPosition();
}

void SRPGMapLayer::executeMapIndex(MapIndex* mapIndex)
{
    // グリッド選択
    CCLOG("onTouchEnded mapIdx x = %d y = %d grid selected", mapIndex->x, mapIndex->y);
    auto* mapItem = m_mapManager.getMapItem(mapIndex);
    CCLOG("mapDataType = %d", mapItem->mapDataType);
    if (mapItem->mapDataType == MapDataType::PLAYER)
    {
        auto* actorMapItem = m_mapManager.getActorMapItem(mapIndex);
        // 移動可能範囲のリストを作成
        std::list<MapIndex> moveList = m_mapManager.createActorFindDist(actorMapItem->mapIndex, actorMapItem->moveDist);
        // 移動可能範囲を表示
        addMapCursor(MapDataType::MOVE_DIST, moveList);
        
        CCLOG("touched player seqNo = %d", actorMapItem->seqNo);
        
    }
    else if (mapItem->mapDataType == MapDataType::MOVE_DIST)
    {
        // 移動対象を取得
        auto* actorMapItem = m_mapManager.getActorMapItemById(1); // TODO: とりあえず1固定
        // 移動可能範囲のリストを作成
        std::list<MapIndex> moveList = m_mapManager.createActorFindDist(actorMapItem->mapIndex, actorMapItem->moveDist);
        // 移動可能範囲を表示
        addMapCursor(MapDataType::MOVE_DIST, moveList);
        // 移動経路の作成と表示
        std::list<MapIndex> list = m_mapManager.createMovePointList(&mapItem->mapIndex, actorMapItem);
        addMapCursor(MapDataType::MOVE_STEP_DIST, list);
        
        // 移動
        auto* pActorSprite = findActorSprite(actorMapItem->seqNo);
        if (pActorSprite)
        {
            // 移動アニメーション作成。移動後にマップカーソルを初期化
            auto* pCallFunc = CallFunc::create([this](void) {
                CCLOG("call func!!！");
                clearAllMapCursor();
            });
            int movePointSize = list.size();
            auto* moveArray = Array::create();
            for (MapIndex mapIndex : list)
            {
                auto* pMoveTo = MoveTo::create(1.0 / movePointSize, indexToPoint(mapIndex));
                moveArray->addObject(pMoveTo);
            }
            auto* pMoveSeq = Sequence::create(moveArray);
            auto* pAnimation = Sequence::create(pMoveSeq, pCallFunc, NULL);
            pActorSprite->runAction(pAnimation);
            
            // マップ情報も更新する
            m_mapManager.moveActor(actorMapItem, &(mapItem->mapIndex));
            m_mapManager.clearCursor();
        }
    }
}

#pragma mark
#pragma mark マップLayer制御

/**
 * グリッド表示
 */
bool SRPGMapLayer::isShowGrid()
{
    auto* node = this->getChildByTag(SRPGMapLayer::kGridLineTag);
    if (node)
    {
        return node->isVisible();
    }
    return false;
}

/**
 * グリッド表示
 */
void SRPGMapLayer::showGrid()
{
    auto* node = this->getChildByTag(SRPGMapLayer::kGridLineTag);
    if (node)
    {
        node->setVisible(true);
    }
}

/**
 * グリッド非表示
 */
void SRPGMapLayer::hideGrid()
{
    auto* node = this->getChildByTag(SRPGMapLayer::kGridLineTag);
    if (node)
    {
        node->setVisible(false);
    }
}

/**
 * マップ座標の移動距離生成
 */
Point SRPGMapLayer::createTouchMoveMapPoint(Point mapPosition, float updateDelta, Point pDelta)
{
    Size winSize = Director::getInstance()->getWinSize();
    
    TMXTiledMap* pTileMap = (TMXTiledMap*) this->getChildByTag(kTiledMapTag);
    Size tileSize = pTileMap->getTileSize();
    
    // 基点からのタップ移動量を計算
    float speed = 4.0;
    Point calcDelta = Point(pDelta.x * updateDelta, pDelta.y * updateDelta) * -1 * speed;
    
    // タイルマップの位置を移動量に応じて移動させる
    Point point = mapPosition + calcDelta;
    // マップの端から出ないように調整する
    Size mapSize = pTileMap->getContentSize();

    // 画面半分まで真っ黒領域が見れる
//    point.x = MAX(point.x, winSize.width * 0.5 - mapSize.width + tileSize.width * 0.5);
//    point.x = MIN(point.x, winSize.width * 0.5 - tileSize.width * 0.5);
//    point.y = MAX(point.y, winSize.height * 0.5 - mapSize.height + tileSize.height * 0.5);
//    point.y = MIN(point.y, winSize.height * 0.5 - tileSize.height * 0.5);
    // 画面ピッタリ
    point.x = MAX(point.x, winSize.width - mapSize.width);
    point.x = MIN(point.x, 0);
    point.y = MAX(point.y, winSize.height - mapSize.height);
    point.y = MIN(point.y, 0);
    
    return point;
}


#pragma mark
#pragma mark マップ座標変換

Point SRPGMapLayer::indexToPoint(MapIndex mapIndex)
{
    return indexToPoint(mapIndex.x, mapIndex.y);
}

Point SRPGMapLayer::indexToPoint(int mapIndex_x, int mapIndex_y)
{
    // タイルサイズを考慮
    return Point(m_baseTileSize.width * mapIndex_x + m_baseTileSize.width * 0.5, m_baseTileSize.height * mapIndex_y + m_baseTileSize.height * 0.5);
}

MapIndex SRPGMapLayer::pointToIndex(Point point)
{
    // タイルサイズを考慮
    point.x = point.x - m_baseTileSize.width * 0.5;
    point.y = point.y - m_baseTileSize.height * 0.5;
    return touchPointToIndex(point);
}

MapIndex SRPGMapLayer::touchPointToIndex(Point point)
{
    MapIndex mapIndex;
    mapIndex.x = point.x / m_baseTileSize.width;
    mapIndex.y = point.y / m_baseTileSize.height;
    return mapIndex;
}

