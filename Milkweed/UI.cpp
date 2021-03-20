/*
* File: UI.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.28
*/

#include "MW.h"

namespace Milkweed {
	namespace UI {
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

		void TextLabel::init(const std::string& text, const glm::vec3& position,
			const glm::vec2& dimensions, const glm::vec3& textPosition,
			float textScale, const glm::vec3& textColor,
			Justification hJustification, Justification vJustification) {
			m_text = text;
			m_position = position;
			m_dimensions = dimensions;
			m_textPosition = textPosition;
			m_textScale = textScale;
			m_textColor = textColor;
			m_hJustification = hJustification;
			m_vJustification = vJustification;
		}

		void TextLabel::draw() {
			m_parent->getTextShader()->upload3fVector(
				m_parent->getTextColorUniform(), m_textColor);
			MW::RENDERER.submit(m_text, m_textPosition, glm::vec4(m_position.x,
				m_position.y, m_dimensions.x, m_dimensions.y), m_textScale,
				m_parent->getFont(), m_parent->getTextShader(), m_hJustification,
				m_vJustification);
		}

		void TextLabel::destroy() {
			m_text = "";
			m_position = glm::vec3();
			m_dimensions = glm::vec2();
			m_textScale = 1.0f;
			m_textColor = glm::vec3();
			m_hJustification = Justification::LEFT;
			m_vJustification = Justification::BOTTOM;
		}

		unsigned int Button::UNSELECTED_EVENT = 0;
		unsigned int Button::SELECTED_EVENT = 1;
		unsigned int Button::CLICKED_EVENT = 2;
		glm::vec4 Button::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 3.0f,
			1.0f);
		glm::vec4 Button::SELECTED_COORDS = glm::vec4(1.0f / 3.0f, 0.0f,
			1.0f / 3.0f, 1.0f);
		glm::vec4 Button::CLICKED_COORDS = glm::vec4(2.0f / 3.0f, 0.0f,
			1.0f / 3.0f, 1.0f);

		void Button::init(const std::string& text, const glm::vec3& position,
			const glm::vec2& dimensions, float textScale,
			const glm::vec3& textColor, Justification textHJustification,
			Justification textVJustification, Texture* texture) {
			((TextLabel*)this)->init(text, position, dimensions, position,
				textScale, textColor, textHJustification, textVJustification);
			m_sprite.init(position, dimensions, texture);
			setPosition(position);
			m_sprite.textureCoords = UNSELECTED_COORDS;
		}

		void Button::draw() {
			MW::RENDERER.submit({ &m_sprite }, m_parent->getSpriteShader());
			((TextLabel)*this).draw();
		}

		void Button::processInput() {
			// Process no input if this button is disabled
			if (!m_enabled) {
				m_selected = false;
				m_sprite.textureCoords = UNSELECTED_COORDS;
				return;
			}

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

		void Button::destroy() {
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

		void Button::setPosition(const glm::vec3& position) {
			m_position = position;
			m_textPosition = position;
			m_sprite.position = glm::vec3(position.x, position.y,
				position.z - DEPTH_INCREMENT);
		}

		void Button::setDimensions(const glm::vec2& dimensions) {
			m_dimensions = dimensions;
			m_sprite.dimensions = dimensions;
		}

		unsigned int TextBox::RETURN_EVENT = 0;
		unsigned int TextBox::SELECTED_EVENT = 1;
		unsigned int TextBox::UNSELECTED_EVENT = 2;
		glm::vec4 TextBox::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f,
			1.0f / 2.0f, 1.0f);
		glm::vec4 TextBox::SELECTED_COORDS = glm::vec4(1.0f / 2.0f, 0.0f,
			1.0f / 2.0f, 1.0f);

		void TextBox::init(const std::string& text, const glm::vec3& position,
			const glm::vec2& dimensions, float textScale,
			const glm::vec3& textColor, Justification textHJustification,
			Justification textVJustification, Texture* texture,
			Texture* cursorTexture, int maxCharacters) {
			((TextLabel*)this)->init(text, position, dimensions, position,
				textScale, textColor, textHJustification, textVJustification);

			m_sprite.init(position, dimensions, texture);
			m_cursor.init(position, glm::vec2(1.0f, dimensions.y),
				cursorTexture);
			setPosition(position);
			m_sprite.textureCoords = UNSELECTED_COORDS;

			m_maxCharacters = maxCharacters;

			MW::INPUT.addInputListener(this);
		}

		void TextBox::draw() {
			MW::RENDERER.submit({ &m_sprite, &m_cursor },
				m_parent->getSpriteShader());
			((TextLabel)*this).draw();
		}

#define UI_UPDATE_TIME 10.0f

		void TextBox::processInput() {
			// Take no input if disabled
			if (!m_enabled) {
				m_sprite.textureCoords = UNSELECTED_COORDS;
				m_selected = false;
				return;
			}

			// Take mouse input for selection
			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				if (rectContains(glm::vec4(m_position.x, m_position.y,
					m_dimensions.x, m_dimensions.y),
					MW::INPUT.getCursorPosition(m_parent->getSpriteShader()
						->getCamera()))) {
					// Selected
					m_selected = true;
					m_sprite.textureCoords = SELECTED_COORDS;
				}
				else {
					// Unselected
					m_selected = false;
					m_sprite.textureCoords = UNSELECTED_COORDS;
				}
			}

			if (!m_selected) {
				return;
			}

			// Set the cursor position
			if (MW::INPUT.isKeyPressed(F_LEFT)
				|| (MW::INPUT.isKeyDown(F_LEFT) && m_timer > UI_UPDATE_TIME)) {
				if (m_cursorPosition > 0) {
					m_timer = 0.0f;
					m_cursorPosition--;
					updateCursorPosition();
					// Update the position of the text if necessary
					if (m_cursor.position.x < m_position.x) {
						m_textPosition.x += m_dimensions.x / 4.0f;
						if (m_textPosition.x > m_position.x) {
							m_textPosition.x = m_position.x;
						}
						updateCursorPosition();
					}
				}
			}
			else if (MW::INPUT.isKeyPressed(F_RIGHT)
				|| (MW::INPUT.isKeyDown(F_RIGHT) && m_timer > UI_UPDATE_TIME)) {
				if (m_cursorPosition < m_text.length()) {
					m_timer = 0.0f;
					m_cursorPosition++;
					updateCursorPosition();
					// Update the position of the text if necessary
					if (m_cursor.position.x + 1.0f
						> m_position.x + m_dimensions.x) {
						m_textPosition.x -= m_dimensions.x / 4.0f;
						updateCursorPosition();
					}
				}
			}

			// Handle backspace
			if (MW::INPUT.isKeyPressed(F_BACKSPACE)
				|| (MW::INPUT.isKeyDown(F_BACKSPACE) && m_timer > UI_UPDATE_TIME)) {
				if (m_cursorPosition > 0 && m_text.length() > 0) {
					m_timer = 0.0f;
					m_text = m_text.substr(0, m_cursorPosition - 1)
						+ m_text.substr(m_cursorPosition, m_text.length());
					m_cursorPosition--;
					updateCursorPosition();
					// Update the position of the text if necessary
					if (m_cursor.position.x < m_position.x) {
						m_textPosition.x += m_dimensions.x / 4.0f;
						if (m_textPosition.x > m_position.x) {
							m_textPosition.x = m_position.x;
						}
					}
				}
			}
		}

		void TextBox::update(float deltaTime) {
			m_timer += deltaTime;
			if (m_timer > UI_UPDATE_TIME * 5) {
				m_timer = 0.0f;
			}
		}

		void TextBox::destroy() {
			m_text = "";
			m_position = glm::vec3();
			m_dimensions = glm::vec2();
			m_textScale = 1.0f;
			m_textColor = glm::vec3();
			m_hJustification = Justification::LEFT;
			m_vJustification = Justification::CENTER;

			m_sprite.destroy();
			m_maxCharacters = 0;
			m_selected = false;
			m_cursorPosition = 0;

			MW::INPUT.removeInputListener(this);
		}

		void TextBox::setPosition(const glm::vec3& position) {
			m_sprite.position = position;
			m_position = glm::vec3(position.x, position.y,
				position.z + 2 * DEPTH_INCREMENT);
			updateCursorPosition();
		}

		void TextBox::setDimensions(const glm::vec2& dimensions) {
			m_dimensions = dimensions;
			m_sprite.dimensions = dimensions;
			m_cursor.dimensions.y = dimensions.y;
		}

		void TextBox::textTyped(char text) {
			if (!m_selected) {
				return;
			}

			m_text = m_text.substr(0, m_cursorPosition)
				+ std::string(1, text)
				+ m_text.substr(m_cursorPosition, m_text.length());
			m_cursorPosition++;
			updateCursorPosition();
			// Update the position of the text if necessary
			if (m_cursor.position.x + 1.0f
						> m_position.x + m_dimensions.x) {
				m_textPosition.x -= m_dimensions.x / 4.0f;
				updateCursorPosition();
			}
		}

		void TextBox::updateCursorPosition() {
			float textWidth = 0.0f;
			for (char c : m_text.substr(0, m_cursorPosition)) {
				textWidth += (float)m_parent->getFont()->characters[c].offset
					* m_textScale;
			}

			m_cursor.position = glm::vec3(m_textPosition.x + textWidth,
				m_position.y, m_position.z + DEPTH_INCREMENT);
		}
	}
}