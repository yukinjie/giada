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

#ifndef G_KERNELAUDIO_H
#define G_KERNELAUDIO_H

#include "deps/rtaudio/RtAudio.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#ifdef WITH_AUDIO_JACK
#include "core/jackTransport.h"
#endif

namespace giada::m::conf
{
struct Data;
}

namespace giada::m
{
class KernelAudio final
{
public:
	struct Device
	{
		size_t           index             = 0;
		bool             probed            = false;
		std::string      name              = "";
		int              maxOutputChannels = 0;
		int              maxInputChannels  = 0;
		int              maxDuplexChannels = 0;
		bool             isDefaultOut      = false;
		bool             isDefaultIn       = false;
		std::vector<int> sampleRates       = {};
	};

	KernelAudio();

	int  openDevice(const m::conf::Data& conf);
	void closeDevice();
	int  startStream();
	int  stopStream();

	bool                       isReady() const;
	bool                       isInputEnabled() const;
	unsigned                   getRealBufSize() const;
	bool                       hasAPI(int API) const;
	int                        getAPI() const;
	void                       logCompiledAPIs() const;
	Device                     getDevice(const char* name) const;
	const std::vector<Device>& getDevices() const;
#ifdef WITH_AUDIO_JACK
	jack_client_t* getJackHandle() const;
#endif

	/* onAudioCallback
	Main callback invoked on each audio block. */

	std::function<int(void*, void*, int)> onAudioCallback;

private:
	static int audioCallback(void*, void*, unsigned, double, RtAudioStreamStatus, void*);

	Device              fetchDevice(size_t deviceIndex) const;
	std::vector<Device> fetchDevices() const;
	void                printDevices(const std::vector<Device>& devices) const;

#ifdef WITH_AUDIO_JACK
	JackTransport m_jackTransport;
#endif
	std::vector<Device>      m_devices;
	std::unique_ptr<RtAudio> m_rtAudio;
	bool                     m_ready;
	bool                     m_inputEnabled;
	unsigned                 m_realBufferSize; // Real buffer size from the soundcard
	int                      m_realSampleRate; // Sample rate might differ if JACK in use
	int                      m_api;
};
} // namespace giada::m

#endif
