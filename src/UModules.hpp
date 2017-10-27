#include "../../../include/rack.hpp"
#include "../../../include/plugin.hpp"


using namespace rack;


extern Plugin *plugin;

struct Circular;

struct BlueValueLight : ColorValueLight {
	BlueValueLight() {
		baseColor = nvgRGB(25, 150, 252);
	}
};

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