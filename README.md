# Endscreen rewards
An API for adding rewards to the endscreen.

# Developer usage
## Setting up for mod usage
Make sure to declare Stats Display API as a dependency in your 'mod.json'
```json
"dependencies": {
	"cosmella.endscreen-rewards": {
			"required": true,
			"version": ">=v1.0.3"
		}
}
```
> note: required can be false however it wont work without the mod

You can now include the API in your source file
`#include <cosmella.endscreen-rewards/include/EndscreenStat.hpp>`

## Examples
RECOMMENDED METHOD
```cpp
$on_mod(Loaded) {
    // this event is fired whenever the statsMenu is added to the EndLevelLayer
    ESR::EndscreenStat::Endscreen().listen([](cocos2d::CCNode* statsMenu, EndLevelLayer* endlayer){
        auto sprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png"); /* scale your sprite */
        sprite->setScale(1.25);
		auto stat = ESR::EndscreenStat::create(
            icon,
            100, // +100
            "exampleReward"_spr, // the ID
			nullptr, // offset (nullptr = 5.f)
            [](EndLevelLayer* endLevelLayer, cocos2d::CCNode* StatNode, float AnimationDelay){
              /*
              run your custom actions here, This for example can be after the "AnimationDelay" so you can play the currency effect
              */
            };
        );
        /*
            ZINDEX ORDER:
            0  - Stars/Moons
            1  - FOR MODS
            2  - Orbs
            3  - FOR MODS
            4  - Diamonds
            5+ - FOR MODS
        */
		statsMenu->addChild(stat, 1); 
    }).leak();
}
```

OTHER METHOD
```cpp
class $modify(EndLevelLayer) {
    void EndLevelLayer::showLayer(bool instant) {
        EndLevelLayer::showLayer(instant);
        // ESR_StatsContainerID is a macro for the ID of the node
        if (auto statsMenu = m_mainLayer->getChildByID(ESR_StatsContainerID)){
            auto sprite = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png"); /* scale your sprite */
            sprite->setScale(1.25);
            auto stat = ESR::EndscreenStat::create(
                icon,
                100, // +100
                "exampleReward"_spr, // the ID
                nullptr, // offset (nullptr = 5.f)
                [](EndLevelLayer* endLevelLayer, cocos2d::CCNode* StatNode, float AnimationDelay){
                   /*
                    run your custom actions here, This for example can be after the "AnimationDelay" so you can play the currency effect
                   */
                };
            );
            /*
                ZINDEX ORDER:
                0  - Stars/Moons
                1  - FOR MODS
                2  - Orbs
                3  - FOR MODS
                4  - Diamonds
                5+ - FOR MODS
            */
            statsMenu->addChild(stat, 1); 
        };
    };
};
```