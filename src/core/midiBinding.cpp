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

#include "midiBinding.h"
#include "midiMsg.h"
#include "midiMsgFilter.h"
#include "utils/log.h"
#include "utils/vector.h"
#include <string>
#include <vector>
#include <map>

namespace giada {
namespace m {

//------------------------------  CONSTRUCTORS  --------------------------------

MidiBinding::MidiBinding(const MB_MODE& mode, const MidiMsg& mm) {

	m_mode = mode;

	if (MMF_NOTEONOFF << mm) {
		m_mmf = MMF_NOTEONOFF & 
			MMF_Channel(mm.getChannel()) &
			MMF_Note(mm.getNote()) &
			MMF_Sender(mm.getMessageSender());
	}

	if (MMF_CC << mm) {
		m_mmf = MMF_CC & 
			MMF_Channel(mm.getChannel()) &
			MMF_Param(mm.getParam()) &
			MMF_Sender(mm.getMessageSender());
	}
}

//----------------------------  MEMBER FUNCTIONS  ------------------------------

int MidiBinding::check(const MidiMsg& mm) const {
	
	if (m_mmf << mm) {
		switch (m_mode){
			case MB_BUTTON:
				return (MMF_BOOL << mm) ? 1 : 0;	
			case MB_POT:
				return mm.getValue();
		}
	}

	return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool MidiBinding::check(const MidiMsg& mm, unsigned char& value) const {
	int o = check(mm);
	if (o >= 0) {
		value = o;
		return true;
	}
	return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void MidiBinding::dump() const {
	
	u::log::print("[MB:dump] Binding mode: %d\n[MB::dump]", m_mode);
	m_mmf.dump();
}

//----------------------------  FRIEND FUNCTIONS  ------------------------------

void to_json(nl::json& j, const MidiBinding& mb){
	j = nl::json{{"mode", mb.m_mode}, {"mmf", mb.m_mmf}};
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void from_json(nl::json& j, MidiBinding& mb) {
	j.at("mode").get_to(mb.m_mode);
	j.at("mmf").get_to(mb.m_mmf);
}

//-------------------------- PRIVATE MEMBER FUNCTIONS --------------------------


//------------------------------ OTHER FUNCTIONS -------------------------------


}} // giada::m::
