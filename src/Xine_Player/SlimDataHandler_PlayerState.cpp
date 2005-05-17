/*
	SlimDataHandler::PlayerState

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "SlimDataHandler.h"
#include "PlutoUtils/StringUtils.h"

string SlimDataHandler::PlayerState::describeState(PlayerState::STATE state)
{
	switch ( state )
	{
		case PLAYER_DISCONNECTED: return "Disconnected";
		case PLAYER_CONNECTED: return "Connected";
		case PLAYER_PLAYING: return "Playing";
		case PLAYER_BUFFERING: return "Buffering";
		case PLAYER_PAUSED: return "Paused";
	}

	return StringUtils::Format("!!!!!!!!! --------- Unknown state %d -------------", state);
}
