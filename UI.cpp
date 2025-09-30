/*
 * Studio Reverb UI Implementation
 * Dynamic UI that adapts to show relevant controls for each reverb algorithm
 */

#include "UI.hpp"
#include "DistrhoPluginInfo.h"
#include <cmath>
#include <cstdio>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------

StudioReverbUI::StudioReverbUI()
    : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT),
      fReverbType(REVERB_ROOM),
      fSampleRate(getSampleRate())
{
    // Initialize NanoVG
    nanovg = getContext();

    // Load font (using built-in DPF font)
    createFontFromMemory("main", font::dejavusans_ttf, font::dejavusans_ttf_size, false);

    // Initialize parameter values
    for (int i = 0; i < paramCount; ++i) {
        fParameters[i] = getParameterDefault(i);
    }

    // Initialize knob positions
    initializeKnobPositions();

    // Set initial visibility based on reverb type
    updateParameterVisibility();
}

StudioReverbUI::~StudioReverbUI()
{
}

// -----------------------------------------------------------------------------
// DSP/Plugin Callbacks

void StudioReverbUI::parameterChanged(uint32_t index, float value)
{
    if (index >= paramCount)
        return;

    fParameters[index] = value;

    // Update visibility if reverb type changed
    if (index == paramReverbType) {
        fReverbType = static_cast<ReverbType>(static_cast<int>(value + 0.5f));
        updateParameterVisibility();
    }

    repaint();
}

void StudioReverbUI::programLoaded(uint32_t index)
{
    // Reload all parameters when a program is loaded
    for (int i = 0; i < paramCount; ++i) {
        fParameters[i] = getParameterValue(i);
    }

    fReverbType = static_cast<ReverbType>(static_cast<int>(fParameters[paramReverbType] + 0.5f));
    updateParameterVisibility();
    repaint();
}

void StudioReverbUI::sampleRateChanged(double sampleRate)
{
    fSampleRate = sampleRate;
}

// -----------------------------------------------------------------------------
// UI Callbacks

void StudioReverbUI::onNanoDisplay()
{
    const float width = getWidth();
    const float height = getHeight();

    // Background
    beginPath();
    rect(0, 0, width, height);
    fillColor(Color(0.12f, 0.12f, 0.14f));
    fill();

    // Header
    drawHeader();

    // Reverb Type Selector
    drawReverbTypeSelector();

    // Draw parameter sections based on visibility
    drawLevelSection();
    drawSpatialSection();
    drawCharacterSection();
    drawFilterSection();

    // Spectrum analyzer (placeholder for now)
    drawSpectrumAnalyzer();
}

bool StudioReverbUI::onMouse(const MouseEvent& ev)
{
    if (ev.button != 1)
        return false;

    const float x = ev.pos.getX();
    const float y = ev.pos.getY();

    if (ev.press) {
        // Check reverb type selector buttons
        for (int i = 0; i < REVERB_TYPE_COUNT; ++i) {
            if (isInReverbTypeButton(x, y, i)) {
                setParameterValue(paramReverbType, static_cast<float>(i));
                return true;
            }
        }

        // Check knobs
        for (const auto& knob : fKnobs) {
            if (knob.visible && isInKnob(x, y, knob)) {
                fDraggingKnob = &knob;
                fDragStartY = y;
                fDragStartValue = fParameters[knob.param];
                return true;
            }
        }
    } else {
        fDraggingKnob = nullptr;
    }

    return false;
}

bool StudioReverbUI::onMotion(const MotionEvent& ev)
{
    if (fDraggingKnob == nullptr)
        return false;

    const float y = ev.pos.getY();
    const float delta = (fDragStartY - y) / 100.0f;

    float newValue = fDragStartValue + delta * (getParameterMax(fDraggingKnob->param) - getParameterMin(fDraggingKnob->param));
    newValue = std::max(getParameterMin(fDraggingKnob->param), std::min(getParameterMax(fDraggingKnob->param), newValue));

    setParameterValue(fDraggingKnob->param, newValue);

    return true;
}

// -----------------------------------------------------------------------------
// Private methods

void StudioReverbUI::initializeKnobPositions()
{
    // Clear existing knobs
    fKnobs.clear();

    // Level controls (always visible)
    fKnobs.push_back({paramDry, 50, 150, 50, true, "Dry"});
    fKnobs.push_back({paramEarly, 120, 150, 50, true, "Early"});
    fKnobs.push_back({paramLate, 190, 150, 50, true, "Late"});

    // Spatial controls
    fKnobs.push_back({paramSize, 50, 250, 50, true, "Size"});
    fKnobs.push_back({paramWidth, 120, 250, 50, true, "Width"});
    fKnobs.push_back({paramPredelay, 190, 250, 50, true, "Pre-Delay"});

    // Character controls
    fKnobs.push_back({paramDecay, 50, 350, 50, true, "Decay"});
    fKnobs.push_back({paramDiffuse, 120, 350, 50, true, "Diffuse"});
    fKnobs.push_back({paramDamping, 190, 350, 50, true, "Damping"});
    fKnobs.push_back({paramModulation, 260, 350, 50, true, "Modulation"});

    // Filter controls
    fKnobs.push_back({paramLowCut, 50, 450, 50, true, "Low Cut"});
    fKnobs.push_back({paramHighCut, 120, 450, 50, true, "High Cut"});
}

void StudioReverbUI::updateParameterVisibility()
{
    // Get visibility settings based on reverb type
    ParameterVisibility vis = getParameterVisibility(fReverbType);

    // Update knob visibility
    for (auto& knob : fKnobs) {
        switch (knob.param) {
            case paramSize:
                knob.visible = vis.showSize;
                break;
            case paramDecay:
                knob.visible = vis.showDecay;
                break;
            case paramDiffuse:
                knob.visible = vis.showDiffuse;
                break;
            case paramDamping:
                knob.visible = vis.showDamping;
                break;
            case paramModulation:
                knob.visible = vis.showModulation;
                break;
            case paramEarly:
                knob.visible = vis.showEarly;
                break;
            case paramLate:
                knob.visible = vis.showLate;
                break;
            default:
                // Other parameters remain visible
                break;
        }
    }
}

ParameterVisibility StudioReverbUI::getParameterVisibility(ReverbType type)
{
    ParameterVisibility vis;

    switch (type) {
        case REVERB_ROOM:
            vis.showSize = true;
            vis.showDecay = true;
            vis.showDiffuse = true;
            vis.showDamping = true;
            vis.showModulation = false;
            vis.showEarly = true;
            vis.showLate = true;
            break;

        case REVERB_HALL:
            vis.showSize = true;
            vis.showDecay = true;
            vis.showDiffuse = true;
            vis.showDamping = true;
            vis.showModulation = true;
            vis.showEarly = false;  // Hall uses combined mix
            vis.showLate = false;
            break;

        case REVERB_PLATE:
            vis.showSize = false;  // Plates don't have variable size
            vis.showDecay = true;
            vis.showDiffuse = true;
            vis.showDamping = true;
            vis.showModulation = true;
            vis.showEarly = false;  // Plate uses combined mix
            vis.showLate = false;
            break;

        case REVERB_EARLY_REFLECTIONS:
            vis.showSize = true;
            vis.showDecay = false;  // No late reverb
            vis.showDiffuse = true;
            vis.showDamping = false;  // No late reverb
            vis.showModulation = false;
            vis.showEarly = false;  // Always 100%
            vis.showLate = false;   // No late reverb
            break;

        default:
            // Default to room settings
            vis.showSize = true;
            vis.showDecay = true;
            vis.showDiffuse = true;
            vis.showDamping = true;
            vis.showModulation = false;
            vis.showEarly = true;
            vis.showLate = true;
            break;
    }

    return vis;
}

void StudioReverbUI::drawHeader()
{
    const float width = getWidth();

    // Header background
    beginPath();
    rect(0, 0, width, 60);
    fillColor(Color(0.15f, 0.15f, 0.17f));
    fill();

    // Title
    fontSize(24);
    fillColor(Color(0.9f, 0.9f, 0.9f));
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    text(width/2, 20, "Studio Reverb", nullptr);

    // Subtitle with current algorithm
    fontSize(14);
    fillColor(Color(0.6f, 0.6f, 0.6f));

    const char* algorithmNames[] = {"Room", "Hall", "Plate", "Early Reflections"};
    char subtitle[64];
    snprintf(subtitle, sizeof(subtitle), "Algorithm: %s", algorithmNames[fReverbType]);
    text(width/2, 40, subtitle, nullptr);
}

void StudioReverbUI::drawReverbTypeSelector()
{
    const float buttonWidth = 150;
    const float buttonHeight = 30;
    const float startX = (getWidth() - (buttonWidth * 4 + 30)) / 2;
    const float y = 70;

    const char* typeNames[] = {"Room", "Hall", "Plate", "Early Ref"};

    for (int i = 0; i < REVERB_TYPE_COUNT; ++i) {
        float x = startX + i * (buttonWidth + 10);

        // Button background
        beginPath();
        roundedRect(x, y, buttonWidth, buttonHeight, 4);

        if (fReverbType == i) {
            fillColor(Color(0.3f, 0.5f, 0.7f));
        } else {
            fillColor(Color(0.2f, 0.2f, 0.22f));
        }
        fill();

        // Button border
        strokeColor(Color(0.4f, 0.4f, 0.45f));
        strokeWidth(1);
        stroke();

        // Button text
        fontSize(14);
        fillColor(Color(0.9f, 0.9f, 0.9f));
        textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
        text(x + buttonWidth/2, y + buttonHeight/2, typeNames[i], nullptr);
    }
}

void StudioReverbUI::drawKnob(const Knob& knob)
{
    if (!knob.visible)
        return;

    const float value = fParameters[knob.param];
    const float min = getParameterMin(knob.param);
    const float max = getParameterMax(knob.param);
    const float normalized = (value - min) / (max - min);

    // Knob colors based on parameter type
    Color knobColor;
    switch (knob.param) {
        case paramDamping:
        case paramLowCut:
        case paramHighCut:
        case paramModulation:
            knobColor = Color(0.8f, 0.4f, 0.2f);  // Orange for tone-affecting
            break;
        case paramDry:
        case paramEarly:
        case paramLate:
        case paramWidth:
            knobColor = Color(0.2f, 0.3f, 0.6f);  // Blue for level controls
            break;
        default:
            knobColor = Color(0.4f, 0.4f, 0.45f);  // Gray for others
            break;
    }

    // Draw knob
    beginPath();
    circle(knob.x, knob.y, knob.radius);
    fillColor(knobColor);
    fill();

    // Draw value arc
    const float startAngle = 0.75f * M_PI;
    const float endAngle = 2.25f * M_PI;
    const float valueAngle = startAngle + normalized * (endAngle - startAngle);

    beginPath();
    arc(knob.x, knob.y, knob.radius - 3, startAngle, valueAngle, NVG_CW);
    strokeColor(Color(0.7f, 0.7f, 0.3f));
    strokeWidth(3);
    stroke();

    // Draw pointer
    save();
    translate(knob.x, knob.y);
    rotate(valueAngle);

    beginPath();
    moveTo(0, 0);
    lineTo(knob.radius - 10, 0);
    strokeColor(Color(0.9f, 0.9f, 0.9f));
    strokeWidth(2);
    stroke();

    restore();

    // Label
    fontSize(12);
    fillColor(Color(0.7f, 0.7f, 0.7f));
    textAlign(ALIGN_CENTER | ALIGN_TOP);
    text(knob.x, knob.y + knob.radius + 5, knob.label, nullptr);

    // Value display
    char valueStr[32];
    formatParameterValue(knob.param, value, valueStr, sizeof(valueStr));
    fontSize(10);
    fillColor(Color(0.5f, 0.5f, 0.5f));
    text(knob.x, knob.y + knob.radius + 20, valueStr, nullptr);
}

void StudioReverbUI::drawLevelSection()
{
    // Section header
    fontSize(14);
    fillColor(Color(0.7f, 0.7f, 0.7f));
    textAlign(ALIGN_LEFT | ALIGN_TOP);
    text(20, 120, "Levels", nullptr);

    // Draw level knobs
    for (const auto& knob : fKnobs) {
        if (knob.param == paramDry || knob.param == paramEarly || knob.param == paramLate) {
            drawKnob(knob);
        }
    }
}

void StudioReverbUI::drawSpatialSection()
{
    // Section header
    fontSize(14);
    fillColor(Color(0.7f, 0.7f, 0.7f));
    textAlign(ALIGN_LEFT | ALIGN_TOP);
    text(20, 220, "Spatial", nullptr);

    // Draw spatial knobs
    for (const auto& knob : fKnobs) {
        if (knob.param == paramSize || knob.param == paramWidth || knob.param == paramPredelay) {
            drawKnob(knob);
        }
    }
}

void StudioReverbUI::drawCharacterSection()
{
    // Section header
    fontSize(14);
    fillColor(Color(0.7f, 0.7f, 0.7f));
    textAlign(ALIGN_LEFT | ALIGN_TOP);
    text(20, 320, "Character", nullptr);

    // Draw character knobs
    for (const auto& knob : fKnobs) {
        if (knob.param == paramDecay || knob.param == paramDiffuse ||
            knob.param == paramDamping || knob.param == paramModulation) {
            drawKnob(knob);
        }
    }
}

void StudioReverbUI::drawFilterSection()
{
    // Section header
    fontSize(14);
    fillColor(Color(0.7f, 0.7f, 0.7f));
    textAlign(ALIGN_LEFT | ALIGN_TOP);
    text(20, 420, "Filters", nullptr);

    // Draw filter knobs
    for (const auto& knob : fKnobs) {
        if (knob.param == paramLowCut || knob.param == paramHighCut) {
            drawKnob(knob);
        }
    }
}

void StudioReverbUI::drawSpectrumAnalyzer()
{
    // Spectrum analyzer area (placeholder)
    const float x = 400;
    const float y = 150;
    const float width = 280;
    const float height = 200;

    // Background
    beginPath();
    rect(x, y, width, height);
    fillColor(Color(0.1f, 0.1f, 0.12f));
    fill();

    // Border
    strokeColor(Color(0.3f, 0.3f, 0.35f));
    strokeWidth(1);
    stroke();

    // Grid lines
    strokeColor(Color(0.2f, 0.2f, 0.22f));
    strokeWidth(0.5f);

    // Horizontal grid
    for (int i = 1; i < 4; ++i) {
        beginPath();
        moveTo(x, y + i * height/4);
        lineTo(x + width, y + i * height/4);
        stroke();
    }

    // Vertical grid
    for (int i = 1; i < 8; ++i) {
        beginPath();
        moveTo(x + i * width/8, y);
        lineTo(x + i * width/8, y + height);
        stroke();
    }

    // Placeholder text
    fontSize(12);
    fillColor(Color(0.4f, 0.4f, 0.4f));
    textAlign(ALIGN_CENTER | ALIGN_MIDDLE);
    text(x + width/2, y + height/2, "Spectrum Analyzer", nullptr);
}

void StudioReverbUI::formatParameterValue(uint32_t param, float value, char* str, size_t maxLen)
{
    switch (param) {
        case paramDry:
        case paramEarly:
        case paramLate:
        case paramSize:
        case paramWidth:
        case paramDiffuse:
        case paramDamping:
        case paramModulation:
            snprintf(str, maxLen, "%.0f%%", value);
            break;
        case paramPredelay:
            snprintf(str, maxLen, "%.0f ms", value);
            break;
        case paramDecay:
            snprintf(str, maxLen, "%.1f s", value);
            break;
        case paramLowCut:
            snprintf(str, maxLen, "%.0f Hz", value);
            break;
        case paramHighCut:
            if (value >= 1000)
                snprintf(str, maxLen, "%.1f kHz", value / 1000);
            else
                snprintf(str, maxLen, "%.0f Hz", value);
            break;
        default:
            snprintf(str, maxLen, "%.2f", value);
            break;
    }
}

bool StudioReverbUI::isInKnob(float x, float y, const Knob& knob)
{
    float dx = x - knob.x;
    float dy = y - knob.y;
    return (dx*dx + dy*dy) <= (knob.radius * knob.radius);
}

bool StudioReverbUI::isInReverbTypeButton(float x, float y, int type)
{
    const float buttonWidth = 150;
    const float buttonHeight = 30;
    const float startX = (getWidth() - (buttonWidth * 4 + 30)) / 2;
    const float buttonY = 70;

    float buttonX = startX + type * (buttonWidth + 10);

    return x >= buttonX && x <= buttonX + buttonWidth &&
           y >= buttonY && y <= buttonY + buttonHeight;
}

float StudioReverbUI::getParameterMin(uint32_t param)
{
    switch (param) {
        case paramReverbType: return 0;
        case paramDry:
        case paramEarly:
        case paramLate:
        case paramSize:
        case paramWidth:
        case paramDiffuse:
        case paramDamping:
        case paramModulation: return 0;
        case paramPredelay: return 0;
        case paramDecay: return 0.1f;
        case paramLowCut: return 20;
        case paramHighCut: return 1000;
        default: return 0;
    }
}

float StudioReverbUI::getParameterMax(uint32_t param)
{
    switch (param) {
        case paramReverbType: return REVERB_TYPE_COUNT - 1;
        case paramDry:
        case paramEarly:
        case paramLate:
        case paramSize:
        case paramWidth:
        case paramDiffuse:
        case paramDamping:
        case paramModulation: return 100;
        case paramPredelay: return 200;
        case paramDecay: return 10;
        case paramLowCut: return 500;
        case paramHighCut: return 20000;
        default: return 1;
    }
}

float StudioReverbUI::getParameterDefault(uint32_t param)
{
    switch (param) {
        case paramReverbType: return REVERB_ROOM;
        case paramDry: return 100;
        case paramEarly: return 75;
        case paramLate: return 75;
        case paramSize: return 50;
        case paramWidth: return 100;
        case paramPredelay: return 10;
        case paramDecay: return 2;
        case paramDiffuse: return 70;
        case paramDamping: return 50;
        case paramModulation: return 20;
        case paramLowCut: return 20;
        case paramHighCut: return 16000;
        default: return 0;
    }
}

// -----------------------------------------------------------------------------

UI* createUI()
{
    return new StudioReverbUI();
}

// -----------------------------------------------------------------------------

END_NAMESPACE_DISTRHO