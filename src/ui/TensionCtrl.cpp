#include "TensionCtrl.h"
#include "../PluginProcessor.h"
#include "../Globals.h"

TensionCtrl::TensionCtrl(RipplerXAudioProcessor& p, juce::String paramId, bool invert) 
    : juce::Component()
    , audioProcessor(p)
    , paramId(paramId)
    , invert(invert)
{
    audioProcessor.params.addParameterListener(paramId, this);
}

TensionCtrl::~TensionCtrl()
{
    audioProcessor.params.removeParameterListener(paramId, this);
}

void TensionCtrl::parameterChanged(const juce::String& parameterID, float newValue)
{
    (void)parameterID;
    (void)newValue;
    juce::MessageManager::callAsync([this] { repaint(); });
}

void TensionCtrl::mouseEnter(const MouseEvent& e)
{
    (void)e;
    mouseOver = true;
    repaint();
};

void TensionCtrl::mouseExit(const MouseEvent& e)
{
    (void)e;
    mouseOver = false;
    repaint();
};


void TensionCtrl::paint(juce::Graphics& g) 
{
    auto bounds = getLocalBounds();
    
    g.setColour(Colours::black.withAlpha(0.1f));
    g.fillRect(bounds);

    g.setColour(Colour(globals::COLOR_NEUTRAL_LIGHT));
    auto val = audioProcessor.params.getRawParameterValue(paramId)->load();
    val = (val + 1.f) * 0.5f; // normalize between 1 and 0
    auto bl = bounds.getBottomLeft().toFloat();
    auto br = bounds.getBottomRight().toFloat();
    auto tl = bounds.getTopLeft().toFloat();
    auto tr = bounds.getTopRight().toFloat();

    Path path;

    if (invert) {
        float cx = bl.x + (tr.x - tl.x) * (1.f - val);
        float cy = br.y - (br.y - tl.y) * (1.f - val);
        path.startNewSubPath(tl.x, tl.y);
        path.quadraticTo(cx, cy, br.x, br.y);
    }
    else {
        float cx = tl.x + (br.x - tl.x) * val;
        float cy = tl.y + (br.y - tl.y) * val;
        path.startNewSubPath(bl.x, bl.y);
        path.quadraticTo(cx, cy, tr.x, tr.y);
    }
    
    g.strokePath(path, PathStrokeType(1.0f));
}

void TensionCtrl::mouseDown(const juce::MouseEvent& e)
{
    e.source.enableUnboundedMouseMovement(true);
    mouse_down = true;
    mouse_down_shift = e.mods.isShiftDown();
    auto param = audioProcessor.params.getParameter(paramId);
    auto cur_val = param->getValue();
    cur_normed_value = cur_val;
    last_mouse_position = e.getPosition();
    setMouseCursor(MouseCursor::NoCursor);
    start_mouse_pos = Desktop::getInstance().getMousePosition();
    repaint();
    param->beginChangeGesture();
}

void TensionCtrl::mouseUp(const juce::MouseEvent& e) {
    mouse_down = false;
    setMouseCursor(MouseCursor::NormalCursor);
    e.source.enableUnboundedMouseMovement(false);
    Desktop::getInstance().setMousePosition(start_mouse_pos);
    repaint();
    auto param = audioProcessor.params.getParameter(paramId);
    param->endChangeGesture();
}

void TensionCtrl::mouseDoubleClick(const juce::MouseEvent& e) {
    (void)e;
    auto param = audioProcessor.params.getParameter(paramId);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getDefaultValue());
    param->endChangeGesture();
}

void TensionCtrl::mouseDrag(const juce::MouseEvent& e) {
    auto change = e.getPosition() - last_mouse_position;
    last_mouse_position = e.getPosition();
    auto speed = (e.mods.isCtrlDown() ? 40.0f : 4.0f) * pixels_per_percent;
    auto slider_change = float(change.getX() - change.getY()) / speed;
    cur_normed_value -= slider_change;
    auto param = audioProcessor.params.getParameter(paramId);
    param->setValueNotifyingHost(cur_normed_value);
}