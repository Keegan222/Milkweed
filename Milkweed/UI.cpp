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

		void UIComponent::setDirections(UIComponent* up, UIComponent* down,
			UIComponent* left, UIComponent* right) {
			m_up = up;
			m_down = down;
			m_left = left;
			m_right = right;
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
				if (c->isVisible()) {
					c->draw();
				}
			}
		}

		void UIGroup::processInput() {
			if (m_selectedComponent != nullptr) {
				m_selectedComponent->setSelected(true);
			}

			// Process selected component changing input
			// From the keyboard
			if (MW::INPUT.isKeyPressed(Key::F_TAB)) {
				if (MW::INPUT.isKeyDown(Key::F_LEFT_SHIFT)) {
					// Move to the component to the left
					moveLeft();
				}
				else {
					// Move to the component to the right
					moveRight();
				}
			}

#define AXIS_THRESHOLD 0.85f
			// From gamepads
			if (m_gamepad != NO_GAMEPAD) {
				if (m_gamepad == ANY_GAMEPAD) {
					int gp = NO_GAMEPAD;
					// Test for input from all connected gamepads
					if (MW::INPUT.isGamepadAxisMoved(GA_LEFT_X, nullptr, &gp)) {
						if (gp != NO_GAMEPAD) {
							float apos = MW::INPUT.getGamepadAxisPosition(gp,
								GA_LEFT_X);
							if (apos > AXIS_THRESHOLD) {
								if (!m_gpRight) {
									m_gpRight = true;
									moveRight();
								}
							}
							else {
								if (m_gpRight) {
									m_gpRight = false;
								}
							}
							if (apos < -AXIS_THRESHOLD) {
								if (!m_gpLeft) {
									m_gpLeft = true;
									moveLeft();
								}
							}
							else {
								if (m_gpLeft) {
									m_gpLeft = false;
								}
							}
						}
					}
					if (MW::INPUT.isGamepadAxisMoved(GA_LEFT_Y, nullptr, &gp)) {
						if (gp != NO_GAMEPAD) {
							float apos = MW::INPUT.getGamepadAxisPosition(gp,
								GA_LEFT_Y);
							if (apos > AXIS_THRESHOLD) {
								if (!m_gpDown) {
									m_gpDown = true;
									moveDown();
								}
							}
							else {
								if (m_gpDown) {
									m_gpDown = false;
								}
							}
							if (apos < -AXIS_THRESHOLD) {
								if (!m_gpUp) {
									m_gpUp = true;
									moveUp();
								}
							}
							else {
								if (m_gpUp) {
									m_gpUp = false;
								}
							}
						}
					}
				}
				else {
					// Test for input from the gamepad which controls this
					// group
					if (MW::INPUT.isGamepadAxisMoved(m_gamepad, GA_LEFT_X)) {
						float apos = MW::INPUT.getGamepadAxisPosition(m_gamepad,
							GA_LEFT_X);
						if (apos > AXIS_THRESHOLD) {
							if (!m_gpRight) {
								m_gpRight = true;
								moveRight();
							}
						}
						else {
							if (m_gpRight) {
								m_gpRight = false;
							}
						}
						if (apos < -AXIS_THRESHOLD) {
							if (!m_gpLeft) {
								m_gpLeft = true;
								moveLeft();
							}
						}
						else {
							if (m_gpLeft) {
								m_gpLeft = false;
							}
						}
					}
					if (MW::INPUT.isGamepadAxisMoved(m_gamepad, GA_LEFT_Y)) {
						float apos = MW::INPUT.getGamepadAxisPosition(m_gamepad,
							GA_LEFT_Y);
						if (apos > AXIS_THRESHOLD) {
							if (!m_gpDown) {
								m_gpDown = true;
								moveDown();
							}
						}
						else {
							if (m_gpDown) {
								m_gpDown = false;
							}
						}
						if (apos < -AXIS_THRESHOLD) {
							if (!m_gpUp) {
								m_gpUp = true;
								moveUp();
							}
						}
						else {
							if (m_gpUp) {
								m_gpUp = false;
							}
						}
					}
				}
			}

			// Process input to all components
			for (UIComponent* c : m_components) {
				if (c->isEnabled()) {
					c->processInput();
				}
			}
		}

		glm::vec2 UIGroup::updateWindowSize() {
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
			return resizeScale;
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

		void UIGroup::setEnabled(bool enabled) {
			for (UIComponent* c : m_components) {
				c->setEnabled(enabled);
			}
		}

		void UIGroup::setVisible(bool visible) {
			for (UIComponent* c : m_components) {
				c->setVisible(visible);
			}
		}

		bool UIGroup::addComponent(UIComponent* c) {
			// Attempt to find the component already in the group
			std::vector<UIComponent*>::iterator it = std::find(
				m_components.begin(), m_components.end(), c);
			if (it == m_components.end()) {
				// The component is not already in this group, set its ID
				// and add it
				c->m_ID = m_CID++;
				c->m_parent = this;
				m_components.push_back(c);
				c->add();
				return true;
			}

			return false;
		}

		void UIGroup::addComponents(const std::vector<UIComponent*>& c) {
			for (unsigned int i = 0; i < c.size(); i++) {
				addComponent(c.at(i));
			}
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

		void UIGroup::setSelectedComponent(UIComponent* component) {
			if (component == nullptr) {
				if (m_selectedComponent != nullptr) {
					m_selectedComponent->setSelected(false);
				}
				m_selectedComponent = nullptr;
				return;
			}
			std::vector<UIComponent*>::iterator it
				= std::find(m_components.begin(), m_components.end(),
					component);
			if (it == m_components.end()) {
				return;
			}
			if (m_selectedComponent != nullptr) {
				m_selectedComponent->setSelected(false);
			}
			m_selectedComponent = component;
			component->setSelected(true);
		}

		int UIGroup::getGamepad() const {
			if (m_gamepad == ANY_GAMEPAD || m_gamepad == NO_GAMEPAD) {
				return m_gamepad;
			}

			if (MW::INPUT.isGamepadConnected(m_gamepad)) {
				return m_gamepad;
			}
			else {
				return ANY_GAMEPAD;
			}
		}

		void UIGroup::setGamepad(int gamepad) {
			m_gpUp = m_gpDown = m_gpLeft = m_gpRight = false;
			if (gamepad == ANY_GAMEPAD || gamepad == NO_GAMEPAD) {
				m_gamepad = gamepad;
			}

			if (MW::INPUT.isGamepadConnected(gamepad)) {
				m_gamepad = gamepad;
			}
		}

		void UIGroup::moveUp() {
			if (m_selectedComponent != nullptr) {
				UIComponent* up = m_selectedComponent->m_up;
				if (up != nullptr) {
					m_selectedComponent->setSelected(false);
					up->setSelected(true);
					m_selectedComponent = up;
				}
			}
		}

		void UIGroup::moveDown() {
			if (m_selectedComponent != nullptr) {
				UIComponent* down = m_selectedComponent->m_down;
				if (down != nullptr) {
					m_selectedComponent->setSelected(false);
					down->setSelected(true);
					m_selectedComponent = down;
				}
			}
		}

		void UIGroup::moveLeft() {
			if (m_selectedComponent != nullptr) {
				UIComponent* left = m_selectedComponent->m_left;
				if (left != nullptr) {
					m_selectedComponent->setSelected(false);
					left->setSelected(true);
					m_selectedComponent = left;
				}
			}
		}

		void UIGroup::moveRight() {
			if (m_selectedComponent != nullptr) {
				UIComponent* right = m_selectedComponent->m_right;
				if (right != nullptr) {
					m_selectedComponent->setSelected(false);
					right->setSelected(true);
					m_selectedComponent = right;
				}
			}
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

		void Button::setSelected(bool selected) {
			m_selected = selected;
			if (m_selected) {
				m_sprite.textureCoords = SELECTED_COORDS;
			}
			else {
				m_sprite.textureCoords = UNSELECTED_COORDS;
			}
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
					m_parent->getSpriteShader()->getCamera()))
					&& MW::WINDOW.isCursorEnabled()) {
				m_sprite.textureCoords = SELECTED_COORDS;
				if (!m_selected) {
					m_parent->componentEvent(m_ID, SELECTED_EVENT);
				}
				m_selected = true;
			}
			else {
				if (this != m_parent->getSelectedComponent()) {
					m_sprite.textureCoords = UNSELECTED_COORDS;
					if (m_selected) {
						m_parent->componentEvent(m_ID, UNSELECTED_EVENT);
					}
					m_selected = false;
				}
			}

			// If the button is selected test if it's been clicked
			if (m_selected) {
				// Test for keyboard clicking
				UIComponent* sc = m_parent->getSelectedComponent();
				if (MW::INPUT.isKeyReleased(F_ENTER)) {
					if (sc == this) {
						m_parent->componentEvent(m_ID, CLICKED_EVENT);
					}
				}
				if (MW::INPUT.isKeyDown(F_ENTER)) {
					if (sc == this) {
						m_sprite.textureCoords = CLICKED_COORDS;
					}
				}
				// Test for mouse clicking
				if (MW::INPUT.isButtonReleased(B_LEFT)
					&& MW::WINDOW.isCursorEnabled()) {
					if (RectContains(glm::vec4(m_position.x, m_position.y,
						m_dimensions.x, m_dimensions.y),
						MW::INPUT.getCursorPosition(
							m_parent->getSpriteShader()->getCamera()))) {
						m_parent->componentEvent(m_ID, CLICKED_EVENT);
					}
				}
				if (MW::INPUT.isButtonDown(B_LEFT)
					&& MW::WINDOW.isCursorEnabled()) {
					if (RectContains(glm::vec4(m_position.x, m_position.y,
						m_dimensions.x, m_dimensions.y),
						MW::INPUT.getCursorPosition(
							m_parent->getSpriteShader()->getCamera()))) {
						m_sprite.textureCoords = CLICKED_COORDS;
					}
				}
				// Test for gamepad clicking
				int gp = m_parent->getGamepad();
				if (gp == NO_GAMEPAD) {
					return;
				}
				else if (gp == ANY_GAMEPAD) {
					if (MW::INPUT.isGamepadButtonReleased(G_A)) {
						if (sc == this) {
							m_parent->componentEvent(m_ID, CLICKED_EVENT);
						}
					}
					if (MW::INPUT.isGamepadButtonDown(G_A)) {
						if (sc == this) {
							m_sprite.textureCoords = CLICKED_COORDS;
						}
					}
				}
				else {
					if (MW::INPUT.isGamepadButtonReleased(gp, G_A)) {
						if (sc == this) {
							m_parent->componentEvent(m_ID, CLICKED_EVENT);
						}
					}
					if (MW::INPUT.isGamepadButtonDown(gp, G_A)) {
						if (sc == this) {
							m_sprite.textureCoords = CLICKED_COORDS;
						}
					}
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
			const glm::vec2& normalDimensions, float cursorWidth,
			float normalMargin, float textScale, const glm::vec3& textColor,
			Justification textHJustification,
			Justification textVJustification, Texture* texture,
			Texture* cursorTexture, int maxCharacters) {
			// Set the text box up as a text label
			((TextLabel*)this)->init(text, normalPosition, normalDimensions,
				normalPosition, textScale, textColor, textHJustification,
				textVJustification);

			// Set up the label of this text box
			m_label.init(labelText, glm::vec3(normalPosition.x,
				normalPosition.y + normalDimensions.y, normalPosition.z),
				normalDimensions, glm::vec3(normalPosition.x,
					normalPosition.y + normalDimensions.y, normalPosition.z),
				textScale, textColor, textHJustification, textVJustification);
			m_label.setOwned(true);

			glm::vec2 winDims = MW::WINDOW.getDimensions();

			m_normalMargin = normalMargin;
			m_margin = m_normalMargin * MW::WINDOW.getDimensions().x;
			
			// Set up the cursor and background of this text box
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			m_sprite.textureCoords = UNSELECTED_COORDS;
			m_cursorWidth = cursorWidth;
			m_cursor.init(glm::vec3((normalPosition.x + m_normalMargin)
				* winDims.x, normalPosition.y * winDims.y, normalPosition.z),
				glm::vec2(cursorWidth, normalDimensions.y * winDims.y),
				cursorTexture);

			// Set the position and dimensions
			setPosition(m_position);
			setDimensions(m_dimensions);

			// Assign internal memory
			m_maxCharacters = maxCharacters;

			MW::INPUT.addInputListener(this);
		}

		void TextBox::setPosition(const glm::vec3& position) {
			// Update the position of the label
			m_label.setPosition(glm::vec3(position.x,
				position.y + m_dimensions.y, position.z));
			m_label.setTextPosition(glm::vec3(position.x + m_margin,
				position.y + m_dimensions.y, position.z + DEPTH_INCREMENT));
			// Update the position of the text
			m_position = glm::vec3(position.x, position.y,
				position.z + DEPTH_INCREMENT);
			m_textPosition = glm::vec3(position.x + m_margin, position.y,
				position.z + 2 * DEPTH_INCREMENT);
			// Update the position of the background sprite
			m_sprite.position = position;
			// The cursor has moved
			updateCursorPosition();
		}

		void TextBox::setDimensions(const glm::vec2& dimensions) {
			m_margin = m_normalMargin * MW::WINDOW.getDimensions().x;
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
			if (!m_selected || (int)m_text.length() >= m_maxCharacters) {
				return;
			}

			m_text = m_text.substr(0, m_cursorPosition)
				+ std::string(1, text)
				+ m_text.substr(m_cursorPosition, m_text.length());
			m_cursorPosition++;
			updateCursorPosition();
			// Update the position of the text if necessary
			if (m_cursor.position.x + m_cursorWidth
				> m_position.x + m_dimensions.x - m_margin) {
				m_textPosition.x -= m_dimensions.x / 4.0f;
				updateCursorPosition();
			}
		}

		void TextBox::add() {
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
				glm::vec2 mousePos = MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera());
				if (RectContains(glm::vec4(m_position.x, m_position.y,
					m_dimensions.x, m_dimensions.y), mousePos)
					&& MW::WINDOW.isCursorEnabled()) {
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

			if (MW::INPUT.isKeyPressed(F_ENTER)
				|| MW::INPUT.isGamepadButtonPressed(G_A)) {
				m_parent->componentEvent(m_ID, RETURN_EVENT);
			}

			if (MW::INPUT.isButtonDown(B_LEFT)) {
				glm::vec2 mousePos = MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera());
				if (RectContains(glm::vec4(m_sprite.position.x,
					m_sprite.position.y, m_sprite.dimensions.x,
					m_sprite.dimensions.y), mousePos)
					&& MW::WINDOW.isCursorEnabled()) {
					// Set the cursor position
					for (m_cursorPosition = 0;
						m_cursorPosition < m_text.length();
						m_cursorPosition++) {
						updateCursorPosition();
						if (mousePos.x < m_cursor.position.x) {
							if (m_cursorPosition > 0) {
								m_cursorPosition--;
							}
							break;
						}
					}
					updateCursorPosition();
				}
			}

			// Set the cursor position
			if (MW::INPUT.isKeyPressed(F_LEFT)
				|| (MW::INPUT.isKeyDown(F_LEFT) && m_timer > UI_UPDATE_TIME)
				|| MW::INPUT.isGamepadButtonPressed(
					m_parent->getGamepad(), G_LEFT)
				|| (MW::INPUT.isGamepadButtonDown(
					m_parent->getGamepad(), G_LEFT)
					&& m_timer > UI_UPDATE_TIME)) {
				moveCursorLeft();
			}
			else if (MW::INPUT.isKeyPressed(F_RIGHT)
				|| (MW::INPUT.isKeyDown(F_RIGHT) && m_timer > UI_UPDATE_TIME)
				|| MW::INPUT.isGamepadButtonPressed(
					m_parent->getGamepad(), G_RIGHT)
				|| (MW::INPUT.isGamepadButtonDown(
					m_parent->getGamepad(), G_RIGHT)
					&& m_timer > UI_UPDATE_TIME)) {
				moveCursorRight();
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
					if (m_cursor.position.x < m_position.x + m_margin) {
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
			m_cursorWidth = 0.0f;
			
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
			m_cursor.dimensions = glm::vec2(m_cursorWidth, m_dimensions.y);
		}

		void TextBox::moveCursorLeft() {
			if (m_cursorPosition > 0) {
				m_timer = 0.0f;
				m_cursorPosition--;
				updateCursorPosition();
				// Update the position of the text if necessary
				if (m_cursor.position.x < m_position.x + m_margin) {
					m_textPosition.x += m_dimensions.x / 4.0f;
					if (m_textPosition.x > m_position.x) {
						m_textPosition.x = m_position.x;
					}
					updateCursorPosition();
				}
			}
		}

		void TextBox::moveCursorRight() {
			if (m_cursorPosition < m_text.length()) {
				m_timer = 0.0f;
				m_cursorPosition++;
				updateCursorPosition();
				// Update the position of the text if necessary
				if (m_cursor.position.x + m_cursorWidth
					> m_position.x + m_dimensions.x - m_margin) {
					m_textPosition.x -= m_dimensions.x / 4.0f;
					updateCursorPosition();
				}
			}
		}

		glm::vec4 Switch::ON_COORDS = glm::vec4(0.0f, 0.0f, 1.0f / 4.0f, 1.0f);
		glm::vec4 Switch::OFF_COORDS = glm::vec4(1.0f / 4.0f, 0.0f,
			1.0f / 4.0f, 1.0f);
		glm::vec4 Switch::ON_SELECTED_COORDS = glm::vec4(2.0f / 4.0f, 0.0f,
			1.0f / 4.0f, 1.0f);
		glm::vec4 Switch::OFF_SELECTED_COORDS = glm::vec4(3.0f / 4.0f, 0.0f,
			1.0f / 4.0f, 1.0f);

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

		void Switch::setSelected(bool selected) {
			m_selected = selected;
			if (m_selected) {
				if (m_on) {
					m_sprite.textureCoords = ON_SELECTED_COORDS;
				}
				else {
					m_sprite.textureCoords = ON_COORDS;
				}
			}
			else {
				if (m_on) {
					m_sprite.textureCoords = OFF_SELECTED_COORDS;
				}
				else {
					m_sprite.textureCoords = OFF_COORDS;
				}
			}
		}

		void Switch::setOn(bool on) {
			m_on = on;
			if (on) {
				if (m_selected) {
					m_sprite.textureCoords = ON_SELECTED_COORDS;
				}
				else {
					m_sprite.textureCoords = ON_COORDS;
				}
			}
			else {
				if (m_selected) {
					m_sprite.textureCoords = OFF_SELECTED_COORDS;
				}
				else {
					m_sprite.textureCoords = OFF_COORDS;
				}
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

			// Test if this switch is selected by the mouse
			if (RectContains(glm::vec4(m_sprite.position.x, m_sprite.position.y,
				m_sprite.dimensions.x, m_sprite.dimensions.y),
				MW::INPUT.getCursorPosition(m_parent->getSpriteShader()
					->getCamera())) && MW::WINDOW.isCursorEnabled()) {
				if (!m_selected) {
					setSelected(true);
					m_parent->componentEvent(m_ID, SELECTED_EVENT);
				}
			}
			else {
				if (this != m_parent->getSelectedComponent()) {
					if (m_selected) {
						setSelected(false);
						m_parent->componentEvent(m_ID, UNSELECTED_EVENT);
					}
				}
			}

			if (!m_selected) {
				return;
			}

			if (RectContains(glm::vec4(m_position.x, m_position.y,
				m_dimensions.x, m_dimensions.y),
				MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera()))) {
				if (MW::INPUT.isButtonPressed(B_LEFT)
					&& MW::WINDOW.isCursorEnabled()) {
					if (m_on) {
						setOn(false);
						m_parent->componentEvent(m_ID, OFF_EVENT);
					}
					else {
						setOn(true);
						m_parent->componentEvent(m_ID, ON_EVENT);
					}
				}
			}
			else {
				if (this == m_parent->getSelectedComponent()) {
					if (MW::INPUT.isGamepadButtonPressed(m_parent->getGamepad(),
						G_A)) {
						if (m_on) {
							setOn(false);
							m_parent->componentEvent(m_ID, OFF_EVENT);
						}
						else {
							setOn(true);
							m_parent->componentEvent(m_ID, ON_EVENT);
						}
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
			float cursorWidth, float normalMargin, float textScale,
			const glm::vec3& textColor, Justification textHJustification,
			Justification textVJustification, Texture* texture,
			Texture* cursorTexture, int min, int value, int max) {
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
			m_cursorWidth = cursorWidth;
			m_cursor.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				glm::vec2(cursorWidth, normalDimensions.y * winDims.y),
				cursorTexture);
			m_normalMargin = normalMargin;
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
			m_margin = MW::WINDOW.getDimensions().x * m_normalMargin;
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
			if (MW::INPUT.isButtonPressed(B_LEFT)
				&& MW::WINDOW.isCursorEnabled()) {
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
			else if (MW::INPUT.isButtonReleased(B_LEFT)
				&& MW::WINDOW.isCursorEnabled()) {
				if (m_selected) {
					m_selected = false;
					m_sprite.textureCoords = UNSELECTED_COORDS;
					m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
				}
			}

			if (m_selected && MW::INPUT.isButtonDown(B_LEFT)
				&& MW::WINDOW.isCursorEnabled()) {
				glm::vec2 mousePos = MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera());
				if (mousePos.x < m_sprite.position.x + m_margin) {
					m_value = m_min;
				}
				else if (mousePos.x
				> m_sprite.position.x + m_sprite.dimensions.x - m_margin) {
					m_value = m_max;
				}
				else {
					m_value = m_min
						+ (int)(((mousePos.x - (m_sprite.position.x + m_margin))
							/ (m_sprite.dimensions.x - 2 * m_margin))\
							* (m_max - m_min));
				}
				updateCursorPosition();
			}

			if (!m_selected) {
				return;
			}

			// Handle gamepad input
			if (MW::INPUT.isGamepadButtonPressed(m_parent->getGamepad(),
				G_LEFT) || (MW::INPUT.isGamepadButtonDown(
					m_parent->getGamepad(), G_LEFT)
					&& m_timer > UI_UPDATE_TIME)) {
				m_timer = 0.0f;
				if (m_value > m_min) {
					m_value--;
					updateCursorPosition();
				}
			}
			else if (MW::INPUT.isGamepadButtonPressed(m_parent->getGamepad(),
				G_RIGHT) || (MW::INPUT.isGamepadButtonDown(
					m_parent->getGamepad(), G_RIGHT)
					&& m_timer > UI_UPDATE_TIME)) {
				m_timer = 0.0f;
				if (m_value < m_max) {
					m_value++;
					updateCursorPosition();
				}
			}
		}

		void Slider::update(float deltaTime) {
			if (m_timer > UI_UPDATE_TIME) {
				m_timer = 0.0f;
			}
			m_timer += deltaTime;
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
			m_cursor.position.x = m_sprite.position.x + m_margin
				+ ((float)(m_value - m_min) / (float)(m_max - m_min))
				* (m_sprite.dimensions.x - 2 * m_margin - m_cursorWidth);
			m_cursor.position.y = m_sprite.position.y;
			m_cursor.dimensions = glm::vec2(m_cursorWidth,
				m_sprite.dimensions.y);
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

		void Cycle::setSelected(bool selected) {
			m_selected = selected;
			m_leftArrowSelected = selected;
			m_rightArrowSelected = selected;
			if (m_selected) {
				m_leftArrow.textureCoords = SELECTED_LEFT_COORDS;
				m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS;
			}
			else {
				m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
				m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;
			}
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
				m_leftArrow.dimensions.y), mousePos)
				&& MW::WINDOW.isCursorEnabled()) {
				if (!m_leftArrowSelected) {
					m_leftArrowSelected = true;
					m_leftArrow.textureCoords = SELECTED_LEFT_COORDS;
				}
			}
			else {
				if (this != m_parent->getSelectedComponent()) {
					if (m_leftArrowSelected) {
						m_leftArrowSelected = false;
						m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
					}
				}
			}

			// Test if the right arrow is selected
			if (RectContains(glm::vec4(m_rightArrow.position.x,
				m_rightArrow.position.y, m_rightArrow.dimensions.x,
				m_rightArrow.dimensions.y), mousePos)
				&& MW::WINDOW.isCursorEnabled()) {
				if (!m_rightArrowSelected) {
					m_rightArrowSelected = true;
					m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS;
				}
			}
			else {
				if (this != m_parent->getSelectedComponent()) {
					if (m_rightArrowSelected) {
						m_rightArrowSelected = false;
						m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;
					}
				}
			}

			// Test if either button is clicked
			if (MW::INPUT.isButtonDown(B_LEFT)
				&& MW::WINDOW.isCursorEnabled()) {
				if (RectContains(glm::vec4(m_leftArrow.position.x,
					m_leftArrow.position.y, m_leftArrow.dimensions.x,
					m_leftArrow.dimensions.y), mousePos)) {
					m_leftArrow.textureCoords = CLICKED_LEFT_COORDS;
				}
				else if (RectContains(glm::vec4(m_rightArrow.position.x,
					m_rightArrow.position.y, m_rightArrow.dimensions.x,
					m_rightArrow.dimensions.y), mousePos)) {
					m_rightArrow.textureCoords = CLICKED_RIGHT_COORDS;
				}
			}
			if (MW::INPUT.isButtonReleased(B_LEFT)
				&& MW::WINDOW.isCursorEnabled()) {
				bool updated = false;
				if (RectContains(glm::vec4(m_leftArrow.position.x,
					m_leftArrow.position.y, m_leftArrow.dimensions.x,
					m_leftArrow.dimensions.y), mousePos)) {
					if (m_selection > 0) {
						m_selection--;
					}
					else {
						m_selection = m_options.size() - 1;
					}
					updated = true;
				}
				else if (RectContains(glm::vec4(m_rightArrow.position.x,
					m_rightArrow.position.y, m_rightArrow.dimensions.x,
					m_rightArrow.dimensions.y), mousePos)) {
					if (m_selection < m_options.size() - 1) {
						m_selection++;
					}
					else {
						m_selection = 0;
					}
					updated = true;
				}

				if (m_leftArrowSelected) {
					m_leftArrow.textureCoords = SELECTED_LEFT_COORDS;
				}
				else {
					m_leftArrow.textureCoords = UNSELECTED_LEFT_COORDS;
				}
				if (m_rightArrowSelected) {
					m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS;
				}
				else {
					m_rightArrow.textureCoords = UNSELECTED_RIGHT_COORDS;
				}
				
				if (updated) {
					m_text = m_options[m_selection];
					m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
				}
			}

			if (!m_selected) {
				return;
			}
			if (MW::INPUT.isGamepadButtonDown(m_parent->getGamepad(), G_LEFT)) {
				m_leftArrow.textureCoords = CLICKED_LEFT_COORDS;
			}
			else if (MW::INPUT.isGamepadButtonDown(m_parent->getGamepad(),
				G_RIGHT)) {
				m_rightArrow.textureCoords = CLICKED_RIGHT_COORDS;
			}
			bool updated = false;
			if (MW::INPUT.isGamepadButtonReleased(m_parent->getGamepad(),
				G_LEFT)) {
				m_leftArrow.textureCoords = SELECTED_LEFT_COORDS;
				if (m_selection > 0) {
					m_selection--;
				}
				else {
					m_selection = m_options.size() - 1;
				}
				updated = true;
				m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
			}
			else if (MW::INPUT.isGamepadButtonReleased(m_parent->getGamepad(),
				G_RIGHT)) {
				m_rightArrow.textureCoords = SELECTED_RIGHT_COORDS;
				if (m_selection < m_options.size() - 1) {
					m_selection++;
				}
				else {
					m_selection = 0;
				}
				updated = true;
				m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
			}
			if (updated) {
				m_text = m_options[m_selection];
				m_parent->componentEvent(m_ID, VALUE_UPDATE_EVENT);
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

		glm::vec4 TextArea::UNSELECTED_COORDS = glm::vec4(0.0f, 0.0f,
			1.0f / 2.0f, 1.0f);
		glm::vec4 TextArea::SELECTED_COORDS = glm::vec4(1.0f / 2.0f, 0.0f,
			1.0f / 2.0f, 1.0f);

		void TextArea::init(const std::string& text, unsigned int lineCount,
			const glm::vec3& normalPosition, const glm::vec2& normalDimensions,
			float cursorWidth, float textScale, const glm::vec3& textColor,
			Justification hJustification, Justification vJustification,
			Texture* texture, Texture* cursorTexture) {
			// Initialize the text labels to use to display lines
			m_labels.resize(lineCount);
			float dy = normalDimensions.y / (float)lineCount;
			for (unsigned int i = 0; i < lineCount; i++) {
				m_labels[i].init("", glm::vec3(normalPosition.x,
					normalPosition.y + dy * (float)(lineCount - 1 - i),
					normalPosition.z), glm::vec2(normalDimensions.x, dy),
					glm::vec3(normalPosition.x,
						normalPosition.y + dy * (float)(lineCount - 1 - i),
						normalPosition.z), textScale, textColor, hJustification,
					vJustification);
			}
			m_text = text;

			// Initialize the background and cursor sprites based on the window
			// size and the normalized position and dimensions
			glm::vec2 winDims = MW::WINDOW.getDimensions();
			m_sprite.init(glm::vec3(normalPosition.x * winDims.x,
				normalPosition.y * winDims.y, normalPosition.z),
				normalDimensions * winDims, texture);
			setTextPosition(normalPosition.x * winDims.x);
			m_sprite.textureCoords = UNSELECTED_COORDS;
			
			m_cursorWidth = cursorWidth;
			m_cursor.init(m_labels[0].getPosition(), glm::vec2(cursorWidth,
				m_labels[0].getDimensions().y), cursorTexture);

			MW::INPUT.addInputListener(this);
		}

		void TextArea::setText(const std::string& text) {
			m_text = text;
			populateLabels();
		}

		const glm::vec3& TextArea::getPosition() const {
			return m_labels[m_labels.size() - 1].getPosition();
		}

		void TextArea::setPosition(const glm::vec3& position) {
			if (m_labels.empty()) {
				return;
			}
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setPosition(glm::vec3(position.x,
					position.y + m_labels[i].getDimensions().y
					* (m_labels.size() - 1 - i), position.z));
				m_labels[i].setTextPosition(m_labels[i].getPosition());
			}
			m_sprite.position = position;
			m_textPosition = position.x;
			updateCursorPosition();
		}

		const glm::vec2& TextArea::getDimensions() const {
			return m_dimensions;
		}

		void TextArea::setDimensions(const glm::vec2& dimensions) {
			if (m_labels.empty()) {
				return;
			}
			m_dimensions = dimensions;
			float dy = dimensions.y / (float)m_labels.size();
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setDimensions(glm::vec2(dimensions.x, dy));
			}
			m_sprite.dimensions = dimensions;
			updateCursorPosition();
		}

		float TextArea::getTextScale() const {
			return m_labels[0].getTextScale();
		}

		void TextArea::setTextScale(float textScale) {
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setTextScale(textScale);
			}
		}

		void TextArea::setEnabled(bool enabled) {
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setEnabled(enabled);
			}
			m_enabled = enabled;
		}

		void TextArea::setVisible(bool visible) {
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setVisible(visible);
			}
			m_visible = visible;
		}

		void TextArea::setLineWrapEnabled(bool lineWrapEnabled) {
			m_lineWrapEnabled = lineWrapEnabled;
			populateLabels();
		}

		void TextArea::setSelected(bool selected) {
			m_selected = selected;
			if (m_selected) {
				m_sprite.textureCoords = SELECTED_COORDS;
			}
			else {
				m_sprite.textureCoords = UNSELECTED_COORDS;
			}
		}

		void TextArea::setTextPosition(float textPosition) {
			m_textPosition = textPosition;
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				glm::vec3 temp = m_labels[i].getTextPosition();
				temp.x = textPosition;
				m_labels[i].setTextPosition(temp);
			}
		}

		void TextArea::add() {
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].setOwned(true);
				m_parent->addComponent(&m_labels[i]);
			}
			populateLabels();
		}

		void TextArea::draw() {
			MW::RENDERER.submit({ &m_sprite }, m_parent->getSpriteShader());
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].draw();
			}
			if (m_enabled && m_selected && m_editable
				&& RectContains(glm::vec4(m_sprite.position.x,
					m_sprite.position.y, m_sprite.dimensions.x,
					m_sprite.dimensions.y), m_cursor.position)
				&& RectContains(glm::vec4(m_sprite.position.x,
					m_sprite.position.y, m_sprite.dimensions.x,
					m_sprite.dimensions.y), glm::vec2(m_cursor.position.x,
						m_cursor.position.y + m_cursor.dimensions.y))) {
				MW::RENDERER.submit({ &m_cursor }, m_parent->getSpriteShader());
			}
		}

		void TextArea::processInput() {
			if (!m_enabled) {
				return;
			}

			// Determine if this text area should be selected
			if (MW::INPUT.isButtonPressed(B_LEFT)) {
				glm::vec2 mousePos = MW::INPUT.getCursorPosition(
					m_parent->getSpriteShader()->getCamera());
				if (RectContains(glm::vec4(m_sprite.position.x,
					m_sprite.position.y, m_sprite.dimensions.x,
					m_sprite.dimensions.y), mousePos)
					&& MW::WINDOW.isCursorEnabled()) {
					if (!m_selected) {
						m_selected = true;
						m_sprite.textureCoords = SELECTED_COORDS;
						m_parent->componentEvent(m_ID, SELECTED_EVENT);
					}
				}
				else {
					if (m_selected) {
						m_selected = false;
						m_sprite.textureCoords = UNSELECTED_COORDS;
						m_parent->componentEvent(m_ID, UNSELECTED_EVENT);
					}
				}
			}

			if (m_selected && m_editable) {
				if (MW::INPUT.isKeyPressed(F_LEFT)) {
					if (m_cursorPosition > 0) {
						m_cursorPosition--;
						updateCursorPosition();
						while (m_cursor.position.x < m_sprite.position.x) {
							setTextPosition(getTextPosition()
								+ m_sprite.dimensions.x / 4.0f);
							updateCursorPosition();
						}
						while (m_cursor.position.x + m_cursorWidth
							> m_sprite.position.x + m_sprite.dimensions.x) {
							setTextPosition(getTextPosition()
								- m_sprite.dimensions.x / 4.0f);
							updateCursorPosition();
						}
						if ((int)m_cursorLine - 1 < m_scroll && m_scroll > 0
							&& m_scrollEnabled) {
							m_scroll = m_cursorLine - 1;
							populateLabels();
							updateCursorPosition();
						}
					}
				}
				else if (MW::INPUT.isKeyPressed(F_RIGHT)) {
					if (m_cursorPosition < m_text.length()) {
						m_cursorPosition++;
						updateCursorPosition();
						while (m_cursor.position.x < m_sprite.position.x) {
							setTextPosition(m_sprite.position.x);
							updateCursorPosition();
						}
						while (m_cursor.position.x + m_cursorWidth
							> m_sprite.position.x + m_sprite.dimensions.x) {
							setTextPosition(getTextPosition()
								- m_sprite.dimensions.x / 4.0f);
							updateCursorPosition();
						}
						while (m_cursorLine >= m_scroll + m_labels.size()
							&& m_scrollEnabled) {
							m_scroll++;
							populateLabels();
							updateCursorPosition();
						}
					}
				}

				if (MW::INPUT.isKeyPressed(F_UP)) {
					if (m_cursorLine > 0) {
						m_cursorPosition = 0;
						bool found = false;
						for (int l = 0; l < (int)m_cursorLine; l++) {
							found = false;
							for (int c = 0; c < (int)m_lines[l].length(); c++) {
								if (l == m_cursorLine - 1) {
									if (m_cursor.position.x < m_textPosition
										+ getStringWidth(m_lines[l].substr(0,
											c))) {
										found = true;
										break;
									}
								}

								m_cursorPosition++;
							}
							if (found) {
								break;
							}
						}
						updateCursorPosition();
					}
				}
				else if (MW::INPUT.isKeyPressed(F_DOWN)) {
					if (!m_lines.empty() && m_cursorLine < m_lines.size() - 1) {
						m_cursorPosition = 0;
						bool found = false;
						for (int l = 0; l < (int)m_cursorLine + 2; l++) {
							found = false;
							for (int c = 0; c < (int)m_lines[l].length(); c++) {
								if (l == m_cursorLine + 1) {
									if (m_cursor.position.x < m_textPosition
										+ getStringWidth(m_lines[l].substr(0,
											c))) {
										found = true;
										break;
									}
								}

								m_cursorPosition++;
							}
							if (found) {
								break;
							}
						}
						updateCursorPosition();
					}
				}

				if (MW::INPUT.isButtonPressed(B_LEFT)
					&& MW::WINDOW.isCursorEnabled()) {
					glm::vec2 mousePos = MW::INPUT.getCursorPosition(
						m_parent->getSpriteShader()->getCamera());
					unsigned int line = 0;
					for (unsigned int i = 0; i < m_labels.size(); i++) {
						if (RectContains(glm::vec4(m_labels[i].getPosition().x,
							m_labels[i].getPosition().y,
							m_labels[i].getDimensions().x,
							m_labels[i].getDimensions().y), mousePos)) {
							line = i + m_scroll;
							break;
						}
					}

					if (line >= m_lines.size()) {
						m_cursorPosition = m_text.length();
						updateCursorPosition();
					}
					else {
						m_cursorPosition = 0;
						bool found = false;
						for (int l = 0; l <= (int)line; l++) {
							found = false;
							for (int c = 0; c < (int)m_lines[l].length(); c++) {
								m_cursorPosition++;
								if (l == line && mousePos.x < m_textPosition
									+ getStringWidth(m_lines[l].substr(0, c))) {
									found = true;
									m_cursorPosition--;
									break;
								}
							}
							if (found) {
								break;
							}
						}
						updateCursorPosition();
					}
				}

				if (MW::INPUT.isKeyPressed(F_ENTER)) {
					textTyped('\n');
				}
				
				if (MW::INPUT.isKeyPressed(F_BACKSPACE)) {
					if (!m_text.empty() && m_cursorPosition > 0) {
						if (m_cursorPosition >= m_text.length()) {
							m_text = m_text.substr(0, m_cursorPosition - 1);
						}
						else {
							m_text = m_text.substr(0, m_cursorPosition - 1)
								+ m_text.substr(m_cursorPosition);
						}
						populateLabels();
						m_cursorPosition--;
					}
					updateCursorPosition();
					while (m_cursor.position.x < m_sprite.position.x) {
						setTextPosition(getTextPosition()
							+ m_sprite.dimensions.x / 4.0f);
						updateCursorPosition();
					}
					while (m_cursor.position.x + m_cursorWidth
						> m_sprite.position.x + m_sprite.dimensions.x) {
						setTextPosition(getTextPosition()
							- m_sprite.dimensions.x / 4.0f);
						updateCursorPosition();
					}
					if ((int)m_cursorLine - 1 < m_scroll && m_scroll > 0
						&& m_scrollEnabled) {
						m_scroll = m_cursorLine - 1;
						populateLabels();
						updateCursorPosition();
					}
				}
				else if (MW::INPUT.isKeyPressed(F_DELETE)) {
					if (!m_text.empty()) {
						if (m_cursorPosition == m_text.length() - 1) {
							m_text = m_text.substr(0, m_cursorPosition);
						}
						else if (m_cursorPosition < m_text.length() - 1) {
							m_text = m_text.substr(0, m_cursorPosition)
								+ m_text.substr(m_cursorPosition + 1);
						}
						populateLabels();
					}
					updateCursorPosition();
				}
			}
		}

		void TextArea::textTyped(char text) {
			if (m_enabled && m_selected && m_editable) {
				if (m_cursorPosition >= m_text.length()) {
					m_text += text;
					populateLabels();
					m_cursorPosition = m_text.length();
				}
				else {
					m_text = m_text.substr(0, m_cursorPosition)
						+ std::string(1, text)
						+ m_text.substr(m_cursorPosition);
					populateLabels();
					m_cursorPosition++;
				}

				updateCursorPosition();
				if (text == '\n' && !m_lineWrapEnabled) {
					setTextPosition(m_sprite.position.x);
					updateCursorPosition();
					while (m_cursorLine >= m_scroll + m_labels.size()) {
						m_scroll++;
						populateLabels();
						updateCursorPosition();
					}
				}
				else {
					while (m_cursor.position.x < m_sprite.position.x) {
						setTextPosition(m_sprite.position.x);
						updateCursorPosition();
					}
					while (m_cursor.position.x + m_cursorWidth
						> m_sprite.position.x + m_sprite.dimensions.x) {
						setTextPosition(getTextPosition()
							- m_sprite.dimensions.x / 4.0f);
						updateCursorPosition();
					}
					while (m_cursorLine >= m_scroll + m_labels.size()
						&& m_scrollEnabled) {
						m_scroll++;
						populateLabels();
						updateCursorPosition();
					}
				}
			}
		}

		void TextArea::scrolled(const glm::vec2& distance) {
			if (m_enabled && m_selected && m_scrollEnabled) {
				m_scroll -= (int)distance.y;
				int back = (int)m_lines.size() - (int)m_labels.size();
				if (m_scroll < 0) {
					m_scroll = 0;
				}
				else if (m_scroll >= back) {
					if (back < 0) {
						m_scroll = 0;
					}
					else {
						m_scroll = back;
					}
				}
				populateLabels();
				updateCursorPosition();
			}
		}

		void TextArea::destroy() {
			m_text = "";
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				m_labels[i].destroy();
			}
			m_labels.clear();
			m_lines.clear();
			m_scrollEnabled = false;
			m_scroll = 0;
			m_lineWrapEnabled = false;
			m_sprite.destroy();
			m_cursor.destroy();
			m_cursorPosition = 0;
			m_cursorWidth = 1.0f;
			m_editable = false;
			MW::INPUT.removeInputListener(this);
		}

		void TextArea::populateLabels() {
			// Break the text up into lines
			m_lines.clear();
			std::string tempLine = "";
			unsigned int spaceIndex = 0;
			unsigned int tSpaceIndex = 0;
			unsigned int lineIndex = 0;
			for (unsigned int i = 0; i < m_text.length(); i++) {
				if (m_text[i] == '\n') {
					m_lines.push_back(tempLine);
					tempLine.clear();
					spaceIndex = 0;
					lineIndex = i;
				}
				else {
					if (m_text[i] == ' ') {
						spaceIndex = i - lineIndex;
						tSpaceIndex = i;
					}
				}
				tempLine += m_text[i];
				if (getStringWidth(tempLine)
					> m_sprite.dimensions.x
					&& m_lineWrapEnabled && spaceIndex > 0) {
					//&& spaceIndex < m_text.length() - 1) {
					lineIndex = tSpaceIndex;
					tempLine = tempLine.substr(0, spaceIndex);
					i = tSpaceIndex - 1;
					m_lines.push_back(tempLine);
					tempLine.clear();
					spaceIndex = 0;
				}
			}
			if (!tempLine.empty()) {
				m_lines.push_back(tempLine);
			}

			// Push the lines of text into the text labels
			for (unsigned int i = 0; i < m_labels.size(); i++) {
				unsigned int lineIndex = i + m_scroll;
				if (lineIndex >= m_lines.size()) {
					m_labels[i].setText("");
				}
				else {
					m_labels[i].setText(m_lines[lineIndex]);
				}
			}
		}

		float TextArea::getStringWidth(const std::string& str) {
			float w = 0.0f;
			for (char c : str) {
				if (c == '\n') {
					w = 0.0f;
					continue;
				}
				w += (float)m_parent->getFont()->characters[c].offset
					* m_labels[0].getTextScale();
			}
			return w;
		}

		void TextArea::pushCursor(unsigned int& count, bool& found, int l,
			int c) {
			if (count == m_cursorPosition) {
				found = true;
				return;
			}

			int index = l - m_scroll;
			if (index >= 0 && index < (int)m_labels.size()) {
				m_cursor.position.x = m_textPosition
					+ getStringWidth(m_lines[l].substr(0, c + 1));
				m_cursor.position.y = m_labels[index].getPosition().y;
			}
			else {
				m_cursor.position.x = m_textPosition;
				m_cursor.position.y = m_sprite.position.y
					+ m_sprite.dimensions.y;
			}

			count++;
		}

		void TextArea::updateCursorPosition() {
			if (m_labels.empty()) {
				return;
			}
			// Make sure that the text area isn't empty
			if (m_text.empty()) {
				m_cursor.position = glm::vec3(m_textPosition,
					m_labels[0].getPosition().y, m_labels[0].getPosition().z);
				m_cursor.dimensions = glm::vec2(1.0f,
					m_labels[0].getDimensions().y);
				return;
			}

			m_cursor.position.x = m_textPosition;
			m_cursor.position.y = m_sprite.position.y + m_sprite.dimensions.y;
			m_cursor.dimensions = glm::vec2(m_cursorWidth,
				m_sprite.dimensions.y / (float)m_labels.size());

			// The number of characters traversed
			unsigned int count = 0;
			// Whether the cursor position has been found
			bool found = false;
			for (int l = 0; l < (int)m_lines.size(); l++) {
				// Iterate through line l
				found = false;
				for (int c = 0; c < (int)m_lines[l].length(); c++) {
					// Push the cursor through the text one character
					pushCursor(count, found, l, c);
					if (found) {
						break;
					}
					m_cursorLine = l;
				}
				// Stop if the cursor was found
				if (found) {
					break;
				}
			}
			if (!found) {
				pushCursor(count, found, m_lines.size() - 1,
					m_lines[m_lines.size() - 1].length());
			}
		}
	}
}