/*
    src/example4.cpp -- C++ version of an example application that shows
    how to use the OpenGL widget. For a Python implementation, see
    '../python/example4.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include "planet.h"

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/glcanvas.h>
#include <iostream>
#include <string>



using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;


class MyGLCanvas : public nanogui::GLCanvas {
public:
    MyGLCanvas(Widget *parent, Planet ** _planet) : nanogui::GLCanvas(parent) {
        *_planet = planet = new Planet();
    }

    ~MyGLCanvas() {}

    virtual void drawGL() override {
        planet->planetDraw();

        //the widgets dotn't draw well if I dont return this back to GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void toggleNormals(bool state) {
        planet->toggleNormals(state);
    }

    void toggleOcean(bool state) {
        planet->toggleOcean(state);
    }

    void toggleWireframe(bool state) {
        planet->toggleWireframe(state);
    }

    int randomizePlanet(int detalization, float frequency, float amplitude) {
        return planet->randomizePlanet(detalization, frequency, amplitude);
    }

    void setFrequency(float frequency) {
        planet->setNoiseFrequency(frequency);
    }
    
    void setAmplitude(float amplitude) {
        planet->setNoiseAmplitude(amplitude);
    }

    void setDetalization(int detalization) {
        planet->setDetalization(detalization);
    }

    void setNoiseSeed(int seed) {
        planet->setNoiseSeed(seed);
    }

private:
    Planet *planet;
};


class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(1329, 889), "NanoGUI Test", false) {
        using namespace nanogui;

        Window *window = new Window(this, "Procedural Planets");
        window->setPosition(Vector2i(0, 0));
        window->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 5));//new GroupLayout());

        mCanvas = new MyGLCanvas(window, &planet);
        mCanvas->setBackgroundColor({100, 100, 100, 255});
        mCanvas->setSize({1000, 800});


        //Tools widget
        Widget *toolsWindow = new Window(this, "Tools");
        //toolsWindow->setFixedSize(Vector2i(300,300));
        toolsWindow->setPosition(Vector2i(1005, 0));
        toolsWindow->setLayout(new GridLayout(Orientation::Horizontal, 1 , Alignment::Minimum, 0, 5));

        new Label(toolsWindow, "Planet Parameters", "sans-bold", 20);
        Widget *planetWidget = new Widget(toolsWindow);
        planetWidget->setLayout(new GridLayout(Orientation::Horizontal, 2 , Alignment::Minimum, 0, 5));
        
        /* detalization factor */ 
        new Label(planetWidget, "Planet Detalization :", "sans");
        auto intBox = new IntBox<int>(planetWidget);
        intBox->setEditable(true);
        intBox->setFixedSize(Vector2i(70, 20));
        intBox->setValue(5);
        intBox->setUnits("ui");
        intBox->setDefaultValue("5");
        intBox->setFontSize(16);
        intBox->setFormat("[1-9][0-9]*");
        intBox->setSpinnable(true);
        intBox->setMinValue(0);
        intBox->setValueIncrement(1);
        intBox->setCallback([this](int value) { mCanvas->setDetalization(value); return true; });

        /* FP frequency */ 
        new Label(planetWidget, "Noise Frequency:", "sans");
        auto frequencyTb = new FloatBox<float>(planetWidget);
        frequencyTb->setEditable(true);
        frequencyTb->setFixedSize(Vector2i(70, 20));
        frequencyTb->setValue(1.0f);
        frequencyTb->setUnits("uf");
        frequencyTb->setDefaultValue("1.0");
        frequencyTb->setFontSize(16);
        frequencyTb->setFormat("[0-9]*\\.?[0-9]+");
        frequencyTb->setSpinnable(true);
        frequencyTb->setMinValue(0);
        frequencyTb->setValueIncrement(0.1f);
        frequencyTb->setCallback([this](float value) { mCanvas->setFrequency(value); return true; });

        /* FP amplitude */ 
        new Label(planetWidget, "Noise Amplitude:", "sans");
        auto amplitudeTb = new FloatBox<float>(planetWidget);
        amplitudeTb->setEditable(true);
        amplitudeTb->setFixedSize(Vector2i(70, 20));
        amplitudeTb->setValue(0.6f);
        amplitudeTb->setUnits("uf");
        amplitudeTb->setDefaultValue("0.6");
        amplitudeTb->setFontSize(16);
        //amplitudeTb->setFormat("[-]?[0-9]*\\.?[0-9]+");
        amplitudeTb->setFormat("[0-9]*\\.?[0-9]+");
        amplitudeTb->setSpinnable(true);
        amplitudeTb->setMinValue(0);
        amplitudeTb->setValueIncrement(0.1f);
        amplitudeTb->setCallback([this](float value) { mCanvas->setAmplitude(value); return true; });    

        /* noise seed */ 
        new Label(planetWidget, "Noise seed :", "sans");
        auto seedIB = new IntBox<int>(planetWidget);
        seedIB->setEditable(true);
        seedIB->setFixedSize(Vector2i(70, 20));
        seedIB->setValue(1);
        seedIB->setUnits("ui");
        seedIB->setDefaultValue("1");
        seedIB->setFontSize(16);
        seedIB->setFormat("[1-9][0-9]*");
        seedIB->setCallback([this](int value) { mCanvas->setNoiseSeed(value); return true; });
    
        Button *genPlanetBtn = new Button(toolsWindow, "RandomizePlanet");
        genPlanetBtn->setCallback([this, intBox, frequencyTb, amplitudeTb, seedIB]() { int newSeed = mCanvas->randomizePlanet(intBox->value(), frequencyTb->value(), amplitudeTb->value());  seedIB->setValue(newSeed); });

        new Label(toolsWindow, " ", "sans-bold", 20);
        new Label(toolsWindow, "Draw Parameters", "sans-bold", 20);
        Button *b0 = new Button(toolsWindow, "Show normals");
        b0->setFlags(Button::ToggleButton);
        b0->setChangeCallback([this](bool state) { mCanvas->toggleNormals(state); });

        Button *b1 = new Button(toolsWindow, "Show ocean");
        b1->setFlags(Button::ToggleButton);
        b1->setPushed(true);
        b1->setChangeCallback([this](bool state) { mCanvas->toggleOcean(state); });

        Button *b2 = new Button(toolsWindow, "Show wireframe");
        b2->setFlags(Button::ToggleButton);
        b2->setChangeCallback([this](bool state) { mCanvas->toggleWireframe(state); });

        performLayout();
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        }

        switch(key){
            case GLFW_KEY_W:
            case GLFW_KEY_S:
            case GLFW_KEY_A: 
            case GLFW_KEY_D:
            case GLFW_KEY_LEFT:
            case GLFW_KEY_RIGHT:
            case GLFW_KEY_UP:
            case GLFW_KEY_DOWN:
                planet->processInput(key, action);
        }

        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Draw the user interface */
        Screen::draw(ctx);
    }
private:
    MyGLCanvas *mCanvas;
    Planet *planet;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
