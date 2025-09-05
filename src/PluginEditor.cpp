// Copyright 2025 tilr

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

RipplerXAudioProcessorEditor::RipplerXAudioProcessorEditor (RipplerXAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    audioProcessor.params.addParameterListener("couple", this);
    audioProcessor.params.addParameterListener("a_on", this);
    audioProcessor.params.addParameterListener("b_on", this);
    audioProcessor.params.addParameterListener("a_model", this);
    audioProcessor.params.addParameterListener("b_model", this);
    audioProcessor.params.addParameterListener("mallet_type", this);

    setSize (650, 485);
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
    logo.onClick = [this] {
        about->setVisible(true);
    };

#if defined(DEBUG)
    addAndMakeVisible(presetExport);
    presetExport.setAlpha(0.f);
    presetExport.setTooltip("DEBUG ONLY - Exports preset to debug console");
    presetExport.setButtonText("Export");
    presetExport.setBounds(10, 10, 100, 25);
    presetExport.onClick = [this] {
        auto state = audioProcessor.params.copyState();
        std::unique_ptr<juce::XmlElement>xml(state.createXml());
        juce::String xmlString = xml->toString();
        DBG(xmlString.toStdString());
    };
#endif

    addAndMakeVisible(sun);
    sun.setAlpha(0.9f);
    juce::MemoryInputStream sunStream(BinaryData::sun_png, BinaryData::sun_pngSize, false);
    juce::Image sunImage = juce::ImageFileFormat::loadFrom(sunStream);
    if (sunImage.isValid())
    {
        sun.setImages(false, true, true, 
            sunImage, 1.0f, juce::Colours::transparentBlack, 
            sunImage, 1.0f, juce::Colours::transparentBlack, 
            sunImage, 1.0f, juce::Colours::transparentBlack);
    }
    sun.setBounds(col+138, row+3, 20, 20);
    sun.onClick = [this] {
        audioProcessor.toggleTheme();
        loadTheme();
        repaint();
    };

    addAndMakeVisible(moon);
    moon.setAlpha(0.9f);
    juce::MemoryInputStream moonStream(BinaryData::moon_png, BinaryData::moon_pngSize, false);
    juce::Image moonImage = juce::ImageFileFormat::loadFrom(moonStream);
    if (moonImage.isValid())
    {
        moon.setImages(false, true, true, 
            moonImage, 1.0f, juce::Colours::transparentBlack, 
            moonImage, 1.0f, juce::Colours::transparentBlack, 
            moonImage, 1.0f, juce::Colours::transparentBlack);
    }
    moon.setBounds(col+138, row+3, 20, 20);
    moon.onClick = [this] {
        audioProcessor.toggleTheme();
        loadTheme();
        repaint();
    };

    col += 160;
    addAndMakeVisible(sizeLabel);
    sizeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    sizeLabel.setFont(FontOptions(16.0f));
    sizeLabel.setText("UI", NotificationType::dontSendNotification);
    sizeLabel.setBounds(col, row, 30, 25);

    addAndMakeVisible(sizeMenu);
    sizeMenu.addItem("100%", 1);
    sizeMenu.addItem("125%", 2);
    sizeMenu.addItem("150%", 3);
    sizeMenu.addItem("175%", 4);
    sizeMenu.addItem("200%", 5);
    sizeMenu.setSelectedId(audioProcessor.scale == 1.0f ? 1 
        : audioProcessor.scale == 1.25f ? 2
        : audioProcessor.scale == 1.5f ? 3 
        : audioProcessor.scale == 1.75f ? 4 
        : 5);
    sizeMenu.onChange = [this]()
        {
            const int value = sizeMenu.getSelectedId();
            auto scale = value == 1 ? 1.0f : value == 2 ? 1.25f : value == 3 ? 1.5f : value == 4 ? 1.75f : 2.0f;
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
    polyLabel.setBounds(col+115+80+5, row, 50, 25);

    addAndMakeVisible(polyMenu);
    polyMenu.addItem("1", 1);
    polyMenu.addItem("4", 2);
    polyMenu.addItem("8", 3);
    polyMenu.addItem("12", 4);
    polyMenu.addItem("16", 5);
    polyMenu.setSelectedId(audioProcessor.polyphony == 1 ? 1 : audioProcessor.polyphony == 4 ? 2 : audioProcessor.polyphony == 8 ? 3 : audioProcessor.polyphony == 12 ? 4 : 5, NotificationType::dontSendNotification);
    polyMenu.onChange = [this]()
        {
            const int value = polyMenu.getSelectedId();
            auto poly = value == 1 ? 1 : value == 2 ? 4 : value == 3 ? 8 : value == 4 ? 12 : 16;
            MessageManager::callAsync([this, poly] {
                audioProcessor.setPolyphony(poly);
            });
        };
    polyMenu.setBounds(col+115+80+50+5,row,55,25);

    addAndMakeVisible(presetMenu);
    presetMenu.setText("Patch");
    presetMenu.addItem("Init", 1);
    presetMenu.addItem("Harpsi", 2);
    presetMenu.addItem("Harp", 3);
    presetMenu.addItem("Sankyo", 4);
    presetMenu.addItem("Tubes", 5);
    presetMenu.addItem("Stars", 6);
    presetMenu.addItem("DoorBell", 7);
    presetMenu.addItem("Bells", 8);
    presetMenu.addItem("Bells2", 9);
    presetMenu.addItem("KeyRing", 10);
    presetMenu.addItem("Sink", 11);
    presetMenu.addItem("Cans", 12);
    presetMenu.addItem("Gong", 13);
    presetMenu.addItem("Bong", 14);
    presetMenu.getRootMenu()->addColumnBreak();
    presetMenu.addItem("Marimba", 15);
    presetMenu.addItem("Fight", 16);
    presetMenu.addItem("Tabla", 17);
    presetMenu.addItem("Tabla2", 18);
    presetMenu.addItem("Strings", 19);
    presetMenu.addItem("OldClock", 20);
    presetMenu.addItem("Crystal", 21);
    presetMenu.addItem("Ride", 22);
    presetMenu.addItem("Ride2", 23);
    presetMenu.addItem("Crash", 24);
    presetMenu.addItem("Vibes", 25);
    presetMenu.addItem("Flute", 26);
    presetMenu.addItem("Fifths", 27);
    presetMenu.addItem("Kalimba", 28);
    if (audioProcessor.currentProgram > -1) {
        presetMenu.setSelectedId(audioProcessor.currentProgram + 1, NotificationType::dontSendNotification);
    }

    presetMenu.onChange = [this]()
        {
            const int value = presetMenu.getSelectedId();
            if (value > 0) {
                MessageManager::callAsync([this, value] {
                    audioProcessor.setCurrentProgram(value - 1);
                });
            }
        };
    presetMenu.setBounds(getWidth() - 110, row, 100, 25);

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

    noiseFreq = std::make_unique<Rotary>(p, "noise_filter_freq", "Freq", LabelFormat::Hz, "vel_noise_freq");
    addAndMakeVisible(*noiseFreq);
    noiseFreq->setBounds(col+70,row,70,75);

    noiseQ = std::make_unique<Rotary>(p, "noise_filter_q", "Q", LabelFormat::float1, "vel_noise_q");
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

    noiseATen = std::make_unique<TensionCtrl>(p, "noise_att_ten", false);
    addAndMakeVisible(*noiseATen);
    noiseATen->setBounds(Rectangle<int>(15, 15).withPosition(noiseA->getBounds().getTopRight().translated(-10, 0)));

    noiseDTen = std::make_unique<TensionCtrl>(p, "noise_dec_ten", true);
    addAndMakeVisible(*noiseDTen);
    noiseDTen->setBounds(Rectangle<int>(15, 15).withPosition(noiseD->getBounds().getTopRight().translated(-10, 0)));

    noiseRTen = std::make_unique<TensionCtrl>(p, "noise_rel_ten", true);
    addAndMakeVisible(*noiseRTen);
    noiseRTen->setBounds(Rectangle<int>(15, 15).withPosition(noiseR->getBounds().getTopRight().translated(-10, 0)));

    // MALLET
    col += 160;
    row = 35 + 10;

    addAndMakeVisible(malletLabel);
    malletLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    malletLabel.setFont(FontOptions(16.0f));
    malletLabel.setText("MALLET", NotificationType::dontSendNotification);
    malletLabel.setBounds(col+5, row, 70, 25);

    addAndMakeVisible(malletButton);
    malletButton.setBounds(malletLabel.getBounds().expanded(15, 5));
    malletButton.setAlpha(0.f);
    malletButton.onClick = [this] { showMalletMenu(); };

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

    malletPitch = std::make_unique<Rotary>(p, "mallet_pitch", "Pitch", LabelFormat::PitchSemis, "", true);
    addAndMakeVisible(*malletPitch);
    malletPitch->setBounds(col, row + 75 + 75, 70, 75);

    malletFilter = std::make_unique<Rotary>(p, "mallet_filter", "Filter", LabelFormat::FilterLPHP, "", true);
    addAndMakeVisible(*malletFilter);
    malletFilter->setBounds(col, row + 75 + 75 + 75, 70, 75);

    addAndMakeVisible(malletSubLabel);
    malletSubLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    malletSubLabel.setFont(FontOptions(11.0f));
    malletSubLabel.setJustificationType(Justification::centred);
    malletSubLabel.setText(audioProcessor.params.getParameter("mallet_type")->getCurrentValueAsText(), dontSendNotification);
    malletSubLabel.setBounds(malletLabel.getBounds().translated(0, 15).expanded(15, 0));
    malletSubLabel.setInterceptsMouseClicks(false, false);
    
    // RES A
    col += 90;
    row = 35 + 10;

    addAndMakeVisible(aLabel);
    aLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    aLabel.setFont(FontOptions(16.0f));
    aLabel.setText("A RES", NotificationType::dontSendNotification);
    aLabel.setBounds(col, row, 50, 25);

    addAndMakeVisible(aOn);
    juce::MemoryInputStream ainputStream(BinaryData::on_png, BinaryData::on_pngSize, false);
    juce::Image aimage = juce::ImageFileFormat::loadFrom(ainputStream);
    if (aimage.isValid())
    {
        aOn.setImages(false, true, true, 
            aimage, 1.0f, juce::Colours::transparentBlack, 
            aimage, 1.0f, juce::Colours::transparentBlack, 
            aimage, 1.0f, juce::Colours::transparentBlack);
    }
    aOn.setBounds(col+50+5, row+5, 16, 17);
    aOn.onClick = [this] {
        MessageManager::callAsync([this] {
            auto param = audioProcessor.params.getParameter("a_on");
            auto newVal = param->getValue() ? 0.0f : 1.0f;
            param->beginChangeGesture();
            param->setValueNotifyingHost(newVal);
            param->endChangeGesture();
        });
    };

    addAndMakeVisible(aModel);
    aModel.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    aModel.setColour(ComboBox::arrowColourId, Colours::white);
    aModel.setColour(ComboBox::textColourId, Colours::white);
    aModel.setColour(ComboBox::outlineColourId, Colours::transparentWhite);
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
    aModel.setBounds(col+50+40-5, row, 100, 25);

    addAndMakeVisible(aPartials);
    aPartials.addItem("4", 1);
    aPartials.addItem("8", 2);
    aPartials.addItem("16", 3);
    aPartials.addItem("32", 4);
    aPartials.addItem("64", 5);
    aPartials.setTooltip("Number of partials");
    aPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "a_partials", aPartials);
    aPartials.setBounds(col+50+110+40-5, row, 60, 25);

    row += 25;

    aDecay = std::make_unique<Rotary>(p, "a_decay", "Decay", LabelFormat::seconds2f, "vel_a_decay");
    addAndMakeVisible(*aDecay);
    aDecay->setBounds(col,row,70,75);

    aDamp = std::make_unique<Rotary>(p, "a_damp", "Material", LabelFormat::Percent, "vel_a_damp", true);
    addAndMakeVisible(*aDamp);
    aDamp->setBounds(col+70,row,70,75);

    aRadius = std::make_unique<Rotary>(p, "a_radius", "Radius", LabelFormat::Percent);
    addAndMakeVisible(*aRadius);
    aRadius->setBounds(col+70,row,70,75);

    aHit = std::make_unique<Rotary>(p, "a_hit", "Hit", LabelFormat::Percent, "vel_a_hit");
    addAndMakeVisible(*aHit);
    aHit->setBounds(col+70+70,row,70,75);
    
    aRel = std::make_unique<Rotary>(p, "a_rel", "Release", LabelFormat::Percent);
    addAndMakeVisible(*aRel);
    aRel->setBounds(col+70+70+70,row,70,75);

    row += 75;

    aTone = std::make_unique<Rotary>(p, "a_tone", "Tone", LabelFormat::Percent, "vel_a_tone", true);
    addAndMakeVisible(*aTone);
    aTone->setBounds(col,row,70,75);

    aInharm = std::make_unique<Rotary>(p, "a_inharm", "Inharm", LabelFormat::float2_100, "vel_a_inharm");
    addAndMakeVisible(*aInharm);
    aInharm->setBounds(col+70,row,70,75);

    aRatio = std::make_unique<Rotary>(p, "a_ratio", "Ratio", LabelFormat::float1);
    addAndMakeVisible(*aRatio);
    aRatio->setBounds(col+70+70,row,70,75);

    aCut = std::make_unique<Rotary>(p, "a_cut", "Filter", LabelFormat::FilterLPHP, "", true);
    addAndMakeVisible(*aCut);
    aCut->setBounds(col+70+70+70,row,70,75);

    // RES B
    row += 90;

    addAndMakeVisible(bLabel);
    bLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    bLabel.setFont(FontOptions(16.0f));
    bLabel.setText("B RES", NotificationType::dontSendNotification);
    bLabel.setBounds(col, row, 50, 25);

    addAndMakeVisible(bOn);
    juce::MemoryInputStream binputStream(BinaryData::on_png, BinaryData::on_pngSize, false);
    juce::Image bimage = juce::ImageFileFormat::loadFrom(binputStream);
    if (bimage.isValid())
    {
        bOn.setImages(false, true, true, 
            bimage, 1.0f, juce::Colours::transparentBlack, 
            bimage, 1.0f, juce::Colours::transparentBlack, 
            bimage, 1.0f, juce::Colours::transparentBlack);
    }
    bOn.setBounds(col+50+5, row+5, 16, 17);
    bOn.onClick = [this] {
        MessageManager::callAsync([this] {
            auto param = audioProcessor.params.getParameter("b_on");
            auto newVal = param->getValue() ? 0.0f : 1.0f;
            param->beginChangeGesture();
            param->setValueNotifyingHost(newVal);
            param->endChangeGesture();
        });
    };

    addAndMakeVisible(bModel);
    bModel.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    bModel.setColour(ComboBox::arrowColourId, Colours::white);
    bModel.setColour(ComboBox::textColourId, Colours::white);
    bModel.setColour(ComboBox::outlineColourId, Colours::transparentWhite);
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
    bModel.setBounds(col+50+40-5, row, 100, 25);

    addAndMakeVisible(bPartials);
    bPartials.addItem("4", 1);
    bPartials.addItem("8", 2);
    bPartials.addItem("16", 3);
    bPartials.addItem("32", 4);
    bPartials.addItem("64", 5);
    bPartials.setTooltip("Number of partials");
    bPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "b_partials", bPartials);
    bPartials.setBounds(col+50+110+40-5, row, 60, 25);

    row += 25;

    bDecay = std::make_unique<Rotary>(p, "b_decay", "Decay", LabelFormat::seconds2f, "vel_b_decay");
    addAndMakeVisible(*bDecay);
    bDecay->setBounds(col,row,70,75);

    bDamp = std::make_unique<Rotary>(p, "b_damp", "Material", LabelFormat::Percent, "vel_b_damp", true);
    addAndMakeVisible(*bDamp);
    bDamp->setBounds(col+70,row,70,75);

    bRadius = std::make_unique<Rotary>(p, "b_radius", "Radius", LabelFormat::Percent);
    addAndMakeVisible(*bRadius);
    bRadius->setBounds(col+70,row,70,75);

    bHit = std::make_unique<Rotary>(p, "b_hit", "Hit", LabelFormat::Percent, "vel_b_hit");
    addAndMakeVisible(*bHit);
    bHit->setBounds(col+70+70,row,70,75);

    bRel = std::make_unique<Rotary>(p, "b_rel", "Release", LabelFormat::Percent);
    addAndMakeVisible(*bRel);
    bRel->setBounds(col+70+70+70,row,70,75);

    row += 75;

    bTone = std::make_unique<Rotary>(p, "b_tone", "Tone", LabelFormat::Percent, "vel_b_tone", true);
    addAndMakeVisible(*bTone);
    bTone->setBounds(col,row,70,75);

    bInharm = std::make_unique<Rotary>(p, "b_inharm", "Inharm", LabelFormat::float2_100, "vel_b_inharm");
    addAndMakeVisible(*bInharm);
    bInharm->setBounds(col+70,row,70,75);

    bRatio = std::make_unique<Rotary>(p, "b_ratio", "Ratio", LabelFormat::float1);
    addAndMakeVisible(*bRatio);
    bRatio->setBounds(col+70+70,row,70,75);

    bCut = std::make_unique<Rotary>(p, "b_cut", "Filter", LabelFormat::FilterLPHP, "", true);
    addAndMakeVisible(*bCut);
    bCut->setBounds(col+70+70+70,row,70,75);

    // COUPLING
    col += 70*4+20;
    row = 35 + 10;

    addAndMakeVisible(couple);
    couple.setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_ACTIVE));
    couple.setColour(ComboBox::arrowColourId, Colours::white);
    couple.setColour(ComboBox::textColourId, Colours::white);
    couple.setColour(ComboBox::outlineColourId, Colours::transparentWhite);
    couple.addItem("A + B", 1);
    couple.addItem("A > B", 2);
    couple.setTooltip("Coupling: Parallel or Serial");
    coupleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "couple", couple);
    couple.setBounds(col, row, 70, 25);

    row += 25;

    abMix = std::make_unique<Rotary>(p, "ab_mix", "A:B", LabelFormat::ABMix, "", true);
    addAndMakeVisible(*abMix);
    abMix->setBounds(col,row,70,75);

    abSplit = std::make_unique<Rotary>(p, "ab_split", "Split", LabelFormat::Percent);
    addAndMakeVisible(*abSplit);
    abSplit->setBounds(col,row,70,75);

    addAndMakeVisible(pitchLabel);
    pitchLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL));
    pitchLabel.setFont(FontOptions(15.0f));
    pitchLabel.setJustificationType(Justification::centred);
    pitchLabel.setText("Pitch", NotificationType::dontSendNotification);
    pitchLabel.setBounds(col, row+85-2, 70, 20);

    aPitch = std::make_unique<Pitch>(p, "A Pitch", "a_coarse", "a_fine");
    addAndMakeVisible(*aPitch);
    aPitch->setBounds(col+5,row+105,70-10,20);

    bPitch = std::make_unique<Pitch>(p, "B Pitch", "b_coarse", "b_fine");
    addAndMakeVisible(*bPitch);
    bPitch->setBounds(col+5,row+130,70-10,20);

    gain = std::make_unique<Rotary>(p, "gain", "Gain", LabelFormat::dB, "", true);
    addAndMakeVisible(*gain);
    gain->setBounds(col, row+75*3+25+10, 70, 75);

    addAndMakeVisible(bendLabel);
    bendLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL));
    bendLabel.setFont(FontOptions(15.0f));
    bendLabel.setJustificationType(Justification::centred);
    bendLabel.setText("PBend", NotificationType::dontSendNotification);
    bendLabel.setBounds(col, row + 85 - 2 + 75, 70, 20);

    bendPitch = std::make_unique<Pitch>(p, "Bend Range", "bend_range", "");
    addAndMakeVisible(*bendPitch);
    bendPitch->setBounds(col + 5, row + 105 + 75, 70 - 10, 20);

    // KEYBOARD
    auto bounds = getLocalBounds();
    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setMidiChannel(1);
    keyboardComponent.setBounds(bounds.withTop(bounds.getHeight() - 60).withLeft(10).withWidth(bounds.getWidth()-20));
    keyboardComponent.setAvailableRange(24, 100);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colour(globals::COLOR_ACTIVE));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour(globals::COLOR_ACTIVE).withAlpha(0.5f));
#if !defined(DEBUG)
    keyboardComponent.clearKeyMappings();
#endif

    // METER
    // meter = std::make_unique<Meter>(p);
    // addAndMakeVisible(*meter);
    // meter->setBounds(bounds.getRight() - 85, 235, 60, 95);

    // ABOUT
    about = std::make_unique<About>();
    addAndMakeVisible(*about);
    about->setBounds(getBounds());
    about->setVisible(false);

    toggleUIComponents();
    loadTheme();
}

RipplerXAudioProcessorEditor::~RipplerXAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    delete customLookAndFeel;
    audioProcessor.params.removeParameterListener("couple", this);
    audioProcessor.params.removeParameterListener("a_on", this);
    audioProcessor.params.removeParameterListener("b_on", this);
    audioProcessor.params.removeParameterListener("a_model", this);
    audioProcessor.params.removeParameterListener("b_model", this);
    audioProcessor.params.removeParameterListener("mallet_type", this);
}

void RipplerXAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue) 
{
    (void)parameterID;
    (void)newValue;
    if (parameterID == "mallet_type") {
        malletSubLabel.setText(audioProcessor.params.getParameter("mallet_type")->getCurrentValueAsText(), dontSendNotification);
    }
    juce::MessageManager::callAsync([this] { 
        toggleUIComponents();
    });
};

void RipplerXAudioProcessorEditor::toggleUIComponents()
{
    auto a_on = (bool)audioProcessor.params.getRawParameterValue("a_on")->load();
    auto b_on = (bool)audioProcessor.params.getRawParameterValue("b_on")->load();
    auto a_model = (int)audioProcessor.params.getRawParameterValue("a_model")->load();
    auto b_model = (int)audioProcessor.params.getRawParameterValue("b_model")->load();
    auto is_serial = (bool)audioProcessor.params.getRawParameterValue("couple")->load();
    auto mallet_type = (MalletType)audioProcessor.params.getRawParameterValue("mallet_type")->load();

    auto alpha = a_on ? 1.0f : 0.5f;
    aModel.setAlpha(alpha);
    aPartials.setAlpha(alpha);
    aDecay.get()->setAlpha(alpha);
    aDamp.get()->setAlpha(alpha);
    aTone.get()->setAlpha(alpha);
    aHit.get()->setAlpha(alpha);
    aRel.get()->setAlpha(alpha);
    aInharm.get()->setAlpha(alpha);
    aRatio.get()->setAlpha(alpha);
    aCut.get()->setAlpha(alpha);
    aRadius.get()->setAlpha(alpha);

    alpha = b_on ? 1.0f : 0.5f;
    bModel.setAlpha(alpha);
    bPartials.setAlpha(alpha);
    bDecay.get()->setAlpha(alpha);
    bDamp.get()->setAlpha(alpha);
    bTone.get()->setAlpha(alpha);
    bHit.get()->setAlpha(alpha);
    bRel.get()->setAlpha(alpha);
    bInharm.get()->setAlpha(alpha);
    bRatio.get()->setAlpha(alpha);
    bCut.get()->setAlpha(alpha);
    bRadius.get()->setAlpha(alpha);

    bool is_tube = a_model >= 7;
    aDamp.get()->setVisible(!is_tube);
    aTone.get()->setVisible(!is_tube);
    aHit.get()->setVisible(!is_tube);
    aInharm.get()->setVisible(!is_tube);
    aRatio.get()->setVisible(!is_tube && (a_model == ModelNames::Beam || a_model == ModelNames::Membrane || a_model == ModelNames::Plate));
    aRadius.get()->setVisible(is_tube);

    is_tube = b_model >= 7;
    bDamp.get()->setVisible(!is_tube);
    bTone.get()->setVisible(!is_tube);
    bHit.get()->setVisible(!is_tube);
    bInharm.get()->setVisible(!is_tube);
    bRatio.get()->setVisible(!is_tube && (b_model == ModelNames::Beam || b_model == ModelNames::Membrane || b_model == ModelNames::Plate));
    bRadius.get()->setVisible(is_tube);

    aPartials.setVisible(a_model < 7);
    bPartials.setVisible(b_model < 7);

    couple.setAlpha((a_on && b_on) ? 1.0f : 0.5f);
    abMix.get()->setAlpha((a_on && b_on) ? 1.0f : 0.5f);
    abSplit.get()->setAlpha((a_on && b_on) ? 1.0f : 0.5f);
    abMix.get()->setVisible(!is_serial);
    abSplit.get()->setVisible(is_serial);

    aPitch.get()->setAlpha(a_on ? 1.0f : 0.5f);
    bPitch.get()->setAlpha(b_on ? 1.0f : 0.5f);

    juce::MemoryInputStream onInputStream(
        audioProcessor.darkTheme ? BinaryData::on_dark_png : BinaryData::on_png,
        audioProcessor.darkTheme ? BinaryData::on_dark_pngSize : BinaryData::on_pngSize,
        false
    );
    juce::Image onImage = juce::ImageFileFormat::loadFrom(onInputStream);

    juce::MemoryInputStream offInputStream(
        audioProcessor.darkTheme ? BinaryData::off_dark_png : BinaryData::off_png,
        audioProcessor.darkTheme ? BinaryData::off_dark_pngSize : BinaryData::off_pngSize,
        false
    );
    juce::Image offImage = juce::ImageFileFormat::loadFrom(offInputStream);

    juce::MemoryInputStream logoInputStream(
        audioProcessor.darkTheme ? BinaryData::logo_dark_png : BinaryData::logo_png,
        audioProcessor.darkTheme ? BinaryData::logo_dark_pngSize : BinaryData::logo_pngSize,
        false
    );
    juce::Image logoImage = juce::ImageFileFormat::loadFrom(logoInputStream);

    if (onImage.isValid() && offImage.isValid())
    {
        aOn.setImages(false, true, true,
            a_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack, 
            a_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack, 
            a_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack);

        bOn.setImages(false, true, true,
            b_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack, 
            b_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack, 
            b_on ? onImage : offImage, 1.0f, juce::Colours::transparentBlack);
    }
    if (logoImage.isValid()) {
        logo.setImages(false, true, true,
            logoImage, 1.0f, juce::Colours::transparentBlack,
            logoImage, 1.0f, juce::Colours::transparentBlack,
            logoImage, 1.0f, juce::Colours::transparentBlack
       );
    }

    bool isSampleMallet = mallet_type >= MalletType::kUserFile;
    malletStiff->setVisible(mallet_type == MalletType::kImpulse);
    malletPitch->setVisible(isSampleMallet);
    malletFilter->setVisible(isSampleMallet);
}

void RipplerXAudioProcessorEditor::loadTheme()
{
    auto isDark = audioProcessor.darkTheme;
    COLOR_BACKGROUND = isDark ? COLOR_BACKGROUND_D : COLOR_BACKGROUND_L;
    COLOR_ACTIVE = isDark ? COLOR_ACTIVE_D : COLOR_ACTIVE_L;
    COLOR_NEUTRAL = isDark ? COLOR_NEUTRAL_D : COLOR_NEUTRAL_L;
    COLOR_NEUTRAL_LIGHT = isDark ? COLOR_NEUTRAL_LIGHT_D : COLOR_NEUTRAL_LIGHT_L;
    COLOR_VEL = isDark ? COLOR_VEL_D : COLOR_VEL_L;

    bModel.setColour(ComboBox::backgroundColourId, Colour(COLOR_ACTIVE));
    bModel.setColour(ComboBox::arrowColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);
    bModel.setColour(ComboBox::textColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);

    aModel.setColour(ComboBox::backgroundColourId, Colour(COLOR_ACTIVE));
    aModel.setColour(ComboBox::arrowColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);
    aModel.setColour(ComboBox::textColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);

    couple.setColour(ComboBox::backgroundColourId, Colour(COLOR_ACTIVE));
    couple.setColour(ComboBox::arrowColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);
    couple.setColour(ComboBox::textColourId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);

    noiseLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    malletLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    malletSubLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    envelopeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    sizeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    pitchLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL));
    polyLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    aLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    bLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL_LIGHT));
    velButton.setColour(juce::TextButton::buttonColourId, Colour(COLOR_BACKGROUND));
    velButton.setColour(juce::TextButton::buttonOnColourId, Colour(COLOR_VEL));
    velButton.setColour(juce::TextButton::textColourOffId, Colour(COLOR_VEL));
    velButton.setColour(juce::TextButton::textColourOnId, isDark ? Colour(COLOR_BACKGROUND).darker(0.7f) : Colours::white);
    velButton.setColour(juce::ComboBox::outlineColourId, Colour(COLOR_VEL));

    moon.setVisible(!isDark);
    sun.setVisible(isDark);

    setLookAndFeel(nullptr);
    delete customLookAndFeel;
    customLookAndFeel = new CustomLookAndFeel(isDark);
    setLookAndFeel(customLookAndFeel);

    toggleUIComponents();
}

//==============================================================================

void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(globals::COLOR_BACKGROUND));
    g.setColour(Colour(globals::COLOR_NEUTRAL_LIGHT).darker(audioProcessor.darkTheme ? 3.5f : 0.0f));
    g.drawVerticalLine(20+70+70, 100.0f, 380.0f); // noise div
    g.drawVerticalLine(20+70+70+20+70, 100.0f, 380.0f); // mallet div
    g.drawVerticalLine(40+70*3+70*4+20, 100.0f, 380.0f); // resonators div

    // draw mallet label dropdown arrow
    g.setColour(Colour(COLOR_NEUTRAL_LIGHT));
    auto bounds = malletLabel.getBounds().removeFromRight(20).translated(13,0);
    Path path;
    path.startNewSubPath((float)bounds.getX() + 3.0f, (float)bounds.getCentreY() - 2.0f);
    path.lineTo((float)bounds.getCentreX(), (float)bounds.getCentreY() + 3.0f);
    path.lineTo((float)bounds.getRight() - 3.0f, (float)bounds.getCentreY() - 2.0f);
    g.strokePath(path, PathStrokeType(2.0f));
}

void RipplerXAudioProcessorEditor::repaintVelSliders()
{
    noiseMix.get()->repaint();
    noiseRes.get()->repaint();
    noiseFreq.get()->repaint();
    noiseQ.get()->repaint();
    malletMix.get()->repaint();
    malletRes.get()->repaint();
    malletStiff.get()->repaint();
    aDecay.get()->repaint();
    aHit.get()->repaint();
    aInharm.get()->repaint();
    bDecay.get()->repaint();
    bHit.get()->repaint();
    bInharm.get()->repaint();
    aDamp.get()->repaint();
    bDamp.get()->repaint();
    aTone.get()->repaint();
    bTone.get()->repaint();
}

void RipplerXAudioProcessorEditor::resized()
{
}

void RipplerXAudioProcessorEditor::showMalletMenu()
{
    auto malletType = (int)audioProcessor.params.getRawParameterValue("mallet_type")->load() + 1;

    PopupMenu mallets;
    mallets.addItem(1, "Impulse", true, malletType == 1);
    mallets.addSeparator();
    mallets.addItem(13, "Click1", true, malletType == 13);
    mallets.addItem(14, "Click2", true, malletType == 14);
    mallets.addItem(15, "Click3", true, malletType == 15);
    mallets.addItem(16, "Click4", true, malletType == 16);
    mallets.addItem(17, "Click5", true, malletType == 17);
    mallets.addItem(18, "Click6", true, malletType == 18);

    auto menuPos = localPointToGlobal(malletLabel.getBounds().getBottomLeft());
    mallets.showMenuAsync(PopupMenu::Options()
        .withTargetScreenArea({ menuPos.getX(), menuPos.getY(), 1, 1 }),
        [this](int result) {
            if (result == 0) return;
            auto param = audioProcessor.params.getParameter("mallet_type");
            param->setValueNotifyingHost(param->convertTo0to1(float(result - 1)));
        });
}