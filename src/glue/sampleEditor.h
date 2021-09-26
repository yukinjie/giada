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

#ifndef G_GLUE_SAMPLE_EDITOR_H
#define G_GLUE_SAMPLE_EDITOR_H

#include "core/types.h"
#include "core/waveFx.h"
#include <functional>
#include <string>

namespace giada::m
{
class Wave;
}

namespace giada::m::channel
{
struct Data;
}

namespace giada::v
{
class gdSampleEditor;
}

namespace giada::c::sampleEditor
{
struct Data
{
	Data() = default;
	Data(const m::channel::Data&);

	ChannelStatus  a_getPreviewStatus() const;
	Frame          a_getPreviewTracker() const;
	const m::Wave& getWaveRef() const; // TODO - getWaveData (or public ptr member to Wave::data)
	Frame          getFramesInBar() const;
	Frame          getFramesInLoop() const;

	ID          channelId;
	std::string name;
	float       volume;
	float       pan;
	float       pitch;
	Frame       begin;
	Frame       end;
	Frame       shift;
	Frame       waveSize;
	int         waveBits;
	int         waveDuration;
	int         waveRate;
	std::string wavePath;
	bool        isLogical;

private:
	const m::channel::Data* m_channel;
};

/* onRefresh --- TODO - wrong name */

void onRefresh(bool gui, std::function<void(v::gdSampleEditor&)> f);

/* getData
Returns a Data object filled with data from a channel. */

Data getData(ID channelId);

/* setBeginEnd
Sets start/end points in the sample editor. */

void setBeginEnd(ID channelId, Frame b, Frame e);

void cut(ID channelId, Frame a, Frame b);
void copy(ID channelId, Frame a, Frame b);
void paste(ID channelId, Frame a);

void trim(ID channelId, Frame a, Frame b);
void reverse(ID channelId, Frame a, Frame b);
void normalize(ID channelId, Frame a, Frame b);
void silence(ID channelId, Frame a, Frame b);
void fade(ID channelId, Frame a, Frame b, m::wfx::Fade type);
void smoothEdges(ID channelId, Frame a, Frame b);
void shift(ID channelId, Frame offset);
void reload(ID channelId);

bool isWaveBufferFull();

void playPreview(bool loop);
void stopPreview();
void setPreviewTracker(Frame f);
void cleanupPreview();

/* toNewChannel
Copies the selected range into a new sample channel. */

void toNewChannel(ID channelId, Frame a, Frame b);
} // namespace giada::c::sampleEditor

#endif
