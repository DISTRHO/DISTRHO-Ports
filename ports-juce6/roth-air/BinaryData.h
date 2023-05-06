/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   airText_png;
    const int            airText_pngSize = 22392;

    extern const char*   bigKnob_light_png;
    const int            bigKnob_light_pngSize = 60475;

    extern const char*   bigKnob_red_png;
    const int            bigKnob_red_pngSize = 38916;

    extern const char*   bigKnob_png;
    const int            bigKnob_pngSize = 47504;

    extern const char*   label_freq_png;
    const int            label_freq_pngSize = 21456;

    extern const char*   label_gain_png;
    const int            label_gain_pngSize = 21661;

    extern const char*   label_mix_png;
    const int            label_mix_pngSize = 21577;

    extern const char*   label_thresh_png;
    const int            label_thresh_pngSize = 21704;

    extern const char*   smallKnob_light_png;
    const int            smallKnob_light_pngSize = 31141;

    extern const char*   smallKnob_png;
    const int            smallKnob_pngSize = 31445;

    extern const char*   title_png;
    const int            title_pngSize = 24948;

    extern const char*   website_png;
    const int            website_pngSize = 23975;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 12;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
