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
			* Get the position of this component.
			*/
			virtual const glm::vec3& getPosition() const = 0;
			/*
			* Set the position of this component
			*/
			virtual void setPosition(const glm::vec3& position) = 0;
			/*
			* Get the dimensions of this component.
			*/
			virtual const glm::vec2& getDimensions() const = 0;
			/*
			* Set the dimensions of this component.
			*/
			virtual void setDimensions(const glm::vec2& dimensions) = 0;
			/*
			* Get the text scale of this component
			*/
			virtual float getTextScale() const = 0;
			/*
			* Set the text scale of this component
			*/
			virtual void setTextScale(float textScale) = 0;
			/*
			* Test whether this component is enabled
			*/
			virtual bool isEnabled() const { return m_enabled; }
			/*
			* Set whether this component is enabled
			*/
			virtual void setEnabled(bool enabled) { m_enabled = enabled; }
			/*
			* Test whether this component is visible
			*/
			virtual bool isVisible() const { return m_visible; }
			/*
			* Set whether this component is visible
			*/
			virtual void setVisible(bool visible) { m_visible = visible; }
			/*
			* Get the ID number of this component
			*/
			virtual unsigned int getID() const { return m_ID; }
			/*
			* Test whether this component's size and position are controlled by
			* another component.
			*/
			bool isOwned() const { return m_owned; }
			/*
			* Set whether this component's size and position are controlled by
			* another component.
			*/
			void setOwned(bool owned) { m_owned = owned; }

		protected:
			// Allow groups to call the draw, processInput, update, and destroy
			// functions on components
			friend UIGroup;
			// The group containing this component
			UIGroup* m_parent = nullptr;
			// Whether this component is currently enabled
			bool m_enabled = true;
			// Whether this component is currently visible
			bool m_visible = true;
			// The ID number of this component within its group
			unsigned int m_ID = 0;
			// Whether this component's size and position are controlled by
			// another component
			bool m_owned = false;

			/*
			* Test whether a 2D point falls in a rectangle
			*
			* @param rect: The rectangle's 2D position and dimensions
			* (x, y, w, h)
			* @param p: The point to test
			* @return Whether the point p falls inside the given rectangle
			*/
			static bool RectContains(const glm::vec4& rect, const glm::vec2& p);
			/*
			* Callback function for registering component with UIGroup
			*/
			virtual void add() = 0;
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
			* Update the positions and sizes of the components in this group
			* when the window changes size
			* 
			* @return The factor by which the size of the window was scaled
			*/
			glm::vec2 updateWindowSize();
			/*
			* Update physics in all components in this group
			*/
			void update(float deltaTime);
			/*
			* Free the memory of this group and all its components
			*/
			void destroy();
			/*
			* Set whether the components in this group are enabled
			*/
			void setEnabled(bool enabled);
			/*
			* Set whether the components in this group are visible
			*/
			void setVisible(bool visible);
			/*
			* Add a component to this group if it is not already in the group
			*
			* @return Whether this component could be added
			*/
			bool addComponent(UIComponent* c);
			/*
			* Add a set of components to this group if not already in the
			* group.
			*/
			void addComponents(const std::vector<UIComponent*>& c);
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
			// The last saved dimensions of the window for dynamic resizing
			// of components
			glm::vec2 m_previousWindowDims = glm::vec2();
		};

		/*
		* Forward declare all the types of UIComponent so they can be friends
		* to the TextLabel class.
		*/
		class Button;
		class TextBox;
		class Switch;
		class Slider;
		class Cycle;
		class TextArea;

		/*
		* A text label for drawing text onto the screen
		*/
		class TextLabel : public UIComponent {
		public:
			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param text: The string of text this label will render
			* @param normalPosition: The position of this label
			* @param normalDimensions: The normalized width and height of the
			* rectangle to render this label's text in
			* @param normalTextPosition: The normalized position of the text
			* in this label.
			* @param scale: The scale to draw this label's text at
			* @param color: The color to draw this label's text in
			* @param hJustification: The justification to draw this label's text with
			* on the x-axis
			* @param vJustification: The justification to draw this label's text with
			* on the y-axis
			*/
			virtual void init(const std::string& text,
				const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions,
				const glm::vec3& normalTextPosition, float textScale,
				const glm::vec3& textColor, Justification hJustification,
				Justification vJustification);
			/*
			* Get the text this label displays
			*/
			virtual const std::string& getText() const { return m_text; }
			/*
			* Set the text this label displays
			*/
			virtual void setText(const std::string& text) { m_text = text; }
			/*
			* Get the position of this label on the screen
			*/
			virtual const glm::vec3& getPosition() const override {
				return m_position;
			}
			/*
			* Set the position of this label on the screen
			*/
			virtual void setPosition(const glm::vec3& position) override {
				m_position = position;
			}
			/*
			* Get the dimensions of the rectangle this label's text appears
			* in on the screen
			*/
			virtual const glm::vec2& getDimensions() const override {
				return m_dimensions;
			}
			/*
			* Set the dimensions of the rectangle this label's text appears
			* in on the screen
			*/
			virtual void setDimensions(const glm::vec2& dimensions) override {
				m_dimensions = dimensions;
			}
			/*
			* Get the position to start this label's text at
			*/
			virtual const glm::vec3& getTextPosition() const {
				return m_textPosition;
			}
			/*
			* Set the position to start this label's text at
			*/
			virtual void setTextPosition(const glm::vec3& textPosition) {
				m_textPosition = textPosition;
			}
			/*
			* Get the scale of the text in this label
			*/
			float getTextScale() const override { return m_textScale; }
			/*
			* Set the scale of the text in this label
			*/
			void setTextScale(float textScale) override {
				m_textScale = textScale;
			}
			/*
			* Get the color of this label's text
			*/
			virtual const glm::vec3& getTextColor() const {
				return m_textColor;
			}
			/*
			* Set the color of this label's text
			*/
			virtual void setTextColor(const glm::vec3& textColor) {
				m_textColor = textColor;
			}
			/*
			* Get the justification of this label's text on the x-axis
			*/
			virtual Justification getHJustification() const {
				return m_hJustification;
			}
			/*
			* Set the justification of this label's text
			*/
			virtual void setHJustification(Justification hJustification) {
				m_hJustification = hJustification;
			}
			/*
			* Get the justification of this label's text on the y-axis
			*/
			virtual Justification getVJustification() const {
				return m_vJustification;
			}
			/*
			* Set the justification of this label's text on the y-axis
			*/
			virtual void setVJustification(Justification vJustification) {
				m_vJustification = vJustification;
			}

		protected:
			// Allow the other UI component classes to access the TextLabel
			// functions and members.
			friend Button;
			friend TextBox;
			friend Switch;
			friend Slider;
			friend Cycle;
			friend TextArea;
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

			/*
			* Add this text label to a UIGroup
			*/
			virtual void add() override {}
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
		};

		/*
		* A button which can be selected by mousing over and clicked
		*/
		class Button : public TextLabel {
		public:
			// The event ID number for a UIButton being unselected
			const static unsigned int UNSELECTED_EVENT = 0;
			// The event ID number for a UIButton being selected
			const static unsigned int SELECTED_EVENT = 1;
			// The event ID number for a click on a UIButton
			const static unsigned int CLICKED_EVENT = 2;

			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param text: The string of text this button will draw
			* @param normalPosition: The normalized position of this button
			* @param normalDimensions: The normalized width and height of the
			* rectangle to render this button's text in
			* @param scale: The scale to draw this button's text at
			* @param color: The color to draw this button's text in
			* @param textHJustification: The justification to draw this button's
			* text with on the x-axis
			* @param textVJustification: The justification to draw this button's
			* text with on the y-axis
			* @param texture: The texture containing the unselected, selected, and
			* clicked textures for this button from left to right
			*/
			void init(const std::string& text, const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions, float textScale,
				const glm::vec3& textColor, Justification textHJustification,
				Justification textVJustification, Texture* texture);
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
			// The texture coordinates of this button's unselected texture
			static glm::vec4 UNSELECTED_COORDS;
			// The texture coordinates of this button's selected texture
			static glm::vec4 SELECTED_COORDS;
			// The texture coordinates of this button's clicked texture
			static glm::vec4 CLICKED_COORDS;
			// Whether the mouse cursor is over this button
			bool m_selected = false;
			// The sprite containing this button's textures
			Sprite m_sprite;

			/*
			* Draw this button
			*/
			void draw() override;
			/*
			* Process input to this button
			*/
			void processInput() override;
			/*
			* Free this button's memory
			*/
			void destroy() override;
		};

		/*
		* A text box for recording keyboard text inputs
		*/
		class TextBox : public TextLabel, InputListener {
		public:
			// The event ID number for a return key press on a text box
			const static unsigned int RETURN_EVENT = 0;
			// The event ID number for a text box being selected
			const static unsigned int SELECTED_EVENT = 1;
			// The event ID number for a text box being unselected
			const static unsigned int UNSELECTED_EVENT = 2;

			/*
			* Initialize this text label with some text to draw and data for it
			*
			* @param labelText: The label to appear above this text box
			* @param text: The string of text this text box will draw
			* @param normalPosition: The normalized position of this text box
			* @param normalDimensions: The normalized width and height of the
			* rectangle to render this text box's text in
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
			void init(const std::string& labelText,
				const std::string& text, const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions, float textScale,
				const glm::vec3& textColor, Justification textHJustification,
				Justification textVJustification, Texture* texture,
				Texture* cursorTexture, int maxCharacters);
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
			// The texture coordinates for this text box's unselected texture
			static glm::vec4 UNSELECTED_COORDS;
			// The texture coordinates for this text box's selected texture
			static glm::vec4 SELECTED_COORDS;
			// The text label to appear above this text box
			TextLabel m_label;
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
			* Add this text box to a UIGroup
			*/
			void add() override;
			/*
			* Draw this text box
			*/
			void draw() override;
			/*
			* Process input to this text box
			*/
			void processInput() override;
			/*
			* Update this text box's timer for backspaces and cursor movement
			*/
			void update(float deltaTime) override;
			/*
			* Free this text box's memory
			*/
			void destroy() override;

		private:
			/*
			* Updates the position of the cursor when the position of this text
			* box has been set
			*/
			void updateCursorPosition();
		};

		/*
		* A switch which can be toggled between two states
		*/
		class Switch : public TextLabel {
		public:
			// The event ID for toggling this switch on
			const static unsigned int ON_EVENT = 0;
			// The event ID for toggling this switch off
			const static unsigned int OFF_EVENT = 1;

			/*
			* Initialize this switch.
			*
			* @param labelText: The text to appear in the label above this
			* switch.
			* @param text: The text to appear on this switch.
			* @param normalPosition: The normalized position of this switch
			* on the screen.
			* @param normalDimensions: The normalized dimensions of this
			* switch on the screen.
			* @param textScale: The factor to scale the text appearing in this
			* switch by.
			* @param textColor: The RGB color to display this switch's text in.
			* @param textHJustification: The horizontal justification of the
			* text in this switch.
			* @param textVJustification: The vertical justification of the text
			* in this switch.
			* @param texture: The off and on textures of this switch.
			* @param on: Whether this switch is in its on state by default.
			*/
			void init(const std::string& labelText, const std::string& text,
				const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions,
				float textScale, const glm::vec3& textColor,
				Justification textHJustification,
				Justification textVJustification, Texture* texture,
				bool on);
			/*
			* Set the position of this switch on the screen.
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Set the dimensions of this switch on the screen.
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Test whether this switch is in its on state
			*/
			bool isOn() const { return m_on; }
			/*
			* Set whether this swith is in its on state
			*/
			void setOn(bool on);

		protected:
			// The texture coordinates of this switch's on texture
			static glm::vec4 ON_COORDS;
			// The texture coordinates of this switch's off texture
			static glm::vec4 OFF_COORDS;
			// Whether this switch is in its on state
			bool m_on = false;
			// The text label to appear above this switch
			TextLabel m_label;
			// The background sprite of this switch
			Sprite m_sprite;

			/*
			* Add this switch to a UI group.
			*/
			void add() override;
			/*
			* Draw this switch on the screen.
			*/
			void draw() override;
			/*
			* Process input to this switch
			*/
			void processInput() override;
			/*
			* Free this switch's memory
			*/
			void destroy() override;
		};

		/*
		* A slider component which can be dragged to different values with the
		* mouse.
		*/
		class Slider : public TextLabel {
		public:
			// The event ID for this slider being selected
			const static unsigned int SELECTED_EVENT = 0;
			// The event ID for this slider's value being set
			const static unsigned int VALUE_UPDATE_EVENT = 1;

			/*
			* Initialize this slider's memory.
			*
			* @param labelText: The text to appear in the label above this
			* slider.
			* @param normalPosition: The normalized position of this slider
			* on the screen.
			* @param normalPimensions: The normalized dimensions of this
			* slider on the screen.
			* @param textScale: The factor to scale the text in this slider by.
			* @param textColor: The color to display this slider's text in.
			* @param textHJustification: The horizontal justification of the
			* text in this slider.
			* @param textVJustification: The vertical justification of the
			* text in this slider.
			* @param texture: The unselected and selected textures of this
			* slider.
			* @param cursorTexture: The texture of this slider's cursor.
			* @param min: The minimum value of this slider.
			* @param value: The initial value of this slider.
			* @param max: The maximum value of this slider.
			*/
			void init(const std::string& labelText,
				const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions, float textScale,
				const glm::vec3& textColor, Justification textHJustification,
				Justification textVJustification, Texture* texture,
				Texture* cursorTexture, int min, int value, int max);
			/*
			* Get the position of this slider's background sprite.
			*/
			const glm::vec3& getPosition() const override {
				return m_sprite.position;
			}
			/*
			* Set the position of this slider on the screen.
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Set the dimensions of this slider on the screen.
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Get the current value of this slider.
			*/
			int getValue() const { return m_value; }
			/*
			* Set the value of this slider.
			*/
			void setValue(int value);

		protected:
			// The unselected texture coords of this slider.
			static glm::vec4 UNSELECTED_COORDS;
			// The selected texture coords of this slider.
			static glm::vec4 SELECTED_COORDS;
			// The text to appear in the label above this slider.
			std::string m_labelText = "";
			// The background sprite of this slider.
			Sprite m_sprite;
			// The cursor sprite of this slider.
			Sprite m_cursor;
			// Whether this slider is selected.
			bool m_selected = false;
			// The minimum value of this slider.
			int m_min = 0;
			// The current value of this slider.
			int m_value = 0;
			// The maxmimum value of this slider.
			int m_max = 0;

			/*
			* Draw this slider on the screen.
			*/
			void draw() override;
			/*
			* Process input to this slider.
			*/
			void processInput() override;
			/*
			* Free this slider's memory.
			*/
			void destroy() override;

		private:
			/*
			* Update the position of this slider's cursor sprite based on its
			* current value.
			*/
			void updateCursorPosition();
		};

		/*
		* A text label which can be cycled between a set of values.
		*/
		class Cycle : public TextLabel {
		public:
			// The event ID for the selection of this cycle being updated
			const static unsigned int VALUE_UPDATE_EVENT = 0;

			/*
			* Initialize this cycle UI component
			*
			* @param labelText: The text to appear above this cycle.
			* @param options: The options to cycle through.
			* @param normalPosition: The normalized position of this cycle on
			* the screen.
			* @param normalDimensions: The normalized dimensions of this
			* cycle on the screen.
			* @param arrowWidth: The width of the left and right arrows of
			* this cycle on the screen.
			* @param textScale: The factor to scale the text size by in this
			* cycle.
			* @param textColor: The RGB color to render this cycle's text in.
			* @param textHJustification: The horizontal justification of the
			* text in this cycle.
			* @param textVJustification: The vertical justification of the text
			* in this cycle.
			* @param texture: The texture of the background of this cycle.
			* @param arrowTexture: The unselected, selected, and clicked left
			* facing arrow textures of this cycle.
			* @param selection: The initial selected index of this cycle.
			*/
			void init(const std::string& labelText,
				const std::vector<std::string>& options,
				const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions, float arrowWidth,
				float textScale, const glm::vec3& textColor,
				Justification textHJustification,
				Justification textVJustification, Texture* texture,
				Texture* arrowTexture, unsigned int selection);
			/*
			* Set the position of this cycle on the screen.
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Set the dimensions of this cycle on the screen.
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Get the selected index of this cycle.
			*/
			unsigned int getSelection() const { return m_selection; }
			/*
			* Set the selected index of this cycle.
			*/
			void setSelection(unsigned int selection);
			/*
			* Set the width of the arrows on either side of this cycle, should
			* be followed by calling setDimensions() to take effect
			*/
			void setArrowWidth(float arrowWidth) { m_arrowWidth = arrowWidth; }

		protected:
			// The texture coordinates of the unselected left arrow
			static glm::vec4 UNSELECTED_LEFT_COORDS;
			// The texture coordinates of the selected left arrow
			static glm::vec4 SELECTED_LEFT_COORDS;
			// The texture coordinates of the clicked left arrow
			static glm::vec4 CLICKED_LEFT_COORDS;
			// The texture coordinates of the unselected right arrow
			static glm::vec4 UNSELECTED_RIGHT_COORDS;
			// The texture coordinates of the selected right arrow
			static glm::vec4 SELECTED_RIGHT_COORDS;
			// The texture coordinates of the clicked right arrow
			static glm::vec4 CLICKED_RIGHT_COORDS;
			// The label to appear above this cycle
			TextLabel m_label;
			// The background sprite of this cycle
			Sprite m_sprite;
			// The width in pixels of the left and right arrows
			float m_arrowWidth = 0.0f;
			// Whether this cycle's left arrow is selected
			bool m_leftArrowSelected = false;
			// Whether this cycle's right arrow is selected
			bool m_rightArrowSelected = false;
			// The left arrow sprite of this cycle
			Sprite m_leftArrow;
			// The right arrow sprite of this cycle
			Sprite m_rightArrow;
			// The current index of this cycle in its options
			unsigned int m_selection = 0;
			// The options this cycle goes through
			std::vector<std::string> m_options;

			/*
			* Add this cycle to a UI group
			*/
			void add() override;
			/*
			* Draw this cycle on the screen
			*/
			void draw() override;
			/*
			* Process input to this cycle.
			*/
			void processInput() override;
			/*
			* Free this cycle's memory.
			*/
			void destroy() override;
		};
	
		/*
		* A handle for a group of text labels which can be used to draw multi-
		* line strings. Acts as a simple text editor when enabled.
		*/
		class TextArea : public UIComponent, InputListener {
		public:
			// The event ID for this text area being unselected
			const static unsigned int UNSELECTED_EVENT = 0;
			// The event ID for this text area being selected
			const static unsigned int SELECTED_EVENT = 1;

			/*
			* Initialize this text area
			* 
			* @param text: The multi-line string of text to appear in this
			* text area.
			* @param lineCount: The number of labels to draw the text with.
			* @param normalPosition: The normalized position of this text area
			* on the screen.
			* @param normalDimensions: The normalized dimensions of this text
			* area on the screen.
			* @param textScale: The scale of the text to appear in the labels.
			* @parma textColor: The color of the text to appear in the labels.
			* @param hJustification: The horizontal justification of the text
			* in the labels.
			* @param vJustification: The vertical justification of the text in
			* the labels.
			* @param texture: The texture of the background sprite of this text
			* area.
			*/
			void init(const std::string& text, unsigned int lineCount,
				const glm::vec3& normalPosition,
				const glm::vec2& normalDimensions, float textScale,
				const glm::vec3& textColor, Justification hJustification,
				Justification vJustification, Texture* texture,
				Texture* cursorTexture);
			/*
			* Get the text displayed by this text area
			*/
			const std::string& getText() const { return m_text; }
			/*
			* Set the text displayed by this text area
			*/
			void setText(const std::string& text);
			/*
			* Get the position of this text area
			*/
			const glm::vec3& getPosition() const override;
			/*
			* Set the position of this text area
			*/
			void setPosition(const glm::vec3& position) override;
			/*
			* Get the dimensions of this text area
			*/
			const glm::vec2& getDimensions() const override;
			/*
			* Set the dimensions of this text area
			*/
			void setDimensions(const glm::vec2& dimensions) override;
			/*
			* Get the text scale of this text area
			*/
			float getTextScale() const override;
			/*
			* Set the text scale of this text area
			*/
			void setTextScale(float textScale) override;
			/*
			* Test whether this text area is enabled
			*/
			bool isEnabled() const override { return m_enabled; }
			/*
			* Set whether this text area is enabled
			*/
			void setEnabled(bool enabled) override;
			/*
			* Test whether this text area is visible
			*/
			bool isVisible() const override { return m_visible; }
			/*
			* Set whether this text area is visible
			*/
			void setVisible(bool visible) override;
			/*
			* Test whether scrolling is enabled on this text area.
			*/
			bool isScrollEnabled() const { return m_scrollEnabled; }
			/*
			* Set whether scrolling is enabled on this text area.
			*/
			void setScrollEnabled(bool scrollEnabled) {
				m_scrollEnabled = scrollEnabled;
			}
			/*
			* Test whether line wrapping is enabled for the text in this text
			* area
			*/
			bool isLineWrapEnabled() const { return m_lineWrapEnabled; }
			/*
			* Test whether line wrapping is enabled for the text in this text
			* area
			*/
			void setLineWrapEnabled(bool lineWrapEnabled);
			/*
			* Test whether this text area is selected
			*/
			bool isSelected() const { return m_selected; }
			/*
			* Set whether this text area is selected
			*/
			void setSelected(bool selected);
			/*
			* Get the x-position of the text in this text area
			*/
			float getTextPosition() const { return m_textPosition; }
			/*
			* Set the x-position of the text in this text area
			*/
			void setTextPosition(float textPosition);
			/*
			* Test whether this text area is editable
			*/
			bool isEditable() const { return m_editable; }
			/*
			* Set whether this text area is editable
			*/
			void setEditable(bool editable) { m_editable = editable; }

		protected:
			// The texture coords of the unselected texture for this text area
			static glm::vec4 UNSELECTED_COORDS;
			// The texture coords of the selected texture for this text area
			static glm::vec4 SELECTED_COORDS;

			/*
			* Add this text area to a UI group
			*/
			void add() override;
			/*
			* Draw this text area to the screen
			*/
			void draw() override;
			/*
			* Process input to this text area
			*/
			void processInput() override;
			/*
			* Process text typed into this text area
			*/
			void textTyped(char text) override;
			/*
			* Process the scroll wheel moving on this text area
			*/
			void scrolled(const glm::vec2& distance) override;
			/*
			* Update this text area
			*/
			void update(float deltaTime) override {};
			/*
			* Free this text area's memory
			*/
			void destroy() override;

		private:
			// The text in this text area
			std::string m_text = "";
			// The number of lines in the text string for this area to display
			std::vector<std::string> m_lines;
			// The set of text labels
			std::vector<TextLabel> m_labels;
			// Whether scrolling is enabled in this text area
			bool m_scrollEnabled = false;
			// The index of the line which appears in the first text label
			int m_scroll = 0;
			// Whether line wrapping is enabled for the text in this text area
			bool m_lineWrapEnabled = false;
			// Whether this text area is currently selected
			bool m_selected = false;
			// The background sprite of this text area.
			Sprite m_sprite;
			// The dimensions of this text area on the screen
			glm::vec2 m_dimensions = glm::vec2(0.0f);
			// The x-value of the text position of all labels in this text area
			float m_textPosition = 0.0f;
			// Whether this text area is editable
			bool m_editable = false;
			// The position of the cursor in the text in this text area
			unsigned int m_cursorPosition = 0;
			// The index of the line the cursor is on, set by updateCursorPosition
			unsigned int m_cursorLine = 0;
			// The sprite representing the cursor in this text area
			Sprite m_cursor;

			/*
			* Fill the text labels with the text from m_text based on the
			* scroll and line wrap values.
			*/
			void populateLabels();
			/*
			* Find the width of a string of text in pixels based on the font
			* and text scale
			*/
			float getStringWidth(const std::string& str);
			/*
			* Helper function for updateCursorPosition()
			*/
			void pushCursor(unsigned int& count, bool& found, int l, int c);
			/*
			* Update the position of the cursor based on the dimensions of the
			* text area and its position in the text
			*/
			void updateCursorPosition();
		};
	}
}

#endif