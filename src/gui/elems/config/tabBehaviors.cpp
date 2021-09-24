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

#include "tabBehaviors.h"
#include "core/conf.h"
#include "core/const.h"
#include "gui/elems/basics/box.h"
#include "gui/elems/basics/check.h"
#include <FL/Fl_Pack.H>

extern giada::m::conf::Data g_conf;

namespace giada::v
{
geTabBehaviors::geTabBehaviors(int X, int Y, int W, int H)
: Fl_Group(X, Y, W, H)
, m_container(X, Y + G_GUI_OUTER_MARGIN, Direction::VERTICAL, G_GUI_OUTER_MARGIN)
, m_chansStopOnSeqHalt(0, 0, 280, 30, "Dynamic channels stop immediately when the sequencer\nis halted")
, m_treatRecsAsLoops(0, 0, 280, 20, "Treat one shot channels with actions as loops")
, m_inputMonitorDefaultOn(0, 0, 280, 20, "New sample channels have input monitor on by default")
, m_overdubProtectionDefaultOn(0, 0, 280, 30, "New sample channels have overdub protection on\nby default")
{
	end();

	label("Behaviors");
	labelsize(G_GUI_FONT_SIZE_BASE);
	selection_color(G_COLOR_GREY_4);

	m_container.add(&m_chansStopOnSeqHalt);
	m_container.add(&m_treatRecsAsLoops);
	m_container.add(&m_inputMonitorDefaultOn);
	m_container.add(&m_overdubProtectionDefaultOn);

	add(m_container);

	m_chansStopOnSeqHalt.value(g_conf.chansStopOnSeqHalt);
	m_treatRecsAsLoops.value(g_conf.treatRecsAsLoops);
	m_inputMonitorDefaultOn.value(g_conf.inputMonitorDefaultOn);
	m_overdubProtectionDefaultOn.value(g_conf.overdubProtectionDefaultOn);
}

/* -------------------------------------------------------------------------- */

void geTabBehaviors::save()
{
	g_conf.chansStopOnSeqHalt         = m_chansStopOnSeqHalt.value();
	g_conf.treatRecsAsLoops           = m_treatRecsAsLoops.value();
	g_conf.inputMonitorDefaultOn      = m_inputMonitorDefaultOn.value();
	g_conf.overdubProtectionDefaultOn = m_overdubProtectionDefaultOn.value();
}
} // namespace giada::v