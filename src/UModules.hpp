#include "../../../include/rack.hpp"
#include "../../../include/plugin.hpp"


using namespace rack;


extern Plugin *plugin;

////////////////////
// module widgets
////////////////////

struct CompareWidget : ModuleWidget {
	CompareWidget();
};

struct UWaveShapeWidget : ModuleWidget{
	UWaveShapeWidget();
};