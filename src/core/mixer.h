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

#ifndef G_MIXER_H
#define G_MIXER_H

#include "core/midiEvent.h"
#include "core/queue.h"
#include "core/ringBuffer.h"
#include "core/sequencer.h"
#include "core/types.h"
#include "deps/mcl-audio-buffer/src/audioBuffer.hpp"
#include "src/core/actions/actions.h"
#include <functional>

namespace mcl
{
class AudioBuffer;
}

namespace giada::m::model
{
struct Mixer;
struct Layout;
} // namespace giada::m::model

namespace giada::m::channel
{
struct Data;
}

namespace giada::m
{
struct Action;
class MixerHandler;
class Mixer
{
public:
	friend MixerHandler;

	static constexpr int MASTER_OUT_CHANNEL_ID = 1;
	static constexpr int MASTER_IN_CHANNEL_ID  = 2;
	static constexpr int PREVIEW_CHANNEL_ID    = 3;

	/* RenderInfo
	Struct of parameters passed to Mixer for rendering. */

	struct RenderInfo
	{
		bool  isAudioReady;
		bool  hasInput;
		bool  shouldLineInRec;
		bool  limitOutput;
		bool  inToOut;
		Frame maxFramesToRec;
		float outVol;
		float inVol;
		float recTriggerLevel;
	};

	/* RecordInfo
	Information regarding the input recording progress. */

	struct RecordInfo
	{
		Frame position;
		Frame maxLength;
	};

	Mixer(model::Model&);

	/* isActive
	Mixer might be inactive (not initialized or suspended). */

	bool isActive() const;

	/* isChannelAudible
	True if the channel 'c' is currently audible: not muted or not included in a 
	solo session. */

	bool isChannelAudible(const channel::Data& c) const;

	Peak getPeakOut() const;
	Peak getPeakIn() const;

	RecordInfo getRecordInfo() const;

	/* reset
	Brings everything back to the initial state. */

	void reset(Frame framesInLoop, Frame framesInBuffer);

	/* enable, disable
	Toggles master callback processing. Useful to suspend the rendering. */

	void enable();
	void disable();

	/* allocRecBuffer
	Allocates new memory for the virtual input channel. */

	void allocRecBuffer(Frame frames);

	/* clearRecBuffer
	Clears internal virtual channel. */

	void clearRecBuffer();

	/* getRecBuffer
	Returns a read-only reference to the internal virtual channel. Use this to
	merge data into channel after an input recording session. */

	const mcl::AudioBuffer& getRecBuffer();

	/* advanceChannels
	Processes Channels' static events (e.g. pre-recorded actions or sequencer 
	events) in the current audio block. Called by the main audio thread when the 
	sequencer is running. */

	void advanceChannels(const Sequencer::EventBuffer& events, const model::Layout&);

	/* render
	Core rendering function. */

	void render(mcl::AudioBuffer& out, const mcl::AudioBuffer& in, const RenderInfo&,
	    const model::Layout&);

	/* onSignalTresholdReached
	Callback fired when audio has reached a certain threshold (record-on-signal 
	mode). */

	std::function<void()> onSignalTresholdReached;

	/* onEndOfRecording
	Callback fired when the audio recording session has ended. */

	std::function<void()> onEndOfRecording;

private:
	/* thresholdReached
	Returns true if left or right channel's peak has reached a certain 
	threshold. */

	bool thresholdReached(Peak p, float threshold) const;

	/* lineInRec
	Records from line in. 'maxFrames' determines how many frames to record 
	before the internal tracker loops over. The value changes whether you are 
	recording in RIGID or FREE mode. */

	void lineInRec(const mcl::AudioBuffer& inBuf, Frame maxFrames, float inVol);

	/* processLineIn
	Computes line in peaks and prepares the internal working buffer for input
	recording. */

	void processLineIn(const model::Mixer& mixer, const mcl::AudioBuffer& inBuf,
	    float inVol, float recTriggerLevel);

	void processChannels(const model::Layout&, mcl::AudioBuffer& out, mcl::AudioBuffer& in);
	void renderMasterIn(const model::Layout&, mcl::AudioBuffer& in);
	void renderMasterOut(const model::Layout&, mcl::AudioBuffer& out);
	void renderPreview(const model::Layout&, mcl::AudioBuffer& out);

	/* limit
	Applies a very dumb hard limiter. */

	void limit(mcl::AudioBuffer& outBuf);

	/* finalizeOutput
	Last touches after the output has been rendered: apply inToOut if any, apply
	output volume, compute peak. */

	void finalizeOutput(const model::Mixer&, mcl::AudioBuffer& outBuf,
	    const RenderInfo&);

	/* startInputRec, stopInputRec
	Starts/stops input recording on frame 'from'. The latter returns the number 
	of recorded frames. */

	void  startInputRec(Frame from);
	Frame stopInputRec();

	model::Model& m_model;

	/* m_recBuffer
	Working buffer for audio recording. */

	mcl::AudioBuffer m_recBuffer;

	/* m_inBuffer
	Working buffer for input channel. Used for the in->out bridge. */

	mcl::AudioBuffer m_inBuffer;

	/* m_inputTracker
	Frame position while recording. */

	Frame m_inputTracker;

	/* m_signalCbFired, m_endOfRecCbFired
	Boolean guards to determine whether the callbacks have been fired or not, 
	to avoid retriggering. */

	bool m_signalCbFired;
	bool m_endOfRecCbFired;
};
} // namespace giada::m

#endif
