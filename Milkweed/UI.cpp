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
		Shader* spriteShader, Shader* textShader,
		const std::string& textColorUniform) {
		// Set the parent scene and ID number of this group
		m_parent = parent;
		m_ID = ID;
		m_font = font;
		m_spriteShader = spriteShader;
		m_textShader = textShader;
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

	void UIGroup::componentEvent(unsigned int componentID,
		unsigned int eventID) {
		m_parent->componentEvent(m_ID, componentID, eventID);
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
		Justification hJustification, Justification vJustification) {
		m_text = text;
		m_position = position;
		m_dimensions = dimensions;
		m_textScale = textScale;
		m_textColor = textColor;
		m_hJustification = hJustification;
		m_vJustification = vJustification;
	}

	void UILabel::draw() {
		m_parent->getTextShader()->upload3fVector(
			m_parent->getTextColorUniform(), m_textColor);
		MW::RENDERER.submit(m_text, m_position, m_dimensions, m_textScale,
			m_parent->getFont(), m_parent->getTextShader(), m_hJustification,
			m_vJustification);
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

	unsigned int UIButton::UNSELECTED_EVENT = 0;
	unsigned int UIButton::SELECTED_EVENT = 1;
	unsigned int UIButton::CLICKED_EVENT = 2;
	glm::vec4 UIButton::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 3.0f,
		1.0f);
	glm::vec4 UIButton::SELECTED_COORDS = glm::vec4(1.0f / 3.0f, 0.0f,
		1.0f / 3.0f, 1.0f);
	glm::vec4 UIButton::CLICKED_COORDS = glm::vec4(2.0f / 3.0f, 0.0f,
		1.0f / 3.0f, 1.0f);

	void UIButton::init(const std::string& text, const glm::vec3& position,
		const glm::vec2& dimensions, float textScale,
		const glm::vec3& textColor, Justification textHJustification,
		Justification textVJustification, Texture* texture) {
		((UILabel*)this)->init(text, position, dimensions, textScale,
			textColor, textHJustification, textVJustification);
		m_sprite.init(position, dimensions, texture);
		setPosition(position);
		m_sprite.textureCoords = UNSELECTED_COORDS;
	}

	void UIButton::draw() {
		MW::RENDERER.submit({ &m_sprite }, m_parent->getSpriteShader());
		((UILabel)*this).draw();
	}

	void UIButton::processInput() {
		// Test if the button is selected, and set texture coordinates
		// appropriately
		if (rectContains(glm::vec4(m_position.x, m_position.y, m_dimensions.x,
			m_dimensions.y), MW::INPUT.getCursorPosition(
				m_parent->getSpriteShader()->getCamera()))) {
			m_sprite.textureCoords = SELECTED_COORDS;
			if (!m_selected) {
				m_parent->componentEvent(m_ID, SELECTED_EVENT);
			}
			m_selected = true;
		}
		else {
			m_sprite.textureCoords = UNSELECTED_COORDS;
			if (m_selected) {
				m_parent->componentEvent(m_ID, UNSELECTED_EVENT);
			}
			m_selected = false;
		}

		// If the button is selected test if it's been clicked
		if (m_selected) {
			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				m_parent->componentEvent(m_ID, CLICKED_EVENT);
			}
			if (MW::INPUT.isButtonDown(B_LEFT)) {
				m_sprite.textureCoords = CLICKED_COORDS;
			}
		}
	}

	void UIButton::destroy() {
		// Destroy all the label memory for this button
		m_text = "";
		m_position = glm::vec3();
		m_dimensions = glm::vec2();
		m_textScale = 1.0f;
		m_textColor = glm::vec3();
		m_hJustification = Justification::CENTER;
		m_vJustification = Justification::CENTER;
		// Destroy this button's texture memory
		m_selected = false;
		m_sprite.destroy();
	}

	void UIButton::setPosition(const glm::vec3& position) {
		((UILabel)*this).setPosition(position);
		m_sprite.position = glm::vec3(position.x, position.y,
			position.z - DEPTH_INCREMENT);
	}
	
	void UIButton::setDimensions(const glm::vec2& dimensions) {
		((UILabel)*this).setDimensions(dimensions);
		m_sprite.dimensions = dimensions;
	}
}