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

#ifndef GD_CONFIG_H
#define GD_CONFIG_H

#include "window.h"

class geButton;
class geCheck;
class geInput;
class geBox;

namespace giada::m::conf
{
struct Data;
}

namespace giada::v
{
class geChoice;
class geTabAudio;
class geTabBehaviors;
class geTabMidi;
class geTabMisc;
#ifdef WITH_VST
class geTabPlugins;
#endif
class gdConfig : public gdWindow
{
public:
	gdConfig(int w, int h, m::conf::Data&);

	geTabAudio*     tabAudio;
	geTabBehaviors* tabBehaviors;
	geTabMidi*      tabMidi;
	geTabMisc*      tabMisc;
#ifdef WITH_VST
	geTabPlugins* tabPlugins;
#endif
	geButton* save;
	geButton* cancel;

private:
	static void cb_save_config(Fl_Widget* /*w*/, void* p);
	static void cb_cancel(Fl_Widget* /*w*/, void* p);
	void        cb_save_config();
	void        cb_cancel();
};
} // namespace giada::v

#endif
