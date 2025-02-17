#include "Pitch.h"
#include "../PluginProcessor.h"
#include "../Globals.h"

Pitch::Pitch(RipplerXAudioProcessor& p, juce::String name, juce::String coarse_param_id, juce::String fine_param_id)
    : juce::Component()
    , audioProcessor(p)
    , coarse_param_id(coarse_param_id)
    , fine_param_id(fine_param_id)
{
    setName(name);
    audioProcessor.params.addParameterListener(coarse_param_id, this);
    audioProcessor.params.addParameterListener(fine_param_id, this);
}

Pitch::~Pitch()
{
    audioProcessor.params.removeParameterListener(coarse_param_id, this);
    audioProcessor.params.removeParameterListener(fine_param_id, this);
}

void Pitch::parameterChanged(const juce::String& parameterID, float newValue) 
{
    (void)parameterID;
    (void)newValue;
    juce::MessageManager::callAsync([this] { repaint(); });
}

void Pitch::paint(juce::Graphics& g) {
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    auto param = audioProcessor.params.getParameter(coarse_param_id);
    auto norm = param->getValue();
    auto coarse_val = param->convertFrom0to1(norm);

    param = audioProcessor.params.getParameter(fine_param_id);
    norm = param->getValue();
    auto fine_val = param->convertFrom0to1(norm);

    g.setColour(Colour(globals::COLOR_ACTIVE));
    g.fillRoundedRectangle(0.f, 0.f, (float)getWidth(), (float)getHeight(), 2.f);
    g.setFont(FontOptions(15.0f));
    g.setColour(Colours::white);
    auto sign = coarse_val < 0 || fine_val < 0 ? -1.f : 1.f;
    std::stringstream ss;
    ss << sign * fabs(coarse_val) << "." << std::setw(2) << std::setfill('0') << fabs(fine_val);

    auto text = ss.str();
    g.drawText(text, 0, 0, getWidth()-15, getHeight(), Justification::centredRight, false);
}

void Pitch::mouseDown(const juce::MouseEvent& e) 
{
    e.source.enableUnboundedMouseMovement(true);
    is_coarse = e.getMouseDownX() < getWidth() / 2.f;
    last_mouse_pos = e.getPosition();
    auto param = audioProcessor.params.getParameter(coarse_param_id);
    auto norm = param->getValue();
    auto val = param->convertFrom0to1(norm);
    cur_coarse = val;
    param = audioProcessor.params.getParameter(fine_param_id);
    norm = param->getValue();
    val = param->convertFrom0to1(norm);
    cur_fine = val;
    setMouseCursor(MouseCursor::NoCursor);
    start_mouse_pos = Desktop::getInstance().getMousePosition();
}

void Pitch::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    is_coarse = event.getMouseDownX() < getWidth() / 2.f;
    auto slider_change = wheel.deltaY > 0 ? 1.0f : wheel.deltaY < 0 ? -1.0f : 0;
    applyChange(slider_change);
}

void Pitch::mouseUp(const juce::MouseEvent& e) {
    setMouseCursor(MouseCursor::NormalCursor);
    e.source.enableUnboundedMouseMovement(false);
    Desktop::getInstance().setMousePosition(start_mouse_pos);
}

void Pitch::mouseDoubleClick(const juce::MouseEvent& e) {
    (void)e;
    auto param = audioProcessor.params.getParameter(is_coarse ? coarse_param_id : fine_param_id);
    param->beginChangeGesture();
    param->setValueNotifyingHost(param->getDefaultValue());
    param->endChangeGesture();
}

void Pitch::mouseDrag(const juce::MouseEvent& e) {
    auto change = e.getPosition() - last_mouse_pos;
    last_mouse_pos = e.getPosition();
    auto speed = (e.mods.isCtrlDown() ? 40.0f : 4.0f) * 10.0f;
    auto slider_change = float(-change.getY()) / speed;
    applyChange(slider_change);
}

void Pitch::applyChange(float change)
{
    if (is_coarse)
        cur_coarse += change;
    else
        cur_fine += change;

    if (!is_coarse && (
        (cur_fine >= 100.f) || 
        (cur_fine <= -100.f) || 
        (std::trunc(cur_coarse) < 0.f && cur_fine >= 1.f) || 
        (std::trunc(cur_coarse) > 0.f && cur_fine < 0))) 
    {
        cur_coarse = std::trunc(cur_coarse);
        if (cur_fine >= 100.f) {
            cur_coarse += 1.f;
            cur_fine = 0.f;
        }
        else if (cur_fine <= -100.f) {
            cur_coarse -= 1.f;
            cur_fine = 0.f;
        }
        else if (cur_coarse > 0.f) {
            cur_coarse += cur_fine >= 100.f ? 1.f : -1.f;
            cur_fine = cur_fine < 0.f ? 99.f : 0.f;
        } else {
            cur_coarse += cur_fine <= -100.f ? -1.f : 1.f;
            cur_fine = cur_fine < -99.f ? 0.f : -99.f;
        }

        last_mouse_pos = getMouseXYRelative();
        auto param = audioProcessor.params.getParameter(coarse_param_id);
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(cur_coarse));
        param->endChangeGesture();
    }

    auto param = audioProcessor.params.getParameter(is_coarse ? coarse_param_id : fine_param_id);
    auto norm = param->convertTo0to1(std::trunc(is_coarse ? cur_coarse : cur_fine));
    param->beginChangeGesture();
    param->setValueNotifyingHost(norm);
    param->endChangeGesture();
}