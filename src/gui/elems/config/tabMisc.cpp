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

#include "tabMisc.h"
#include "core/const.h"

namespace giada::v
{
geTabMisc::geTabMisc(int X, int Y, int W)
: geGroup(X, Y)
, m_data(c::config::getMiscData())
, m_debugMsg(W - 230, 9, 230, 20, "Debug messages")
, m_tooltips(W - 230, 37, 230, 20, "Tooltips")
{
	add(&m_debugMsg);
	add(&m_tooltips);

	m_debugMsg.add("Disabled");
	m_debugMsg.add("To standard output");
	m_debugMsg.add("To file");

	m_tooltips.add("Disabled");
	m_tooltips.add("Enabled");

	m_debugMsg.value(m_data.logMode);
	m_tooltips.value(m_data.showTooltips);

	copy_label("Misc");
	labelsize(G_GUI_FONT_SIZE_BASE);
	selection_color(G_COLOR_GREY_4);
}

/* -------------------------------------------------------------------------- */

void geTabMisc::save()
{
	c::config::save(m_data);
}
} // namespace giada::v