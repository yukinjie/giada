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

#include "beatsInput.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/mixer.h"
#include "core/sequencer.h"
#include "glue/main.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/check.h"
#include "gui/elems/basics/input.h"
#include "mainWindow.h"
#include "utils/gui.h"
#include "utils/string.h"
#include <cstring>

extern giada::v::gdMainWindow* mainWin;
extern giada::m::Sequencer     g_sequencer;

namespace giada
{
namespace v
{
gdBeatsInput::gdBeatsInput()
: gdWindow(u::gui::centerWindowX(180), u::gui::centerWindowY(36), 180, 36, "Beats")
{
	set_modal();

	beats = new geInput(8, 8, 43, G_GUI_UNIT);
	bars  = new geInput(beats->x() + beats->w() + 4, 8, 43, G_GUI_UNIT);
	ok    = new geButton(bars->x() + bars->w() + 4, 8, 70, G_GUI_UNIT, "Ok");
	end();

	beats->maximum_size(2);
	beats->value(std::to_string(g_sequencer.getBeats()).c_str());
	beats->type(FL_INT_INPUT);

	bars->maximum_size(2);
	bars->value(std::to_string(g_sequencer.getBars()).c_str());
	bars->type(FL_INT_INPUT);

	ok->shortcut(FL_Enter);
	ok->callback(cb_update, (void*)this);

	u::gui::setFavicon(this);
	setId(WID_BEATS);
	show();
}

/* -------------------------------------------------------------------------- */

void gdBeatsInput::cb_update(Fl_Widget* /*w*/, void* p) { ((gdBeatsInput*)p)->cb_update(); }

/* -------------------------------------------------------------------------- */

void gdBeatsInput::cb_update()
{
	if (!strcmp(beats->value(), "") || !strcmp(bars->value(), ""))
		return;
	c::main::setBeats(atoi(beats->value()), atoi(bars->value()));
	do_callback();
}

} // namespace v
} // namespace giada