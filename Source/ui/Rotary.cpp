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
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    auto param = audioProcessor.params.getParameter(paramId);
    auto normValue = param->getValue();
    auto value = param->convertFrom0to1(normValue);

    draw_rotary_slider(g, normValue);
    
    auto velNorm = -1.0f;
    if (!velId.isEmpty()) {
        auto velParam = audioProcessor.params.getParameter(velId);
        velNorm = velParam->getValue();
        draw_vel_arc(g, normValue, velNorm);
    }

    
    draw_label(g, value, velNorm);
}

void Rotary::draw_label(juce::Graphics& g, float slider_val, float vel_val)
{
    auto text = name;
    if (mouse_down) {
        if ((mouse_down_shift || audioProcessor.velMap) && vel_val > -1) {
            text = std::to_string((int)std::round((vel_val * 100))) + " %";
        }
        else {
            if (format == LabelFormat::Percent) text = std::to_string((int)std::round((slider_val * 100))) + " %";
            else if (format == LabelFormat::millis) text = std::to_string((int)slider_val) + " ms";
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
        }
    }

    g.setColour(juce::Colour(globals::COLOR_NEUTRAL));
    g.setFont(15.0f);
    g.drawText(text, 0, getHeight() - 16, getWidth(), 16, juce::Justification::centred, true);
    //g.drawRect(getLocalBounds());
}

void Rotary::mouseDown(const juce::MouseEvent& e) 
{
    e.source.enableUnboundedMouseMovement(true);
    mouse_down = true;
    mouse_down_shift = e.mods.isShiftDown();
    auto param = audioProcessor.params.getParameter((mouse_down_shift || audioProcessor.velMap) && velId.isNotEmpty() ? velId : paramId);
    auto cur_val = param->getValue();
    cur_normed_value = cur_val;
    last_mouse_position = e.getPosition();
    setMouseCursor(MouseCursor::NoCursor);
    start_mouse_pos = Desktop::getInstance().getMousePosition();
    repaint();
}

void Rotary::mouseUp(const juce::MouseEvent& e) {
    mouse_down = false;
    setMouseCursor(MouseCursor::NormalCursor);
    e.source.enableUnboundedMouseMovement(false);
    Desktop::getInstance().setMousePosition(start_mouse_pos);
    repaint();
}

void Rotary::mouseDoubleClick(const juce::MouseEvent& e) {
    auto param = audioProcessor.params.getParameter((e.mods.isShiftDown() || audioProcessor.velMap) && velId.isNotEmpty() ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getDefaultValue());
    param->endChangeGesture();
    juce::ignoreUnused(e);
}

void Rotary::mouseDrag(const juce::MouseEvent& e) {
    auto change = e.getPosition() - last_mouse_position;
    last_mouse_position = e.getPosition();
    auto speed = (e.mods.isCtrlDown() ? 40.0f : 4.0f) * pixels_per_percent;
    auto slider_change = float(change.getX() - change.getY()) / speed;
    cur_normed_value += slider_change;
    auto param = audioProcessor.params.getParameter((mouse_down_shift || audioProcessor.velMap) && velId.isNotEmpty() ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(cur_normed_value);
    param->endChangeGesture();
}

void Rotary::draw_rotary_slider(juce::Graphics& g, float slider_pos) {
    auto bounds = getBounds();
    const float radius = 16.0f;
    const float angle = -deg130 + slider_pos * (deg130 - -deg130);

    juce::DropShadow shadow(Colour(0xff888888), 15, {4, 4}); // (Color, Blur Radius, Offset)
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

void Rotary::draw_vel_arc(juce::Graphics& g, float slider_pos, float vel_pos) const {
    auto bounds = getBounds();
    const float radius = bounds.getHeight() / 2.0f - 20.0f;
    const float slider_angle = -deg130 + slider_pos * (deg130 - -deg130);
    const float vel_angle = fmin(deg130, slider_angle + vel_pos * (deg130 - -deg130));
    g.setColour(Colour(globals::COLOR_VEL));

    if (vel_pos && slider_angle < deg130) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f - 4.0f, radius + 8.0f, radius + 8.0f, 0, slider_angle, vel_angle, true);
        g.strokePath(arc, PathStrokeType(2.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }
}