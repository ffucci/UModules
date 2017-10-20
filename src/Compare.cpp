#include "UModules.hpp"
#include "../../../include/engine.hpp"
#include "OscillatorLib.hpp"
#include <math.h>

struct Compare : Module {
	enum ParamIds {
		OFFSET_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		SIGNALA1_INPUT,
		SIGNALB1_INPUT,
		SIGNALOFF_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		COMP1_OUTPUT,
		COMPLESS_OUTPUT,
		COMPIN_OUTPUT,
		NUM_OUTPUTS
	};

	float phase = 0.0;

	Compare() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step();

	float computeGreaterThan(float elementA, float elementB, float offset);
	float computeLessThan(float elementA, float elementB, float offset);
	float computeInterval(float elementA, float elementB, float offset, float range);

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - initialize, randomize: implements special behavior when user clicks these from the context menu
};

#define MAX 5.0f
#define MIN -5.0f

void Compare::step() {
	float elementA = inputs[SIGNALA1_INPUT].value;
	float elementB = inputs[SIGNALB1_INPUT].value;
	float offset = params[OFFSET_PARAM].value;
	float offsetSig = inputs[SIGNALOFF_INPUT].value + offset;
	outputs[COMP1_OUTPUT].value = computeGreaterThan(elementA,elementB,offsetSig);
	outputs[COMPLESS_OUTPUT].value = computeLessThan(elementA,elementB,offsetSig);
	outputs[COMPIN_OUTPUT].value = computeInterval(elementA,elementB,offsetSig,1.5f);
}

float Compare::computeGreaterThan(float elementA, float elementB, float offset) {
	float computedValue = 0.0f;
	if(offset < 0.5 && offset > -0.5f){
		computedValue = elementA > elementB ? elementA : elementB;
	}else{
		//Maybe it's better a wavefolding mode
		float compOut = 0.5*elementA + 0.5*elementB + offset;
		if(compOut > MAX){
			computedValue = 2*MAX - compOut;
		}else if(compOut < MIN){
			computedValue = -2*MIN + compOut;
		}else{
			if(elementA > elementB){
				float elOut = elementA + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}else{
				float elOut = elementB + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}

		}

	}
	return computedValue;
}

float Compare::computeLessThan(float elementA, float elementB, float offset) {
	float computedValue = 0.0f;
	if(offset < 0.5 && offset > -0.5f){
		computedValue = elementA < elementB ? elementA : elementB;
	}else{
		//Maybe it's better a wavefolding mode
		float compOut = 0.5*elementA + 0.5*elementB + offset;
		if(compOut > MAX){
			computedValue = 2*MAX - compOut;
		}else if(compOut < MIN){
			computedValue = -2*MIN + compOut;
		}else{
			if(elementA < elementB){
				float elOut = elementA + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}else{
				float elOut = elementB + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}
		}

	}
	return computedValue;
}

float Compare::computeInterval(float elementA, float elementB, float offset, float range) {
	float computedValue = 0.0f;
	if(offset < 0.5 && offset > -0.5f){
		float aVal = fabsf(elementA - elementB);
		computedValue = aVal <= range ? elementA : elementB;
	}else{
		//Maybe it's better a wavefolding mode
		float compOut = 0.5*elementA + 0.5*elementB + offset;
		if(compOut > MAX){
			computedValue = 2*MAX - compOut;
		}else if(compOut < MIN){
			computedValue = -2*MIN + compOut;
		}else{
			float aVal = fabsf(elementA - elementB);

			if(aVal <= range){
				float elOut = elementA + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}else{
				float elOut = elementB + offset;
				if(elOut > MAX){
					computedValue = 2*MAX - elOut;
				}else if(elementA + offset < MIN){
					computedValue = -2*MIN + elOut;
				}else{
					computedValue = elOut;
				}
			}
		}

	}
	return computedValue;

}

CompareWidget::CompareWidget() {
	Compare *module = new Compare();
	setModule(module);
	box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Compare.svg")));
		addChild(panel);
	}


	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 365)));

//First block input
	addInput(createInput<PJ301MPort>(Vec(5, 45), module, Compare::SIGNALA1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(30, 45), module, Compare::SIGNALB1_INPUT));
	addInput(createInput<PJ301MPort>(Vec(16, 145), module, Compare::SIGNALOFF_INPUT));

	addParam(createParam<Davies1900hBlackKnob>(Vec(12, 95), module, Compare::OFFSET_PARAM, -5.0, 5.0, 0.0));
	addOutput(createOutput<PJ301MPort>(Vec(16, 225), module, Compare::COMP1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(16, 255), module, Compare::COMPLESS_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(16, 285), module, Compare::COMPIN_OUTPUT));


/*
	addParam(createParam<Davies1900hBlackKnob>(Vec(28, 87), module, Compare::PITCH_PARAM, -3.0, 3.0, 0.0));

	addInput(createInput<PJ301MPort>(Vec(33, 186), module, Compare::PITCH_INPUT));

	addOutput(createOutput<PJ301MPort>(Vec(20, 275), module, Compare::SINE_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(50,275),module, Compare::SAW_OUTPUT));*/
}
