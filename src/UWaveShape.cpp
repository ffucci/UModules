//
// Created by Francesco Fucci on 22/10/17.
//

#include "UModules.hpp"
#include "../../../include/engine.hpp"
#include "../../../include/math.hpp"

#define OFFSET 170
#define OFFSET2 -5

struct UWaveShape : Module {
    enum ParamIds {
        FREQ_PARAM,
        FREQ2_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        SIGNALA1_INPUT,
        SIGNALB1_INPUT,
        WSHAPECV_INPUT,
        SIGNALA2_INPUT,
        SIGNALB2_INPUT,
        WSHAPECV2_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        WSHAPE1_OUTPUT,
        WSHAPE2_OUTPUT,
        NUM_OUTPUTS
    };

    UWaveShape() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
    void step();

    float doWaveShape(float signal1, float signal2, float freqratio, bool sine=true){
        float outSignal = 0.0f;
        if(sine){
            outSignal = signal1 * sinf(freqratio*signal2);
        }else{
            outSignal = signal1 * sinf(freqratio*signal2 + 90.0f);
        }
        return outSignal;
    }

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data

    // - onSampleRateChange: event triggered by a change of sample rate
    // - initialize, randomize: implements special behavior when user clicks these from the context menu
};

void UWaveShape::step() {
    float elementA = inputs[SIGNALA1_INPUT].value;
    float elementB = inputs[SIGNALB1_INPUT].value;
    float elementA2 = inputs[SIGNALA2_INPUT].value;
    float elementB2 = inputs[SIGNALB2_INPUT].value;

    float freqRatio = params[FREQ_PARAM].value;
    freqRatio += inputs[WSHAPECV_INPUT].value;

    float freqRatio2 = params[FREQ2_PARAM].value;
    freqRatio2 += inputs[WSHAPECV2_INPUT].value;

    float outputA = doWaveShape(elementA,elementB,freqRatio);
    float outputB = doWaveShape(elementA2,elementB2,freqRatio2);

    outputs[WSHAPE1_OUTPUT].value = outputA;
    outputs[WSHAPE2_OUTPUT].value = outputB;
}

UWaveShapeWidget::UWaveShapeWidget(){
    UWaveShape *module = new UWaveShape();
    setModule(module);
    box.size = Vec(4 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/UWaveShape.svg")));
        addChild(panel);
    }


    addChild(createScrew<ScrewSilver>(Vec(15, 0)));
    addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 0)));
    addChild(createScrew<ScrewSilver>(Vec(15, 365)));
    addChild(createScrew<ScrewSilver>(Vec(box.size.x - 30, 365)));

    //First block input
    addInput(createInput<PJ301MPort>(Vec(6, 30 + OFFSET2), module, UWaveShape::SIGNALA1_INPUT));
    addInput(createInput<PJ301MPort>(Vec(30, 100 + OFFSET2), module, UWaveShape::SIGNALB1_INPUT));
    addParam(createParam<RoundBlackKnob>(Vec(4, 135 + OFFSET2), module, UWaveShape::FREQ_PARAM, 0.0, 4.0, 0.0));
    addInput(createInput<PJ301MPort>(Vec(32, 135 + OFFSET2), module, UWaveShape::WSHAPECV_INPUT));
    addOutput(createOutput<PJ301MPort>(Vec(16, 175 + OFFSET2), module, UWaveShape::WSHAPE1_OUTPUT));
    //addOutput(createOutput<PJ301MPort>(Vec(16, 255), module, UWaveShape::COMPLESS_OUTPUT));
    //addOutput(createOutput<PJ301MPort>(Vec(16, 285), module, UWaveShape::COMPIN_OUTPUT));

    addInput(createInput<PJ301MPort>(Vec(6, 30+OFFSET), module, UWaveShape::SIGNALA2_INPUT));
    addInput(createInput<PJ301MPort>(Vec(30, 100+OFFSET), module, UWaveShape::SIGNALB2_INPUT));
    addParam(createParam<RoundBlackKnob>(Vec(4, 135+OFFSET), module, UWaveShape::FREQ2_PARAM, 0.0, 4.0, 0.0));
    addInput(createInput<PJ301MPort>(Vec(32, 135+OFFSET), module, UWaveShape::WSHAPECV2_INPUT));
    addOutput(createOutput<PJ301MPort>(Vec(16, 175+OFFSET), module, UWaveShape::WSHAPE2_OUTPUT));

}
