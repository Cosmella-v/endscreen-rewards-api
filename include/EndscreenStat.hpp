#pragma once

#include <Geode/Geode.hpp>
#include <Geode/loader/Dispatch.hpp>

#ifdef MY_MOD_ID
#undef MY_MOD_ID
#endif

#define MY_MOD_ID "cosmella.endscreen-rewards"

#ifndef ESR_DISABLEAPI_ContainerID
#define ESR_StatsContainerID "cosmella.endscreen-rewards/stat-container"
#endif

namespace ESR {
// type used for the SpawnEvent
using addSpawnEvent = geode::CopyableFunction<void(EndLevelLayer *, cocos2d::CCNode *, float)>;
/*
Check if the mod is loaded
*/
inline bool isLoaded() {
	return geode::Loader::get()->isModLoaded(MY_MOD_ID);
};
/*
allows putting nullptr instead of forcing a number
*/
struct nullFloat {
	std::optional<float> value;
	nullFloat(std::nullptr_t) : value(std::nullopt) {}
	nullFloat(float v) : value(v) {}
};
namespace EndscreenStat {
	/*When the endscreen's Stats appear*/
	class Endscreen : public geode::GlobalEvent<Endscreen, bool(cocos2d::CCNode *, EndLevelLayer *), bool> {
	  public:
		using GlobalEvent::GlobalEvent;
	};

	inline cocos2d::CCNode *addSpawnEvent(cocos2d::CCNode *Object, ESR::addSpawnEvent function) GEODE_EVENT_EXPORT_NORES(&addSpawnEvent, (Object, std::move(function)));
	// the base function which allows you to put whatever text
    inline cocos2d::CCNode *create(
	    cocos2d::CCNode *displayNode,
	    std::string text = "",
	    std::string nodeID = "",
	    nullFloat spacing = nullptr,
	    ESR::addSpawnEvent spawnEvent = nullptr) {
		auto newNode = cocos2d::CCNodeRGBA::create();
		newNode->setCascadeOpacityEnabled(true);
		if (nodeID.empty()) {
			nodeID = displayNode ? displayNode->getID() : "unknownStat"_spr;
		}
		newNode->setID(nodeID);
		displayNode->setID(nodeID + "-icon");
		displayNode->retain();
		displayNode->removeFromParentAndCleanup(false);
		newNode->addChild(displayNode);
		displayNode->setAnchorPoint({0, 0.5f});
		displayNode->ignoreAnchorPointForPosition(false);
		displayNode->release();
		auto labelNode = geode::Label::create(
		    text,
		    "bigFont.fnt");
		labelNode->setID(nodeID + "-label");
		labelNode->setScale(0.7f);
		labelNode->setAnchorPoint({1, 0.5f});
		labelNode->ignoreAnchorPointForPosition(false);
		newNode->addChild(labelNode);
		labelNode->setLimitLabelWidth(40.0f, 0.7f, 0.0f);
		labelNode->m_eObjType = (cocos2d::CCObjectType)4;
		displayNode->setPosition({40.f + spacing.value.value_or(5.f), 16.f});
		labelNode->setPosition({40.f, 16.f});
		newNode->setContentSize({84.f, 32.f});
		if (spawnEvent) {
			static bool loaded = ESR::isLoaded();
			if (loaded) EndscreenStat::addSpawnEvent(newNode,std::move(spawnEvent));
		}
		return newNode;
	}
    /*
    Creates the base node
    */
	inline cocos2d::CCNode *create(
	    cocos2d::CCNode *displayNode,
	    int displayedNumber,
	    std::string nodeID = "",
	    nullFloat spacing = nullptr,
	    ESR::addSpawnEvent spawnEvent = nullptr) {
		return create(
		    displayNode,
		    fmt::format("+{}", std::to_string(displayedNumber)).c_str(),
		    std::move(nodeID),
		    spacing,
		    std::move(spawnEvent));
	}
}; // namespace EndscreenStat

/*
Enabling ESR_ENABLEAPI_CAST adds EndscreenStats to be castable however not the functions
*/
#ifdef ESR_ENABLEAPI_CAST
class EndscreenStats : public CCNode {};
#endif

}; // namespace ESR