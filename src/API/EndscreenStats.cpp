using namespace geode::prelude;
#include "EndscreenStats.hpp"
#include <EndscreenStat.hpp>
#include <Geode/Geode.hpp>
#include "Shaders/ShaderEffect.hpp"
using namespace ESR;
geode::Layout* EndscreenStats::createDefaultListLayout(float gap ){
    auto gapLayout = geode::ScrollLayer::createDefaultListLayout(gap);
    if (auto scl = typeinfo_cast<SimpleColumnLayout*>(gapLayout)) scl->setPadding({0, 5, 0, 0});
    return gapLayout;
};
EndscreenStats *EndscreenStats::create() {
	EndscreenStats *s = new EndscreenStats();
	if (s && s->init()) {
		s->autorelease();
		return s;
	}
	delete s;
	return nullptr;
};
void EndscreenStats::setContentSize(CCSize const& size){
    if(m_scrollbar != nullptr) m_scrollbar->setPositionX(size.width+m_scrollbar->getContentWidth());
    CCNode::setContentSize(size);
};
bool EndscreenStats::init() {
	if (!geode::ScrollLayer::init())
		return false;

    /*make sure it isn't already spawned*/
    if (!Shaders::s_FadeEffect.isReady()) {
		auto CCFileUtils = CCFileUtils::get();
		if (CCFileUtils) {
			std::string vertPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-vert.glsl").data(), false);
			std::string fragPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-frag.glsl").data(), false);

			auto vertSrc = file::readString(vertPath);
			auto fragSrc = file::readString(fragPath);

			Shaders::s_FadeEffect.init(vertSrc.unwrap(), fragSrc.unwrap());
		} else {
			log::error("failed to CCFileUtils::get()? Report this");
		}
    }
	m_contentLayer->setLayout(createDefaultListLayout(1.0f));
    m_scrollbar = EndscreenStats::Scrollbar::create(this);
    m_scrollbar->setAnchorPoint({1,0});
    m_scrollbar->setPositionX(getContentWidth()+m_scrollbar->getContentWidth());

    this->addChild(m_scrollbar);
    /*block addchild calls*/
    m_init = true;
	return m_init;
};

void EndscreenStats::updateLayout() {
	CCNode::updateLayout();
	if (m_contentLayer) {
		m_contentLayer->updateLayout();
	}
};

void EndscreenStats::addChild(cocos2d::CCNode *child) {
	HandleNewChild(child);
	if (m_init) {
		m_contentLayer->CCNode::addChild(child);
	} else {
		CCNode::addChild(child);
	}
}

void EndscreenStats::addChild(cocos2d::CCNode *child, int zOrder) {
	HandleNewChild(child);

	if (m_init) {
		m_contentLayer->CCNode::addChild(child, zOrder);
	} else {
		CCNode::addChild(child, zOrder);
	}
}

void EndscreenStats::addChild(cocos2d::CCNode *child, int zOrder, int tag) {
	HandleNewChild(child);
	if (m_init) {
		m_contentLayer->CCNode::addChild(child, zOrder, tag);
	} else {
		CCNode::addChild(child, zOrder, tag);
	}
}

void EndscreenStats::HandleNewChild(cocos2d::CCNode *child) {
	log::debug("called");
	if (m_alreadyRan) {
		startAction(child);
	};
}

void EndscreenStats::runMostActions() {
	CCSize contentSize = {0, 5};
	this->setScale(3.f);
	this->runAction(cocos2d::CCSequence::create(
	    cocos2d::CCHide::create(),
	    cocos2d::CCDelayTime::create(delay),
	    cocos2d::CCShow::create(),
	    cocos2d::CCEaseBounceOut::create(cocos2d::CCScaleTo::create(0.3f, 1)),
	    nullptr));
	for (auto child : CCArrayExt<cocos2d::CCNode *>(m_contentLayer->getChildren())) {
		if (child == m_verticalScrollbar || child == m_horizontalScrollbar)
			continue;
		// child
		startAction(child);
		child->setPositionY(contentSize.height);
		contentSize = contentSize + child->getContentSize();
		m_contentLayer->setContentSize({this->getContentWidth(),contentSize.height});
		child->runAction(cocos2d::CCSequence::create(
		    cocos2d::CCFadeOut::create(0),
		    cocos2d::CCDelayTime::create(delay),
		    cocos2d::CCFadeIn::create(0.3f),
		    nullptr));
	};
	geode::Loader::get()->queueInMainThread([wfStats = geode::WeakRef<ESR::EndscreenStats>(this)] {
		if (auto _this = wfStats.lock()) {
			_this->updateLayout();
			_this->scrollToTop();
			_this->m_contentLayer->updateLayout();
		};
	});
	m_alreadyRan = true;
	this->setUserFlag("ran"_spr);
};

void EndscreenStats::startAction(CCNode *child) {
	ESR::HandleNode(child).send(m_endscreen.lock(), child, delay);
}


void EndscreenStats::visit() {
    if (!isVisible()) return CCNode::visit();
    if (!Shaders::s_FadeEffect.applyShader(this, [this](CCNode* child){
		// blocks the scrollbar from being drawn
        if(this->m_scrollbar != child) child->visit();
    }, [this](GLuint program) {
        /*register my own vars*/
        glUniform1f(glGetUniformLocation(program, "u_fadeSize"), 5.f / getContentSize().height);
    })) {
        /*i really only need to call this if it fails*/
        CCNode::visit();
    } else {
		/*Draws the scrollbar without the effect applied*/
        kmGLPushMatrix();
        CCNode::transform();
        if (m_scrollbar) m_scrollbar->visit();
        kmGLPopMatrix();
    }
}
/*
LET THE SHADERS BE RELOADED WHEN THE GAME IS RELOADED TO LET "Texture packs?" Mess with the Opacity shader used but also make it run better
*/
$on_game(TexturesUnloaded){
	 if (Shaders::s_FadeEffect.isReady()) {
		Shaders::s_FadeEffect.unload();
	};
}
$on_game(TexturesLoaded){
	 if (!Shaders::s_FadeEffect.isReady()) {
		auto CCFileUtils = CCFileUtils::get();
		if (CCFileUtils) {
			std::string vertPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-vert.glsl").data(), false);
			std::string fragPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-frag.glsl").data(), false);

			auto vertSrc = file::readString(vertPath);
			auto fragSrc = file::readString(fragPath);

			Shaders::s_FadeEffect.init(vertSrc.unwrap(), fragSrc.unwrap());
		} else {
			log::error("failed to CCFileUtils::get()? Report this");
		}
	};
}