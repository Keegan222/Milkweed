/*
* File: UI.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.28
*/

#include "UI.h"

namespace Milkweed {
	void UIGroup::init(Scene* parent, unsigned int ID) {
		// Set the parent scene and ID number of this group
		m_parent = parent;
		m_ID = ID;
	}

	void UIGroup::draw() {
		// Draw all components
		for (UIComponent* c : m_components) {
			c->draw();
		}
	}

	void UIGroup::processInput() {
		// Process input to all components
		for (UIComponent* c : m_components) {
			if (c->isEnabled()) {
				c->processInput();
			}
		}
	}

	void UIGroup::update(float deltaTime) {
		// Update physics in all components
		for (UIComponent* c : m_components) {
			c->update(deltaTime);
		}
	}

	void UIGroup::destroy() {
		// Destroy all components
		for (UIComponent* c : m_components) {
			c->destroy();
		}

		// Reset group identifiers and clear components
		m_parent = nullptr;
		m_ID = 0;
		m_components.clear();
	}

	void UIGroup::enable() {
		// Enable all disabled components
		for (UIComponent* c : m_components) {
			if (!c->isEnabled()) {
				c->setEnabled(true);
			}
		}
	}

	void UIGroup::disable() {
		// Disable all enabled components
		for (UIComponent* c : m_components) {
			if (c->isEnabled()) {
				c->setEnabled(false);
			}
		}
	}

	bool UIGroup::addComponent(UIComponent* c) {
		// Attempt to find the component already in the group
		std::vector<UIComponent*>::iterator it = std::find(m_components.begin(),
			m_components.end(), c);
		if (it == m_components.end()) {
			// The component is not already in this group, set its ID and add it
			c->m_ID = m_CID++;
			m_components.push_back(c);
			return true;
		}

		return false;
	}

	bool UIGroup::removeComponent(UIComponent* c) {
		// Attempt to find the component in this group
		std::vector<UIComponent*>::iterator it = std::find(m_components.begin(),
			m_components.end(), c);
		if (it != m_components.end()) {
			// The component is in the group, remove it
			m_components.erase(it);
			return true;
		}

		return false;
	}
}