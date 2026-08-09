#include <EndscreenStat.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
using namespace geode::prelude;
#include "API/EndscreenStats.hpp"
#include "API/EndLevelLayer.hpp"
// due to the fact we can't use the event thing in this mod i'd just make a macro for me
#define EndscreenMacro(sprite, number,id, pad, function, zorder) {auto stat = ESR::EndscreenStat::create(sprite, number, id, pad); stat->addEventListener(ESR::HandleNode(stat), [](EndLevelLayer* EndlevelLayer, cocos2d::CCNode* StatNode, float AnimationDelay){function }, 0); statsMenu->addChild(stat); stat->setZOrder(zorder);}
$on_mod(Loaded) {
    ESR::EndscreenStat::Endscreen().listen([](cocos2d::CCNode* statsMenu, EndLevelLayer* endlayer){
        if (auto pl = endlayer->m_playLayer){
           if (endlayer->m_moons)
           {
                auto sprite = CCSprite::createWithSpriteFrameName("GJ_bigMoon_001.png");
                sprite->setScale(0.8);
                EndscreenMacro(sprite,endlayer->m_moons, "moon-container",0.f, { StatNode->setTag(Effect::Moons);
                    StatNode->runAction(CCSequence::create(
                            CCDelayTime::create(AnimationDelay + 0.2f),
                           CCCallFuncO::create(EndlevelLayer, callfuncO_selector(ESR_EndLevelLayer::playRewardEffect), StatNode),
                            nullptr
                        )
                    );}, 0);
          } else if (pl->m_endLayerStars && endlayer->m_stars) 
          {
                auto sprite = CCSprite::createWithSpriteFrameName("GJ_bigStar_001.png");
                sprite->setScale(0.8);
                EndscreenMacro(sprite,endlayer->m_stars, "star-container",0.f, { StatNode->setTag(Effect::Stars);
                    StatNode->runAction(CCSequence::create(
                            CCDelayTime::create(AnimationDelay + 0.2f),
                            CCCallFuncO::create(EndlevelLayer, callfuncO_selector(ESR_EndLevelLayer::playRewardEffect), StatNode),
                            nullptr
                        )
                    );}, 0);
          }
         if (pl->m_orbs) 
           {
                auto sprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
                sprite->setScale(1.25);
                EndscreenMacro(sprite,pl->m_orbs, "orb-container",nullptr, {
                    StatNode->setTag(Effect::Orbs);
                    StatNode->runAction(CCSequence::create(
                            CCDelayTime::create(AnimationDelay + 0.2f),
                           CCCallFuncO::create(EndlevelLayer, callfuncO_selector(ESR_EndLevelLayer::playRewardEffect), StatNode),
                            nullptr
                        )
                    );}, 2);
           }
           if (pl->m_diamonds) 
             {
                auto sprite = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
                sprite->setScale(0.7f);
                EndscreenMacro(sprite,pl->m_diamonds, "diamond-container",0.f, {
                    StatNode->setTag(Effect::Diamonds);
                    StatNode->runAction(CCSequence::create(
                            CCDelayTime::create(AnimationDelay + 0.2f),
                            CCCallFuncO::create(EndlevelLayer, callfuncO_selector(ESR_EndLevelLayer::playRewardEffect), StatNode),
                            nullptr
                        )
                    );
                }, 4);
            }
            statsMenu->updateLayout();
        }
    }).leak();
}