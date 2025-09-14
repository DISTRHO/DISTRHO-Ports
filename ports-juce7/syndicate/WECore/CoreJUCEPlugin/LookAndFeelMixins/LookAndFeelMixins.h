/*
 *	File:		LookAndFeelMixinsV2.h
 *
 *	Version:	1.0.0
 *
 *	Created:	19/03/2019
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "ButtonV2.h"
#include "ComboBoxV2.h"
#include "GroupComponentV2.h"
#include "LinearSliderV2.h"
#include "MidAnchoredRotarySlider.h"
#include "PopupMenuV2.h"
#include "RotarySliderV2.h"
#include "CoreJUCEPlugin/CoreLookAndFeel.h"

/**
 * Mixin LookAndFeel classes that can be used to augment any existing class derived from
 * juce::LookAndFeel.
 *
 * Mixin classes have been provided rather than a single LookAndFeel as it allows different
 * LookAndFeels for different components to be mixed and overriden separately. Each mixin class
 * handles all the drawing for a particular component.
 *
 * @section Example Usage
 *
 * To create a LookAndFeel class which uses juce::LookAndFeel_V4 but with WECore's buttons:
 *
 * @code
 * #include "CoreJUCEPlugin/LookAndFeelMixins/LookAndFeelMixins.h"
 *
 * using WECore::LookAndFeelMixins;
 * typedef ButtonV2<LookAndFeel_V4> WECoreButtonsLookAndFeel;
 *
 * WECoreButtonsLookAndFeel _myLookAndFeelInstance;
 * @endcode
 *
 *  To create a LookAndFeel class which uses juce::LookAndFeel_V4 but with WECore's buttons *and*
 *  combo boxes:
 *
 * @code
 * #include "CoreJUCEPlugin/LookAndFeelMixins/LookAndFeelMixins.h"
 *
 * using WECore::LookAndFeelMixins;
 * typedef ButtonV2<ComboBoxV2<LookAndFeel_V4>> WECoreComboLookAndFeel;
 *
 * WECoreComboLookAndFeel _myLookAndFeelInstance;
 * @endcode
 *
 * You can then futher customise these typedef'd LookAndFeel classes in the same way you would
 * customise a normal JUCE LookAndFeelClass:
 *
 * @code
 * #include "CoreJUCEPlugin/LookAndFeelMixins/LookAndFeelMixins.h"
 *
 * using WECore::LookAndFeelMixins;
 * typedef ButtonV2<ComboBoxV2<LookAndFeel_V4>> WECoreComboLookAndFeel;
 *
 * class MyCustomLookAndFeel : public WECoreComboLookAndFeel {
 *     ... custom overrides ...
 * }
 * @endcode
 *
 * or, if you already have a custom LookAndFeel which you'd like to augment with the mixin classes,
 * you can do that too:
 *
 * @code
 * #include "CoreJUCEPlugin/LookAndFeelMixins/LookAndFeelMixins.h"
 *
 * using WECore::LookAndFeelMixins;
 * typedef ButtonV2<ComboBoxV2<CustomLookAndFeelIMadeEarlier>> NewCustomLookAndFeel;
 *
 * NewCustomLookAndFeel _myLookAndFeelInstance;
 * @endcode
 */
namespace WECore::LookAndFeelMixins {
    /**
     * typedef'd all V2 mixins over the CoreLookAndFeel for convenience.
     */
    typedef ButtonV2<
            ComboBoxV2<
            GroupComponentV2<
            LinearSliderV2<
            PopupMenuV2<
            RotarySliderV2<
                WECore::JUCEPlugin::CoreLookAndFeel
            >>>>>> WEV2LookAndFeel;
}
