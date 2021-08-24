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

#include "config.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/kernelAudio.h"
#include "deps/rtaudio/RtAudio.h"

extern giada::m::KernelAudio g_kernelAudio;
extern giada::m::conf::Data  g_conf;

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
} // namespace giada::c::config