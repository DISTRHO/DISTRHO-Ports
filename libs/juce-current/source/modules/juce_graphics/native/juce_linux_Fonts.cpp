/*
  ==============================================================================

   This file is part of the JUCE 7 technical preview.
   Copyright (c) 2022 - Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

static std::unique_ptr<XmlElement> findFontsConfFile()
{
    static const char* pathsToSearch[] = { "/etc/fonts/fonts.conf",
                                           "/usr/share/fonts/fonts.conf",
                                           "/usr/local/etc/fonts/fonts.conf",
                                           "/usr/share/defaults/fonts/fonts.conf" };

    for (auto* path : pathsToSearch)
        if (auto xml = parseXML (File (path)))
            return xml;

    return {};
}

StringArray FTTypefaceList::getDefaultFontDirectories()
{
    StringArray fontDirs;

    fontDirs.addTokens (String (CharPointer_UTF8 (getenv ("JUCE_FONT_PATH"))), ";,", "");
    fontDirs.removeEmptyStrings (true);

    if (fontDirs.isEmpty())
    {
        if (auto fontsInfo = findFontsConfFile())
        {
            for (auto* e : fontsInfo->getChildWithTagNameIterator ("dir"))
            {
                auto fontPath = e->getAllSubText().trim();

                if (fontPath.isNotEmpty())
                {
                    if (e->getStringAttribute ("prefix") == "xdg")
                    {
                        auto xdgDataHome = SystemStats::getEnvironmentVariable ("XDG_DATA_HOME", {});

                        if (xdgDataHome.trimStart().isEmpty())
                            xdgDataHome = "~/.local/share";

                        fontPath = File (xdgDataHome).getChildFile (fontPath).getFullPathName();
                    }

                    fontDirs.add (fontPath);
                }
            }
        }
    }

    if (fontDirs.isEmpty())
        fontDirs.add ("/usr/X11R6/lib/X11/fonts");

    fontDirs.removeDuplicates (false);
    return fontDirs;
}

Typeface::Ptr Typeface::createSystemTypefaceFor (const Font& font)
{
    return new FreeTypeTypeface (font);
}

Typeface::Ptr Typeface::createSystemTypefaceFor (const void* data, size_t dataSize)
{
    return new FreeTypeTypeface (data, dataSize);
}

void Typeface::scanFolderForFonts (const File& folder)
{
    FTTypefaceList::getInstance()->scanFontPaths (StringArray (folder.getFullPathName()));
}

StringArray Font::findAllTypefaceNames()
{
    return FTTypefaceList::getInstance()->findAllFamilyNames();
}

StringArray Font::findAllTypefaceStyles (const String& family)
{
    return FTTypefaceList::getInstance()->findAllTypefaceStyles (family);
}

bool TextLayout::createNativeLayout (const AttributedString&)
{
    return false;
}

//==============================================================================
struct DefaultFontInfo
{
    struct Characteristics
    {
        explicit Characteristics (String nameIn) : name (nameIn) {}

        Characteristics withStyle (String styleIn) const
        {
            auto copy = *this;
            copy.style = std::move (styleIn);
            return copy;
        }

        String name, style;
    };

    DefaultFontInfo()
        : defaultSans  (getDefaultSansSerifFontCharacteristics()),
          defaultSerif (getDefaultSerifFontCharacteristics()),
          defaultFixed (getDefaultMonospacedFontCharacteristics())
    {
    }

    Characteristics getRealFontCharacteristics (const String& faceName) const
    {
        if (faceName == Font::getDefaultSansSerifFontName())    return defaultSans;
        if (faceName == Font::getDefaultSerifFontName())        return defaultSerif;
        if (faceName == Font::getDefaultMonospacedFontName())   return defaultFixed;

        return Characteristics { faceName };
    }

    Characteristics defaultSans, defaultSerif, defaultFixed;

private:
    template <typename Range>
    static Characteristics pickBestFont (const StringArray& names, Range&& choicesArray)
    {
        for (auto& choice : choicesArray)
            if (names.contains (choice.name, true))
                return choice;

        for (auto& choice : choicesArray)
            for (auto& name : names)
                if (name.startsWithIgnoreCase (choice.name))
                    return Characteristics { name }.withStyle (choice.style);

        for (auto& choice : choicesArray)
            for (auto& name : names)
                if (name.containsIgnoreCase (choice.name))
                    return Characteristics { name }.withStyle (choice.style);

        return Characteristics { *names.begin() };
    }

    static Characteristics getDefaultSansSerifFontCharacteristics()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getSansSerifNames (allFonts);

        static const Characteristics targets[] { Characteristics { "Verdana" },
                                                 Characteristics { "Bitstream Vera Sans" }.withStyle ("Roman"),
                                                 Characteristics { "Luxi Sans" },
                                                 Characteristics { "Liberation Sans" },
                                                 Characteristics { "DejaVu Sans" },
                                                 Characteristics { "Sans" } };
        return pickBestFont (allFonts, targets);
    }

    static Characteristics getDefaultSerifFontCharacteristics()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getSerifNames (allFonts);

        static const Characteristics targets[] { Characteristics { "Bitstream Vera Serif" }.withStyle ("Roman"),
                                                 Characteristics { "Times" },
                                                 Characteristics { "Nimbus Roman" },
                                                 Characteristics { "Liberation Serif" },
                                                 Characteristics { "DejaVu Serif" },
                                                 Characteristics { "Serif" } };
        return pickBestFont (allFonts, targets);
    }

    static Characteristics getDefaultMonospacedFontCharacteristics()
    {
        StringArray allFonts;
        FTTypefaceList::getInstance()->getMonospacedNames (allFonts);

        static const Characteristics targets[] { Characteristics { "DejaVu Sans Mono" },
                                                 Characteristics { "Bitstream Vera Sans Mono" }.withStyle ("Roman"),
                                                 Characteristics { "Sans Mono" },
                                                 Characteristics { "Liberation Mono" },
                                                 Characteristics { "Courier" },
                                                 Characteristics { "DejaVu Mono" },
                                                 Characteristics { "Mono" } };
        return pickBestFont (allFonts, targets);
    }

    JUCE_DECLARE_NON_COPYABLE (DefaultFontInfo)
};

Typeface::Ptr Font::getDefaultTypefaceForFont (const Font& font)
{
    static const DefaultFontInfo defaultInfo;

    Font f (font);

    const auto characteristics = defaultInfo.getRealFontCharacteristics (font.getTypefaceName());
    f.setTypefaceName (characteristics.name);

    if (font.getTypefaceStyle() == Font::getDefaultStyle())
        f.setTypefaceStyle (characteristics.style);

    return Typeface::createSystemTypefaceFor (f);
}

} // namespace juce
