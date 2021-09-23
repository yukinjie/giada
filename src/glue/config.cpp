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

#include "glue/config.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/kernelAudio.h"
#include "core/kernelMidi.h"
#include "core/midiMap.h"
#include "core/plugins/pluginManager.h"
#include "deps/rtaudio/RtAudio.h"
#include "utils/fs.h"
#include "utils/vector.h"
#include <FL/Fl_Tooltip.H>

extern giada::m::KernelAudio   g_kernelAudio;
extern giada::m::KernelMidi    g_kernelMidi;
extern giada::m::PluginManager g_pluginManager;
extern giada::m::midiMap::Data g_midiMap;
extern giada::m::conf::Data    g_conf;

namespace giada::c::config
{
namespace
{
AudioDeviceData getAudioDeviceData_(DeviceType type, size_t index, int channelsCount, int channelsStart)
{
	for (const m::KernelAudio::Device& device : g_kernelAudio.getDevices())
		if (device.index == index)
			return AudioDeviceData(type, device, channelsCount, channelsStart);
	return AudioDeviceData();
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

AudioDeviceData::AudioDeviceData(DeviceType type, const m::KernelAudio::Device& device,
    int channelsCount, int channelsStart)
: type(type)
, index(device.index)
, name(device.name)
, channelsMax(type == DeviceType::OUTPUT ? device.maxOutputChannels : device.maxInputChannels)
, sampleRates(device.sampleRates)
, channelsCount(channelsCount)
, channelsStart(channelsStart)
{
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void AudioData::setOutputDevice(int index)
{
	for (AudioDeviceData& d : outputDevices)
	{
		if (index != d.index)
			continue;
		outputDevice = d;
	}
}

/* -------------------------------------------------------------------------- */

void AudioData::setInputDevice(int index)
{
	for (AudioDeviceData& d : inputDevices)
	{
		if (index == d.index)
		{
			inputDevice = d;
			return;
		}
	}
	inputDevice = {};
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

AudioData getAudioData()
{
	AudioData audioData;

	audioData.apis[G_SYS_API_NONE] = "(none)";

#if defined(G_OS_LINUX)

	if (g_kernelAudio.hasAPI(RtAudio::LINUX_ALSA))
		audioData.apis[G_SYS_API_ALSA] = "ALSA";
	if (g_kernelAudio.hasAPI(RtAudio::UNIX_JACK))
		audioData.apis[G_SYS_API_JACK] = "Jack";
	if (g_kernelAudio.hasAPI(RtAudio::LINUX_PULSE))
		audioData.apis[G_SYS_API_PULSE] = "PulseAudio";

#elif defined(G_OS_FREEBSD)

	if (g_kernelAudio.hasAPI(RtAudio::UNIX_JACK))
		audioData.apis[G_SYS_API_JACK] = "Jack";
	if (g_kernelAudio.hasAPI(RtAudio::LINUX_PULSE))
		audioData.apis[G_SYS_API_PULSE] = "PulseAudio";

#elif defined(G_OS_WINDOWS)

	if (g_kernelAudio.hasAPI(RtAudio::WINDOWS_DS))
		audioData.apis[G_SYS_API_DS] = "DirectSound";
	if (g_kernelAudio.hasAPI(RtAudio::WINDOWS_ASIO))
		audioData.apis[G_SYS_API_ASIO] = "ASIO";
	if (g_kernelAudio.hasAPI(RtAudio::WINDOWS_WASAPI))
		audioData.apis[G_SYS_API_WASAPI] = "WASAPI";

#elif defined(G_OS_MAC)

	if (g_kernelAudio.hasAPI(RtAudio::MACOSX_CORE))
		audioData.apis[G_SYS_API_CORE] = "CoreAudio";

#endif

	std::vector<m::KernelAudio::Device> devices = g_kernelAudio.getDevices();

	for (const m::KernelAudio::Device& device : devices)
	{
		if (device.maxOutputChannels > 0)
			audioData.outputDevices.push_back(AudioDeviceData(DeviceType::OUTPUT, device, G_MAX_IO_CHANS, 0));
		if (device.maxInputChannels > 0)
			audioData.inputDevices.push_back(AudioDeviceData(DeviceType::INPUT, device, 1, 0));
	}

	audioData.api             = g_conf.soundSystem;
	audioData.bufferSize      = g_conf.buffersize;
	audioData.sampleRate      = g_conf.samplerate;
	audioData.limitOutput     = g_conf.limitOutput;
	audioData.recTriggerLevel = g_conf.recTriggerLevel;
	audioData.resampleQuality = g_conf.rsmpQuality;
	audioData.outputDevice    = getAudioDeviceData_(DeviceType::OUTPUT,
        g_conf.soundDeviceOut, g_conf.channelsOutCount,
        g_conf.channelsOutStart);
	audioData.inputDevice     = getAudioDeviceData_(DeviceType::INPUT,
        g_conf.soundDeviceIn, g_conf.channelsInCount,
        g_conf.channelsInStart);

	return audioData;
}

/* -------------------------------------------------------------------------- */

MidiData getMidiData()
{
	MidiData midiData;

#if defined(G_OS_LINUX)

	if (g_kernelMidi.hasAPI(RtMidi::LINUX_ALSA))
		midiData.apis[G_MIDI_API_ALSA] = "ALSA";
	if (g_kernelMidi.hasAPI(RtMidi::UNIX_JACK))
		midiData.apis[G_MIDI_API_JACK] = "JACK";

#elif defined(G_OS_FREEBSD)

	if (g_kernelMidi.hasAPI(RtMidi::UNIX_JACK))
		midiData.apis[G_MIDI_API_JACK] = "JACK";

#elif defined(G_OS_WINDOWS)

	if (g_kernelMidi.hasAPI(RtMidi::WINDOWS_MM))
		midiData.apis[G_MIDI_API_MM] = "Multimedia MIDI";

#elif defined(G_OS_MAC)

	if (g_kernelMidi.hasAPI(RtMidi::MACOSX_CORE))
		midiData.apis[G_MIDI_API_CORE] = "OSX Core MIDI";

#endif

	midiData.syncModes[G_MIDI_SYNC_NONE]    = "(disabled)";
	midiData.syncModes[G_MIDI_SYNC_CLOCK_M] = "MIDI Clock (master)";
	midiData.syncModes[G_MIDI_SYNC_MTC_M]   = "MTC (master)";

	midiData.midiMaps = g_midiMap.maps;
	midiData.midiMap  = u::vector::indexOf(midiData.midiMaps, g_conf.midiMapPath);

	for (unsigned i = 0; i < g_kernelMidi.countOutPorts(); i++)
		midiData.outPorts.push_back(g_kernelMidi.getOutPortName(i));
	for (unsigned i = 0; i < g_kernelMidi.countInPorts(); i++)
		midiData.inPorts.push_back(g_kernelMidi.getInPortName(i));

	midiData.api      = g_conf.midiSystem;
	midiData.syncMode = g_conf.midiSync;
	midiData.outPort  = g_conf.midiPortOut;
	midiData.inPort   = g_conf.midiPortIn;

	return midiData;
}

/* -------------------------------------------------------------------------- */

PluginData getPluginData()
{
	PluginData pluginData;
	pluginData.numAvailablePlugins = g_pluginManager.countAvailablePlugins();
	pluginData.pluginPath          = g_conf.pluginPath;
	return pluginData;
}

/* -------------------------------------------------------------------------- */

MiscData getMiscData()
{
	MiscData miscData;
	miscData.logMode      = g_conf.logMode;
	miscData.showTooltips = g_conf.showTooltips;
	return miscData;
}
/* -------------------------------------------------------------------------- */

void save(const AudioData& data)
{
	g_conf.soundSystem      = data.api;
	g_conf.soundDeviceOut   = data.outputDevice.index;
	g_conf.soundDeviceIn    = data.inputDevice.index;
	g_conf.channelsOutCount = data.outputDevice.channelsCount;
	g_conf.channelsOutStart = data.outputDevice.channelsStart;
	g_conf.channelsInCount  = data.inputDevice.channelsCount;
	g_conf.channelsInStart  = data.inputDevice.channelsStart;
	g_conf.limitOutput      = data.limitOutput;
	g_conf.rsmpQuality      = data.resampleQuality;
	g_conf.buffersize       = data.bufferSize;
	g_conf.recTriggerLevel  = data.recTriggerLevel;
	g_conf.samplerate       = data.sampleRate;
}

/* -------------------------------------------------------------------------- */

void save(const PluginData& data)
{
	g_conf.pluginPath = data.pluginPath;
}

/* -------------------------------------------------------------------------- */

void save(const MidiData& data)
{
	g_conf.midiSystem  = data.api;
	g_conf.midiPortOut = data.outPort;
	g_conf.midiPortIn  = data.inPort;
	g_conf.midiMapPath = ""; // TODO
	g_conf.midiSync    = data.syncMode;
}

/* -------------------------------------------------------------------------- */

void save(const MiscData& data)
{
	g_conf.logMode      = data.logMode;
	g_conf.showTooltips = data.showTooltips;
	Fl_Tooltip::enable(g_conf.showTooltips);
}

/* -------------------------------------------------------------------------- */

void scanPlugins(std::string dir, const std::function<void(float)>& progress)
{
	g_pluginManager.scanDirs(dir, progress);
	g_pluginManager.saveList(u::fs::getHomePath() + G_SLASH + "plugins.xml");
}
} // namespace giada::c::config