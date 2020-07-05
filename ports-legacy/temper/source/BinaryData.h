/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Background_png;
    const int            Background_pngSize = 1105568;

    extern const char*   BeeStingPreset_xml;
    const int            BeeStingPreset_xmlSize = 250;

    extern const char*   DefaultPreset_xml;
    const int            DefaultPreset_xmlSize = 249;

    extern const char*   FlyingUnitedPreset_xml;
    const int            FlyingUnitedPreset_xmlSize = 252;

    extern const char*   GraphBackground_png;
    const int            GraphBackground_pngSize = 179814;

    extern const char*   MontserratLight_otf;
    const int            MontserratLight_otfSize = 91496;

    extern const char*   MorningAtTheDMVPreset_xml;
    const int            MorningAtTheDMVPreset_xmlSize = 254;

    extern const char*   StubbedToePreset_xml;
    const int            StubbedToePreset_xmlSize = 252;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Number of elements in the namedResourceList array.
    const int namedResourceListSize = 8;

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes) throw();
}
