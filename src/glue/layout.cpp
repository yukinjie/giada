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

#include "glue/layout.h"
#include "core/conf.h"
#include "core/patch.h"
#include "core/sequencer.h"
#include "glue/storage.h"
#include "gui/dialogs/about.h"
#include "gui/dialogs/actionEditor/midiActionEditor.h"
#include "gui/dialogs/actionEditor/sampleActionEditor.h"
#include "gui/dialogs/browser/browserDir.h"
#include "gui/dialogs/browser/browserLoad.h"
#include "gui/dialogs/browser/browserSave.h"
#include "gui/dialogs/channelNameInput.h"
#include "gui/dialogs/config.h"
#include "gui/dialogs/keyGrabber.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/midiIO/midiInputChannel.h"
#include "gui/dialogs/midiIO/midiInputMaster.h"
#include "gui/dialogs/midiIO/midiOutputMidiCh.h"
#include "gui/dialogs/midiIO/midiOutputSampleCh.h"
#include "gui/dialogs/pluginList.h"
#include "gui/dialogs/sampleEditor.h"

extern giada::v::gdMainWindow* G_MainWin;
extern giada::m::conf::Data    g_conf;
extern giada::m::patch::Data   g_patch;
extern giada::m::Sequencer     g_sequencer;

namespace giada::c::layout
{
void openBrowserForProjectLoad()
{
	v::gdWindow* childWin = new v::gdBrowserLoad("Open project", g_conf.patchPath,
	    c::storage::loadProject, 0, g_conf);
	u::gui::openSubWindow(G_MainWin, childWin, WID_FILE_BROWSER);
}

/* -------------------------------------------------------------------------- */

void openBrowserForProjectSave()
{
	v::gdWindow* childWin = new v::gdBrowserSave("Save project", g_conf.patchPath,
	    g_patch.name, c::storage::saveProject, 0, g_conf);
	u::gui::openSubWindow(G_MainWin, childWin, WID_FILE_BROWSER);
}

/* -------------------------------------------------------------------------- */

void openBrowserForSampleLoad(ID channelId)
{
	v::gdWindow* w = new v::gdBrowserLoad("Browse sample",
	    g_conf.samplePath.c_str(), c::storage::loadSample, channelId, g_conf);
	u::gui::openSubWindow(G_MainWin, w, WID_FILE_BROWSER);
}

/* -------------------------------------------------------------------------- */

void openBrowserForSampleSave(ID channelId)
{
	v::gdWindow* w = new v::gdBrowserSave("Save sample",
	    g_conf.samplePath.c_str(), "", c::storage::saveSample, channelId, g_conf);
	u::gui::openSubWindow(G_MainWin, w, WID_FILE_BROWSER);
}

/* -------------------------------------------------------------------------- */

void openBrowserForPlugins(v::gdWindow& parent)
{
	v::gdBrowserDir* browser = new v::gdBrowserDir("Add plug-ins directory",
	    g_conf.patchPath, c::plugin::setPluginPathCb, g_conf);
	parent.addSubWindow(browser);
}

/* -------------------------------------------------------------------------- */

void openAboutWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdAbout(), WID_ABOUT);
}

/* -------------------------------------------------------------------------- */

void openKeyGrabberWindow(const c::channel::Data& data)
{
	u::gui::openSubWindow(G_MainWin, new v::gdKeyGrabber(data), WID_KEY_GRABBER);
}

/* -------------------------------------------------------------------------- */

void openConfigWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdConfig(400, 370, g_conf), WID_CONFIG);
}

/* -------------------------------------------------------------------------- */

void openMasterMidiInputWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdMidiInputMaster(g_conf), WID_MIDI_INPUT);
}

/* -------------------------------------------------------------------------- */

void openChannelMidiInputWindow(ID channelId)
{
	u::gui::openSubWindow(G_MainWin, new v::gdMidiInputChannel(channelId, g_conf),
	    WID_MIDI_INPUT);
}

/* -------------------------------------------------------------------------- */

void openSampleChannelMidiOutputWindow(ID channelId)
{
	u::gui::openSubWindow(G_MainWin, new v::gdMidiOutputSampleCh(channelId),
	    WID_MIDI_OUTPUT);
}

/* -------------------------------------------------------------------------- */

void openMidiChannelMidiOutputWindow(ID channelId)
{
	u::gui::openSubWindow(G_MainWin, new v::gdMidiOutputMidiCh(channelId), WID_MIDI_OUTPUT);
}

/* -------------------------------------------------------------------------- */

void openSampleActionEditor(ID channelId)
{
	u::gui::openSubWindow(G_MainWin,
	    new v::gdSampleActionEditor(channelId, g_conf, g_sequencer.getFramesInBeat()),
	    WID_ACTION_EDITOR);
}

/* -------------------------------------------------------------------------- */

void openMidiActionEditor(ID channelId)
{
	u::gui::openSubWindow(G_MainWin,
	    new v::gdMidiActionEditor(channelId, g_conf, g_sequencer.getFramesInBeat()),
	    WID_ACTION_EDITOR);
}

/* -------------------------------------------------------------------------- */

void openSampleEditor(ID channelId)
{
	u::gui::openSubWindow(G_MainWin, new v::gdSampleEditor(channelId, g_conf),
	    WID_SAMPLE_EDITOR);
}

/* -------------------------------------------------------------------------- */

void openRenameChannelWindow(const c::channel::Data& data)
{
	u::gui::openSubWindow(G_MainWin, new v::gdChannelNameInput(data),
	    WID_SAMPLE_NAME);
}

/* -------------------------------------------------------------------------- */

void openChannelPluginListWindow(ID channelId)
{
	u::gui::openSubWindow(G_MainWin, new v::gdPluginList(channelId, g_conf),
	    WID_FX_LIST);
}

/* -------------------------------------------------------------------------- */

void openMasterInPluginListWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdPluginList(m::Mixer::MASTER_IN_CHANNEL_ID, g_conf),
	    WID_FX_LIST);
}

/* -------------------------------------------------------------------------- */

void openMasterOutPluginListWindow()
{
	u::gui::openSubWindow(G_MainWin, new v::gdPluginList(m::Mixer::MASTER_OUT_CHANNEL_ID, g_conf),
	    WID_FX_LIST);
}

} // namespace giada::c::layout
