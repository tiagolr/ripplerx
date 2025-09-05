#include "Rotary.h"
#include "../PluginProcessor.h"
#include "../Globals.h"

Rotary::Rotary(RipplerXAudioProcessor& p, juce::String paramId, juce::String name, LabelFormat format, juce::String velId, bool isSymmetric)
    : juce::SettableTooltipClient()
    , juce::Component()
    , audioProcessor(p)
    , paramId(paramId)
    , name(name)
    , format(format)
    , velId(velId)
    , isSymmetric(isSymmetric)
{
    setName(name);
    audioProcessor.params.addParameterListener(paramId, this);
    if (velId.isNotEmpty()) {
        audioProcessor.params.addParameterListener(velId, this);
    }
}

Rotary::~Rotary()
{
    audioProcessor.params.removeParameterListener(paramId, this);
    if (velId.isNotEmpty()) {
        audioProcessor.params.removeParameterListener(velId, this);
    }
}

void Rotary::parameterChanged(const juce::String& parameterID, float newValue) 
{
    (void)parameterID;
    (void)newValue;
    juce::MessageManager::callAsync([this] { repaint(); });
}

void Rotary::paint(juce::Graphics& g) {
    //g.fillAll(Colour(globals::COLOR_BACKGROUND));

    auto param = audioProcessor.params.getParameter(paramId);
    auto normValue = param->getValue();
    auto value = param->convertFrom0to1(normValue);

    draw_rotary_slider(g, normValue);
    
    auto velVal = -2.0f;
    if (!velId.isEmpty()) {
        auto velParam = audioProcessor.params.getParameter(velId);
        velVal = velParam->convertFrom0to1(velParam->getValue());
        draw_vel_arc(g, normValue, velVal);
    }

    draw_label(g, value, velVal);
}

void Rotary::draw_label(juce::Graphics& g, float slider_val, float vel_val)
{
    auto text = name;
    if (mouse_down) {
        if ((mouse_down_shift || audioProcessor.velMap) && vel_val > -2.0f) {
            text = std::to_string((int)std::round((vel_val * 100))) + " %";
        }
        else {
            if (format == LabelFormat::Percent) text = std::to_string((int)std::round((slider_val * 100))) + " %";
            else if (format == LabelFormat::millis) {
                if (slider_val < 1000.0f) {
                    text = std::to_string((int)slider_val) + " ms";
                }
                else {
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(2) << (slider_val / 1000.f) << " s";
                    text = ss.str();
                }
            }
            else if (format == LabelFormat::Hz) text = std::to_string((int)slider_val) + " Hz";
            else if (format == LabelFormat::float1) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(1) << slider_val;
                text = ss.str();
            }
            else if (format == LabelFormat::float2_100) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << slider_val * 100;
                text = ss.str();
            }
            else if (format == LabelFormat::seconds2f) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << slider_val << " s";
                text = ss.str();
            }
            else if (format == LabelFormat::ABMix) text = std::to_string((int)((1-slider_val) * 100)) + ":" + std::to_string((int)(slider_val * 100));
            else if (format == LabelFormat::dB) text = std::to_string((int)slider_val) + " dB";
            else if (format == LabelFormat::FilterLPHP) {
                if (slider_val == 0.0) {
                    text = "Off";
                }
                else {
                    std::stringstream ss;
                    double freq = 20.0 * std::pow(1000, slider_val < 0.0 ? 1 + slider_val : slider_val); // map 1..0 to 20..20000

                    ss << (slider_val < 0.0 ? "LP " : "HP ");
                    if (freq >= 1000.0) {
                        ss << std::fixed << std::setprecision(1) << (freq / 1000.0) << " kHz";
                    }
                    else {
                        ss << std::fixed << std::setprecision(0) << freq << " Hz";
                    }

                    text = ss.str();
                }
            }
            else if (format == LabelFormat::PitchSemis) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(0) << slider_val << " Sem";
                text = ss.str();
            }
        }
    }

    bool isShowingFilter = mouse_down && format == LabelFormat::FilterLPHP;
    g.setColour(juce::Colour(globals::COLOR_NEUTRAL));
    g.setFont(isShowingFilter ? 14.f : 15.0f);
    g.drawText(text, 0, getHeight() - 16, getWidth(), 16, juce::Justification::centred, true);
    //g.drawRect(getLocalBounds());
}

void Rotary::mouseDown(const juce::MouseEvent& e) 
{
    e.source.enableUnboundedMouseMovement(true);
    mouse_down = true;
    mouse_down_shift = e.mods.isShiftDown();
    editingVel = (mouse_down_shift || audioProcessor.velMap) && velId.isNotEmpty();
    auto param = audioProcessor.params.getParameter(editingVel ? velId : paramId);
    auto cur_val = param->getValue();
    cur_normed_value = cur_val;
    last_mouse_position = e.getPosition();
    setMouseCursor(MouseCursor::NoCursor);
    start_mouse_pos = Desktop::getInstance().getMousePosition();
    repaint();
    param->beginChangeGesture();
}

void Rotary::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (event.mods.isLeftButtonDown() || event.mods.isRightButtonDown()) {
        return; // prevent crash, param is already mutating
    }
    auto speed = (event.mods.isCtrlDown() ? 0.01f : 0.05f);
    auto slider_change = wheel.deltaY > 0 ? speed : wheel.deltaY < 0 ? -speed : 0;
    auto param = audioProcessor.params.getParameter(editingVel ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getValue() + slider_change);
    while (wheel.deltaY > 0.0f && param->getValue() == 0.0f) { // FIX wheel not working when value is zero, first step takes more than 0.05% 
        slider_change += 0.05f;
        param->setValueNotifyingHost(param->getValue() + slider_change);
    }
    param->endChangeGesture();
}

void Rotary::mouseUp(const juce::MouseEvent& e) {
    mouse_down = false;
    setMouseCursor(MouseCursor::NormalCursor);
    e.source.enableUnboundedMouseMovement(false);
    Desktop::getInstance().setMousePosition(start_mouse_pos);
    repaint();
    auto param = audioProcessor.params.getParameter(editingVel ? velId : paramId);
    param->endChangeGesture();
}

void Rotary::mouseDoubleClick(const juce::MouseEvent& e) {
    auto param = audioProcessor.params.getParameter((e.mods.isShiftDown() || audioProcessor.velMap) && velId.isNotEmpty() ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getDefaultValue());
    param->endChangeGesture();
}

void Rotary::mouseDrag(const juce::MouseEvent& e) {
    auto change = e.getPosition() - last_mouse_position;
    last_mouse_position = e.getPosition();
    auto speed = (e.mods.isCtrlDown() ? 40.0f : 4.0f) * pixels_per_percent;
    auto slider_change = float(change.getX() - change.getY()) / speed;
    cur_normed_value += slider_change;
    auto param = audioProcessor.params.getParameter((mouse_down_shift || audioProcessor.velMap) && velId.isNotEmpty() ? velId : paramId);

    if (format == LabelFormat::PitchSemis && !e.mods.isCtrlDown()) {
        // snap values for pitch knob
        auto val = param->convertFrom0to1(cur_normed_value);
        param->setValueNotifyingHost(param->convertTo0to1(std::round(val)));
    }
    else {
        param->setValueNotifyingHost(cur_normed_value);
    }
}

void Rotary::draw_rotary_slider(juce::Graphics& g, float slider_pos) {
    auto bounds = getBounds();
    const float radius = 16.0f;
    const float angle = -deg130 + slider_pos * (deg130 - -deg130);

    juce::DropShadow shadow(Colour(audioProcessor.darkTheme ? 0xff000000 : 0xff888888), 15, {4, 4}); // (Color, Blur Radius, Offset)
    juce::Path circlePath;
    circlePath.addEllipse(bounds.getWidth()/2.0f-radius, bounds.getHeight()/2.0f-radius-4.0f, radius*2.0f, radius*2.0f); // x, y, width, height
    shadow.drawForPath(g, circlePath);
    g.setColour(Colour(globals::COLOR_BACKGROUND));
    g.fillPath(circlePath);

    g.setColour(Colour(globals::COLOR_ACTIVE));
    if ((isSymmetric && slider_pos != 0.5f) || (!isSymmetric && slider_pos)) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f - 4.0f, radius + 2.0f, radius + 2.0f, 0, isSymmetric ? 0 : -deg130, angle, true);
        g.strokePath(arc, PathStrokeType(2.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }

    g.setColour(Colour(globals::COLOR_NEUTRAL));
    juce::Path p;
    p.addLineSegment (juce::Line<float>(0.0f, -5.0f, 0.0f, -radius + 5.0f), 0.1f);
    juce::PathStrokeType(3.0f, PathStrokeType::JointStyle::curved, PathStrokeType::rounded).createStrokedPath(p, p);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f - 4.0f));

    if (velId.isNotEmpty() && audioProcessor.velMap) {
        g.setColour(Colour(globals::COLOR_VEL).withAlpha(0.5f));
        g.fillEllipse(bounds.getWidth()/2.0f-radius, bounds.getHeight()/2.0f-radius-4.0f, radius*2.0f, radius*2.0f);
    }
}

void Rotary::draw_vel_arc(juce::Graphics& g, float slider_pos, float vel_val) const {
    auto bounds = getBounds();
    const float radius = bounds.getHeight() / 2.0f - 20.0f;
    const float slider_angle = -deg130 + slider_pos * (deg130 - -deg130);
    const float vel_angle = fmax(-deg130, fmin(deg130, slider_angle + vel_val * (deg130 - -deg130)));
    g.setColour(Colour(globals::COLOR_VEL));

    if ((vel_val > 0.0f && slider_angle < deg130) || (vel_val < 0.0f && slider_angle > -deg130)) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f - 4.0f, radius + 8.0f, radius + 8.0f, 0, slider_angle, vel_angle, true);
        g.strokePath(arc, PathStrokeType(2.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }
}