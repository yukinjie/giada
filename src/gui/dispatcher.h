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

#ifndef G_V_DISPATCHER_H
#define G_V_DISPATCHER_H

#include "core/types.h"
#include <functional>

namespace giada::v
{
class geChannel;
class Dispatcher final
{
public:
	Dispatcher();

	/* dispatchKey
    Processes a key pressed on the physical keyboard. */

	void dispatchKey(int event);

	/* dispatchTouch
    Processes a mouse click/touch event. */

	void dispatchTouch(const geChannel& gch, bool status);

	/* onEventOccured
    Callback fired when a key has been pressed or a mouse button clicked. */

	std::function<void()> onEventOccured;

private:
	void perform(ID channelId, int event) const;

	/* dispatchChannels
    Walks channels array, trying to match button's bound key with the event. If 
    found, trigger the key-press/key-release function. */

	void dispatchChannels(int event) const;

	bool m_backspace;
	bool m_end;
	bool m_enter;
	bool m_space;
	bool m_esc;
	bool m_key;
};
} // namespace giada::v

#endif