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

#include "config.h"
#include "core/conf.h"
#include "core/const.h"
#include "gui/elems/basics/boxtypes.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/config/tabAudio.h"
#include "gui/elems/config/tabBehaviors.h"
#include "gui/elems/config/tabMidi.h"
#include "gui/elems/config/tabMisc.h"
#include "gui/elems/config/tabPlugins.h"
#include "utils/gui.h"
#include <FL/Fl_Tabs.H>

namespace giada::v
{
gdConfig::gdConfig(int w, int h, m::conf::Data& conf)
: gdWindow(u::gui::centerWindowX(w), u::gui::centerWindowY(h), w, h, "Configuration")
{
	Fl_Tabs* tabs = new Fl_Tabs(8, 8, w - 16, h - 44);
	tabs->box(G_CUSTOM_BORDER_BOX);
	tabs->labelcolor(G_COLOR_LIGHT_2);
	tabs->begin();

	tabAudio     = new geTabAudio(tabs->x() + 10, tabs->y() + 20, tabs->w() - 20, tabs->h() - 40);
	tabMidi      = new geTabMidi(tabs->x() + 10, tabs->y() + 20, tabs->w() - 20, tabs->h() - 40);
	tabBehaviors = new geTabBehaviors(tabs->x() + 10, tabs->y() + 20, tabs->w() - 20, tabs->h() - 40, conf);
	tabMisc      = new geTabMisc(tabs->x() + 10, tabs->y() + 20, tabs->w() - 20);
#ifdef WITH_VST
	tabPlugins = new geTabPlugins(tabs->x() + 10, tabs->y() + 20, tabs->w() - 20, tabs->h() - 40);
#endif

	tabs->end();

	save   = new geButton(w - 88, h - 28, 80, 20, "Save");
	cancel = new geButton(w - 176, h - 28, 80, 20, "Cancel");

	end();

	save->callback(cb_save_config, (void*)this);
	cancel->callback(cb_cancel, (void*)this);

	u::gui::setFavicon(this);
	setId(WID_CONFIG);
	show();
}

/* -------------------------------------------------------------------------- */

void gdConfig::cb_save_config(Fl_Widget* /*w*/, void* p) { ((gdConfig*)p)->cb_save_config(); }
void gdConfig::cb_cancel(Fl_Widget* /*w*/, void* p) { ((gdConfig*)p)->cb_cancel(); }

/* -------------------------------------------------------------------------- */

void gdConfig::cb_save_config()
{
	tabAudio->save();
	tabBehaviors->save();
	tabMidi->save();
	tabMisc->save();
#ifdef WITH_VST
	tabPlugins->save();
#endif
	do_callback();
}

/* -------------------------------------------------------------------------- */

void gdConfig::cb_cancel()
{
	do_callback();
}
} // namespace giada::v
