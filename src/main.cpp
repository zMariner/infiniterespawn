#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

// global toggle
static bool s_infiniteRespawnEnabled = true;

// ---- PlayLayer hook ----
class $modify(InfiniteRespawn, PlayLayer) {
    void destroyPlayer(PlayerObject* player, GameObject* cause) {
        if (!s_infiniteRespawnEnabled) {
            PlayLayer::destroyPlayer(player, cause);
            return;
        }

        log::info("Death detected — auto respawn blocked");

        // Call base destroyPlayer so death animation still plays
        PlayLayer::destroyPlayer(player, cause);

        // Cancel scheduled reset
        this->stopAllActions();
    }
};

// ---- Pause menu button ----
class $modify(InfiniteRespawnPause, PauseLayer) {
    struct Fields {
        CCNode* iconNode = nullptr;
    };

    void customSetup() {
        PauseLayer::customSetup();

        auto leftMenu = this->getChildByID("left-button-menu");
        if (!leftMenu) return;

        // How many buttons are already there?
        int count = leftMenu->getChildrenCount();

        // Create a circular button sprite
        auto circle = CCSprite::createWithSpriteFrameName("GJ_button_01.png");
        circle->setScale(0.8f);

        // Create a container node for the icon
        m_fields->iconNode = CCNode::create();
        m_fields->iconNode->setPosition(circle->getContentSize() / 2);
        circle->addChild(m_fields->iconNode);

        // Update the icon based on current state
        updateIcon();

        auto button = CCMenuItemSpriteExtra::create(
            circle,
            this,
            menu_selector(InfiniteRespawnPause::onToggle)
        );

        // Vertical spacing between buttons
        float spacing = 55.f;

        // Position BELOW existing buttons
        button->setPosition({
            0.f,
            -spacing * count
        });

        leftMenu->addChild(button);
    }

    void updateIcon() {
        if (!m_fields->iconNode) return;

        // Remove old icon
        m_fields->iconNode->removeAllChildren();

        // Add new icon based on state
        if (s_infiniteRespawnEnabled) {
            // Background button
            auto background = CCSprite::createWithSpriteFrameName("GJ_button_01.png");
            background->setScale(0.7f);
            background->setColor(ccc3(150, 150, 150));
            m_fields->iconNode->addChild(background);

            // Checkmark on top
            auto checkmark = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
            checkmark->setScale(1.350f);
            m_fields->iconNode->addChild(checkmark);
        } else {
            auto button = CCSprite::createWithSpriteFrameName("GJ_button_01.png");
            button->setScale(0.7f);
            button->setColor(ccc3(150, 150, 150));
            m_fields->iconNode->addChild(button);
        }
    }

    void onToggle(CCObject*) {
        s_infiniteRespawnEnabled = !s_infiniteRespawnEnabled;
        updateIcon();
    }
};
