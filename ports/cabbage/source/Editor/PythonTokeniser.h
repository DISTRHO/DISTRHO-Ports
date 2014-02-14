/*
  Copyright (C) 2013 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

#ifndef __PYTHON_TOKER__
#define __PYTHON_TOKER__

#include "../JuceHeader.h"

class PythonTokeniser : public CodeTokeniser
{
public:
	PythonTokeniser(){}
	~PythonTokeniser(){}
	 
	//==============================================================================
    enum TokenType
    {
        tokenType_error = 0,
        tokenType_comment,
        tokenType_builtInKeyword,
        tokenType_identifier,
        tokenType_integerLiteral,
        tokenType_floatLiteral,
        tokenType_stringLiteral,
        tokenType_operator,
        tokenType_bracket,
        tokenType_punctuation,
        tokenType_preprocessor,
		tokenType_csdTag
    };

	CodeEditorComponent::ColourScheme getDefaultColourScheme()
	{
		struct Type
		{
			const char* name;
			uint32 colour;
		};

		const Type types[] =
		{
			{ "Error",              Colours::black.getARGB() },
			{ "Comment",            Colours::green.getARGB() },
			{ "Keyword",            Colours::blue.getARGB() },
			{ "Identifier",         Colours::black.getARGB() },
			{ "Integer",            Colours::orange.getARGB() },
			{ "Float",              Colours::black.getARGB() },
			{ "String",             Colours::red.getARGB() },
			{ "Operator",           Colours::pink.getARGB() },
			{ "Bracket",            Colours::darkgreen.getARGB() },
			{ "Punctuation",        Colours::black.getARGB() },
			{ "Preprocessor Text",  Colours::green.getARGB() },
			{ "Csd Tag",			Colours::brown.getARGB() }
		};

		CodeEditorComponent::ColourScheme cs;

		for (int i = 0; i < sizeof (types) / sizeof (types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
			cs.set (types[i].name, Colour (types[i].colour));

		return cs;
	}

	CodeEditorComponent::ColourScheme getDarkColourScheme()
	{
		struct Type
		{
			const char* name;
			uint32 colour;
		};

		const Type types[] =
		{
			{ "Error",              Colours::white.getARGB() },
			{ "Comment",            Colours::green.getARGB() },
			{ "Keyword",            Colours::cornflowerblue.getARGB() },
			{ "Identifier",         Colours::white.getARGB() },
			{ "Integer",            Colours::orange.getARGB() },
			{ "Float",              Colours::lime.getARGB() },
			{ "String",             Colours::red.getARGB() },
			{ "Operator",           Colours::pink.getARGB() },
			{ "Bracket",            Colours::darkgreen.getARGB() },
			{ "Punctuation",        Colours::white.getARGB() },
			{ "Preprocessor Text",  Colours::green.getARGB() },
			{ "Csd Tag",			Colours::brown.getARGB() }
		};

		CodeEditorComponent::ColourScheme cs;

		for (int i = 0; i < sizeof (types) / sizeof (types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
			cs.set (types[i].name, Colour (types[i].colour));

		return cs;
	}


private:
	//==============================================================================
	StringArray getTokenTypes()
	{
    StringArray s;
    s.add ("Error");
    s.add ("Comment");
    s.add ("C++ keyword");
    s.add ("Identifier");
    s.add ("Integer literal");
    s.add ("Float literal");
    s.add ("String literal");
    s.add ("Operator");
    s.add ("Bracket");
    s.add ("Punctuation");
    s.add ("Preprocessor line");
	s.add ("CSD Tag");
    return s;
	}

	//==============================================================================
	void skipQuotedString (CodeDocument::Iterator& source)
	{
    const juce_wchar quote = source.nextChar();
    for (;;)
    {
        const juce_wchar c = source.nextChar();
        if (c == quote || c == 0)
            break;

        if (c == '\\')
            source.skip();
		}
	}


	//==============================================================================
    void skipCSDTag (CodeDocument::Iterator& source) noexcept
    {
        for (;;)
        {
            const juce_wchar c = source.nextChar();
            if (c == 0 || (c == '>'))
                break;
        }
    }

	//==============================================================================
	bool isIdentifierStart (const char c) 
	{
		return CharacterFunctions::isLetter (c)
				|| c == '_' || c == '@';
	}

	//==============================================================================
	bool isIdentifierBody (const char c)
	{
		return CharacterFunctions::isLetter (c)
				|| CharacterFunctions::isDigit (c)
				|| c == '_' || c == '@';
	}

	//==============================================================================
    bool isReservedKeyword (String::CharPointerType token, const int tokenLength) noexcept
    {
	//populate char array with Csound keywords
	//this list of keywords is not completely up to date! 
 		 static const char* const keywords[] =
		 {
				"and",
				"as",
				"assert",
				"break",
				"class",
				"continue",
				"def",
				"del",
				"elif",
				"else",
				"except",
				"exec",
				"finally",
				"for",
				"from",
				"global",
				"if",
				"import",
				"in",
				"is",
				"lambda",
				"not",
				"or",
				"pass",
				"print",
				"raise",
				"return",
				"try",
				"while",
				"with",
				"yield"};


        const char* const* k;

       if (tokenLength < 2 || tokenLength > 16)
                    return false;

	   else
		  k = keywords;


        int i = 0;
        while (k[i] != 0)
        {
            if (token.compare (CharPointer_ASCII (k[i])) == 0)
                return true;

            ++i;
        }
        return false;
    }

	//==============================================================================
   int parseIdentifier (CodeDocument::Iterator& source) noexcept
    {
        int tokenLength = 0;
        String::CharPointerType::CharType possibleIdentifier [100];
        String::CharPointerType possible (possibleIdentifier);

        while (isIdentifierBody (source.peekNextChar()))
        {
            const juce_wchar c = source.nextChar();

            if (tokenLength < 20)
                possible.write (c);

            ++tokenLength;
        }

        if (tokenLength > 1 && tokenLength <= 16)
        {
            possible.writeNull();

            if (isReservedKeyword (String::CharPointerType (possibleIdentifier), tokenLength))
                return CsoundTokeniser::tokenType_builtInKeyword;
        }

        return CsoundTokeniser::tokenType_identifier;
    }

   //==============================================================================
	int readNextToken (CodeDocument::Iterator& source)
	{
    int result = tokenType_error;
    source.skipWhitespace();
    char firstChar = source.peekNextChar();

    switch (firstChar)
    {
    case 0:
        source.skip();
        break;

    case ';':
		source.skipToEndOfLine();
        result = tokenType_comment;
        break;

	case '"':
 //   case T('\''):
    	skipQuotedString (source);
       result = tokenType_stringLiteral;
       break;

	case '<':
		source.skip();
		if((source.peekNextChar() == 'C') ||
			(source.peekNextChar() == '/')){
		skipCSDTag (source);
        result = tokenType_csdTag;
		}
		break;

    default:
		if (isIdentifierStart (firstChar)){
            result = parseIdentifier (source);
		}
        else
            source.skip();
        break;
    }
    //jassert (result != tokenType_unknown);
    return result;
	}
};

#endif