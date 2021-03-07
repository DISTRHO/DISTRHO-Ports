/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   logo_svg;
    const int            logo_svgSize = 7224;

    extern const char*   presets_xml;
    const int            presets_xmlSize = 11837;

    extern const char*   PTSansBold_ttf;
    const int            PTSansBold_ttfSize = 287936;

    extern const char*   PTSansRegular_ttf;
    const int            PTSansRegular_ttfSize = 278168;

    extern const char*   PTSansNarrowRegular_ttf;
    const int            PTSansNarrowRegular_ttfSize = 234208;

    extern const char*   tooltips_xml;
    const int            tooltips_xmlSize = 3455;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 6;

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
