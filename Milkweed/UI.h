/*
* File: UI.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.27
*/

#pragma once

#include "Sprite.h"

namespace Milkweed {
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

	private:
		// Allow groups to edit component ID's
		friend UIGroup;
		// Whether this component is currently enabled
		bool m_enabled = true;
		// The ID number of this component within its group
		unsigned int m_ID = 0;
	};

	// Forward declare the Scene class from MW.h
	class Scene;

	/*
	* A set of UIComponents which can be drawn and process input as a group
	*/
	class UIGroup {
	public:
		/*
		* Initialize this group with a parent and an ID
		* 
		* @param parent: The scene containing this UI group
		* @param font: The font to draw the text in the components of this UI
		* group
		* @param shader: The shader to draw the components of this UI group with
		* (this shader must have a uniform vec3 textColor)
		* @param textColorUniform: The uniform name of the text color in the
		* shader
		*/
		void init(Scene* parent, Font* font, Shader* shader,
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
		* Remove a component from this group if it is in the group
		* 
		* @return Whether this component was found and removed
		*/
		bool removeComponent(UIComponent* c);
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
		Shader* getShader() { return m_shader; }
		/*
		* Set the shader used to draw the components of this UI group
		*/
		void setShader(Shader* shader) { m_shader = shader; }
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
		// The font to draw components of this UI group in
		Font* m_font = nullptr;
		// The shader to draw components of this UI group in
		Shader* m_shader = nullptr;
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
	class UILabel : public UIComponent {
	public:
		/*
		* Initialize this text label with some text to draw and data for it
		*
		* @param text: The string of text this label will render
		* @param position: The position of this label in the camera space
		* @param bounds: The width and height of the rectangle to render this
		* label's text in
		* @param scale: The scale to draw this label's text at
		* @param color: The color to draw this label's text in
		* @param justification: the justification to draw this label's text with
		*/
		void init(const std::string& text, const glm::vec3& position,
			const glm::vec2& bounds, float scale, const glm::vec3& color,
			Justification justification = Justification::LEFT);

	private:
		// The text for this label to display
		std::string m_text = "";
		// The position to display this label's text at
		glm::vec3 m_position = glm::vec3();
		// The bounds to draw this label's text in
		glm::vec2 m_bounds = glm::vec2();
		// The scale to draw this label's text at
		float m_scale = 1.0f;
		// The color to draw this label's text in
		glm::vec3 m_color = glm::vec3();
		// The justification to draw this label's text with
		Justification m_justification = Justification::LEFT;

		/*
		* Draw this label's text to the screen
		*/
		void draw() override;
		/*
		* Process input to this label (does nothing)
		*/
		void processInput() override {}
		/*
		* Update physics in this label (does nothing)
		*/
		void update(float deltaTime) override {}
		/*
		* Free this label's memory
		*/
		void destroy() override;
	};
}