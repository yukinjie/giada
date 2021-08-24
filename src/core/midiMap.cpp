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

#include "core/midiMap.h"
#include "core/const.h"
#include "core/kernelMidi.h"
#include "core/midiEvent.h"
#include "deps/json/single_include/nlohmann/json.hpp"
#include "utils/fs.h"
#include "utils/log.h"
#include "utils/string.h"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace nl = nlohmann;

namespace giada::m::midiMap
{
namespace
{
bool readInitCommands_(Data& midiMap, const nl::json& j)
{
	if (j.find(MIDIMAP_KEY_INIT_COMMANDS) == j.end())
		return false;

	for (const auto& jc : j[MIDIMAP_KEY_INIT_COMMANDS])
	{
		Message m;
		m.channel  = jc[MIDIMAP_KEY_CHANNEL];
		m.valueStr = jc[MIDIMAP_KEY_MESSAGE];
		m.value    = strtoul(m.valueStr.c_str(), nullptr, 16);

		midiMap.midiMap.initCommands.push_back(m);
	}

	return true;
}

/* -------------------------------------------------------------------------- */

bool readCommand_(const nl::json& j, Message& m, const std::string& key)
{
	if (j.find(key) == j.end())
		return false;

	const nl::json& jc = j[key];

	m.channel  = jc[MIDIMAP_KEY_CHANNEL];
	m.valueStr = jc[MIDIMAP_KEY_MESSAGE];

	return true;
}

/* -------------------------------------------------------------------------- */

void parse_(Message& message)
{
	/* Remove '0x' part from the original string. */

	std::string input = message.valueStr;

	std::size_t f = input.find("0x"); // check if "0x" is there
	if (f != std::string::npos)
		input = message.valueStr.replace(f, 2, "");

	/* Then transform string value into the actual uint32_t value, by parsing
	each char (i.e. nibble) in the original string. Substitute 'n' with
	zeros. */

	std::string output;
	for (unsigned i = 0, p = 24; i < input.length(); i++, p -= 4)
	{
		if (input[i] == 'n')
		{
			output += '0';
			if (message.offset == -1) // do it once
				message.offset = p;
		}
		else
			output += input[i];
	}

	/* From string to uint32_t */

	message.value = strtoul(output.c_str(), nullptr, 16);

	u::log::print("[parse] parsed chan=%d valueStr=%s value=%#x, offset=%d\n",
	    message.channel, message.valueStr, message.value, message.offset);
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void init(Data& midiMap)
{
	midiMap.midimapsPath = u::fs::getHomePath() + G_SLASH + "midimaps" + G_SLASH;

	/* scan dir of midi maps and load the filenames into <>maps. */

	u::log::print("[midiMapConf::init] scanning midimaps directory '%s'...\n",
	    midiMap.midimapsPath);

	if (!std::filesystem::exists(midiMap.midimapsPath))
	{
		u::log::print("[midiMapConf::init] unable to scan midimaps directory!\n");
		return;
	}

	for (const auto& d : std::filesystem::directory_iterator(midiMap.midimapsPath))
	{
		// TODO - check if is a valid midiMap file (verify headers)
		if (!d.is_regular_file())
			continue;
		u::log::print("[midiMapConf::init] found midiMap '%s'\n", d.path().filename().string());
		midiMap.maps.push_back(d.path().filename().string());
	}

	u::log::print("[midiMapConf::init] total midimaps found: %d\n", midiMap.maps.size());
}

/* -------------------------------------------------------------------------- */

bool isDefined(const Message& m)
{
	return m.offset != -1;
}

/* -------------------------------------------------------------------------- */

int read(Data& midiMap, const std::string& file)
{
	if (file.empty())
	{
		u::log::print("[midiMapConf::read] midiMap not specified, nothing to do\n");
		return MIDIMAP_NOT_SPECIFIED;
	}

	u::log::print("[midiMapConf::read] reading midiMap file '%s'\n", file);

	std::ifstream ifs(midiMap.midimapsPath + file);
	if (!ifs.good())
		return MIDIMAP_UNREADABLE;

	nl::json j = nl::json::parse(ifs);

	midiMap.midiMap.brand  = j[MIDIMAP_KEY_BRAND];
	midiMap.midiMap.device = j[MIDIMAP_KEY_DEVICE];

	if (!readInitCommands_(midiMap, j))
		return MIDIMAP_UNREADABLE;
	if (readCommand_(j, midiMap.midiMap.muteOn, MIDIMAP_KEY_MUTE_ON))
		parse_(midiMap.midiMap.muteOn);
	if (readCommand_(j, midiMap.midiMap.muteOff, MIDIMAP_KEY_MUTE_OFF))
		parse_(midiMap.midiMap.muteOff);
	if (readCommand_(j, midiMap.midiMap.soloOn, MIDIMAP_KEY_SOLO_ON))
		parse_(midiMap.midiMap.soloOn);
	if (readCommand_(j, midiMap.midiMap.soloOff, MIDIMAP_KEY_SOLO_OFF))
		parse_(midiMap.midiMap.soloOff);
	if (readCommand_(j, midiMap.midiMap.waiting, MIDIMAP_KEY_WAITING))
		parse_(midiMap.midiMap.waiting);
	if (readCommand_(j, midiMap.midiMap.playing, MIDIMAP_KEY_PLAYING))
		parse_(midiMap.midiMap.playing);
	if (readCommand_(j, midiMap.midiMap.stopping, MIDIMAP_KEY_STOPPING))
		parse_(midiMap.midiMap.stopping);
	if (readCommand_(j, midiMap.midiMap.stopped, MIDIMAP_KEY_STOPPED))
		parse_(midiMap.midiMap.stopped);
	if (readCommand_(j, midiMap.midiMap.playingInaudible, MIDIMAP_KEY_PLAYING_INAUDIBLE))
		parse_(midiMap.midiMap.playingInaudible);

	return MIDIMAP_READ_OK;
}

/* -------------------------------------------------------------------------- */

void sendInitMessages(KernelMidi& kernelMidi, const Data& midiMap)
{
	for (const midiMap::Message& m : midiMap.midiMap.initCommands)
	{
		if (m.value == 0x0 || m.channel == -1)
			continue;
		u::log::print("[KM] MIDI send (init) - Channel %x - Event 0x%X\n", m.channel, m.value);
		MidiEvent e(m.value);
		e.setChannel(m.channel);
		kernelMidi.send(e.getRaw());
	}
}

/* -------------------------------------------------------------------------- */

void sendMidiLightning(KernelMidi& kernelMidi, uint32_t learnt, const midiMap::Message& m)
{
	// Skip lightning message if not defined in midi map

	if (!isDefined(m))
	{
		u::log::print("[midiMap::sendMidiLightning] message skipped (not defined in midiMap)");
		return;
	}

	u::log::print("[midiMap::sendMidiLightning] learnt=0x%X, chan=%d, msg=0x%X, offset=%d\n",
	    learnt, m.channel, m.value, m.offset);

	/* Isolate 'channel' from learnt message and offset it as requested by 'nn' in 
	the midiMap configuration file. */

	uint32_t out = ((learnt & 0x00FF0000) >> 16) << m.offset;

	/* Merge the previously prepared channel into final message, and finally send 
	it. */

	out |= m.value | (m.channel << 24);
	kernelMidi.send(out);
}
} // namespace giada::m::midiMap