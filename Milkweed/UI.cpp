/*
* File: UI.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.28
*/

#include "MW.h"

namespace Milkweed {
	void UIGroup::init(Scene* parent, Font* font, Shader* shader,
		const std::string& textColorUniform) {
		// Set the parent scene and ID number of this group
		m_parent = parent;
		m_font = font;
		m_shader = shader;
		m_textColorUniform = textColorUniform;
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
			c->m_parent = nullptr;
			c->destroy();
		}

		// Reset group identifiers and clear components
		m_parent = nullptr;
		m_font = nullptr;
		m_components.clear();
		m_CID = 0;
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
			c->m_parent = this;
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
			c->m_parent = nullptr;
			m_components.erase(it);
			return true;
		}

		return false;
	}

	void UILabel::init(const std::string& text, const glm::vec3& position,
		const glm::vec2& bounds, float scale, const glm::vec3& color,
		Justification justification) {
		m_text = text;
		m_position = position;
		m_bounds = bounds;
		m_scale = scale;
		m_color = color;
		m_justification = justification;
	}

	void UILabel::draw() {
		m_parent->getShader()->upload3fVector(m_parent->getTextColorUniform(),
			m_color);
		MW::RENDERER.submit(m_text, m_position, m_bounds, m_scale,
			m_parent->getFont(), m_parent->getShader(), m_justification);
		MW::RENDERER.end();
	}

	void UILabel::destroy() {
		m_text = "";
		m_position = glm::vec3();
		m_bounds = glm::vec2();
		m_scale = 1.0f;
		m_color = glm::vec3();
		m_justification = Justification::LEFT;
	}
}