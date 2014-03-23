/*
  Copyright (C) 2012 Rory Walsh

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

#ifndef CABBMESS_H
#define CABBMESS_H

#include "CabbageUtils.h"
#ifndef Cabbage_No_Csound
#include "csound.hpp"
#endif

using namespace std;


//simple channel message classe
class CabbageChannelMessage : public CabbageUtils
{
public:
	String channelName;
#ifndef Cabbage_No_Csound
	MYFLT value;
#else
float value;
#endif
	String type;
	String fStatement;
	String stringVal;

	CabbageChannelMessage(String chan, double val, String _type)
	{
		channelName = chan;
		value = val;
		type = _type;		
	}
	
	CabbageChannelMessage(String chan, String val, String _type)
	{
		channelName = chan;
		stringVal = val;
		type = _type;
	}

	~CabbageChannelMessage()
	{}

};

//message queue class
class CabbageMessageQueue : public CabbageUtils
{
	Array<CabbageChannelMessage> outgoingChannelMessages;

public:
	CabbageMessageQueue(){}
	~CabbageMessageQueue(){}

	void addOutgoingChannelMessageToQueue(String _chan, double _val, String _type){
	outgoingChannelMessages.add(CabbageChannelMessage(_chan, _val, _type));
	}

	void addOutgoingChannelMessageToQueue(String _chan, String _val, String _type){
	outgoingChannelMessages.add(CabbageChannelMessage(_chan, _val, _type));
	}
	
	void addOutgoingTableUpdateMessageToQueue(String fStatement){
	CabbageChannelMessage tableMessage("", 0.f, "updateTable");
	tableMessage.fStatement = fStatement;
	outgoingChannelMessages.add(tableMessage);
	}

	CabbageChannelMessage getOutgoingChannelMessageFromQueue(int index){
		return outgoingChannelMessages.getReference(index);
	}

    int getNumberOfOutgoingChannelMessagesInQueue(){
		return outgoingChannelMessages.size();
	}

	void flushOutgoingChannelMessages(){
		outgoingChannelMessages.clear();
	}

};



#endif
