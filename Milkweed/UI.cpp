/*
* File: UI.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.28
*/

#include "MW.h"

namespace Milkweed {
	namespace UI {
		bool UIComponent::RectContains(const glm::vec4& rect,
			const glm::vec2& p) {
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
			m_previousWindowDims = MW::WINDOW.getDimensions();
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

		void UIGroup::updateWindowSize() {
			// Get the scale of the window resizing
			glm::vec2 windowDims = MW::WINDOW.getDimensions();
			glm::vec2 resizeScale = glm::vec2(
				windowDims.x / m_previousWindowDims.x,
				windowDims.y / m_previousWindowDims.y);

			for (UIComponent* c : m_components) {
				c->setTextScale(c->getTextScale() * resizeScale.y);
				if (c->isOwned()) {
					continue;
				}
				c->setDimensions(c->getDimensions() * resizeScale);
				glm::vec3 cPosition = c->getPosition();
				cPosition.x *= resizeScale.x;
				cPosition.y *= resizeScale.y;
				c->setPosition(cPosition);
			}

			if (m_spriteShader != nullptr) {
				m_spriteShader->getCamera()->destroy();
				m_spriteShader->getCamera()->init();
			}
			if (m_textShader != nullptr && m_spriteShader != m_textShader) {
				m_textShader->getCamera()->destroy();
				m_textShader->getCamera()->init();
			}

			m_previousWindowDims = windowDims;
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
			std::cout << "Adding component" << std::endl;
			// Attempt to find the component already in the group
			std::vector<UIComponent*>::iterator it = std::find(
				m_components.begin(), m_components.end(), c);
			std::cout << "Looked for it" << std::endl;
			if (it == m_components.end()) {
				std::cout << "It wasn't found" << std::endl;
				// The component is not already in this group, set its ID
				// and add it
				c->m_ID = m_CID++;
				std::cout << "Incremented ID" << std::endl;
				c->m_parent = this;
				std::cout << "Set parent" << std::endl;
				m_components.push_back(c);
				c->add();
				std::cout << "Pushed it back" << std::endl;
				return true;
			}
			std::cout << "It was found" << std::endl;

			return false;
		}

		void UIGroup::componentEvent(unsigned int componentID,
			unsigned int eventID) {
			m_parent->componentEvent(m_ID, componentID, eventID);
		}

		bool UIGroup::removeComponent(UIComponent* c) {
			// Attempt to find the component in this group
			std::vector<UIComponent*>::iterator it = std::find(
				m_components.begin(), m_components.end(), c);
			if (it != m_components.end()) {
				// The component is in the group, remove it
				c->m_parent = nullptr;
				m_components.erase(it);
				return true;
			}

			return false;
		}

		void TextLabel::init(const std::string& text,
			const glm::vec3& normalPosition,
			const glm::vec2& normalDimensions,
			const glm::vec3& normalTextPosition,
			float textScale, const glm::vec3& textColor,
			Justification hJustification, Justification vJustification) {
			glm::vec2 winDims = MW::WINDOW.getDimensions();
			m_text = text;
			m_position = glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z);
			m_dimensions = normalDimensions * winDims;
			m_textPosition = glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z);
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

		glm::vec4 Button::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 3.0f,
			1.0f);
		glm::vec4 Button::SELECTED_COORDS = glm::vec4(1.0f / 3.0f, 0.0f,
			1.0f / 3.0f, 1.0f);
		glm::vec4 Button::CLICKED_COORDS = glm::vec4(2.0f / 3.0f, 0.0f,
			1.0f / 3.0f, 1.0f);

		void Button::init(const std::string& text,
			const glm::vec3& normalPosition,
			const glm::vec2& normalDimensions, float textScale,
			const glm::vec3& textColor, Justification textHJustification,
			Justification textVJustification, Texture* texture) {
			glm::vec2 winDims = MW::WINDOW.getDimensions();
			((TextLabel*)this)->init(text, normalPosition, normalDimensions, 
				normalPosition, textScale, textColor, textHJustification,
				textVJustification);
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			setPosition(m_position);
			m_sprite.textureCoords = UNSELECTED_COORDS;
		}

		void Button::setPosition(const glm::vec3& position) {
			// Update the position of the text label
			m_position = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_textPosition = position;
			// Update the position of the background sprite
			m_sprite.position = glm::vec3(position.x, position.y,
				position.z);
		}

		void Button::setDimensions(const glm::vec2& dimensions) {
			// Update the dimensions of the text label
			m_dimensions = dimensions;
			// Update the dimensions of the background sprite
			m_sprite.dimensions = dimensions;
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
			if (RectContains(glm::vec4(m_position.x, m_position.y, m_dimensions.x,
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

		glm::vec4 TextBox::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f,
			1.0f / 2.0f, 1.0f);
		glm::vec4 TextBox::SELECTED_COORDS = glm::vec4(1.0f / 2.0f, 0.0f,
			1.0f / 2.0f, 1.0f);

		void TextBox::init(const std::string& labelText,
			const std::string& text, const glm::vec3& normalPosition,
			const glm::vec2& normalDimensions, float textScale,
			const glm::vec3& textColor, Justification textHJustification,
			Justification textVJustification, Texture* texture,
			Texture* cursorTexture, int maxCharacters) {
			// Set the text box up as a text label
			((TextLabel*)this)->init(text, normalPosition, normalDimensions,
				normalPosition, textScale, textColor, textHJustification,
				textVJustification);

			// Set up the label of this text box
			std::cout << "Initializing label" << std::endl;
			m_label.init(labelText, glm::vec3(normalPosition.x,
				normalPosition.y + normalDimensions.y, normalPosition.z),
				normalDimensions, glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y, normalPosition.z),
				textScale, textColor, textHJustification, textVJustification);
			m_label.setOwned(true);
			std::cout << "Initialized label" << std::endl;

			glm::vec2 winDims = MW::WINDOW.getDimensions();
			
			// Set up the cursor and background of this text box
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			m_sprite.textureCoords = UNSELECTED_COORDS;
			m_cursor.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				glm::vec2(1.0f, normalDimensions.y * winDims.y),
				cursorTexture);

			// Set the position and dimensions
			setPosition(m_position);
			setDimensions(m_dimensions);

			// Assign internal memory
			m_maxCharacters = maxCharacters;

			MW::INPUT.addInputListener(this);
		}

		void TextBox::setPosition(const glm::vec3& position) {
			std::cout << "Updated position to " << position.x << ", "
				<< position.y << ", " << position.z << std::endl;
			// Update the position of the label
			m_label.setPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z));
			m_label.setTextPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z + DEPTH_INCREMENT));
			// Update the position of the text
			m_position = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_textPosition = glm::vec3(position.x, position.y,
				position.z + 2 * DEPTH_INCREMENT);
			// Update the position of the background sprite
			m_sprite.position = position;
			// The cursor has moved
			updateCursorPosition();
		}

		void TextBox::setDimensions(const glm::vec2& dimensions) {
			// Set the label's dimensions
			m_label.setDimensions(dimensions);
			// Set the text's dimensions
			m_dimensions = dimensions;
			// Set the background sprite's dimensions
			m_sprite.dimensions = dimensions;
			// The size of the cursor has changed
			updateCursorPosition();
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

		void TextBox::add() {
			std::cout << "Adding" << std::endl;
			m_parent->addComponent(&m_label);
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
				if (RectContains(glm::vec4(m_position.x, m_position.y,
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
				|| (MW::INPUT.isKeyDown(F_BACKSPACE)
					&& m_timer > UI_UPDATE_TIME)) {
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
			m_label.destroy();
			m_sprite.destroy();
			m_maxCharacters = 0;
			m_selected = false;
			m_cursorPosition = 0;

			MW::INPUT.removeInputListener(this);
		}

		void TextBox::updateCursorPosition() {
			float textWidth = 0.0f;
			for (char c : m_text.substr(0, m_cursorPosition)) {
				textWidth += (float)m_parent->getFont()->characters[c].offset
					* m_textScale;
			}

			m_cursor.position = glm::vec3(m_textPosition.x + textWidth,
				m_position.y, m_position.z + DEPTH_INCREMENT);
			m_cursor.dimensions = glm::vec2(1.0f, m_dimensions.y);
		}

		glm::vec4 Switch::ON_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 2.0f, 1.0f);
		glm::vec4 Switch::OFF_COORDS = glm::vec4(1.0f / 2.0f, 0.0f,
			1.0f / 2.0f, 1.0f);

		void Switch::init(const std::string& labelText, const std::string& text,
			const glm::vec3& normalPosition, const glm::vec2& normalDimensions,
			float textScale, const glm::vec3& textColor,
			Justification textHJustification, Justification textVJustification,
			Texture* texture, bool on) {
			m_label.init(labelText, glm::vec3(normalPosition.x,
				normalPosition.y + normalDimensions.y, normalPosition.z),
				normalDimensions, glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y, normalPosition.z),
				textScale, textColor, textHJustification, textVJustification);
			m_label.setOwned(true);
			((TextLabel*)this)->init(text, normalPosition, normalDimensions,
				normalPosition, textScale, textColor, Justification::CENTER,
				textVJustification);
			
			glm::vec2 winDims = MW::WINDOW.getDimensions();
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			setPosition(m_position);
			setDimensions(m_dimensions);
			setOn(on);
		}

		void Switch::setPosition(const glm::vec3& position) {
			// Set the position of the label
			m_label.setPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z));
			m_label.setTextPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z + DEPTH_INCREMENT));
			// Set the position of the text
			m_position = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_textPosition = glm::vec3(position.x, position.y,
				position.z + 2 * DEPTH_INCREMENT);
			// Set the position of the background sprite
			m_sprite.position = position;
		}

		void Switch::setDimensions(const glm::vec2& dimensions) {
			m_dimensions = dimensions;
			m_sprite.dimensions = dimensions;
			m_label.setDimensions(dimensions);
		}

		void Switch::setOn(bool on) {
			m_on = on;
			if (on) {
				m_sprite.textureCoords = ON_COORDS;
			}
			else {
				m_sprite.textureCoords = OFF_COORDS;
			}
		}

		void Switch::add() {
			m_parent->addComponent(&m_label);
		}

		void Switch::draw() {
			MW::RENDERER.submit({ &m_sprite }, m_parent->getSpriteShader());
			((TextLabel)*this).draw();
		}

		void Switch::processInput() {
			if (!m_enabled) {
				return;
			}

			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				if (RectContains(glm::vec4(m_position.x, m_position.y,
					m_dimensions.x, m_dimensions.y),
					MW::INPUT.getCursorPosition(
						m_parent->getSpriteShader()->getCamera()))) {
					if (m_on) {
						m_on = false;
						m_sprite.textureCoords = OFF_COORDS;
						m_parent->componentEvent(m_ID, OFF_EVENT);
					}
					else {
						m_on = true;
						m_sprite.textureCoords = ON_COORDS;
						m_parent->componentEvent(m_ID, ON_EVENT);
					}
				}
			}
		}

		void Switch::destroy() {
			m_sprite.destroy();
			m_label.destroy();
			m_text = "";
			m_position = glm::vec3();
			m_dimensions = glm::vec2();
			m_textScale = 1.0f;
			m_textColor = glm::vec3();
			m_hJustification = Justification::LEFT;
			m_vJustification = Justification::CENTER;
			m_on = false;
		}

		glm::vec4 Slider::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 2.0f,
			1.0f);
		glm::vec4 Slider::SELECTED_COORDS = glm::vec4(1.0f / 2.0f, 0.0f,
			1.0f / 2.0f, 1.0f);

		void Slider::init(const std::string& labelText,
			const glm::vec3& normalPosition, const glm::vec2& normalDimensions,
			float textScale, const glm::vec3& textColor,
			Justification textHJustification, Justification textVJustification,
			Texture* texture, Texture* cursorTexture, int min, int value,
			int max) {
			m_labelText = labelText;
			m_min = min;
			m_max = max;
			((TextLabel*)this)->init(labelText + ": " + std::to_string(min),
				glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y,
					normalPosition.z), normalDimensions,
				glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y, normalPosition.z),
				textScale, textColor, textHJustification, textVJustification);
			glm::vec2 winDims = MW::WINDOW.getDimensions();
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			m_sprite.textureCoords = UNSELECTED_COORDS;
			m_cursor.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				glm::vec2(1.0f, normalDimensions.y * winDims.y), cursorTexture);
			setPosition(m_sprite.position);
			setDimensions(m_sprite.dimensions);
			setValue(value);
		}

		void Slider::setPosition(const glm::vec3& position) {
			// Set the position of the label
			m_position = glm::vec3(position.x, position.y + m_dimensions.y,
				position.z);
			m_textPosition = glm::vec3(position.x, position.y + m_dimensions.y,
				position.z);
			// Set the position of the background sprite
			m_sprite.position = position;
			updateCursorPosition();
		}

		void Slider::setDimensions(const glm::vec2& dimensions) {
			m_dimensions = dimensions;
			m_sprite.dimensions = dimensions;
			updateCursorPosition();
		}

		void Slider::setValue(int value) {
			m_value = value;
			updateCursorPosition();
		}

		void Slider::draw() {
			m_text = m_labelText + ": " + std::to_string(m_value);
			((TextLabel)*this).draw();
			MW::RENDERER.submit({ &m_sprite, &m_cursor },
				m_parent->getSpriteShader());
		}

		void Slider::processInput() {
			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				if (RectContains(glm::vec4(m_sprite.position.x,
					m_sprite.position.y, m_dimensions.x, m_dimensions.y),
					MW::INPUT.getCursorPosition(
						m_parent->getSpriteShader()->getCamera()))) {
					if (!m_selected) {
						m_selected = true;
						m_sprite.textureCoords = SELECTED_COORDS;
						m_parent->componentEvent(m_ID, SELECTED_EVENT);
					}
				}
			}
			else if (MW::INPUT.isButtonReleased(B_LEFT)) {
				if (m_selected) {
					m_selected = false;
					m_sprite.textureCoords = UNSELECTED_COORDS;
					m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
				}
			}

			if (m_selected && MW::INPUT.isButtonDown(B_LEFT)) {
				glm::vec2 mousePos = MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera());
				if (mousePos.x < m_sprite.position.x) {
					m_value = m_min;
				}
				else if (mousePos.x
				> m_sprite.position.x + m_sprite.dimensions.x) {
					m_value = m_max;
				}
				else {
					m_value = m_min
						+ (int)(((mousePos.x - m_sprite.position.x)
							/ (m_sprite.dimensions.x)) * (m_max - m_min));
				}
				updateCursorPosition();
			}
		}

		void Slider::destroy() {
			m_text = "";
			m_position = glm::vec3();
			m_dimensions = glm::vec2();
			m_textScale = 1.0f;
			m_textColor = glm::vec3();
			m_hJustification = Justification::LEFT;
			m_vJustification = Justification::CENTER;
			m_labelText = "";
			m_sprite.destroy();
			m_cursor.destroy();
		}

		void Slider::updateCursorPosition() {
			m_cursor.position.x = m_sprite.position.x
				+ ((float)(m_value - m_min) / (float)(m_max - m_min))
				* m_sprite.dimensions.x;
			m_cursor.position.y = m_sprite.position.y;
			m_cursor.dimensions = glm::vec2(1.0f, m_sprite.dimensions.y);
		}

		glm::vec4 Cycle::UNSELECTED_LEFT_COORDS = glm::vec4(0.0f, 0.0f,
			1.0f / 3.0f, 1.0f);
		glm::vec4 Cycle::SELECTED_LEFT_COORDS = glm::vec4(1.0f / 3.0f,
			0.0f, 1.0f / 3.0f, 1.0f);
		glm::vec4 Cycle::CLICKED_LEFT_COORDS = glm::vec4(2.0f / 3.0f,
			0.0f, 1.0f / 3.0f, 1.0f);
		glm::vec4 Cycle::UNSELECTED_RIGHT_COORDS = glm::vec4(1.0f / 3.0f, 0.0f,
			-(1.0f / 3.0f), 1.0f);
		glm::vec4 Cycle::SELECTED_RIGHT_COORDS = glm::vec4(2.0f / 3.0f, 0.0f,
			-(1.0f / 3.0f), 1.0f);
		glm::vec4 Cycle::CLICKED_RIGHT_COORDS = glm::vec4(1.0f, 0.0f,
			-(1.0f / 3.0f), 1.0f);

		void Cycle::init(const std::string& labelText,
			const std::vector<std::string>& options,
			const glm::vec3& normalPosition, const glm::vec2& normalDimensions,
			float arrowWidth, float textScale, const glm::vec3& textColor,
			Justification textHJustification, Justification textVJustification,
			Texture* texture, Texture* arrowTexture, unsigned int selection) {
			// Initialize this cycle's memory
			for (std::string option : options) {
				m_options.push_back(option);
			}
			m_arrowWidth = arrowWidth;

			// Initialize the label to appear above this cycle
			m_label.init(labelText, glm::vec3(normalPosition.x,
				normalPosition.y + normalDimensions.y, normalPosition.z),
				normalDimensions, glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y, normalPosition.z),
				textScale, textColor, textHJustification, textVJustification);
			m_label.setOwned(true);

			// Set up this cycle as a text label
			((TextLabel*)this)->init(options[0],
				normalPosition, normalDimensions, normalPosition,
				textScale, textColor, Justification::CENTER,
				textVJustification);

			glm::vec2 winDims = MW::WINDOW.getDimensions();
			// Set up the background sprite and left / right arrow sprites
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			m_leftArrow.init(m_sprite.position,
				glm::vec2(arrowWidth, m_sprite.dimensions.y), arrowTexture);
			m_rightArrow.init(glm::vec3(
				m_sprite.position.x + m_sprite.dimensions.x,
				m_sprite.position.y, m_sprite.position.z),
				glm::vec2(arrowWidth, m_sprite.dimensions.y),
				arrowTexture);
			m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
			m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;

			setPosition(m_position);
			setDimensions(m_dimensions);
			setSelection(selection);
		}

		void Cycle::setPosition(const glm::vec3& position) {
			m_label.setPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z));
			m_label.setTextPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z));
			m_position = position;
			m_textPosition = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_sprite.position = m_position;
			m_leftArrow.position = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_rightArrow.position = glm::vec3(position.x
					+ m_dimensions.x - m_arrowWidth,
				position.y, position.z + DEPTH_INCREMENT);
		}

		void Cycle::setDimensions(const glm::vec2& dimensions) {
			m_label.setDimensions(dimensions);
			m_dimensions = dimensions;
			m_sprite.dimensions = dimensions;
			m_leftArrow.dimensions = glm::vec2(m_arrowWidth, dimensions.y);
			m_rightArrow.dimensions = glm::vec2(m_arrowWidth, dimensions.y);
		}

		void Cycle::setSelection(unsigned int selection) {
			m_selection = selection;
			m_text = m_options[selection];
		}

		void Cycle::add() {
			m_parent->addComponent(&m_label);
		}

		void Cycle::draw() {
			((TextLabel)*this).draw();
			MW::RENDERER.submit({ &m_sprite, &m_leftArrow, &m_rightArrow },
				m_parent->getSpriteShader());
		}

		void Cycle::processInput() {
			glm::vec2 mousePos = MW::INPUT.getCursorPosition(
				m_parent->getSpriteShader()->getCamera());

			// Test if the left arrow is selected
			if (RectContains(glm::vec4(m_leftArrow.position.x,
				m_leftArrow.position.y, m_leftArrow.dimensions.x,
				m_leftArrow.dimensions.y), mousePos)) {
				if (!m_leftArrowSelected) {
					m_leftArrowSelected = true;
					m_leftArrow.textureCoords = SELECTED_LEFT_COORDS;
				}
			}
			else {
				if (m_leftArrowSelected) {
					m_leftArrowSelected = false;
					m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
				}
			}

			// Test if the right arrow is selected
			if (RectContains(glm::vec4(m_rightArrow.position.x,
				m_rightArrow.position.y, m_rightArrow.dimensions.x,
				m_rightArrow.dimensions.y), mousePos)) {
				if (!m_rightArrowSelected) {
					m_rightArrowSelected = true;
					m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS;
				}
			}
			else {
				if (m_rightArrowSelected) {
					m_rightArrowSelected = false;
					m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;
				}
			}

			// Test if either button is clicked
			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				bool updated = false;
				if (m_leftArrowSelected) {
					if (m_selection > 0) {
						m_selection--;
					}
					else {
						m_selection = m_options.size() - 1;
					}
					m_leftArrow.textureCoords = CLICKED_LEFT_COORDS;
					updated = true;
				}
				else if (m_rightArrowSelected) {
					if (m_selection < m_options.size() - 1) {
						m_selection++;
					}
					else {
						m_selection = 0;
					}
					m_rightArrow.textureCoords = CLICKED_RIGHT_COORDS;
					updated = true;
				}

				if (updated) {
					m_text = m_options[m_selection];
					m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
				}
			}
			else if (MW::INPUT.isButtonReleased(B_LEFT)) {
				// Reset the texture coordinates from clicked
				m_rightArrowSelected
					? m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS
					: m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;
				m_leftArrowSelected
					? m_leftArrow.textureCoords = SELECTED_LEFT_COORDS
					: m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
			}
		}

		void Cycle::destroy() {
			// Destroy the text label memory of this cycle
			m_text = "";
			m_position = glm::vec3();
			m_dimensions = glm::vec2();
			m_textScale = 1.0f;
			m_textColor = glm::vec3();
			m_hJustification = Justification::LEFT;
			m_vJustification = Justification::CENTER;

			// Destroy the background and arrows sprites
			m_sprite.destroy();
			m_leftArrow.destroy();
			m_rightArrow.destroy();
			m_arrowWidth = 0.0f;
			m_selection = 0;
			m_options.clear();
		}
	}
}