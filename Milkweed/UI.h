/*
* File: UI.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.27
*/

#ifndef MW_UI_H
#define MW_UI_H

#include "Sprite.h"
#include "Input.h"

#define DEPTH_INCREMENT 0.01f

namespace Milkweed {
	// Forward declare the Scene class from MW.h
	class Scene;

	namespace UI {
		// Forward declare the UIGroup class
		class UIGroup;

		/*
		* An abstract UI component, the parent of TextBox, Label, Button, etc.
		*/
		class UIComponent {
		public:
			/*
			* Test whether this component is enabled
			*/
			bool isEnabled() const { return m_enabled; }
			/*
			* Set whether this component is enabled
			*/
			virtual void setEnabled(bool enabled) { m_enabled = enabled; }
			/*
			* Get the ID number of this component
			*/
			unsigned int getID() const { return m_ID; }

		protected:
			// Allow groups to call the draw, processInput, update, and destroy
			// functions on components
			friend UIGroup;
			// The group containing this component
			UIGroup* m_parent = nullptr;
			// Whether this component is currently enabled
			bool m_enabled = true;
			// The ID number of this component within its group
			unsigned int m_ID = 0;

			/*
			* Test whether a 2D point falls in a rectangle
			*
			* @param rect: The rectangle's 2D position and dimensions (x, y, w, h)
			* @param p: The point to test
			* @return Whether the point p falls inside the given rectangle
			*/
			static bool rectContains(const glm::vec4& rect, const glm::vec2& p);
			/*
			* Draw this component's graphics in the screen
			*/
			virtual void draw() = 0;
			/*
			* Process input to this component
			*/
			virtual void processInput() = 0;
			/*
			* Update physics in this component
			*/
			virtual void update(float deltaTime) = 0;
			/*
			* Free this component's memory
			*/
			virtual void destroy() = 0;
		};

		/*
		* A set of UIComponents which can be drawn and process input as a group
		*/
		class UIGroup {
		public:
			/*
			* Initialize this group with a parent and an ID
			*
			* @param parent: The scene containing this UI group
			* @param ID: The ID number of this UI group in its scene
			* @param font: The font to draw the text in the components of this UI
			* group
			* @param spriteShader: The shader to draw the components of this UI
			* group with
			* @param textShader: The shader to draw text for all components of this
			* UI group with (this shader must have a uniform vec3 textColor)
			* @param textColorUniform: The uniform name of the text color in the
			* shader
			*/
			void init(Scene* parent, unsigned int ID, Font* font,
				Shader* spriteShader, Shader* textShader,
				const std::string& textColorUniform);
			/*
			* Draw all components in this group
			*/
			void draw();
			/*
			* Process input to all components in this group which are enabled
			*/
			void processInput();
			/*
			* Update physics in all components in this group
			*/
			void update(float deltaTime);
			/*
			* Free the memory of this group and all its components
			*/
			void destroy();
			/*
			* Enable all components in this group if not already enabled
			*/
			void enable();
			/*
			* Disable all components in this group if not already disabled
			*/
			void disable();
			/*
			* Add a component to this group if it is not already in the group
			*
			* @return Whether this component could be added
			*/
			bool addComponent(UIComponent* c);
			/*
			* An event has occurred in one of the components of this UI group and
			* the parent scene must be notified
			*
			* @param componentID: The ID number of the relevant component
			* @param eventID: The ID number of the event
			*/
			void componentEvent(unsigned int componentID, unsigned int eventID);
			/*
			* Remove a component from this group if it is in the group
			*
			* @return Whether this component was found and removed
			*/
			bool removeComponent(UIComponent* c);
			/*
			* Get the ID number of this group within its parent scene
			*/
			unsigned int getID() const { return m_ID; }
			/*
			* Get the font used to draw the components of this UI group
			*/
			Font* getFont() { return m_font; }
			/*
			* Set the font used to draw the components of this UI group
			*/
			void setFont(Font* font) { m_font = font; }
			/*
			* Get the shader used to draw the components of this UI group
			*/
			Shader* getSpriteShader() { return m_spriteShader; }
			/*
			* Set the shader used to draw the components of this UI group
			*/
			void setSpriteShader(Shader* spriteShader) {
				m_spriteShader = spriteShader;
			}
			/*
			* Get the shader used to draw text in this UI group
			*/
			Shader* getTextShader() { return m_textShader; }
			/*
			* Set the shader used to draw text in this UI group
			*/
			void setTextShader(Shader* textShader) {
				m_textShader = textShader;
			}
			/*
			* Get the uniform name of the text color in the shader
			*/
			const std::string& getTextColorUniform() const {
				return m_textColorUniform;
			}
			/*
			* Set the uniform name of the text color in the shader
			*/
			void setTextColorUniform(const std::string& textColorUniform) {
				m_textColorUniform = textColorUniform;
			}

		private:
			// The parent scene of this group
			Scene* m_parent = nullptr;
			// The ID number of this group in its scene
			unsigned int m_ID = 0;
			// The font to draw components of this UI group in
			Font* m_font = nullptr;
			// The shader to draw components of this UI group in
			Shader* m_spriteShader = nullptr;
			// The shader to draw text for components of this UI group in
			Shader* m_textShader = nullptr;
			// The uniform name of the text color in the shader
			std::string m_textColorUniform = "";
			// The components in this group mapped to their ID numbers
			std::vector<UIComponent*> m_components;
			// The working component ID for adding new components to this group
			unsigned int m_CID = 0;
		};

		/*
		* A text label for drawing text onto the screen
		*/
		class TextLabel : public UIComponent {
		public:
			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param text: The string of text this label will render
			* @param position: The position of this label
			* @param dimensions: The width and height of the rectangle to render
			* this label's text in
			* @param scale: The scale to draw this label's text at
			* @param color: The color to draw this label's text in
			* @param hJustification: The justification to draw this label's text with
			* on the x-axis
			* @param vJustification: The justification to draw this label's text with
			* on the y-axis
			*/
			virtual void init(const std::string& text, const glm::vec3& position,
				const glm::vec2& dimensions, const glm::vec3& textPosition,
				float textScale, const glm::vec3& textColor,
				Justification hJustification, Justification vJustification);
			/*
			* Draw this label's text to the screen
			*/
			virtual void draw() override;
			/*
			* Process input to this label (does nothing)
			*/
			virtual void processInput() override {}
			/*
			* Update physics in this label (does nothing by default)
			*/
			virtual void update(float deltaTime) override {}
			/*
			* Free this label's memory
			*/
			virtual void destroy() override;
			/*
			* Get the text this label displays
			*/
			const std::string& getText() const { return m_text; }
			/*
			* Set the text this label displays
			*/
			void setText(const std::string& text) { m_text = text; }
			/*
			* Get the position of this label on the screen
			*/
			const glm::vec3& getPosition() const { return m_position; }
			/*
			* Set the position of this label on the screen
			*/
			virtual void setPosition(const glm::vec3& position) {
				m_position = position;
			}
			/*
			* Get the dimensions of the rectangle this label's text appears in on
			* the screen
			*/
			const glm::vec2& getDimensions() const { return m_dimensions; }
			/*
			* Set the dimensions of the rectangle this label's text appears in on
			* the screen
			*/
			virtual void setDimensions(const glm::vec2& dimensions) {
				m_dimensions = dimensions;
			}
			/*
			* Get the position to start this label's text at
			*/
			const glm::vec3& getTextPosition() const { return m_textPosition; }
			/*
			* Set the position to start this label's text at
			*/
			const glm::vec3& setTextPosition(const glm::vec3& textPosition) {
				m_textPosition = textPosition;
			}
			/*
			* Get the scale of the text in this label
			*/
			float getTextScale() const { return m_textScale; }
			/*
			* Set the scale of the text in this label
			*/
			void setTextScale(float textScale) { m_textScale = textScale; }
			/*
			* Get the color of this label's text
			*/
			const glm::vec3& getTextColor() const { return m_textColor; }
			/*
			* Set the color of this label's text
			*/
			void setTextColor(const glm::vec3& textColor) { m_textColor = textColor; }
			/*
			* Get the justification of this label's text on the x-axis
			*/
			Justification getHJustification() const { return m_hJustification; }
			/*
			* Set the justification of this label's text
			*/
			void setHJustification(Justification hJustification) {
				m_hJustification = hJustification;
			}
			/*
			* Get the justification of this label's text on the y-axis
			*/
			Justification getVJustification() const { return m_vJustification; }
			/*
			* Set the justification of this label's text on the y-axis
			*/
			void setVJustification(Justification vJustification) {
				m_vJustification = vJustification;
			}

		protected:
			// The text for this label to display
			std::string m_text = "";
			// The position of this text label
			glm::vec3 m_position = glm::vec3();
			// The bounds to draw this label's text in
			glm::vec2 m_dimensions = glm::vec2();
			// The position to start the text at
			glm::vec3 m_textPosition = glm::vec3();
			// The scale to draw this label's text at
			float m_textScale = 1.0f;
			// The color to draw this label's text in
			glm::vec3 m_textColor = glm::vec3();
			// The justification to draw this label's text with on the x-axis
			Justification m_hJustification = Justification::LEFT;
			// The justification to draw this label's text with on the y-axis
			Justification m_vJustification = Justification::BOTTOM;
		};

		/*
		* A button which can be selected by mousing over and clicked
		*/
		class Button : public TextLabel {
		public:
			// The event ID number for a UIButton being unselected
			static unsigned int UNSELECTED_EVENT;
			// The event ID number for a UIButton being selected
			static unsigned int SELECTED_EVENT;
			// The event ID number for a click on a UIButton
			static unsigned int CLICKED_EVENT;

			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param text: The string of text this button will draw
			* @param position: The position of this button
			* @param dimensions: The width and height of the rectangle to render
			* this button's text in
			* @param scale: The scale to draw this button's text at
			* @param color: The color to draw this button's text in
			* @param textHJustification: The justification to draw this button's
			* text with on the x-axis
			* @param textVJustification: The justification to draw this button's
			* text with on the y-axis
			* @param texture: The texture containing the unselected, selected, and
			* clicked textures for this button from left to right
			*/
			void init(const std::string& text, const glm::vec3& position,
				const glm::vec2& dimensions, float textScale,
				const glm::vec3& textColor, Justification textHJustification,
				Justification textVJustification, Texture* texture);
			/*
			* Draw this button
			*/
			void draw() override;
			/*
			* Process input to this button
			*/
			virtual void processInput() override;
			/*
			* Free this button's memory
			*/
			void destroy() override;
			/*
			* Set the position of this button
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Set the dimensions of this button
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Test if this button is selected
			*/
			bool isSelected() const { return m_selected; }

		protected:
			// Whether the mouse cursor is over this button
			bool m_selected = false;
			// The sprite containing this button's textures
			Sprite m_sprite;

		private:
			// The texture coordinates of this button's unselected texture
			static glm::vec4 UNSELECTED_COORDS;
			// The texture coordinates of this button's selected texture
			static glm::vec4 SELECTED_COORDS;
			// The texture coordinates of this button's clicked texture
			static glm::vec4 CLICKED_COORDS;
		};

		/*
		* A text box for recording keyboard text inputs
		*/
		class TextBox : public TextLabel, InputListener {
		public:
			// The event ID number for a return key press on a text box
			static unsigned int RETURN_EVENT;
			// The event ID number for a text box being selected
			static unsigned int SELECTED_EVENT;
			// The event ID number for a text box being unselected
			static unsigned int UNSELECTED_EVENT;

			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param text: The string of text this text box will draw
			* @param position: The position of this text box
			* @param dimensions: The width and height of the rectangle to render
			* this text box's text in
			* @param scale: The scale to draw this text box's text at
			* @param color: The color to draw this text box's text in
			* @param textHJustification: The justification to draw this text box's
			* text with on the x-axis
			* @param textVJustification: The justification to draw this text box's
			* text with on the y-axis
			* @param texture: The texture containing the unselected, selected, and
			* clicked textures for this text box from left to right
			* @param cursorTexture: The texture for the cursor in this text box
			* @param maxCharacters: The maximum number of characters which this
			* text box can hold (0 or less for infinite)
			*/
			void init(const std::string& text, const glm::vec3& position,
				const glm::vec2& dimensions, float textScale,
				const glm::vec3& textColor, Justification textHJustification,
				Justification textVJustification, Texture* texture,
				Texture* cursorTexture, int maxCharacters);
			/*
			* Draw this text box
			*/
			void draw() override;
			/*
			* Process input to this text box
			*/
			virtual void processInput() override;
			/*
			* Update this text box's timer for backspaces and cursor movement
			*/
			virtual void update(float deltaTime) override;
			/*
			* Free this text box's memory
			*/
			void destroy() override;
			/*
			* Set the position of this text box
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Set the dimensions of this text box
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Test if this text box is selected
			*/
			bool isSelected() const { return m_selected; }
			/*
			* Text has been typed on the keyboard
			*/
			void textTyped(char text) override;

		protected:
			// The sprite used to render this text box's texture
			Sprite m_sprite;
			// The sprite used to render this text box's cursor
			Sprite m_cursor;
			// Whether this text box is currently selected
			bool m_selected = false;
			// The position of the cursor in the string
			unsigned int m_cursorPosition = 0;
			// The maximum characters allowed in this text box
			int m_maxCharacters = 0;
			// The timer for this text box's cursor movements
			float m_timer = 0.0f;

			/*
			* Updates the position of the cursor when the position of this text
			* box has been set
			*/
			void updateCursorPosition();

		private:
			// The texture coordinates for this text box's unselected texture
			static glm::vec4 UNSELECTED_COORDS;
			// The texture coordinates for this text box's selected texture
			static glm::vec4 SELECTED_COORDS;
		};
	}
}

#endif