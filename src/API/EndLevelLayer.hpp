#pragma once
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
using namespace geode::prelude;

enum Effect {
    Diamonds = 1,
    Orbs = 2,
    Stars = 3,
    Moons = 4
};

class $modify(ESR_EndLevelLayer, EndLevelLayer) {
	static void onModify(auto &self);
    virtual void showLayer(bool instant);
    virtual void customSetup();

    void playStarEffect(float);
    void playCurrencyEffect(float);
    void playDiamondEffect(float);
    
    void playRewardEffect(CCObject*);
};