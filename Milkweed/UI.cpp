/*
* File: UI.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.28
*/

#include "MW.h"

namespace Milkweed {
	bool UIComponent::rectContains(const glm::vec4& rect, const glm::vec2& p) {
		if (rect.x > p.x || rect.x + rect.z < p.x) {
			// The point is outside the rectangle on the x-axis
			return false;
		}
		if (rect.y > p.y || rect.y + rect.w < p.y) {
			// The point is outside the rectangle on the y-axis
			return false;
		}

		return true;
	}

	void UIGroup::init(Scene* parent, unsigned int ID, Font* font,
		Shader* shader, const std::string& textColorUniform) {
		// Set the parent scene and ID number of this group
		m_parent = parent;
		m_ID = ID;
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
		const glm::vec2& dimensions, float textScale, const glm::vec3& textColor,
		Justification hJustification, Justification vJustification,
		bool lineWrap) {
		m_text = text;
		m_position = position;
		m_dimensions = dimensions;
		m_textScale = textScale;
		m_textColor = textColor;
		m_hJustification = hJustification;
		m_vJustification = vJustification;
		m_lineWrap = lineWrap;
	}

	void UILabel::draw() {
		m_parent->getShader()->upload3fVector(m_parent->getTextColorUniform(),
			m_textColor);
		MW::RENDERER.submit(m_text, m_position, m_dimensions, m_textScale,
			m_parent->getFont(), m_parent->getShader(), m_hJustification,
			m_vJustification, m_lineWrap);
		MW::RENDERER.end();
	}

	void UILabel::destroy() {
		m_text = "";
		m_position = glm::vec3();
		m_dimensions = glm::vec2();
		m_textScale = 1.0f;
		m_textColor = glm::vec3();
		m_hJustification = Justification::LEFT;
		m_vJustification = Justification::BOTTOM;
	}
}