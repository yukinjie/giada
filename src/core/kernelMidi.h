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

#ifndef G_KERNELMIDI_H
#define G_KERNELMIDI_H

#include "midiMap.h"
#include <RtMidi.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace giada::m
{
class KernelMidi final
{
public:
	KernelMidi();

	/* getStatus
    Returns current engine status. */

	bool getStatus() const;

	unsigned countInPorts() const;
	unsigned countOutPorts() const;

	/* getIn/OutPortName
    Returns the name of the port 'p'. */

	std::string getInPortName(unsigned p) const;
	std::string getOutPortName(unsigned p) const;

	bool hasAPI(int API) const;

	/* send
    Sends a MIDI message 's' as uint32_t or as separate bytes. */

	void send(uint32_t s);
	void send(int b1, int b2 = -1, int b3 = -1);

	/* setApi
    Sets the Api in use for both in & out messages. */

	void setApi(int api);

	/* open/close/in/outDevice */

	int openOutDevice(int port);
	int openInDevice(int port);
	int closeInDevice();
	int closeOutDevice();

	std::function<void(uint32_t)> onMidiReceived;

private:
	static void callback(double, std::vector<unsigned char>*, void*);
	void        callback(std::vector<unsigned char>*);

	std::unique_ptr<RtMidiOut> m_midiOut;
	std::unique_ptr<RtMidiIn>  m_midiIn;
	bool                       m_status;
	int                        m_api;
	unsigned                   m_numOutPorts;
	unsigned                   m_numInPorts;
};
} // namespace giada::m

#endif
