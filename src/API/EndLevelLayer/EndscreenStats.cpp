using namespace geode::prelude;
#include "EndscreenStats.hpp"
#include "../Shaders/ShaderEffect.hpp"
#include <EndscreenStat.hpp>
#include <Geode/Geode.hpp>
using namespace ESR;
/*Function to load the shader*/
bool loadShader() {
	if (!Shaders::s_enabledShaders)
		return false;
	/*make sure it isn't already spawned*/
	if (!Shaders::s_FadeEffect.isReady()) {
		auto CCFileUtils = CCFileUtils::get();
		if (CCFileUtils) {
			std::string vertPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-vert.glsl").data(), false);
			std::string fragPath = CCFileUtils->fullPathForFilename(Mod::get()->expandSpriteName("fade-frag.glsl").data(), false);

			auto vertSrc = file::readString(vertPath);
			auto fragSrc = file::readString(fragPath);

			if (Shaders::s_FadeEffect.init(vertSrc.unwrap(), fragSrc.unwrap()))
				return true;
			log::error("failed to initilise shader! Forcing shaderless");
			return false;
		} else {
			log::error("failed to CCFileUtils::get()? Report this");
			return false;
		}
	}
	return true;
}
geode::Layout *EndscreenStats::createDefaultListLayout(float gap) {
	auto gapLayout = geode::ScrollLayer::createDefaultListLayout(gap);
	if (auto scl = typeinfo_cast<SimpleColumnLayout *>(gapLayout))
		scl->setPadding({0, 5, 0, 0});
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
void EndscreenStats::setContentSize(CCSize const &size) {
	if (m_scrollbar != nullptr)
		m_scrollbar->setPositionX(size.width + m_scrollbar->getContentWidth());
	if (m_clip != nullptr) {
		m_clip->setContentSize(size);
		if (auto Stencil = m_clip->getStencil())
			Stencil->setContentSize(size);
	}
	CCNode::setContentSize(size);
};
bool EndscreenStats::init() {
	if (!geode::ScrollLayer::init())
		return false;

	auto size = getContentSize();
	m_contentLayer->setLayout(createDefaultListLayout(1.0f));

	m_scrollbar = EndscreenStats::Scrollbar::create(this);
	m_scrollbar->setAnchorPoint({1, 0});
	m_scrollbar->setPositionX(size.width + m_scrollbar->getContentWidth());
	this->addChild(m_scrollbar);

	m_hasShader = loadShader();
	if (!m_hasShader) {
		/* User doesn't have shaders enabled or it failed, so lets add better clipping, this doesn't affect the alpha but I prefer this over nothing at all */
		m_contentLayer->retain();
		m_contentLayer->removeFromParentAndCleanup(false);
		m_clip = CCClippingNode::create(CCLayerColor::create(
		    ccc4(255, 255, 255, 255),
		    size.width,
		    size.height));
		m_clip->setContentSize(size);
		m_clip->addChild(m_contentLayer);
		m_contentLayer->release();
		this->addChild(m_clip);
	}

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
	return addChild(child, child->m_nZOrder, child->m_nTag);
}

void EndscreenStats::addChild(cocos2d::CCNode *child, int zOrder) {
	return addChild(child, zOrder, child->m_nTag);
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
		m_contentLayer->setContentSize({this->getContentWidth(), contentSize.height});
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
	if (!m_hasShader)
		return CCNode::visit();
	if (!isVisible())
		return CCNode::visit();
	if (!Shaders::s_FadeEffect.applyShader(this, [this](CCNode *child) {
		// blocks the scrollbar from being drawn
        if(this->m_scrollbar != child) child->visit(); }, [this](GLuint program) {
        /*register my own vars*/
        glUniform1f(glGetUniformLocation(program, "u_fadeSize"), 5.f / getContentSize().height); })) {
		/*i really only need to call this if it fails*/
		CCNode::visit();
	} else {
		/*Draws the scrollbar without the effect applied*/
		kmGLPushMatrix();
		CCNode::transform();
		if (m_scrollbar)
			m_scrollbar->visit();
		kmGLPopMatrix();
	}
}

/*
LET THE SHADERS BE RELOADED WHEN THE GAME IS RELOADED TO LET "Texture packs?" Mess with the Opacity shader used but also make it run better
*/
$on_game(TexturesUnloaded) {
	if (Shaders::s_FadeEffect.isReady()) {
		Shaders::s_FadeEffect.unload();
	};
}

$on_game(TexturesLoaded) {
	loadShader();
}

$on_mod(Loaded) {
	Shaders::s_enabledShaders = !Mod::get()->getSettingValue<bool>("shaderless");
	listenForSettingChanges<bool>("shaderless", [](bool value) {
		Shaders::s_enabledShaders = !value;
		if (Shaders::s_enabledShaders) {
			loadShader();
		} else {
			if (Shaders::s_FadeEffect.isReady()) {
				Shaders::s_FadeEffect.unload();
			};
		}
	});
	if (Shaders::s_enabledShaders) {
		loadShader();
	} else {
		if (Shaders::s_FadeEffect.isReady()) {
			Shaders::s_FadeEffect.unload();
		};
	}
};