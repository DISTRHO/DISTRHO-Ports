/*
	==============================================================================
	This file is part of Obxd synthesizer.

	Copyright � 2013-2014 Filatov Vadim
	
	Contact author via email :
	justdat_@_e1.ru

	This file may be licensed under the terms of of the
	GNU General Public License Version 2 (the ``GPL'').

	Software distributed under the License is distributed
	on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
	express or implied. See the GPL for the specific language
	governing rights and limitations.

	You should have received a copy of the GPL along with this
	program. If not, go to http://www.gnu.org/licenses/gpl.html
	or write to the Free Software Foundation, Inc.,  
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
	==============================================================================
 */
#pragma once
#include "ObxdVoice.h"
#include "ParamsEnum.h"
const int PROGRAMCOUNT = 128;
class ObxdBank
{
public:
	ObxdParams programs[PROGRAMCOUNT];
	ObxdParams* currentProgramPtr;
	int currentProgram;
	ObxdBank()
	{
		currentProgram=0;
		currentProgramPtr = programs+currentProgram;
	}
};