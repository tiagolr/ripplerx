#include "Rotary.h"
#include "../PluginProcessor.h"
#include "../Globals.h"

Rotary::Rotary(RipplerXAudioProcessor& p, juce::String paramId, juce::String name, LabelFormat format, juce::String velId)
    : juce::SettableTooltipClient()
    , juce::Component()
    , audioProcessor(p)
    , paramId(paramId)
    , name(name)
    , format(format)
    , velId(velId)
{
    setName(name);
    //setTooltip("Test1233");
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
    repaint();
}

void Rotary::paint(juce::Graphics& g) {
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    auto param = audioProcessor.params.getParameter(paramId);
    auto cur_val = param->getValue();
    auto normed_val = param->convertTo0to1(cur_val);

    DBG("SLIDER VALUE " << param->convertFrom0to1(cur_val) << " NORM VAL " << cur_val);

    draw_rotary_slider(g, normed_val);
    
    auto vel_norm = -1.0f;
    if (!velId.isEmpty()) {
        auto vel_param = audioProcessor.params.getParameter(velId);
        auto vel_value = vel_param->getValue();
        vel_norm = param->convertTo0to1(vel_value);
        draw_vel_arc(g, normed_val, vel_norm);
    }

    
    draw_label(g, cur_val, vel_norm);
}

void Rotary::draw_label(juce::Graphics& g, float slider_val, float vel_val)
{
    auto text = name;
    if (mouse_down) {
        if (mouse_down_shift && vel_val > -1) {
            text = std::to_string((int)std::round((vel_val * 100))) + " %";
        }
        else {
            text = std::to_string((int)std::round((slider_val * 100))) + " %";
        }
    }

    g.setColour(juce::Colour(globals::COLOR_NEUTRAL));
    g.setFont(16);
    g.drawText(text, 0, getHeight() - 16, getWidth(), 16, juce::Justification::centred, true);
    //g.drawRect(getLocalBounds());
}

void Rotary::mouseDown(const juce::MouseEvent& e) 
{
    e.source.enableUnboundedMouseMovement(true);
    mouse_down = true;
    mouse_down_shift = e.mods.isShiftDown();
    auto param = audioProcessor.params.getParameter(mouse_down_shift && velId.isNotEmpty() ? velId : paramId);
    auto cur_val = param->getValue();
    cur_normed_value = param->convertTo0to1(cur_val);
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
    auto param = audioProcessor.params.getParameter(e.mods.isShiftDown() && velId.isNotEmpty() ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getDefaultValue());
    param->endChangeGesture();
    juce::ignoreUnused(e);
}

void Rotary::mouseDrag(const juce::MouseEvent& e) {
    auto change = e.getPosition() - last_mouse_position;
    last_mouse_position = e.getPosition();
    auto speed = (e.mods.isCtrlDown() ? 20.0f : 1.0f) * pixels_per_percent;
    auto slider_change = float(change.getX() - change.getY()) / speed;
    cur_normed_value += slider_change;
    auto param = audioProcessor.params.getParameter(mouse_down_shift && velId.isNotEmpty() ? velId : paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(cur_normed_value);
    param->endChangeGesture();
}

void Rotary::draw_rotary_slider(juce::Graphics& g, float slider_pos) const {
    auto bounds = getBounds();
    const float radius = bounds.getHeight() / 2.0f - 20.0f;
    const float angle = -deg130 + slider_pos * (deg130 - -deg130);

    g.setColour(Colour(0xffaaaaaa));
    g.fillEllipse(bounds.getWidth()/2.0f-radius, bounds.getHeight()/2.0f-radius, radius*2.0f, radius*2.0f);
    g.setColour(Colour(globals::COLOR_ACTIVE));

    if (slider_pos) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f, radius + 4.0f, radius + 4.0f, 0, -deg130, angle, true);
        g.strokePath(arc, PathStrokeType(3.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }

    juce::Path p;
    p.addLineSegment (juce::Line<float>(0.0f, -5.0f, 0.0f, -radius + 5.0f), 0.1f);
    juce::PathStrokeType(3.0f, PathStrokeType::JointStyle::curved, PathStrokeType::rounded).createStrokedPath(p, p);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f));
}

void Rotary::draw_vel_arc(juce::Graphics& g, float slider_pos, float vel_pos) const {
    auto bounds = getBounds();
    const float radius = bounds.getHeight() / 2.0f - 20.0f;
    const float slider_angle = -deg130 + slider_pos * (deg130 - -deg130);
    const float vel_angle = fmin(deg130, slider_angle + vel_pos * (deg130 - -deg130));
    g.setColour(Colour(globals::COLOR_VEL));

    if (vel_pos && slider_angle < deg130) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2.0f, bounds.getHeight() / 2.0f, radius + 8.0f, radius + 8.0f, 0, slider_angle, vel_angle, true);
        g.strokePath(arc, PathStrokeType(2.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }
}