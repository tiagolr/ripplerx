#include "Slider.h"
#include "../PluginProcessor.h"
#include "../Globals.h"

Rotary::Rotary(RipplerXAudioProcessor& p, juce::String paramId, juce::String name)
    : juce::SettableTooltipClient()
    , juce::Component()
    , audioProcessor(p)
    , paramId(paramId)
    , name(name)
{
    setOpaque(true);
    setColour(0, juce::Colour(0xff000000));
    setName(name);
    setTooltip("Test1233");
    audioProcessor.params.addParameterListener(paramId, this);
}

Rotary::~Rotary()
{
    audioProcessor.params.removeParameterListener(paramId, this);
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

    draw_rotary_slider(g, normed_val);

    g.setColour(juce::Colour(globals::COLOR_NEUTRAL));
    g.drawText(name, 0, proportionOfHeight(0.75f), getWidth(), proportionOfHeight(0.25f), juce::Justification::centred, true);
    g.drawRect(getLocalBounds());
}

void Rotary::mouseDown(const juce::MouseEvent& e) 
{
    auto param = audioProcessor.params.getParameter(paramId);
    auto cur_val = param->getValue();
    cur_normed_value = param->convertTo0to1(cur_val);
    last_mouse_position = e.getPosition();
    setMouseCursor(MouseCursor::NoCursor);
    start_mouse_pos = Desktop::getInstance().getMousePosition();
}

void Rotary::mouseUp(const juce::MouseEvent& e) {
    setMouseCursor(MouseCursor::NormalCursor);
    Desktop::getInstance().setMousePosition(start_mouse_pos);
}

void Rotary::mouseDoubleClick(const juce::MouseEvent& e) {
    auto param = audioProcessor.params.getParameter(paramId);
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
    auto param = audioProcessor.params.getParameter(paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(cur_normed_value);
    param->endChangeGesture();
}

void Rotary::draw_rotary_slider(juce::Graphics& g, float slider_pos) {
    auto bounds = getBounds();
    const float radius = bounds.getHeight() / 2.0f - 20.0f;
    const float angle = -deg130 + slider_pos * (deg130 - -deg130);

    g.setColour(Colour(0xffaaaaaa));
    g.fillEllipse(bounds.getWidth()/2.0f-radius, bounds.getHeight()/2.0f-radius, radius*2.0f, radius*2.0f);
    g.setColour(Colour(globals::COLOR_ACTIVE));

    if (slider_pos) {
        juce::Path arc;
        arc.addCentredArc(bounds.getWidth() / 2, bounds.getHeight() / 2, radius + 4.0f, radius + 4.0f, 0, -deg130, angle, true);
        g.strokePath(arc, PathStrokeType(4.0, PathStrokeType::JointStyle::curved, PathStrokeType::rounded));
    }

    juce::Path p;
    p.addLineSegment (juce::Line<float>(0.0f, -5.0f, 0.0f, -radius + 5.0f), 0.1f);
    juce::PathStrokeType(4.0f, PathStrokeType::JointStyle::curved, PathStrokeType::rounded).createStrokedPath(p, p);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated(bounds.getWidth() / 2, bounds.getHeight() / 2));
}