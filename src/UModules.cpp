#include "UModules.hpp"


// The plugin-wide instance of the Plugin class
Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	plugin->slug = "UModules";
	plugin->name = "UModules";
	//plugin->homepageUrl = "https://github.com/VCVRack/SimpleModule";

	createModel<CompareWidget>(plugin, "Compare", "UModules Compare");
	createModel<UWaveShapeWidget>(plugin, "UWaveShape", "UModules Waveshape");

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables within this file or the individual module files to reduce startup times of Rack.
}
