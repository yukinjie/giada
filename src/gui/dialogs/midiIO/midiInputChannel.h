/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * midiInputChannel
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

#ifndef GD_MIDI_INPUT_CHANNEL_H
#define GD_MIDI_INPUT_CHANNEL_H

#include "glue/io.h"
#include "gui/elems/midiIO/midiLearnerPack.h"
#include "midiInputBase.h"

class geCheck;

namespace giada::m::conf
{
struct Data;
}

namespace giada::v
{
class geChoice;
class geScrollPack;
class geChannelLearnerPack : public geMidiLearnerPack
{
public:
	geChannelLearnerPack(int x, int y, const c::io::Channel_InputData& d);

	void update(const c::io::Channel_InputData&);
};

/* -------------------------------------------------------------------------- */

#ifdef WITH_VST

class gePluginLearnerPack : public geMidiLearnerPack
{
public:
	gePluginLearnerPack(int x, int y, const c::io::PluginData&);

	void update(const c::io::PluginData&, bool enabled);
};

#endif

/* -------------------------------------------------------------------------- */

class gdMidiInputChannel : public gdMidiInputBase
{
public:
	gdMidiInputChannel(ID channelId, m::conf::Data&);

	void rebuild() override;

private:
	static void cb_enable(Fl_Widget* /*w*/, void* p);
	static void cb_setChannel(Fl_Widget* /*w*/, void* p);
	static void cb_veloAsVol(Fl_Widget* /*w*/, void* p);
	void        cb_enable();
	void        cb_setChannel();
	void        cb_veloAsVol();

	ID m_channelId;

	c::io::Channel_InputData m_data;

	geScrollPack* m_container;
	geCheck*      m_veloAsVol;
};
} // namespace giada::v

#endif
