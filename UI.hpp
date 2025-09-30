/*
 * Studio Reverb UI Header
 */

#ifndef STUDIO_REVERB_UI_HPP_INCLUDED
#define STUDIO_REVERB_UI_HPP_INCLUDED

#include "DistrhoUI.hpp"
#include "DistrhoPluginInfo.h"
#include "NanoVG.hpp"
#include <vector>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------

class StudioReverbUI : public UI,
                      public NanoVG
{
public:
    StudioReverbUI();
    ~StudioReverbUI();

protected:
    // DSP/Plugin Callbacks
    void parameterChanged(uint32_t index, float value) override;
    void programLoaded(uint32_t index) override;
    void sampleRateChanged(double sampleRate) override;

    // UI Callbacks
    void onNanoDisplay() override;
    bool onMouse(const MouseEvent& ev) override;
    bool onMotion(const MotionEvent& ev) override;

private:
    // Knob structure
    struct Knob {
        uint32_t param;
        float x, y;
        float radius;
        bool visible;
        const char* label;
    };

    // UI State
    ReverbType fReverbType;
    float fParameters[paramCount];
    double fSampleRate;

    // Knob management
    std::vector<Knob> fKnobs;
    const Knob* fDraggingKnob;
    float fDragStartY;
    float fDragStartValue;

    // NanoVG context
    NVGcontext* nanovg;

    // Drawing methods
    void initializeKnobPositions();
    void updateParameterVisibility();
    ParameterVisibility getParameterVisibility(ReverbType type);

    void drawHeader();
    void drawReverbTypeSelector();
    void drawKnob(const Knob& knob);
    void drawLevelSection();
    void drawSpatialSection();
    void drawCharacterSection();
    void drawFilterSection();
    void drawSpectrumAnalyzer();

    // Helper methods
    void formatParameterValue(uint32_t param, float value, char* str, size_t maxLen);
    bool isInKnob(float x, float y, const Knob& knob);
    bool isInReverbTypeButton(float x, float y, int type);

    float getParameterMin(uint32_t param);
    float getParameterMax(uint32_t param);
    float getParameterDefault(uint32_t param);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StudioReverbUI)
};

// -----------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // STUDIO_REVERB_UI_HPP_INCLUDED