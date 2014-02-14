#ifndef ROTATINGTOGGLEBUTTON_H
#define ROTATINGTOGGLEBUTTON_H

#include "juce_PluginHeaders.h"

class RotatingToggleButton : public ToggleButton, public Timer
{
	public:
		RotatingToggleButton(const String& buttonText);
		virtual ~RotatingToggleButton();
		virtual void setRotationState(float rotationState);
	protected:
		/** @internal */
		void paintButton (Graphics& g,
						  bool isMouseOverButton,
						  bool isButtonDown);

		void clicked();

		void timerCallback();

		void parentHierarchyChanged();

		unsigned last_timer_call;

		Path path;
		float rotationState;
	private:
};

#endif // ROTATINGTOGGLEBUTTON_H
