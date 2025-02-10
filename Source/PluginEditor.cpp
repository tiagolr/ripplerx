/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

//==============================================================================
RipplerXAudioProcessorEditor::RipplerXAudioProcessorEditor (RipplerXAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(&customLookAndFeel);

    setSize (650, 485);
    auto bounds = getLocalBounds();
    setScaleFactor(audioProcessor.scale);
    auto col = 10;
    auto row = 10; 

    // TOP BAR

    juce::MemoryInputStream inputStream(BinaryData::logo_png, BinaryData::logo_pngSize, false);
    juce::Image image = juce::ImageFileFormat::loadFrom(inputStream);
    if (image.isValid())
    {
        logo.setImages(false, true, true, 
            image, 1.0f, juce::Colours::transparentBlack, 
            image, 1.0f, juce::Colours::transparentBlack, 
            image, 1.0f, juce::Colours::transparentBlack);
    }
    addAndMakeVisible(logo);
    logo.setBounds(col, row+5, 120, 16);

    col += 170;
    addAndMakeVisible(sizeLabel);
    sizeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    sizeLabel.setFont(FontOptions(16.0f));
    sizeLabel.setText("UI", NotificationType::dontSendNotification);
    sizeLabel.setBounds(col, row, 30, 25);

    addAndMakeVisible(sizeMenu);
    sizeMenu.addItem("100%", 1);
    sizeMenu.addItem("150%", 2);
    sizeMenu.addItem("200%", 3);
    sizeMenu.setSelectedId(audioProcessor.scale == 1.0f ? 1 : audioProcessor.scale == 1.5f ? 2 : 3);
    sizeMenu.onChange = [this]()
        {
            const int value = sizeMenu.getSelectedId();
            auto scale = value == 1 ? 1.0f : value == 2 ? 1.5f : 2.0f;
            audioProcessor.setScale(scale);
            setScaleFactor(audioProcessor.scale);
        };
    sizeMenu.setBounds(col+25,row,80,25);

    addAndMakeVisible(velButton);
    velButton.setTooltip("Toggle velocity mapping, can also be set by holding Shift+Drag on knobs");
    velButton.setColour(juce::TextButton::buttonColourId, Colour(globals::COLOR_BACKGROUND));
    velButton.setColour(juce::TextButton::buttonOnColourId, Colour(globals::COLOR_VEL));
    velButton.setColour(juce::TextButton::textColourOffId, Colour(globals::COLOR_VEL));
    velButton.setColour(juce::ComboBox::outlineColourId, Colour(globals::COLOR_VEL));
    velButton.onClick = [this]() 
        {
            audioProcessor.velMap = !audioProcessor.velMap;
            velButton.setToggleState(audioProcessor.velMap, NotificationType::dontSendNotification);
            repaintVelSliders();
        };
    velButton.setButtonText("Vel Map");
    velButton.setBounds(col+115, row, 80, 25);

    addAndMakeVisible(polyLabel);
    polyLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    polyLabel.setFont(FontOptions(16.0f));
    polyLabel.setText("Voices", NotificationType::dontSendNotification);
    polyLabel.setBounds(col+115+80, row, 50, 25);

    addAndMakeVisible(polyMenu);
    polyMenu.addItem("4", 1);
    polyMenu.addItem("8", 2);
    polyMenu.addItem("12", 3);
    polyMenu.addItem("16", 4);
    polyMenu.setSelectedId(audioProcessor.polyphony == 4 ? 1 : audioProcessor.polyphony == 8 ? 2 : audioProcessor.polyphony == 12 ? 3 : 4, NotificationType::dontSendNotification);
    polyMenu.onChange = [this]()
        {
            const int value = polyMenu.getSelectedId();
            auto poly = value == 1 ? 4 : value == 2 ? 8 : value == 3 ? 12 : 16;
            audioProcessor.setPolyphony(poly);
        };
    polyMenu.setBounds(col+115+80+50,row,55,25);

    // NOISE SLIDERS
    col = 10; row += 35;

    addAndMakeVisible(noiseLabel);
    noiseLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    noiseLabel.setFont(FontOptions(16.0f));
    noiseLabel.setText("NOISE", NotificationType::dontSendNotification);
    noiseLabel.setBounds(col, row, 70, 25);
    noiseLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(noiseFilterMenu);
    noiseFilterMenu.addItem("LP", 1);
    noiseFilterMenu.addItem("BP", 2);
    noiseFilterMenu.addItem("HP", 3);
    noiseFilterMenu.setTooltip("Noise filter mode");
    noiseFilterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "noise_filter_mode", noiseFilterMenu);
    noiseFilterMenu.setBounds(col+75, row, 60, 25);
    row += 25;

    noiseMix = std::make_unique<Rotary>(p, "noise_mix", "Mix", LabelFormat::Percent, "vel_noise_mix");
    addAndMakeVisible(*noiseMix);
    noiseMix->setBounds(col,row,70,75);

    noiseRes = std::make_unique<Rotary>(p, "noise_res", "Res", LabelFormat::Percent, "vel_noise_res");
    addAndMakeVisible(*noiseRes);
    noiseRes->setBounds(col,row+75,70,75);

    noiseFreq = std::make_unique<Rotary>(p, "noise_filter_freq", "Freq", LabelFormat::Hz);
    addAndMakeVisible(*noiseFreq);
    noiseFreq->setBounds(col+70,row,70,75);

    noiseQ = std::make_unique<Rotary>(p, "noise_filter_q", "Q", LabelFormat::float1);
    addAndMakeVisible(*noiseQ);
    noiseQ->setBounds(col+70,row+75,70,75);

    row += 165;
    envelopeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    envelopeLabel.setFont(FontOptions(16.0f));
    addAndMakeVisible(envelopeLabel);
    envelopeLabel.setText("ADSR", NotificationType::dontSendNotification);
    envelopeLabel.setBounds(col+10, row, 60, 25);
    row += 25;

    noiseA = std::make_unique<Rotary>(p, "noise_att", "Attack", LabelFormat::millis);
    addAndMakeVisible(*noiseA);
    noiseA->setBounds(col,row,70,75);

    noiseS = std::make_unique<Rotary>(p, "noise_sus", "Sus", LabelFormat::Percent);
    addAndMakeVisible(*noiseS);
    noiseS->setBounds(col,row+75,70,75);

    noiseD = std::make_unique<Rotary>(p, "noise_dec", "Decay", LabelFormat::millis);
    addAndMakeVisible(*noiseD);
    noiseD->setBounds(col+70,row,70,75);

    noiseR = std::make_unique<Rotary>(p, "noise_rel", "Release", LabelFormat::millis);
    addAndMakeVisible(*noiseR);
    noiseR->setBounds(col+70,row+75,70,75);

    // MALLET
    col += 160;
    row = 35 + 10;

    addAndMakeVisible(malletLabel);
    malletLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    malletLabel.setFont(FontOptions(16.0f));
    malletLabel.setText("MALLET", NotificationType::dontSendNotification);
    malletLabel.setBounds(col+5, row, 70, 25);

    row += 25;

    malletMix = std::make_unique<Rotary>(p, "mallet_mix", "Mix", LabelFormat::Percent, "vel_mallet_mix");
    addAndMakeVisible(*malletMix);
    malletMix->setBounds(col,row,70,75);

    malletRes = std::make_unique<Rotary>(p, "mallet_res", "Res", LabelFormat::Percent, "vel_mallet_res");
    addAndMakeVisible(*malletRes);
    malletRes->setBounds(col,row+75,70,75);

    malletStiff = std::make_unique<Rotary>(p, "mallet_stiff", "Stiff", LabelFormat::Hz, "vel_mallet_stiff");
    addAndMakeVisible(*malletStiff);
    malletStiff->setBounds(col,row+75+75,70,75);
    
    // RES A
    col += 90;
    row = 35 + 10;

    addAndMakeVisible(aLabel);
    aLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    aLabel.setFont(FontOptions(16.0f));
    aLabel.setText("A RES", NotificationType::dontSendNotification);
    aLabel.setBounds(col, row, 50, 25);

    addAndMakeVisible(aModel);
    aModel.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    aModel.setColour(ComboBox::arrowColourId, Colours::white);
    aModel.setColour(ComboBox::textColourId, Colours::white);
    aModel.addItem("String", 1);
    aModel.addItem("Beam", 2);
    aModel.addItem("Squared", 3);
    aModel.addItem("Membrane", 4);
    aModel.addItem("Plate", 5);
    aModel.addItem("Drumhead", 6);
    aModel.addItem("Marimba", 7);
    aModel.addItem("Open Tube", 8);
    aModel.addItem("Closed Tube", 9);
    aModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "a_model", aModel);
    aModel.setBounds(col+50+40, row, 100, 25);

    addAndMakeVisible(aPartials);
    aPartials.addItem("4", 1);
    aPartials.addItem("8", 2);
    aPartials.addItem("16", 3);
    aPartials.addItem("32", 4);
    aPartials.addItem("64", 5);
    aPartials.setTooltip("Number of partials");
    aPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "a_partials", aPartials);
    aPartials.setBounds(col+50+110+40, row, 60, 25);

    row += 25;

    aDecay = std::make_unique<Rotary>(p, "a_decay", "Decay", LabelFormat::seconds2f, "vel_a_decay");
    addAndMakeVisible(*aDecay);
    aDecay->setBounds(col,row,70,75);

    aDamp = std::make_unique<Rotary>(p, "a_damp", "Material", LabelFormat::Percent, "", true);
    addAndMakeVisible(*aDamp);
    aDamp->setBounds(col+70,row,70,75);

    aHit = std::make_unique<Rotary>(p, "a_hit", "Hit", LabelFormat::Percent, "vel_a_hit");
    addAndMakeVisible(*aHit);
    aHit->setBounds(col+70+70,row,70,75);
    
    aRel = std::make_unique<Rotary>(p, "a_rel", "Release", LabelFormat::Percent);
    addAndMakeVisible(*aRel);
    aRel->setBounds(col+70+70+70,row,70,75);

    row += 75;

    aTone = std::make_unique<Rotary>(p, "a_tone", "Tone", LabelFormat::Percent, "", true);
    addAndMakeVisible(*aTone);
    aTone->setBounds(col,row,70,75);

    aInharm = std::make_unique<Rotary>(p, "a_inharm", "Inharm", LabelFormat::Percent, "vel_a_inharm");
    addAndMakeVisible(*aInharm);
    aInharm->setBounds(col+70,row,70,75);

    aRatio = std::make_unique<Rotary>(p, "a_ratio", "Ratio", LabelFormat::float1);
    addAndMakeVisible(*aRatio);
    aRatio->setBounds(col+70+70,row,70,75);

    aCut = std::make_unique<Rotary>(p, "a_cut", "LowCut", LabelFormat::Hz);
    addAndMakeVisible(*aCut);
    aCut->setBounds(col+70+70+70,row,70,75);

    // RES B
    row += 90;

    addAndMakeVisible(bLabel);
    bLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    bLabel.setFont(FontOptions(16.0f));
    bLabel.setText("B RES", NotificationType::dontSendNotification);
    bLabel.setBounds(col, row, 50, 25);

    addAndMakeVisible(bModel);
    bModel.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    bModel.setColour(ComboBox::arrowColourId, Colours::white);
    bModel.setColour(ComboBox::textColourId, Colours::white);
    bModel.addItem("String", 1);
    bModel.addItem("Beam", 2);
    bModel.addItem("Squared", 3);
    bModel.addItem("Membrane", 4);
    bModel.addItem("Plate", 5);
    bModel.addItem("Drumhead", 6);
    bModel.addItem("Marimba", 7);
    bModel.addItem("Open Tube", 8);
    bModel.addItem("Closed Tube", 9);
    bModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "b_model", bModel);
    bModel.setBounds(col+50+40, row, 100, 25);

    addAndMakeVisible(bPartials);
    bPartials.addItem("4", 1);
    bPartials.addItem("8", 2);
    bPartials.addItem("16", 3);
    bPartials.addItem("32", 4);
    bPartials.addItem("64", 5);
    bPartials.setTooltip("Number of partials");
    bPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "b_partials", bPartials);
    bPartials.setBounds(col+50+110+40, row, 60, 25);

    row += 25;

    bDecay = std::make_unique<Rotary>(p, "b_decay", "Decay", LabelFormat::seconds2f, "vel_b_decay");
    addAndMakeVisible(*bDecay);
    bDecay->setBounds(col,row,70,75);

    bDamp = std::make_unique<Rotary>(p, "b_damp", "Material", LabelFormat::Percent, "", true);
    addAndMakeVisible(*bDamp);
    bDamp->setBounds(col+70,row,70,75);

    bHit = std::make_unique<Rotary>(p, "b_hit", "Hit", LabelFormat::Percent, "vel_b_hit");
    addAndMakeVisible(*bHit);
    bHit->setBounds(col+70+70,row,70,75);

    bRel = std::make_unique<Rotary>(p, "b_rel", "Release", LabelFormat::Percent);
    addAndMakeVisible(*bRel);
    bRel->setBounds(col+70+70+70,row,70,75);

    row += 75;

    bTone = std::make_unique<Rotary>(p, "b_tone", "Tone", LabelFormat::Percent, "", true);
    addAndMakeVisible(*bTone);
    bTone->setBounds(col,row,70,75);

    bInharm = std::make_unique<Rotary>(p, "b_inharm", "Inharm", LabelFormat::Percent, "vel_b_inharm");
    addAndMakeVisible(*bInharm);
    bInharm->setBounds(col+70,row,70,75);

    bRatio = std::make_unique<Rotary>(p, "b_ratio", "Ratio", LabelFormat::float1);
    addAndMakeVisible(*bRatio);
    bRatio->setBounds(col+70+70,row,70,75);

    bCut = std::make_unique<Rotary>(p, "b_cut", "LowCut", LabelFormat::Hz);
    addAndMakeVisible(*bCut);
    bCut->setBounds(col+70+70+70,row,70,75);

    // COUPLING
    col += 70*4+20;
    row = 35 + 10;

    addAndMakeVisible(couple);
    couple.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    couple.setColour(ComboBox::arrowColourId, Colours::white);
    couple.setColour(ComboBox::textColourId, Colours::white);
    couple.addItem("A + B", 1);
    couple.addItem("A > B", 2);
    couple.setTooltip("Coupling: Parallel or Serial");
    coupleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "couple", couple);
    couple.setBounds(col, row, 70, 25);

    row += 25;

    abMix = std::make_unique<Rotary>(p, "ab_mix", "A+B", LabelFormat::ABMix, "", true);
    addAndMakeVisible(*abMix);
    abMix->setBounds(col,row,70,75);

    gain = std::make_unique<Rotary>(p, "gain", "Gain", LabelFormat::dB, "", true);
    addAndMakeVisible(*gain);
    gain->setBounds(col, row+75*3+25+10, 70, 75);

    // KEYBOARD

    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setMidiChannel(1);
    keyboardComponent.setBounds(bounds.withTop(bounds.getHeight() - 60).withLeft(10).withWidth(bounds.getWidth()-20));
    keyboardComponent.setAvailableRange(24, 100);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colour(globals::COLOR_ACTIVE));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour(globals::COLOR_ACTIVE).withAlpha(0.5f));
}

RipplerXAudioProcessorEditor::~RipplerXAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================

void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(globals::COLOR_BACKGROUND));
    g.setColour(Colour(globals::COLOR_NEUTRAL_LIGHT));
    g.drawVerticalLine(20+70+70, 100.0f, 380.0f); // noise div
    g.drawVerticalLine(20+70+70+20+70, 100.0f, 380.0f); // mallet div
    g.drawVerticalLine(40+70*3+70*4+20, 100.0f, 380.0f); // resonators div
}

void RipplerXAudioProcessorEditor::repaintVelSliders()
{
    noiseMix.get()->repaint();
    noiseRes.get()->repaint();
    malletMix.get()->repaint();
    malletRes.get()->repaint();
    malletStiff.get()->repaint();
    aDecay.get()->repaint();
    aHit.get()->repaint();
    aInharm.get()->repaint();
    bDecay.get()->repaint();
    bHit.get()->repaint();
    bInharm.get()->repaint();
}

void RipplerXAudioProcessorEditor::resized()
{
}
