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

#ifndef G_CHANNEL_MANAGER_H
#define G_CHANNEL_MANAGER_H

#include "core/idManager.h"
#include "core/types.h"

namespace giada::m
{
class KernelAudio;
}

namespace giada::m::model
{
class Model;
}

namespace giada::m::channel
{
struct Data;
struct State;
struct Buffer;
} // namespace giada::m::channel

namespace giada::m::conf
{
struct Data;
}

namespace giada::m::patch
{
struct Channel;
}

namespace giada::m
{
class ChannelManager final
{
public:
	ChannelManager(const conf::Data&, model::Model&);

	/* getNextId
	Returns the next channel ID that will be assigned to a new channel. */

	ID getNextId() const;

	/* reset
    Resets internal ID generator. */

	void reset();

	/* create (1)
    Creates a new channel. If channelId == 0 generates a new ID, reuse the one 
    passed in otherwise. */

	channel::Data create(ID channelId, ChannelType type, ID columnId, int bufferSize);

	/* create (2)
    Creates a new channel given an existing one (i.e. clone). */

	channel::Data create(const channel::Data& ch, int bufferSize);

	/* (de)serializeWave
    Creates a new channel given the patch raw data and vice versa. */

	channel::Data        deserializeChannel(const patch::Channel& c, float samplerateRatio, int bufferSize);
	const patch::Channel serializeChannel(const channel::Data& c);

private:
	channel::State&  makeState_(ChannelType type);
	channel::Buffer& makeBuffer_(int bufferSize);

	IdManager m_channelId;

	const conf::Data& m_conf;
	model::Model&     m_model;
};
} // namespace giada::m

#endif
