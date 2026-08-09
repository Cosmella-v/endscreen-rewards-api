#pragma once
using namespace geode::prelude;
#include <Geode/Geode.hpp> 
#include <Geode/ui/ScrollLayer.hpp>
#include "Shaders/ShaderEffect.hpp"
namespace ESR {
class HandleNode : public geode::GlobalEvent<HandleNode, bool(EndLevelLayer*, cocos2d::CCNode*, float), cocos2d::CCNode*> {
public:
    using GlobalEvent::GlobalEvent;
};
namespace Shaders {
    static ShaderEffect s_FadeEffect;
}
class EndscreenStats : public geode::ScrollLayer {
    public:
        static EndscreenStats* create();
        EndscreenStats() : ScrollLayer({0, 0, 90,43}, true, true) {}
        virtual void setContentSize(CCSize const& size) override;
        void addChild(cocos2d::CCNode* child) override;
        void addChild(cocos2d::CCNode* child, int zOrder) override;
        void addChild(cocos2d::CCNode* child, int zOrder, int tag) override;
        void startAction(cocos2d::CCNode* node);
        void updateLayout();
        bool init() override;
        void visit() override;
        void runMostActions();
        static geode::Layout *createDefaultListLayout(float gap = (2.5F));
        float delay = 0.f;
        geode::WeakRef<EndLevelLayer> m_endscreen;
        class Scrollbar : public geode::Scrollbar {
            public:
                virtual void visit();
                static EndscreenStats::Scrollbar* create(CCScrollLayerExt* list);
            private:
                    geode::NineSlice* m_track;
                    bool init(CCScrollLayerExt*);
        }; 
    private:
        Scrollbar* m_scrollbar = nullptr;
        bool m_init = false;
        bool m_alreadyRan = false;
        bool m_cutContent = true;
        void HandleNewChild(cocos2d::CCNode* child);
    };
}