#define GEODE_DEFINE_EVENT_EXPORTS

#include <EndscreenStat.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include "EndLevelLayer/EndscreenStats.hpp"
using namespace geode::prelude;
namespace api = ESR;

cocos2d::CCNode* api::EndscreenStat::addSpawnEvent(CCNode* Object, api::addSpawnEvent function) {
    Object->addEventListener(HandleNode(Object), std::move(function), 0);
    return Object;
};