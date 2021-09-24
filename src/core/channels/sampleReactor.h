/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2020 Giovanni A. Zuliani | Monocasual
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

#ifndef G_CHANNEL_SAMPLE_REACTOR_H
#define G_CHANNEL_SAMPLE_REACTOR_H

#include "core/eventDispatcher.h"
#include "core/quantizer.h"

namespace giada::m::channel
{
struct Data;
}

/* sampleReactor
Reacts to manual events sent to Sample Channels: key press, key release, 
sequencer stop, ... . */

namespace giada::m::sampleReactor
{
struct Data
{
	Data(ID channelId);
	Data(const Data&) = default;
	Data(Data&&)      = default;
	Data& operator=(const Data&) = default;
	Data& operator=(Data&&) = default;
};

void react(channel::Data& ch, const EventDispatcher::Event& e);
} // namespace giada::m::sampleReactor

#endif
