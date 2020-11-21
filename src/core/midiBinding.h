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


#ifndef G_MIDIBINDING_H
#define G_MIDIBINDING_H

#include <vector>
#include "midiMsg.h"
#include "midiMsgFilter.h"

#include "deps/json/single_include/nlohmann/json.hpp"
namespace nl = nlohmann;

namespace giada {
namespace m {

// Aaand we ended up with a fancy midiMsgFilter wrapper for now.
// However, this will grow to support modifiers in the future!

				// By default the return value is -1.
enum MB_MODE {	MB_BUTTON,	// Returns 1 when pressed and 0 when released
		MB_POT};	// Returns pot value

class MidiBinding
{
	public:

	MidiBinding(const MB_MODE& mode, const MidiMsg& mms);

	// Check whether MidiMsg triggers a binding
	// returns -1 if not
	// Returns 1 on button press
	// returns 0 on button release
	// Returns pot CC value on pot action
	// Bool variant places result in a supplied variable if triggered. 
	int			check(const MidiMsg& mm) const;
	bool			check(const MidiMsg& mm,
						unsigned char& value) const;
	void			dump() const;

	friend void		to_json(nl::json& j, const MidiBinding& mb);
	friend void		from_json(nl::json& j, MidiBinding& mb);

	private:

	// m_mmf - an action-triggering button or pot filter
	// m_mode - a binding type
	MidiMsgFilter				m_mmf;
	MB_MODE					m_mode;

};

// Friendly json serializer and deserializer
void		to_json(nl::json& j, const MidiBinding& mb);
void		from_json(nl::json& j, MidiBinding& mb);

}} // giada::m

#endif
