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

#ifndef G_GLUE_LAYOUT_H
#define G_GLUE_LAYOUT_H

#include "core/types.h"

namespace giada::v
{
class gdWindow;
}

namespace giada::c::channel
{
struct Data;
}

namespace giada::c::layout
{
void openBrowserForProjectLoad();
void openBrowserForProjectSave();
void openBrowserForSampleLoad(ID channelId);
void openBrowserForSampleSave(ID channelId);
void openBrowserForPlugins(v::gdWindow& parent);
void openAboutWindow();
void openKeyGrabberWindow(const c::channel::Data&);
void openConfigWindow();
void openMasterMidiInputWindow();
void openChannelMidiInputWindow(ID channelId);
void openSampleChannelMidiOutputWindow(ID channelId);
void openMidiChannelMidiOutputWindow(ID channelId);
void openSampleActionEditor(ID channelId);
void openMidiActionEditor(ID channelId);
void openSampleEditor(ID channelId);
void openRenameChannelWindow(const c::channel::Data&);
void openChannelPluginListWindow(ID channelId);
void openMasterInPluginListWindow();
void openMasterOutPluginListWindow();
} // namespace giada::c::layout

#endif
