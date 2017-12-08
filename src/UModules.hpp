#include "../../../include/rack.hpp"
#include "../../../include/plugin.hpp"


using namespace rack;


extern Plugin *plugin;

struct Circular;
////////////////////
// module widgets
////////////////////

struct CompareWidget : ModuleWidget {
	CompareWidget();
};

struct UWaveShapeWidget : ModuleWidget{
	UWaveShapeWidget();
};

struct CircularWidget : ModuleWidget{
	CircularWidget();
	void generateLights(Circular* module,int radius,int row);
};