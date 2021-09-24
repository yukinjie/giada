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

#include "core/synchronizer.h"
#include "core/conf.h"
#include "core/kernelAudio.h"
#include "core/kernelMidi.h"
#include "core/model/model.h"

namespace giada::m
{
Synchronizer::Synchronizer(const conf::Data& c, KernelMidi& k)
: onJackRewind(nullptr)
, onJackChangeBpm(nullptr)
, onJackStart(nullptr)
, onJackStop(nullptr)
, m_kernelMidi(k)
, m_conf(c)
{
	reset();
}

/* -------------------------------------------------------------------------- */

void Synchronizer::reset()
{
	m_midiTCrate = static_cast<int>((m_conf.samplerate / m_conf.midiTCfps) * G_MAX_IO_CHANS); // stereo values
}

/* -------------------------------------------------------------------------- */

void Synchronizer::sendMIDIsync(const model::Sequencer& clock)
{
	/* Sending MIDI sync while waiting is meaningless. */

	if (clock.status == SeqStatus::WAITING)
		return;

	int currentFrame = clock.state->currentFrame.load();

	/* TODO - only Master (_M) is implemented so far. */

	if (m_conf.midiSync == MIDI_SYNC_CLOCK_M)
	{
		if (currentFrame % (clock.framesInBeat / 24) == 0)
			m_kernelMidi.send(MIDI_CLOCK, -1, -1);
		return;
	}

	if (m_conf.midiSync == MIDI_SYNC_MTC_M)
	{

		/* check if a new timecode frame has passed. If so, send MIDI TC
		 * quarter frames. 8 quarter frames, divided in two branches:
		 * 1-4 and 5-8. We check timecode frame's parity: if even, send
		 * range 1-4, if odd send 5-8. */

		if (currentFrame % m_midiTCrate != 0) // no timecode frame passed
			return;

		/* frame low nibble
		 * frame high nibble
		 * seconds low nibble
		 * seconds high nibble */

		if (m_midiTCframes % 2 == 0)
		{
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCframes & 0x0F) | 0x00, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCframes >> 4) | 0x10, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCseconds & 0x0F) | 0x20, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCseconds >> 4) | 0x30, -1);
		}

		/* minutes low nibble
		 * minutes high nibble
		 * hours low nibble
		 * hours high nibble SMPTE frame rate */

		else
		{
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCminutes & 0x0F) | 0x40, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTCminutes >> 4) | 0x50, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTChours & 0x0F) | 0x60, -1);
			m_kernelMidi.send(MIDI_MTC_QUARTER, (m_midiTChours >> 4) | 0x70, -1);
		}

		m_midiTCframes++;

		/* check if total timecode frames are greater than timecode fps:
		 * if so, a second has passed */

		if (m_midiTCframes > m_conf.midiTCfps)
		{
			m_midiTCframes = 0;
			m_midiTCseconds++;
			if (m_midiTCseconds >= 60)
			{
				m_midiTCminutes++;
				m_midiTCseconds = 0;
				if (m_midiTCminutes >= 60)
				{
					m_midiTChours++;
					m_midiTCminutes = 0;
				}
			}
			//u::log::print("%d:%d:%d:%d\n", m_midiTChours, m_midiTCminutes, m_midiTCseconds, m_midiTCframes);
		}
	}
}

/* -------------------------------------------------------------------------- */

void Synchronizer::sendMIDIrewind()
{
	m_midiTCframes  = 0;
	m_midiTCseconds = 0;
	m_midiTCminutes = 0;
	m_midiTChours   = 0;

	/* For cueing the slave to a particular start point, Quarter Frame messages 
    are not used. Instead, an MTC Full Frame message should be sent. The Full 
    Frame is a SysEx message that encodes the entire SMPTE time in one message. */

	if (m_conf.midiSync == MIDI_SYNC_MTC_M)
	{
		m_kernelMidi.send(MIDI_SYSEX, 0x7F, 0x00); // send msg on channel 0
		m_kernelMidi.send(0x01, 0x01, 0x00);       // hours 0
		m_kernelMidi.send(0x00, 0x00, 0x00);       // mins, secs, frames 0
		m_kernelMidi.send(MIDI_EOX, -1, -1);       // end of sysex
	}
	else if (m_conf.midiSync == MIDI_SYNC_CLOCK_M)
		m_kernelMidi.send(MIDI_POSITION_PTR, 0, 0);
}

/* -------------------------------------------------------------------------- */

void Synchronizer::sendMIDIstart()
{
	if (m_conf.midiSync == MIDI_SYNC_CLOCK_M)
	{
		m_kernelMidi.send(MIDI_START, -1, -1);
		m_kernelMidi.send(MIDI_POSITION_PTR, 0, 0);
	}
}

/* -------------------------------------------------------------------------- */

void Synchronizer::sendMIDIstop()
{
	if (m_conf.midiSync == MIDI_SYNC_CLOCK_M)
		m_kernelMidi.send(MIDI_STOP, -1, -1);
}

/* -------------------------------------------------------------------------- */

#ifdef WITH_AUDIO_JACK

void Synchronizer::recvJackSync(const JackTransport::State& state)
{
	assert(onJackRewind != nullptr);
	assert(onJackChangeBpm != nullptr);
	assert(onJackStart != nullptr);
	assert(onJackStop != nullptr);

	JackTransport::State jackStateCurr = state;

	if (jackStateCurr != m_jackStatePrev)
	{
		if (jackStateCurr.frame != m_jackStatePrev.frame && jackStateCurr.frame == 0)
		{
			G_DEBUG("JackState received - rewind to frame 0");
			onJackRewind();
		}

		// jackStateCurr.bpm == 0 if JACK doesn't send that info
		if (jackStateCurr.bpm != m_jackStatePrev.bpm && jackStateCurr.bpm > 1.0f)
		{
			G_DEBUG("JackState received - bpm=" << jackStateCurr.bpm);
			onJackChangeBpm(jackStateCurr.bpm);
		}

		if (jackStateCurr.running != m_jackStatePrev.running)
		{
			G_DEBUG("JackState received - running=" << jackStateCurr.running);
			jackStateCurr.running ? onJackStart() : onJackStop();
		}
	}

	m_jackStatePrev = jackStateCurr;
}

#endif
} // namespace giada::m
