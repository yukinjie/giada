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

#ifdef WITH_VST

#include "gui/elems/plugin/pluginBrowser.h"
#include "core/const.h"
#include "core/plugins/pluginManager.h"
#include "glue/plugin.h"
#include "gui/elems/basics/boxtypes.h"
#include <FL/fl_draw.H>

namespace giada::v
{
gePluginBrowser::gePluginBrowser(int x, int y, int w, int h)
: Fl_Browser(x, y, w, h)
, m_widths{0}
{
	box(G_CUSTOM_BORDER_BOX);
	textsize(G_GUI_FONT_SIZE_BASE);
	textcolor(G_COLOR_LIGHT_2);
	selection_color(G_COLOR_GREY_4);
	color(G_COLOR_GREY_2);

	this->scrollbar.color(G_COLOR_GREY_2);
	this->scrollbar.selection_color(G_COLOR_GREY_4);
	this->scrollbar.labelcolor(G_COLOR_LIGHT_1);
	this->scrollbar.slider(G_CUSTOM_BORDER_BOX);

	this->hscrollbar.color(G_COLOR_GREY_2);
	this->hscrollbar.selection_color(G_COLOR_GREY_4);
	this->hscrollbar.labelcolor(G_COLOR_LIGHT_1);
	this->hscrollbar.slider(G_CUSTOM_BORDER_BOX);

	type(FL_HOLD_BROWSER);

	computeWidths();

	column_widths(m_widths);
	column_char('\t'); // tabs as column delimiters

	refresh();

	end();
}

/* -------------------------------------------------------------------------- */

void gePluginBrowser::refresh()
{
	clear();

	add("NAME\tMANUFACTURER\tCATEGORY\tFORMAT\tUID");
	add("---\t---\t---\t---\t---");

	for (m::PluginManager::PluginInfo pi : c::plugin::getPluginsInfo())
	{
		std::string s;
		if (pi.isKnown)
		{
			std::string m = pi.exists ? "" : "@-";

			s = m + pi.name + "\t" + m + pi.manufacturerName + "\t" + m +
			    pi.category + "\t" + m + pi.format + "\t" + m + pi.uid;
		}
		else
			std::string s = "?\t?\t?\t?\t? " + pi.uid + " ?";

		add(s.c_str());
	}
}

/* -------------------------------------------------------------------------- */

void gePluginBrowser::computeWidths()
{
	int w0, w1, w3;
	for (m::PluginManager::PluginInfo pi : c::plugin::getPluginsInfo())
	{
		w0 = static_cast<int>(fl_width(pi.name.c_str()));
		w1 = static_cast<int>(fl_width(pi.manufacturerName.c_str()));
		w3 = static_cast<int>(fl_width(pi.format.c_str()));
		if (w0 > m_widths[0])
			m_widths[0] = w0;
		if (w1 > m_widths[1])
			m_widths[1] = w1;
		if (w3 > m_widths[3])
			m_widths[3] = w3;
	}
	m_widths[0] += 60;
	m_widths[1] += 60;
	m_widths[2] = static_cast<int>(fl_width("CATEGORY") + 60);
	m_widths[3] += 60;
	m_widths[4] = 0;
}
} // namespace giada::v

#endif
