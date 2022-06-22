/*
 *  Swanky Amp tube amplifier simulation
 *  Copyright (C) 2020  Garrin McGoldrick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "TooltipsData.h"

TooltipsData::TooltipsData(const std::unique_ptr<XmlElement>& xml)
{
  if (xml == nullptr) return;

  XmlElement* element = xml->getFirstChildElement();
  while (element != nullptr)
  {
    if (element->hasTagName("param") && element->hasAttribute("id"))
    {
      const String& id = element->getStringAttribute("id");
      const String& text = element->getAllSubText().trimStart().trimEnd();
      paramStrings.insert(std::pair<const String&, const String&>(id, text));
    }

    element = element->getNextElement();
  }
}

const String& TooltipsData::getForParam(const String& name) const
{
  const static String empty = "";
  if (paramStrings.find(name) == paramStrings.end())
    return empty;
  else
    return paramStrings.at(name);
}
