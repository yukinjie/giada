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

#ifndef GD_MIDI_INPUT_MASTER_H
#define GD_MIDI_INPUT_MASTER_H

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
class geMasterLearnerPack : public geMidiLearnerPack
{
public:
	geMasterLearnerPack(int x, int y);

	void update(const c::io::Master_InputData&);
};

/* -------------------------------------------------------------------------- */

class gdMidiInputMaster : public gdMidiInputBase
{
public:
	gdMidiInputMaster(m::conf::Data&);

	void rebuild() override;

private:
	static void cb_enable(Fl_Widget* /*w*/, void* p);
	static void cb_setChannel(Fl_Widget* /*w*/, void* p);
	void        cb_enable();
	void        cb_setChannel();

	c::io::Master_InputData m_data;

	geMasterLearnerPack* m_learners;
};
} // namespace giada::v

#endif
