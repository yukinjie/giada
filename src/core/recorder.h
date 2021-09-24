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

#ifndef G_REC_MANAGER_H
#define G_REC_MANAGER_H

#include "core/types.h"

namespace giada::m::model
{
class Model;
}

namespace giada::m
{
class ActionRecorder;
class MixerHandler;
class Sequencer;
class Recorder final
{
public:
	Recorder(model::Model&, Sequencer&, MixerHandler&);

	bool isRecording() const;
	bool isRecordingAction() const;
	bool isRecordingInput() const;

	/* canEnableRecOnSignal
    True if rec-on-signal can be enabled: can't set it while sequencer is 
    running, in order to prevent mistakes while live recording. */

	bool canEnableRecOnSignal() const;

	/* canEnableFreeInputRec
    True if free loop-length can be enabled: Can't set it if there's already a 
    filled Sample Channel in the current project. */

	bool canEnableFreeInputRec() const;

	void prepareActionRec(RecTriggerMode);
	void startActionRec();
	void startActionRecOnCallback();
	void stopActionRec(ActionRecorder&);

	bool prepareInputRec(RecTriggerMode, InputRecMode);
	void startInputRec();
	void startInputRecOnCallback();
	void stopInputRec(InputRecMode, int sampleRate);

private:
	void setRecordingAction(bool v);
	void setRecordingInput(bool v);

	model::Model& m_model;
	Sequencer&    m_sequencer;
	MixerHandler& m_mixerHandler;
};
} // namespace giada::m

#endif
