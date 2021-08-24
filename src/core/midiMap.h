/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2021 Giovanni A. Zuliani | Monocasual
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */

#ifndef G_MIDIMAP_H
#define G_MIDIMAP_H

#include <string>
#include <vector>

namespace giada::m
{
class KernelMidi;
}

namespace giada::m::midiMap
{
struct Message
{
	int         channel  = 0;
	std::string valueStr = "";
	int         offset   = -1;
	uint32_t    value    = 0;
};

struct MidiMap
{
	std::string          brand;
	std::string          device;
	std::vector<Message> initCommands;
	Message              muteOn;
	Message              muteOff;
	Message              soloOn;
	Message              soloOff;
	Message              waiting;
	Message              playing;
	Message              stopping;
	Message              stopped;
	Message              playingInaudible;
};

struct Data
{
	/* midimap
	The actual MidiMap struct with data. */

	MidiMap midiMap;

	/* midimapsPath
	Path to folder containing midimap files, different between OSes. */

	std::string midimapsPath;

	/* maps
	Maps are the available .giadamap files. Each element of the std::vector 
	represents a .giadamap file found in the midimap folder. */

	std::vector<std::string> maps;
};

/* -------------------------------------------------------------------------- */

/* init
Parses the MIDI maps folders and find the available maps. */

void init(Data& midiMap);

/* isDefined
Checks whether a specific message has been defined within MIDI map file. */

bool isDefined(const Message& msg);

/* read
Reads a MIDI map from file 'file'. */

int read(Data& midiMap, const std::string& file);

/* sendInitMessages
Sends initialization messages to the connected MIDI devices. */

void sendInitMessages(KernelMidi&, const Data& midiMap);

/* sendMidiLightning
Sends a MIDI lightning message defined by 'msg'. */

void sendMidiLightning(KernelMidi&, uint32_t learnt, const midiMap::Message& msg);
} // namespace giada::m::midiMap

#endif
