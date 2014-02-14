#ifndef KEYBOARDBUTTON_H
#define KEYBOARDBUTTON_H

#include "RotatingToggleButton.h"


class KeyboardButton : public RotatingToggleButton
{
	public:
		KeyboardButton(const String& buttonText);
		virtual ~KeyboardButton();
		void setRotationState(float rotState);
	protected:
		void parentHierarchyChanged();
		int parentHeight;
	private:
};

#endif // KEYBOARDBUTTON_H
