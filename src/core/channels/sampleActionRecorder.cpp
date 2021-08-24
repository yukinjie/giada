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

#include "sampleActionRecorder.h"
#include "core/channels/channel.h"
#include "core/conf.h"
#include "core/eventDispatcher.h"
#include "core/mixer.h"
#include "core/recorder.h"
#include "src/core/actions/action.h"
#include "src/core/actions/actionRecorder.h"
#include <cassert>

extern giada::m::Sequencer      g_sequencer;
extern giada::m::ActionRecorder g_actionRecorder;
extern giada::m::Recorder       g_recorder;
extern giada::m::conf::Data     g_conf;

namespace giada::m::sampleActionRecorder
{
namespace
{
void record_(channel::Data& ch, int note);
void onKeyPress_(channel::Data& ch);
void toggleReadActions_(channel::Data& ch);
void startReadActions_(channel::Data& ch);
void stopReadActions_(channel::Data& ch, ChannelStatus curRecStatus);
void killReadActions_(channel::Data& ch);
bool canRecord_(const channel::Data& ch);

/* -------------------------------------------------------------------------- */

bool canRecord_(const channel::Data& ch)
{
	return g_recorder.isRecordingAction() &&
	       g_sequencer.isRunning() &&
	       !g_recorder.isRecordingInput() &&
	       !ch.samplePlayer->isAnyLoopMode();
}

/* -------------------------------------------------------------------------- */

void onKeyPress_(channel::Data& ch)
{
	if (!canRecord_(ch))
		return;
	record_(ch, MidiEvent::NOTE_ON);

	/* Skip reading actions when recording on ChannelMode::SINGLE_PRESS to 
	prevent	existing actions to interfere with the keypress/keyrel combo. */

	if (ch.samplePlayer->mode == SamplePlayerMode::SINGLE_PRESS)
		ch.state->readActions.store(false);
}

/* -------------------------------------------------------------------------- */

void record_(channel::Data& ch, int note)
{
	g_actionRecorder.liveRec(ch.id, MidiEvent(note, 0, 0),
	    g_sequencer.quantize(g_sequencer.getCurrentFrame()));

	ch.hasActions = true;
}

/* -------------------------------------------------------------------------- */

void toggleReadActions_(channel::Data& ch)
{
	/* When you start reading actions while conf::treatRecsAsLoops is true, the
	value ch.state->readActions actually is not set to true immediately, because
	the channel is in wait mode (REC_WAITING). readActions will become true on
	the next first beat. So a 'stop rec' command should occur also when
	readActions is false but the channel is in wait mode; this check will
	handle the case of when you press 'R', the channel goes into REC_WAITING and
	then you press 'R' again to undo the status. */

	if (!ch.hasActions)
		return;

	const bool          readActions = ch.state->readActions.load();
	const ChannelStatus recStatus   = ch.state->recStatus.load();

	if (readActions || (!readActions && recStatus == ChannelStatus::WAIT))
		stopReadActions_(ch, recStatus);
	else
		startReadActions_(ch);
}

/* -------------------------------------------------------------------------- */

void startReadActions_(channel::Data& ch)
{
	if (g_conf.treatRecsAsLoops)
		ch.state->recStatus.store(ChannelStatus::WAIT);
	else
	{
		ch.state->recStatus.store(ChannelStatus::PLAY);
		ch.state->readActions.store(true);
	}
}

/* -------------------------------------------------------------------------- */

void stopReadActions_(channel::Data& ch, ChannelStatus curRecStatus)
{
	/* First of all, if the clock is not running or treatRecsAsLoops is off, 
	just stop and disable everything. Otherwise make sure a channel with actions
	behave like a dynamic one. */

	if (!g_sequencer.isRunning() || !g_conf.treatRecsAsLoops)
	{
		ch.state->recStatus.store(ChannelStatus::OFF);
		ch.state->readActions.store(false);
	}
	else if (curRecStatus == ChannelStatus::WAIT)
		ch.state->recStatus.store(ChannelStatus::OFF);
	else if (curRecStatus == ChannelStatus::ENDING)
		ch.state->recStatus.store(ChannelStatus::PLAY);
	else
		ch.state->recStatus.store(ChannelStatus::ENDING);
}

/* -------------------------------------------------------------------------- */

void killReadActions_(channel::Data& ch)
{
	/* Killing Read Actions, i.e. shift + click on 'R' button is meaningful only 
	when the conf::treatRecsAsLoops is true. */

	if (!g_conf.treatRecsAsLoops)
		return;
	ch.state->recStatus.store(ChannelStatus::OFF);
	ch.state->readActions.store(false);
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

void react(channel::Data& ch, const EventDispatcher::Event& e)
{
	if (!ch.hasWave())
		return;

	switch (e.type)
	{

	case EventDispatcher::EventType::KEY_PRESS:
		onKeyPress_(ch);
		break;

		/* Record a stop event only if channel is SINGLE_PRESS. For any other 
		mode the key release event is meaningless. */

	case EventDispatcher::EventType::KEY_RELEASE:
		if (canRecord_(ch) && ch.samplePlayer->mode == SamplePlayerMode::SINGLE_PRESS)
			record_(ch, MidiEvent::NOTE_OFF);
		break;

	case EventDispatcher::EventType::KEY_KILL:
		if (canRecord_(ch))
			record_(ch, MidiEvent::NOTE_KILL);
		break;

	case EventDispatcher::EventType::CHANNEL_TOGGLE_READ_ACTIONS:
		toggleReadActions_(ch);
		break;

	case EventDispatcher::EventType::CHANNEL_KILL_READ_ACTIONS:
		killReadActions_(ch);
		break;

	default:
		break;
	}
}
} // namespace giada::m::sampleActionRecorder