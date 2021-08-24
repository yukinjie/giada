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

#include <atomic>
#include <ctime>
#include <thread>
#ifdef __APPLE__
#include <pwd.h>
#endif
#if (defined(__linux__) || defined(__FreeBSD__)) && defined(WITH_VST)
#include <X11/Xlib.h> // For XInitThreads
#endif
#include "core/actions/actionRecorder.h"
#include "core/actions/actions.h"
#include "core/channels/channelManager.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/eventDispatcher.h"
#include "core/init.h"
#include "core/kernelAudio.h"
#include "core/kernelMidi.h"
#include "core/midiMap.h"
#include "core/mixerHandler.h"
#include "core/model/model.h"
#include "core/model/storage.h"
#include "core/patch.h"
#include "core/plugins/pluginHost.h"
#include "core/plugins/pluginManager.h"
#include "core/recorder.h"
#include "core/sequencer.h"
#include "core/synchronizer.h"
#include "core/wave.h"
#include "core/waveManager.h"
#include "deps/json/single_include/nlohmann/json.hpp"
#include "glue/main.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/warnings.h"
#include "gui/updater.h"
#include "utils/fs.h"
#include "utils/gui.h"
#include "utils/log.h"
#include "utils/time.h"
#include "utils/ver.h"
#include <FL/Fl.H>

extern giada::v::gdMainWindow*  G_MainWin;
extern giada::m::model::Model   g_model;
extern giada::m::KernelAudio    g_kernelAudio;
extern giada::m::Sequencer      g_sequencer;
extern giada::m::ActionRecorder g_actionRecorder;
extern giada::m::MixerHandler   g_mixerHandler;
extern giada::m::Synchronizer   g_synchronizer;
extern giada::m::PluginHost     g_pluginHost;
extern giada::m::PluginManager  g_pluginManager;
extern giada::m::ChannelManager g_channelManager;
extern giada::m::WaveManager    g_waveManager;
extern giada::m::KernelMidi     g_kernelMidi;
extern giada::m::conf::Data     g_conf;
extern giada::m::patch::Data    g_patch;
extern giada::m::midiMap::Data  g_midiMap;

namespace giada::m::init
{
namespace
{
void initConf_()
{
	if (!conf::read(g_conf))
		u::log::print("[init] Can't read configuration file! Using default values\n");

	model::load(g_conf);

	if (!u::log::init(g_conf.logMode))
		u::log::print("[init] log init failed! Using default stdout\n");

	midiMap::init(g_midiMap);
	if (midiMap::read(g_midiMap, g_conf.midiMapPath) != MIDIMAP_READ_OK)
		u::log::print("[init] MIDI map read failed!\n");
}

/* -------------------------------------------------------------------------- */

void initAudio_()
{
	g_kernelAudio.openDevice(g_conf);
	if (!g_kernelAudio.isReady())
		return;
	g_mixerHandler.startRendering();
	g_kernelAudio.startStream();
}

/* -------------------------------------------------------------------------- */

void initMIDI_()
{
	g_kernelMidi.setApi(g_conf.midiSystem);
	g_kernelMidi.openOutDevice(g_conf.midiPortOut);
	g_kernelMidi.openInDevice(g_conf.midiPortIn);
	midiMap::sendInitMessages(g_kernelMidi, g_midiMap);
}

/* -------------------------------------------------------------------------- */

void initGUI_(int argc, char** argv)
{
	/* This is of paramount importance on Linux with VST enabled, otherwise many
	plug-ins go nuts and crash hard. It seems that some plug-ins or our Juce-based
	PluginHost use Xlib concurrently. */

#if (defined(__linux__) || defined(__FreeBSD__)) && defined(WITH_VST)
	XInitThreads();
#endif

	G_MainWin = new v::gdMainWindow(G_MIN_GUI_WIDTH, G_MIN_GUI_HEIGHT, "", argc, argv);
	G_MainWin->resize(g_conf.mainWindowX, g_conf.mainWindowY, g_conf.mainWindowW,
	    g_conf.mainWindowH);

	u::gui::updateMainWinLabel(g_patch.name == "" ? G_DEFAULT_PATCH_NAME : g_patch.name);

	if (!g_kernelAudio.isReady())
		v::gdAlert("Your soundcard isn't configured correctly.\n"
		           "Check the configuration and restart Giada.");

	v::updater::init();
	u::gui::updateStaticWidgets();
}

/* -------------------------------------------------------------------------- */

void shutdownAudio_()
{
	if (g_kernelAudio.isReady())
	{
		g_kernelAudio.closeDevice();
		u::log::print("[init] KernelAudio closed\n");
		g_mixerHandler.stopRendering();
		u::log::print("[init] Mixer closed\n");
	}
}

/* -------------------------------------------------------------------------- */

void shutdownGUI_()
{
	u::gui::closeAllSubwindows();

	u::log::print("[init] All subwindows and UI thread closed\n");
}

/* -------------------------------------------------------------------------- */

void printBuildInfo_()
{
	u::log::print("[init] Giada %s\n", G_VERSION_STR);
	u::log::print("[init] Build date: " BUILD_DATE "\n");
#ifdef G_DEBUG_MODE
	u::log::print("[init] Debug build\n");
#else
	u::log::print("[init] Release build\n");
#endif
	u::log::print("[init] Dependencies:\n");
	u::log::print("[init]   FLTK - %d.%d.%d\n", FL_MAJOR_VERSION, FL_MINOR_VERSION, FL_PATCH_VERSION);
	u::log::print("[init]   RtAudio - %s\n", u::ver::getRtAudioVersion());
	u::log::print("[init]   RtMidi - %s\n", u::ver::getRtMidiVersion());
	u::log::print("[init]   Libsamplerate\n"); // TODO - print version
	u::log::print("[init]   Libsndfile - %s\n", u::ver::getLibsndfileVersion());
	u::log::print("[init]   JSON for modern C++ - %d.%d.%d\n",
	    NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
#ifdef WITH_VST
	u::log::print("[init]   JUCE - %d.%d.%d\n", JUCE_MAJOR_VERSION, JUCE_MINOR_VERSION, JUCE_BUILDNUMBER);
#endif
	g_kernelAudio.logCompiledAPIs();
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void startup(int argc, char** argv)
{
	printBuildInfo_();

	initConf_();
	initAudio_();
	initMIDI_();
	initGUI_(argc, argv);
}

/* -------------------------------------------------------------------------- */

void closeMainWindow()
{
	if (!v::gdConfirmWin("Warning", "Quit Giada: are you sure?"))
		return;

	v::updater::close();
	G_MainWin->hide();
	delete G_MainWin;
}

/* -------------------------------------------------------------------------- */

void reset()
{
	u::gui::closeAllSubwindows();
	G_MainWin->clearKeyboard();

	g_mixerHandler.stopRendering();

	g_model.reset();
	g_channelManager.reset();
	g_waveManager.reset();
	g_synchronizer.reset();
	g_sequencer.reset(g_conf.samplerate);
	g_mixerHandler.reset(g_sequencer.getMaxFramesInLoop(g_conf.samplerate),
	    g_kernelAudio.getRealBufSize(), g_channelManager);
	g_actionRecorder.reset();
#ifdef WITH_VST
	g_pluginHost.reset(g_kernelAudio.getRealBufSize());
	g_pluginManager.reset(static_cast<PluginManager::SortMethod>(g_conf.pluginSortMethod));
#endif
	g_mixerHandler.startRendering();

	u::gui::updateMainWinLabel(G_DEFAULT_PATCH_NAME);
	u::gui::updateStaticWidgets();
}

/* -------------------------------------------------------------------------- */

void shutdown()
{
	shutdownGUI_();

	model::store(g_conf);

	if (!conf::write(g_conf))
		u::log::print("[init] error while saving configuration file!\n");
	else
		u::log::print("[init] configuration saved\n");

	shutdownAudio_();

	u::log::print("[init] Giada %s closed\n\n", G_VERSION_STR);
	u::log::close();
}
} // namespace giada::m::init
