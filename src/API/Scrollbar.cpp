using namespace geode::prelude;
#include "EndscreenStats.hpp"
using namespace ESR;

EndscreenStats::Scrollbar *EndscreenStats::Scrollbar::create(CCScrollLayerExt *create) {
	auto r = new EndscreenStats::Scrollbar();
	if (r && r->init(create))
		return r;
	delete r;
	return r;
};

void EndscreenStats::Scrollbar::visit() {
	if (m_track) {
		if (auto target = getTarget()) {
            if (auto content = target->m_contentLayer) if (content->getContentHeight() < target->getContentHeight()) return;
		};
	};
	CCLayer::visit();
};

bool EndscreenStats::Scrollbar::init(CCScrollLayerExt *list) {
	if (!geode::Scrollbar::init(list))
		return false;
	m_track = getTrack();
	if (m_track)
		m_track->setOpacity(0);

	return true;
};