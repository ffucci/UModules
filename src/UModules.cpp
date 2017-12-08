#include "UModules.hpp"


// The plugin-wide instance of the Plugin class
Plugin *plugin;

void init(rack::Plugin *p) {

	plugin = p;
	p->slug = "UModules";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif
	p->website = "https://github.com/ffucci/UModules";

	p->addModel(createModel<UWaveShapeWidget>("UModules", "UWaveShape", "UWaveShape", EFFECT_TAG));
	p->addModel(createModel<CompareWidget>("UModules", "Compare", "UCompare", EFFECT_TAG));
	p->addModel(createModel<CircularWidget>("UModules", "Circular", "UCircular", SEQUENCER_TAG));
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables within this file or the individual module files to reduce startup times of Rack.
}
