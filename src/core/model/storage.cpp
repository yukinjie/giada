/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2019 Giovanni A. Zuliani | Monocasual
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

#include "core/model/storage.h"
#include "core/channels/channelManager.h"
#include "core/conf.h"
#include "core/kernelAudio.h"
#include "core/model/model.h"
#include "core/patch.h"
#include "core/plugins/pluginManager.h"
#include "core/sequencer.h"
#include "core/waveManager.h"
#include "src/core/actions/actionRecorder.h"
#include <cassert>

extern giada::m::model::Model   g_model;
extern giada::m::Sequencer      g_sequencer;
extern giada::m::KernelAudio    g_kernelAudio;
extern giada::m::ActionRecorder g_actionRecorder;
extern giada::m::conf::Data     g_conf;
extern giada::m::patch::Data    g_patch;
extern giada::m::PluginManager  g_pluginManager;
extern giada::m::ChannelManager g_channelManager;
extern giada::m::WaveManager    g_waveManager;

namespace giada::m::model
{
namespace
{
void loadChannels_(const std::vector<patch::Channel>& channels, int samplerate)
{
	float samplerateRatio = g_conf.samplerate / static_cast<float>(samplerate);

	for (const patch::Channel& pchannel : channels)
		g_model.get().channels.push_back(g_channelManager.deserializeChannel(pchannel, samplerateRatio, g_kernelAudio.getRealBufSize()));
}

/* -------------------------------------------------------------------------- */

void loadActions_(const std::vector<patch::Action>& pactions)
{
	g_model.getAll<Actions::Map>() = std::move(g_actionRecorder.deserializeActions(pactions));
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void store(patch::Data& patch)
{
	const Layout& layout = g_model.get();

	patch.bars       = layout.sequencer.bars;
	patch.beats      = layout.sequencer.beats;
	patch.bpm        = layout.sequencer.bpm;
	patch.quantize   = layout.sequencer.quantize;
	patch.metronome  = g_sequencer.isMetronomeOn(); // TODO - add bool metronome to Layout
	patch.samplerate = g_conf.samplerate;

#ifdef WITH_VST
	for (const auto& p : g_model.getAll<PluginPtrs>())
		patch.plugins.push_back(g_pluginManager.serializePlugin(*p));
#endif

	patch.actions = g_actionRecorder.serializeActions(g_model.getAll<Actions::Map>());

	for (const auto& w : g_model.getAll<WavePtrs>())
		patch.waves.push_back(g_waveManager.serializeWave(*w));

	for (const channel::Data& c : layout.channels)
		patch.channels.push_back(g_channelManager.serializeChannel(c));
}

/* -------------------------------------------------------------------------- */

void store(conf::Data& conf)
{
	const Layout& layout = g_model.get();

	conf.midiInEnabled    = layout.midiIn.enabled;
	conf.midiInFilter     = layout.midiIn.filter;
	conf.midiInRewind     = layout.midiIn.rewind;
	conf.midiInStartStop  = layout.midiIn.startStop;
	conf.midiInActionRec  = layout.midiIn.actionRec;
	conf.midiInInputRec   = layout.midiIn.inputRec;
	conf.midiInMetronome  = layout.midiIn.metronome;
	conf.midiInVolumeIn   = layout.midiIn.volumeIn;
	conf.midiInVolumeOut  = layout.midiIn.volumeOut;
	conf.midiInBeatDouble = layout.midiIn.beatDouble;
	conf.midiInBeatHalf   = layout.midiIn.beatHalf;
}

/* -------------------------------------------------------------------------- */

void load(const patch::Data& patch)
{
	DataLock lock = g_model.lockData();

	/* Clear and re-initialize channels first. */

	g_model.get().channels = {};
	g_model.getAll<ChannelBufferPtrs>().clear();
	g_model.getAll<ChannelStatePtrs>().clear();

	/* Load external data first: plug-ins and waves. */

#ifdef WITH_VST
	g_model.getAll<PluginPtrs>().clear();
	for (const patch::Plugin& pplugin : patch.plugins)
		g_model.getAll<PluginPtrs>().push_back(g_pluginManager.deserializePlugin(pplugin, patch.version, g_conf.samplerate, g_kernelAudio.getRealBufSize()));
#endif

	g_model.getAll<WavePtrs>().clear();
	for (const patch::Wave& pwave : patch.waves)
	{
		std::unique_ptr<Wave> w = g_waveManager.deserializeWave(pwave, g_conf.samplerate,
		    g_conf.rsmpQuality);
		if (w != nullptr)
			g_model.getAll<WavePtrs>().push_back(std::move(w));
	}

	/* Then load up channels, actions and global properties. */

	loadChannels_(patch.channels, g_patch.samplerate);
	loadActions_(patch.actions);

	g_model.get().sequencer.status   = SeqStatus::STOPPED;
	g_model.get().sequencer.bars     = patch.bars;
	g_model.get().sequencer.beats    = patch.beats;
	g_model.get().sequencer.bpm      = patch.bpm;
	g_model.get().sequencer.quantize = patch.quantize;
}

/* -------------------------------------------------------------------------- */

void load(const conf::Data& c)
{
	g_model.get().midiIn.enabled    = c.midiInEnabled;
	g_model.get().midiIn.filter     = c.midiInFilter;
	g_model.get().midiIn.rewind     = c.midiInRewind;
	g_model.get().midiIn.startStop  = c.midiInStartStop;
	g_model.get().midiIn.actionRec  = c.midiInActionRec;
	g_model.get().midiIn.inputRec   = c.midiInInputRec;
	g_model.get().midiIn.volumeIn   = c.midiInVolumeIn;
	g_model.get().midiIn.volumeOut  = c.midiInVolumeOut;
	g_model.get().midiIn.beatDouble = c.midiInBeatDouble;
	g_model.get().midiIn.beatHalf   = c.midiInBeatHalf;
	g_model.get().midiIn.metronome  = c.midiInMetronome;

	g_model.swap(SwapType::NONE);
}
} // namespace giada::m::model
