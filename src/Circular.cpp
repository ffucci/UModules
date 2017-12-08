//
// Created by Francesco Fucci on 26/10/17.
//

#include "UModules.hpp"
#include "../../../include/engine.hpp"
#include "../../../include/math.hpp"
#include "dsp/digital.hpp"
#include <vector>

#define MAX_STEPS 16
#define LINES 2

struct Circular : Module {
    enum ParamIds {
        RESET_PARAM,
        RUN_PARAM,
        GATE_PARAM,
        AUTOROTATE_PARAM,
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

    enum LightIds {
        RUNNING_LIGHT,
        RESET_LIGHT,
        AUTOROTATE_LIGHT,
        GATE_LIGHTS,
        NUM_LIGHTS = GATE_LIGHTS + LINES*MAX_STEPS
    };


    int index[LINES] = {};
    bool nextStep[LINES] = {};

    float arr_lights[LINES][MAX_STEPS] = {};
    bool gateState[LINES][MAX_STEPS] = {};
    float gateLights[LINES][MAX_STEPS] = {};

    SchmittTrigger clockTrigger[LINES];
    SchmittTrigger gateTriggers[LINES][MAX_STEPS];
    PulseGenerator gatePulse[LINES];

    SchmittTrigger resetTrigger;
    float resetLight;
    bool resetLines = false;


    // For buttons
    SchmittTrigger runningTrigger;
    bool running = true;
    float phase = 0.0;

    Circular() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS,NUM_LIGHTS) { }

    void step();


    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data

    // - onSampleRateChange: event triggered by a change of sample rate
    // - initialize, randomize: implements special behavior when user clicks these from the context menu

    /*
    void initialize() {
        for(int i = 0; i < LINES; i++){
            index[i] = 0;
            for(int k = 0; k < MAX_STEPS; k++){
                gateState[i][k] = false;
            }
        }
    }*/

    json_t *toJson() {
        json_t *rootJ = json_object();

        // running
        //json_object_set_new(rootJ, "running", json_boolean(running));

        // gates
        json_t *gatesJ = json_array();
        for(int k = 0; k < LINES; k++){
            for (int i = 0; i < MAX_STEPS; i++) {
                json_t *gateJ = json_integer((int) gateState[k][i]);
                json_array_append_new(gatesJ, gateJ);
            }
        }

        json_object_set_new(rootJ, "gates", gatesJ);

        // gateMode
        //json_t *gateModeJ = json_integer((int) gateMode);
        //json_object_set_new(rootJ, "gateMode", gateModeJ);

        return rootJ;
    }

    void fromJson(json_t *rootJ){
        // gates
        json_t *gatesJ = json_object_get(rootJ, "gates");
        if (gatesJ) {
            for(int k = 0; k < LINES; k++){
                for (int i = 0; i < MAX_STEPS; i++) {
                    json_t *gateJ = json_array_get(gatesJ, i + k*MAX_STEPS);
                    if (gateJ)
                        gateState[k][i] = !!json_integer_value(gateJ);
                }
            }

        }
    }

    void advanceClock(int k,float lightLambda);

    void rotatePattern(int shift){}
};

void Circular::advanceClock(int k,float lightLambda){
    nextStep[k] = false;

    if (inputs[CLOCK_INPUT+k].active) {
        // External clock
        if (clockTrigger[k].process(inputs[CLOCK_INPUT+k].value)) {
            phase = 0.0;
            nextStep[k] = true;
            //printf("OK next step %d\n", index[0]);
        }
    }

    if(resetLines){
        index[k] = MAX_STEPS;
        resetLines = false;
    }

    // Reset
    if (resetTrigger.process(params[RESET_PARAM].value)) {
        phase = 0.0;
        index[k] = MAX_STEPS;
        nextStep[k] = true;
        resetLight = 1.0;
        resetLines = true;
    }


    if (nextStep[k]) {
        // Advance step
        //int numSteps = clampi(roundf(params[STEPS_PARAM].value + inputs[STEPS_INPUT].value), 1, 8);
        index[k] = (index[k] + 1) % MAX_STEPS;
        arr_lights[k][index[k]] = 1.0;
        //stepLights[index] = 1.0;
        gatePulse[k].trigger(1e-3);
    }

    resetLight -= resetLight / lightLambda / engineGetSampleRate();
    bool pulse = gatePulse[k].process(1.0 / engineGetSampleRate());

    for (int i = 0; i < MAX_STEPS; i++) {
        if (gateTriggers[k][i].process(params[GATE_PARAM + i + k*MAX_STEPS].value)) {
            gateState[k][i] = !gateState[k][i];
        }

        arr_lights[k][i] -= arr_lights[k][i] / lightLambda / engineGetSampleRate();
        lights[GATE_LIGHTS + i + k*MAX_STEPS].value = gateState[k][i] ? 1.0 - arr_lights[k][i] : arr_lights[k][i];
    }

    bool gateOn = (gateState[k][index[k]]);
    gateOn = gateOn && pulse;
    outputs[GATE_OUTPUT + k].value = gateOn ? 10.0 : 0.0;

}

void Circular::step() {
    const float lightLambda = 0.075;
    if (runningTrigger.process(params[RUN_PARAM].value)) {
        running = !running;
    }
    lights[RUNNING_LIGHT].value = running ? 1.0 : 0.0;

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


    //This generates the circular lights
    float radius = 70.0f;
    for(int k = 1; k < 3; k++){
        this->generateLights(module,k*radius,k);
    }

    addOutput(createOutput<PJ301MPort>(Vec(313, 347), module, Circular::GATE_OUTPUT));
    addOutput(createOutput<PJ301MPort>(Vec(343, 347), module, Circular::GATE_OUTPUT + 1));

    addParam(createParam<LEDButton>(Vec(10, 70), module, Circular::RUN_PARAM, 0.0, 1.0, 0.0));
    addChild(createLight<SmallLight<GreenLight>>(Vec(15, 75), module, Circular::RUNNING_LIGHT));

    addParam(createParam<LEDButton>(Vec(10, 33), module, Circular::RESET_PARAM, 0.0, 1.0, 0.0));
    addChild(createLight<SmallLight<GreenLight>>(Vec(15, 38), module,Circular::RESET_LIGHT));

}

void CircularWidget::generateLights(Circular* module,int radius,int row){
    if(row > LINES){
        return;
    }

    for(int i = 0; i < MAX_STEPS; i++){
        int computeX = (int) (185.0f + radius*cos(2*i*M_PI/MAX_STEPS - M_PI/2));
        int computeY = (int) (185.0f + radius*sin(2*i*M_PI/MAX_STEPS - M_PI/2));
        addParam(createParam<LEDButton>(Vec(computeX,computeY), module, Circular::GATE_PARAM + i + MAX_STEPS*(row-1), 0.0, 1.0, 0.0));
        addChild(createLight<SmallLight<BlueLight>>(Vec(computeX+5,computeY+5), module , Circular::GATE_LIGHTS + i + MAX_STEPS*(row-1)));
    }
}
