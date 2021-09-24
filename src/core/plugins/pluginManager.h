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

#ifdef WITH_VST

#ifndef G_PLUGIN_MANAGER_H
#define G_PLUGIN_MANAGER_H

#include "core/idManager.h"
#include "deps/juce-config.h"
#include "plugin.h"

namespace giada::m::patch
{
struct Plugin;
struct Version;
} // namespace giada::m::patch

namespace giada::m::model
{
class Model;
}

namespace giada::m
{
class PluginManager final
{
public:
	enum class SortMethod : int
	{
		NAME = 0,
		CATEGORY,
		MANUFACTURER,
		FORMAT
	};

	struct PluginInfo
	{
		std::string uid;
		std::string name;
		std::string category;
		std::string manufacturerName;
		std::string format;
		bool        isInstrument;
	};

	PluginManager(SortMethod);

	/* getAvailablePluginInfo
	Returns the available plugin information (name, type, ...) given a plug-in
	index. */

	PluginInfo getAvailablePluginInfo(int index) const;

	std::string getUnknownPluginInfo(int index) const;
	bool        doesPluginExist(const std::string& pid) const;
	bool        hasMissingPlugins() const;

	/* countAvailablePlugins
	Returns how many plug-ins are ready and available for usage. */

	int countAvailablePlugins() const;

	/* countUnknownPlugins
	Returns how many plug-ins are in a unknown/not-found state. */

	int countUnknownPlugins() const;

	/* reset
	Brings everything back to the initial state. */

	void reset(SortMethod);

	/* scanDirs
	Parses plugin directories (semicolon-separated) and store list in 
	knownPluginList. The callback is called on each plugin found. Used to update
	the main window from the GUI thread. */

	int scanDirs(const std::string& paths, const std::function<void(float)>& cb);

	/* (save|load)List
	(Save|Load) knownPluginList (in|from) an XML file. */

	bool saveList(const std::string& path) const;
	bool loadList(const std::string& path);

	std::unique_ptr<Plugin> makePlugin(const std::string& pid, int sampleRate, int bufferSize, ID id = 0);
	std::unique_ptr<Plugin> makePlugin(int index, int sampleRate, int bufferSize);
	std::unique_ptr<Plugin> makePlugin(const Plugin& other, int sampleRate, int bufferSize);

	/* (de)serializePlugin
	Transforms patch data into a Plugin object and vice versa. */

	const patch::Plugin     serializePlugin(const Plugin& p) const;
	std::unique_ptr<Plugin> deserializePlugin(const patch::Plugin& p, patch::Version version, int sampleRate, int bufferSize);
	std::vector<Plugin*>    hydratePlugins(std::vector<ID> pluginIds, model::Model& model);

	void sortPlugins(SortMethod sortMethod);

private:
	std::unique_ptr<Plugin> makeInvalidPlugin(const std::string& pid, ID id);

	IdManager m_pluginId;

	/* formatManager
	Plugin format manager. */

	juce::AudioPluginFormatManager m_formatManager;

	/* knownPuginList
	List of known (i.e. scanned) plugins. */

	juce::KnownPluginList m_knownPluginList;

	/* unknownPluginList
	List of unrecognized plugins found in a patch. */

	std::vector<std::string> m_unknownPluginList;

	/* missingPlugins
	If some plugins from any stack are missing. */

	bool m_missingPlugins;
};
} // namespace giada::m

#endif

#endif // #ifdef WITH_VST
