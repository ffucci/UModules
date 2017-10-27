//
// Created by Francesco Fucci on 26/10/17.
//

#include "UModules.hpp"
#include "../../../include/engine.hpp"
#include "../../../include/math.hpp"
#include "dsp/digital.hpp"
#include <vector>

#define MAX_STEPS 16
#define LINES 3

struct Circular : Module {
    enum ParamIds {
        GATE_PARAM,
        NUM_PARAMS = GATE_PARAM + LINES*MAX_STEPS
    };

    enum InputIds {
        CLOCK_INPUT,
        NUM_INPUTS = CLOCK_INPUT + LINES
    };

    enum OutputIds {
        GATE_OUTPUT,
        NUM_OUTPUTS = GATE_OUTPUT + LINES
    };

    int index[LINES] = {};
    float arr_lights[LINES][MAX_STEPS] = {};
    bool gateState[LINES][MAX_STEPS] = {};
    float gateLights[LINES][MAX_STEPS] = {};

    SchmittTrigger clockTrigger[LINES];
    SchmittTrigger gateTriggers[LINES][MAX_STEPS];
    PulseGenerator gatePulse[LINES];

    float phase = 0.0;

    Circular() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) { }

    void step();


    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data

    // - onSampleRateChange: event triggered by a change of sample rate
    // - initialize, randomize: implements special behavior when user clicks these from the context menu

    void initialize() {
        for(int i = 0; i < LINES; i++){
            index[i] = 0;
            for(int k = 0; k < MAX_STEPS; k++){
                gateState[i][k] = false;
            }
        }
    }

    void advanceClock(int k,float lightLambda);
};

void Circular::advanceClock(int k,float lightLambda){
    bool nextStep = false;

    if (inputs[CLOCK_INPUT+k].active) {
        // External clock
        if (clockTrigger[k].process(inputs[CLOCK_INPUT+k].value)) {
            phase = 0.0;
            nextStep = true;
            //printf("OK next step %d\n", index[0]);
        }
    }

    if (nextStep) {
        // Advance step
        //int numSteps = clampi(roundf(params[STEPS_PARAM].value + inputs[STEPS_INPUT].value), 1, 8);
        index[k] = (index[k] + 1) % MAX_STEPS;
        arr_lights[k][index[k]] = 1.0;
        //stepLights[index] = 1.0;
        gatePulse[k].trigger(1e-3);
    }

    bool pulse = gatePulse[k].process(1.0 / gSampleRate);

    for (int i = 0; i < MAX_STEPS; i++) {
        if (gateTriggers[k][i].process(params[GATE_PARAM + i + k*MAX_STEPS].value)) {
            gateState[k][i] = !gateState[k][i];
        }
        //if (gateMode == TRIGGER)
        //else if (gateMode == RETRIGGER)
        //    gateOn = gateOn && !pulse;
        arr_lights[k][i] -= arr_lights[k][i] / lightLambda / gSampleRate;
        gateLights[k][i] = gateState[k][i] ? 1.0 - arr_lights[k][i] : arr_lights[k][i];
    }

    bool gateOn = (gateState[k][index[k]]);
    gateOn = gateOn && pulse;
    outputs[GATE_OUTPUT + k].value = gateOn ? 10.0 : 0.0;

}

void Circular::step() {
    const float lightLambda = 0.075;
    for(int i = 0; i < LINES; i++){
        advanceClock(i,lightLambda);
    }

}

CircularWidget::CircularWidget() {
    Circular *module = new Circular();
    setModule(module);
    box.size = Vec(26 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Circular.svg")));
        addChild(panel);
    }

    addInput(createInput<PJ301MPort>(Vec(313, 380-367), module, Circular::CLOCK_INPUT));
    addInput(createInput<PJ301MPort>(Vec(343, 380-367), module, Circular::CLOCK_INPUT+1));

    float radius = 70.0f;


    for(int k = 1; k < 3; k++){
        this->generateLights(module,k*radius,k);
    }

    addOutput(createOutput<PJ301MPort>(Vec(313, 347), module, Circular::GATE_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(343, 347), module, Circular::GATE_OUTPUT + 1));

/*
    for(int k = 1; k < 2; k++){
        for(int i = 0; i < MAX_STEPS; i++){
            int computeX = (int) (185.0f + radius*cos(2*i*M_PI/MAX_STEPS));
            int computeY = (int) (185.0f + radius*sin(2*i*M_PI/MAX_STEPS));
            addParam(createParam<LEDButton>(Vec(computeX+2,computeY+2), module, Circular::GATE_PARAM + i + MAX_STEPS*(k-1), 0.0, 1.0, 0.0));
            addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(computeX,computeY), &module->arr_lights[k-1][i]));
        }
    }
*/

}

void CircularWidget::generateLights(Circular* module,int radius,int row){
    if(row > LINES){
        return;
    }

    for(int i = 0; i < MAX_STEPS; i++){
        int computeX = (int) (185.0f + radius*cos(2*i*M_PI/MAX_STEPS));
        int computeY = (int) (185.0f + radius*sin(2*i*M_PI/MAX_STEPS));
        addParam(createParam<LEDButton>(Vec(computeX,computeY), module, Circular::GATE_PARAM + i + MAX_STEPS*(row-1), 0.0, 1.0, 0.0));
        if(row % 3 == 0){
            addChild(createValueLight<SmallLight<GreenValueLight>>(Vec(computeX+5,computeY+5), &module->gateLights[row-1][i]));
        }else{
            addChild(createValueLight<SmallLight<BlueValueLight>>(Vec(computeX+5,computeY+5), &module->gateLights[row-1][i]));
        }
    }
}
