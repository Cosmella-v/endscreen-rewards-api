#include "EndLevelLayer.hpp"
#include "EndscreenStats.hpp"
#include <EndscreenStat.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
using namespace geode::prelude;

void ESR_EndLevelLayer::onModify(auto &self) {
	/*
	fully "replace" them
	*/
	(void)self.setHookPriority("EndLevelLayer::playStarEffect", Priority::Replace);
	(void)self.setHookPriority("EndLevelLayer::playCurrencyEffect", Priority::Replace);
	(void)self.setHookPriority("EndLevelLayer::playDiamondEffect", Priority::Replace);
};

void ESR_EndLevelLayer::playRewardEffect(CCObject* object){
	if (object){
		/*faster then checking the ids*/
		switch (object->getTag()) {
			case Effect::Diamonds: {
				if (auto node = typeinfo_cast<CCNode*>(object)){ 
					if (auto parent =  node->getParent()) m_diamondsPosition = parent->convertToWorldSpace(node->getPosition());
				};
				return diamondEnterFinished();
			}

			case Effect::Orbs: {
				//log::debug("Currency effect played");
				if (auto node = typeinfo_cast<CCNode*>(object)){ 
					if (auto parent =  node->getParent()) m_orbsPosition = parent->convertToWorldSpace(node->getPosition());
				};
				return currencyEnterFinished();
			}
			case Effect::Stars:
			case Effect::Moons: {
				//log::debug("star effect played");
				if (auto node = typeinfo_cast<CCNode*>(object)){ 
    				if (auto parent =  node->getParent()) m_starsPosition = parent->convertToWorldSpace(node->getPosition());
				};
				return starEnterFinished();
			}
			default: {
				return log::error("unknown tag");
			}
		}
	};
};

void ESR_EndLevelLayer::showLayer(bool instant) {
	EndLevelLayer::showLayer(instant);
	bool trueInstant = instant || GameManager::get()->getGameVariable("0168");
	if (auto Stats = typeinfo_cast<ESR::EndscreenStats *>(m_mainLayer->getChildByID(ESR_StatsContainerID))) {
		Stats->delay = trueInstant ? 0.f : m_coinsToAnimate ? (m_coinsToAnimate->count() * 0.35f + 0.7f) : 0.7;
		ESR::EndscreenStat::Endscreen(trueInstant).send((CCNode *)Stats, (EndLevelLayer *)this);
		if (auto delayObject = typeinfo_cast<CCFloat*>(this->getUserObject("Animation-delay"_spr))){
			Stats->delay += delayObject->getValue();
		};
		geode::Loader::get()->queueInMainThread([wfStats = geode::WeakRef<ESR::EndscreenStats>(Stats)] {
			if (auto Stats = wfStats.lock()) {
				Stats->runMostActions();
			}
		});
	};
};

void ESR_EndLevelLayer::customSetup() {
	EndLevelLayer::customSetup();
	/*
	Add my stats here
	star-container: 404.500, 178.000 @  569x320
	cosmella.endscreen-rewards/stat-container: 370.500, 196 | Content size: 67, 87 @ 569x320
	Anchor point {0, 1}
	*/
	if (auto Stats = ESR::EndscreenStats::create()) {
		Stats->setID(ESR_StatsContainerID);
		auto director = cocos2d::CCDirector::get();
		auto size = director->getWinSize();
		Stats->setPosition(size.width * .5 + 75,size.height*.5 - 60);
		Stats->setAnchorPoint({0.5, 0.5});
		Stats->setContentSize({90, 105});
		Stats->m_endscreen = WeakRef<EndLevelLayer>(this);
		m_mainLayer->addChild(Stats, 10);
	};
};

/*
Blocks robtop from spawning them
*/
void ESR_EndLevelLayer::playStarEffect(float) {
	return;
}
void ESR_EndLevelLayer::playCurrencyEffect(float) {
	return;
}
void ESR_EndLevelLayer::playDiamondEffect(float) {
	return;
}
