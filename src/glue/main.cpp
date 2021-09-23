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

#include "main.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/init.h"
#include "core/kernelAudio.h"
#include "core/kernelMidi.h"
#include "core/mixer.h"
#include "core/mixerHandler.h"
#include "core/model/model.h"
#include "core/plugins/pluginHost.h"
#include "core/plugins/pluginManager.h"
#include "core/recorder.h"
#include "core/sequencer.h"
#include "core/synchronizer.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dialogs/warnings.h"
#include "gui/elems/mainWindow/keyboard/keyboard.h"
#include "gui/elems/mainWindow/keyboard/sampleChannel.h"
#include "gui/elems/mainWindow/mainIO.h"
#include "gui/elems/mainWindow/mainTimer.h"
#include "src/core/actions/actionRecorder.h"
#include "src/core/actions/actions.h"
#include "utils/gui.h"
#include "utils/log.h"
#include "utils/string.h"
#include <FL/Fl.H>
#include <cassert>
#include <cmath>

extern giada::v::gdMainWindow*  G_MainWin;
extern giada::m::model::Model   g_model;
extern giada::m::KernelAudio    g_kernelAudio;
extern giada::m::Mixer          g_mixer;
extern giada::m::MixerHandler   g_mixerHandler;
extern giada::m::ActionRecorder g_actionRecorder;
extern giada::m::Recorder       g_recorder;
extern giada::m::Sequencer      g_sequencer;
extern giada::m::Synchronizer   g_synchronizer;
extern giada::m::conf::Data     g_conf;

namespace giada::c::main
{
Timer::Timer(const m::model::Sequencer& c)
: bpm(c.bpm)
, beats(c.beats)
, bars(c.bars)
, quantize(c.quantize)
, isUsingJack(g_kernelAudio.getAPI() == G_SYS_API_JACK)
, isRecordingInput(g_recorder.isRecordingInput())
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

IO::IO(const m::channel::Data& out, const m::channel::Data& in, const m::model::Mixer& m)
: masterOutVol(out.volume)
, masterInVol(in.volume)
#ifdef WITH_VST
, masterOutHasPlugins(out.plugins.size() > 0)
, masterInHasPlugins(in.plugins.size() > 0)
#endif
, inToOut(m.inToOut)
{
}

/* -------------------------------------------------------------------------- */

Peak IO::getMasterOutPeak()
{
	return g_mixer.getPeakOut();
}

Peak IO::getMasterInPeak()
{
	return g_mixer.getPeakIn();
}

/* -------------------------------------------------------------------------- */

bool IO::isKernelReady()
{
	return g_kernelAudio.isReady();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

Timer getTimer()
{
	return Timer(g_model.get().sequencer);
}

/* -------------------------------------------------------------------------- */

IO getIO()
{
	return IO(g_model.get().getChannel(m::Mixer::MASTER_OUT_CHANNEL_ID),
	    g_model.get().getChannel(m::Mixer::MASTER_IN_CHANNEL_ID),
	    g_model.get().mixer);
}

/* -------------------------------------------------------------------------- */

Sequencer getSequencer()
{
	Sequencer out;

	m::Mixer::RecordInfo recInfo = g_mixer.getRecordInfo();

	out.isFreeModeInputRec = g_recorder.isRecordingInput() && g_conf.inputRecMode == InputRecMode::FREE;
	out.shouldBlink        = u::gui::shouldBlink() && (g_sequencer.getStatus() == SeqStatus::WAITING || out.isFreeModeInputRec);
	out.beats              = g_sequencer.getBeats();
	out.bars               = g_sequencer.getBars();
	out.currentBeat        = g_sequencer.getCurrentBeat();
	out.recPosition        = recInfo.position;
	out.recMaxLength       = recInfo.maxLength;

	return out;
}

/* -------------------------------------------------------------------------- */

Transport getTransport()
{
	Transport transport;
	transport.isRunning         = g_sequencer.isRunning();
	transport.isRecordingAction = g_recorder.isRecordingAction();
	transport.isRecordingInput  = g_recorder.isRecordingInput();
	transport.isMetronomeOn     = g_sequencer.isMetronomeOn();
	transport.recTriggerMode    = g_conf.recTriggerMode;
	transport.inputRecMode      = g_conf.inputRecMode;
	return transport;
}

/* -------------------------------------------------------------------------- */

MainMenu getMainMenu()
{
	MainMenu mainMenu;
	mainMenu.hasAudioData = g_mixerHandler.hasAudioData();
	mainMenu.hasActions   = g_mixerHandler.hasActions();
	return mainMenu;
}

/* -------------------------------------------------------------------------- */

void setBpm(const char* i, const char* f)
{
	/* Never change this stuff while recording audio. */

	if (g_recorder.isRecordingInput())
		return;

	g_sequencer.setBpm(std::atof(i) + (std::atof(f) / 10.0f), g_conf.samplerate);
}

/* -------------------------------------------------------------------------- */

void setBpm(float f)
{
	/* Never change this stuff while recording audio. */

	if (g_recorder.isRecordingInput())
		return;

	g_sequencer.setBpm(f, g_conf.samplerate);
}

/* -------------------------------------------------------------------------- */

void setBeats(int beats, int bars)
{
	/* Never change this stuff while recording audio. */

	if (g_recorder.isRecordingInput())
		return;

	g_sequencer.setBeats(beats, bars, g_conf.samplerate);
	g_mixer.allocRecBuffer(g_sequencer.getMaxFramesInLoop(g_conf.samplerate));
}

/* -------------------------------------------------------------------------- */

void quantize(int val)
{
	g_sequencer.setQuantize(val, g_conf.samplerate);
}

/* -------------------------------------------------------------------------- */

void clearAllSamples()
{
	if (!v::gdConfirmWin("Warning", "Free all Sample channels: are you sure?"))
		return;
	G_MainWin->delSubWindow(WID_SAMPLE_EDITOR);
	g_sequencer.setStatus(SeqStatus::STOPPED);
	g_synchronizer.sendMIDIstop();
	g_mixerHandler.freeAllChannels();
	g_actionRecorder.clearAllActions();
}

/* -------------------------------------------------------------------------- */

void clearAllActions()
{
	if (!v::gdConfirmWin("Warning", "Clear all actions: are you sure?"))
		return;
	G_MainWin->delSubWindow(WID_ACTION_EDITOR);
	g_actionRecorder.clearAllActions();
}

/* -------------------------------------------------------------------------- */

void setInToOut(bool v)
{
	g_mixerHandler.setInToOut(v);
}

/* -------------------------------------------------------------------------- */

void toggleRecOnSignal()
{
	if (!g_recorder.canEnableRecOnSignal())
	{
		g_conf.recTriggerMode = RecTriggerMode::NORMAL;
		return;
	}
	g_conf.recTriggerMode = g_conf.recTriggerMode == RecTriggerMode::NORMAL ? RecTriggerMode::SIGNAL : RecTriggerMode::NORMAL;
}

/* -------------------------------------------------------------------------- */

void toggleFreeInputRec()
{
	if (!g_recorder.canEnableFreeInputRec())
	{
		g_conf.inputRecMode = InputRecMode::RIGID;
		return;
	}
	g_conf.inputRecMode = g_conf.inputRecMode == InputRecMode::FREE ? InputRecMode::RIGID : InputRecMode::FREE;
}

/* -------------------------------------------------------------------------- */

void printDebugInfo()
{
	g_model.debug();
}

/* -------------------------------------------------------------------------- */

void closeProject()
{
	if (!v::gdConfirmWin("Warning", "Close project: are you sure?"))
		return;
	m::init::reset();
	g_mixer.enable();
}

/* -------------------------------------------------------------------------- */

void quitGiada()
{
	G_MainWin->do_callback();
}
} // namespace giada::c::main
