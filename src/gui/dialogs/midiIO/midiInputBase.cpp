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

#include "midiInputBase.h"
#include "core/conf.h"
#include "glue/io.h"

extern giada::m::conf::Data g_conf;

namespace giada::v
{
gdMidiInputBase::gdMidiInputBase(int x, int y, int w, int h, const char* title)
: gdWindow(x, y, w, h, title)
{
}

/* -------------------------------------------------------------------------- */

gdMidiInputBase::~gdMidiInputBase()
{
	c::io::stopMidiLearn();

	g_conf.midiInputX = x();
	g_conf.midiInputY = y();
	g_conf.midiInputW = w();
	g_conf.midiInputH = h();
}

/* -------------------------------------------------------------------------- */

void gdMidiInputBase::cb_close(Fl_Widget* /*w*/, void* p) { ((gdMidiInputBase*)p)->cb_close(); }

/* -------------------------------------------------------------------------- */

void gdMidiInputBase::cb_close()
{
	do_callback();
}
} // namespace giada::v