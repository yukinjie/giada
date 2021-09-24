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

#include "tabMidi.h"
#include "core/conf.h"
#include "core/const.h"
#include "core/kernelMidi.h"
#include "core/midiMap.h"
#include "gui/elems/basics/box.h"
#include "gui/elems/basics/check.h"
#include "gui/elems/basics/choice.h"
#include "utils/gui.h"
#include <RtMidi.h>
#include <string>

extern giada::m::KernelMidi    g_kernelMidi;
extern giada::m::conf::Data    g_conf;
extern giada::m::midiMap::Data g_midiMap;

namespace giada::v
{
geTabMidi::geTabMidi(int X, int Y, int W, int H)
: Fl_Group(X, Y, W, H, "MIDI")
{
	begin();
	system  = new geChoice(x() + w() - 250, y() + 9, 250, 20, "System");
	portOut = new geChoice(x() + w() - 250, system->y() + system->h() + 8, 250, 20, "Output port");
	portIn  = new geChoice(x() + w() - 250, portOut->y() + portOut->h() + 8, 250, 20, "Input port");
	midiMap = new geChoice(x() + w() - 250, portIn->y() + portIn->h() + 8, 250, 20, "Output Midi Map");
	sync    = new geChoice(x() + w() - 250, midiMap->y() + midiMap->h() + 8, 250, 20, "Sync");
	new geBox(x(), sync->y() + sync->h() + 8, w(), h() - 150, "Restart Giada for the changes to take effect.");
	end();

	labelsize(G_GUI_FONT_SIZE_BASE);
	selection_color(G_COLOR_GREY_4);

	system->callback(cb_changeSystem, (void*)this);

	fetchSystems();
	fetchOutPorts();
	fetchInPorts();
	fetchMidiMaps();

	sync->add("(disabled)");
	sync->add("MIDI Clock (master)");
	sync->add("MTC (master)");
	if (g_conf.midiSync == MIDI_SYNC_NONE)
		sync->value(0);
	else if (g_conf.midiSync == MIDI_SYNC_CLOCK_M)
		sync->value(1);
	else if (g_conf.midiSync == MIDI_SYNC_MTC_M)
		sync->value(2);

	systemInitValue = system->value();
}

/* -------------------------------------------------------------------------- */

void geTabMidi::fetchOutPorts()
{
	if (g_kernelMidi.countOutPorts() == 0)
	{
		portOut->add("-- no ports found --");
		portOut->value(0);
		portOut->deactivate();
	}
	else
	{

		portOut->add("(disabled)");

		for (unsigned i = 0; i < g_kernelMidi.countOutPorts(); i++)
			portOut->add(u::gui::removeFltkChars(g_kernelMidi.getOutPortName(i)).c_str());

		portOut->value(g_conf.midiPortOut + 1); // +1 because midiPortOut=-1 is '(disabled)'
	}
}

/* -------------------------------------------------------------------------- */

void geTabMidi::fetchInPorts()
{
	if (g_kernelMidi.countInPorts() == 0)
	{
		portIn->add("-- no ports found --");
		portIn->value(0);
		portIn->deactivate();
	}
	else
	{

		portIn->add("(disabled)");

		for (unsigned i = 0; i < g_kernelMidi.countInPorts(); i++)
			portIn->add(u::gui::removeFltkChars(g_kernelMidi.getInPortName(i)).c_str());

		portIn->value(g_conf.midiPortIn + 1); // +1 because midiPortIn=-1 is '(disabled)'
	}
}

/* -------------------------------------------------------------------------- */

void geTabMidi::fetchMidiMaps()
{
	if (g_midiMap.maps.size() == 0)
	{
		midiMap->add("(no MIDI maps available)");
		midiMap->value(0);
		midiMap->deactivate();
		return;
	}

	int i = 0;
	for (const std::string& imap : g_midiMap.maps)
	{
		midiMap->add(imap.c_str());
		if (g_conf.midiMapPath == imap)
			midiMap->value(i);
		i++;
	}

	/* Preselect the 0-th midiMap if nothing is selected but midiMaps exist. */

	if (midiMap->value() == -1 && g_midiMap.maps.size() > 0)
		midiMap->value(0);
}

/* -------------------------------------------------------------------------- */

void geTabMidi::save()
{
	std::string text = system->text(system->value());

	if (text == "ALSA")
		g_conf.midiSystem = RtMidi::LINUX_ALSA;
	else if (text == "Jack")
		g_conf.midiSystem = RtMidi::UNIX_JACK;
	else if (text == "Multimedia MIDI")
		g_conf.midiSystem = RtMidi::WINDOWS_MM;
	else if (text == "OSX Core MIDI")
		g_conf.midiSystem = RtMidi::MACOSX_CORE;

	g_conf.midiPortOut = portOut->value() - 1; // -1 because midiPortOut=-1 is '(disabled)'
	g_conf.midiPortIn  = portIn->value() - 1;  // -1 because midiPortIn=-1 is '(disabled)'
	g_conf.midiMapPath = g_midiMap.maps.size() == 0 ? "" : midiMap->text(midiMap->value());

	if (sync->value() == 0)
		g_conf.midiSync = MIDI_SYNC_NONE;
	else if (sync->value() == 1)
		g_conf.midiSync = MIDI_SYNC_CLOCK_M;
	else if (sync->value() == 2)
		g_conf.midiSync = MIDI_SYNC_MTC_M;
}

/* -------------------------------------------------------------------------- */

void geTabMidi::fetchSystems()
{
#if defined(__linux__)

	if (g_kernelMidi.hasAPI(RtMidi::LINUX_ALSA))
		system->add("ALSA");
	if (g_kernelMidi.hasAPI(RtMidi::UNIX_JACK))
		system->add("Jack");

#elif defined(__FreeBSD__)

	if (g_kernelMidi.hasAPI(RtMidi::UNIX_JACK))
		system->add("Jack");

#elif defined(_WIN32)

	if (g_kernelMidi.hasAPI(RtMidi::WINDOWS_MM))
		system->add("Multimedia MIDI");

#elif defined(__APPLE__)

	system->add("OSX Core MIDI");

#endif

	switch (g_conf.midiSystem)
	{
	case RtMidi::LINUX_ALSA:
		system->showItem("ALSA");
		break;
	case RtMidi::UNIX_JACK:
		system->showItem("Jack");
		break;
	case RtMidi::WINDOWS_MM:
		system->showItem("Multimedia MIDI");
		break;
	case RtMidi::MACOSX_CORE:
		system->showItem("OSX Core MIDI");
		break;
	default:
		system->value(0);
		break;
	}
}

/* -------------------------------------------------------------------------- */

void geTabMidi::cb_changeSystem(Fl_Widget* /*w*/, void* p) { ((geTabMidi*)p)->cb_changeSystem(); }

/* -------------------------------------------------------------------------- */

void geTabMidi::cb_changeSystem()
{
	/* if the user changes MIDI device (eg ALSA->JACK) device menu deactivates.
	 * If it returns to the original system, we re-fill the list by
	 * querying m::kernelMidi. */

	if (systemInitValue == system->value())
	{
		portOut->clear();
		fetchOutPorts();
		portOut->activate();
		portIn->clear();
		fetchInPorts();
		portIn->activate();
		sync->activate();
	}
	else
	{
		portOut->deactivate();
		portOut->clear();
		portOut->add("-- restart to fetch device(s) --");
		portOut->value(0);
		portIn->deactivate();
		portIn->clear();
		portIn->add("-- restart to fetch device(s) --");
		portIn->value(0);
		sync->deactivate();
	}
}
} // namespace giada::v
