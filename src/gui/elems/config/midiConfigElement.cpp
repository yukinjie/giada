/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2020 Giovanni A. Zuliani | Monocasual
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


#include <cassert>
#include <string>
#include "core/graphics.h"
#include "core/plugins/pluginHost.h"
#include "core/plugins/plugin.h"
#include "utils/gui.h"
#include "utils/log.h"
#include "glue/plugin.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/choice.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/pluginList.h"
#include "gui/dialogs/pluginWindowGUI.h"
#include "gui/dialogs/pluginWindow.h"
#include "gui/elems/plugin/pluginElement.h"
#include "gui/elems/config/midiConfigElement.h"


namespace giada {
namespace v
{
geMidiConfigElement::geMidiConfigElement(int x, int y)
: gePack(x, y, Direction::HORIZONTAL) 
, enable(0, 0, G_GUI_UNIT, G_GUI_UNIT)
, name  (0, 0, 132, G_GUI_UNIT)
, edit  (0, 0, 100, G_GUI_UNIT, "Edit")
{
	add(&enable);
	add(&name);
	add(&edit);

	resizable(name);
}
}} // giada::v::
