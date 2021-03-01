/*
* File: UI.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2021.01.27
*/

#pragma once

#include "MW.h"
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
		* Initialize this component with a parent and ID
		* 
		* @param parent: A pointer to the UIGroup containing this component
		*/
		virtual void init(UIGroup* parent) {
			m_parent = parent;
		}
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
		// The ID number of this component within its group
		unsigned int m_ID = 0;
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
		* @param ID: The ID number of this group in its scene
		*/
		void init(Scene* parent, unsigned int ID);
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

	private:
		// The parent scene of this group
		Scene* m_parent = nullptr;
		// The ID number of this group
		unsigned int m_ID = 0;
		// The components in this group mapped to their ID numbers
		std::vector<UIComponent*> m_components;
		// The working component ID for adding new components to this group
		unsigned int m_CID = 0;
	};
}