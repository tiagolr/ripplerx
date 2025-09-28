// Copyright 2025 tilr

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

RipplerXAudioProcessorEditor::RipplerXAudioProcessorEditor(RipplerXAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    audioProcessor.params.addParameterListener("couple", this);
    audioProcessor.params.addParameterListener("a_on", this);
    audioProcessor.params.addParameterListener("b_on", this);
    audioProcessor.params.addParameterListener("a_model", this);
    audioProcessor.params.addParameterListener("b_model", this);
    audioProcessor.params.addParameterListener("mallet_type", this);

    setSize(650, 485);
    setScaleFactor(audioProcessor.scale); // FIX - Logic

    juce::Component::SafePointer<RipplerXAudioProcessorEditor> safeThis(this); // FIX Renoise DAW crashing on plugin instantiated
    MessageManager::callAsync([safeThis] {
        if (safeThis != nullptr)
            safeThis->setScaleFactor(safeThis->audioProcessor.scale); // FIX - Reaper
    });
    
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
    sun.setBounds(col+128, row+3, 20, 20);
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
    moon.setBounds(col+128, row+3, 20, 20);
    moon.onClick = [this] {
        audioProcessor.toggleTheme();
        loadTheme();
        repaint();
    };

    col += 215;

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
    velButton.setBounds(col, row, 80, 25);

    addAndMakeVisible(polyLabel);
    polyLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    polyLabel.setFont(FontOptions(16.0f));
    polyLabel.setText("Voices", NotificationType::dontSendNotification);
    polyLabel.setBounds(col+80+5, row, 50, 25);

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
    polyMenu.setBounds(col+80+50+5,row,55,25);

    addAndMakeVisible(presetMenu);
    presetMenu.setText("Patch");
    presetMenu.addItem("Import", 7777);
    presetMenu.addItem("Export", 8888);
    presetMenu.addSeparator();
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
    presetMenu.getRootMenu()->addColumnBreak();
    presetMenu.addItem("Bong", 14);
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

    auto resetPresetMenu = [this]() {
        if (audioProcessor.currentProgram > -1) {
            presetMenu.setSelectedId(audioProcessor.currentProgram + 1, NotificationType::dontSendNotification);
        }
    };

    resetPresetMenu();

    presetMenu.onChange = [this, resetPresetMenu]()
        {
            const int value = presetMenu.getSelectedId();
            if (value == 7777) {
                mFileChooser.reset(new juce::FileChooser(importWindowTitle, juce::File(), patchExtension));
                mFileChooser->launchAsync(juce::FileBrowserComponent::openMode |
                    juce::FileBrowserComponent::canSelectFiles,
                    [this](const juce::FileChooser& fc)
                    {
                        const auto u = fc.getURLResult();
                        auto file = !u.isEmpty() ? u.getLocalFile() : File();

                        if (!file.existsAsFile())
                            return;

                        juce::String fileContent = file.loadFileAsString();
                        auto xml = juce::XmlDocument::parse(fileContent);
                        if (xml != nullptr) {
                            juce::MemoryBlock mb;
                            audioProcessor.copyXmlToBinary(*xml, mb);
                            audioProcessor.setStateInformation(mb.getData(), (int)mb.getSize());
                        }
                    });
                resetPresetMenu();
            }
            else if (value == 8888) {
                mFileChooser.reset(new juce::FileChooser(exportWindowTitle, juce::File::getSpecialLocation(juce::File::userDesktopDirectory), patchExtension));
                mFileChooser->launchAsync(juce::FileBrowserComponent::saveMode |
                    juce::FileBrowserComponent::canSelectFiles |
                    juce::FileBrowserComponent::warnAboutOverwriting, [this](const juce::FileChooser& fc)
                    {
                        auto file = fc.getResult();
                        if (file == juce::File{})
                            return;

                        juce::MemoryBlock mb;
                        processor.getStateInformation(mb);

                        if (auto xml = audioProcessor.getXmlFromBinary(mb.getData(), (int)mb.getSize())) {
                            juce::String xmlString = xml->toString();
                            file.replaceWithText(xmlString.toStdString());
                        }
                    });
                resetPresetMenu();
            }
            else if (value > 0) {
                MessageManager::callAsync([this, value] {
                    audioProcessor.setCurrentProgram(value - 1);
                });
            }
        };
    presetMenu.setBounds(getWidth() - 110, row, 100, 25);

    addAndMakeVisible(settingsBtn);
    settingsBtn.setBounds(Rectangle<int>(25, 25).withPosition(presetMenu.getBounds().getTopLeft().translated(-25 - 10+3, 0)));
    settingsBtn.setAlpha(0.f);
    settingsBtn.onClick = [this]() { showSettingsMenu(); };

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

    addAndMakeVisible(noiseOSC);
    noiseDCAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "noise_osc", noiseOSC);
    noiseOSC.setComponentID("noise_osc");
    noiseOSC.setSliderStyle(Slider::LinearBar);
    noiseOSC.setTooltip("Use oscillators to excite the resonators instead of noise");
    noiseOSC.setBounds(col + 65, row+2, 80, 20);
    noiseOSC.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    noiseOSC.setTextBoxStyle(Slider::NoTextBox, true, 10, 10);

    row += 25;

    noiseA = std::make_unique<Rotary>(p, "noise_att", "Attack", LabelFormat::millis, "vel_noise_att");
    addAndMakeVisible(*noiseA);
    noiseA->setBounds(col,row,70,75);

    noiseS = std::make_unique<Rotary>(p, "noise_sus", "Sus", LabelFormat::Percent, "vel_noise_sus");
    addAndMakeVisible(*noiseS);
    noiseS->setBounds(col,row+75,70,75);

    noiseD = std::make_unique<Rotary>(p, "noise_dec", "Decay", LabelFormat::millis, "vel_noise_dec");
    addAndMakeVisible(*noiseD);
    noiseD->setBounds(col+70,row,70,75);

    noiseR = std::make_unique<Rotary>(p, "noise_rel", "Release", LabelFormat::millis, "vel_noise_rel");
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

    //addAndMakeVisible(ktrackBtn);
    //ktrackBtn.setBounds(col, row + 75 + 75 + 15, 70, 25);
    //ktrackBtn.setButtonText("KTrack");
    //ktrackBtn.setComponentID("button");
    //ktrackBtn.onClick = [this]()
    //    {
    //        auto param = audioProcessor.params.getParameter("mallet_ktrack");
    //        param->setValueNotifyingHost(param->getValue() == 0.0f ? 1.0f : 0.0f);
    //        toggleUIComponents();
    //    };

    addAndMakeVisible(ktrackSlider);
    ktrackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.params, "mallet_ktrack", ktrackSlider);
    ktrackSlider.setComponentID("ktrack");
    ktrackSlider.setSliderStyle(Slider::LinearBar);
    ktrackSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    ktrackSlider.setTextBoxStyle(Slider::NoTextBox, true, 10, 10);
    ktrackSlider.setBounds(col, row + 75 + 75 + 15, 70, 25);

    malletPitch = std::make_unique<Rotary>(p, "mallet_pitch", "Pitch", LabelFormat::PitchSemis, "", true);
    addAndMakeVisible(*malletPitch);
    malletPitch->setBounds(col, row + 75 + 75 + 40, 70, 75);

    malletFilter = std::make_unique<Rotary>(p, "mallet_filter", "Filter", LabelFormat::FilterLPHP, "", true);
    addAndMakeVisible(*malletFilter);
    malletFilter->setBounds(col, row + 75 + 75 + 75 + 40, 70, 75);

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
    aModel.addItem("Marimba2", 10);
    aModel.addItem("Bell", 11);
    aModel.addItem("Djembe", 12);
    aModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "a_model", aModel);
    aModel.setBounds(col+50+40-5, row, 100, 25);

    // because comboBoxes only work with ordered items
    // use an invisible button on top to show a menu with the items in custom order
    addAndMakeVisible(aModelBtn);
    aModelBtn.setAlpha(0.0f);
    aModelBtn.setBounds(aModel.getBounds().expanded(2));
    aModelBtn.onClick = [this]()
        {
            showModelMenu(true);
        };

    addAndMakeVisible(aPartials);
    aPartials.addItem("4", 1);
    aPartials.addItem("8", 2);
    aPartials.addItem("16", 3);
    aPartials.addItem("32", 4);
    aPartials.addItem("64", 5);
    aPartials.addItem("1", 6);
    aPartials.addItem("2", 7);
    aPartials.setTooltip("Number of partials");
    aPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "a_partials", aPartials);
    aPartials.setBounds(col+50+110+40-5, row, 60, 25);

    // because comboBoxes only work with ordered items
    // use an invisible button on top to show a menu with the items in custom order
    addAndMakeVisible(aPartialsBtn);
    aPartialsBtn.setAlpha(0.0f);
    aPartialsBtn.setBounds(aPartials.getBounds().expanded(2));
    aPartialsBtn.onClick = [this]()
        {
            showPartialsMenu(true);
        };


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
    bModel.addItem("Marimba2", 10);
    bModel.addItem("Bell", 11);
    bModel.addItem("Djembe", 12);
    bModelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "b_model", bModel);
    bModel.setBounds(col+50+40-5, row, 100, 25);

    // because comboBoxes only work with ordered items
    // use an invisible button on top to show a menu with the items in custom order
    addAndMakeVisible(bModelBtn);
    bModelBtn.setAlpha(0.0f);
    bModelBtn.setBounds(bModel.getBounds().expanded(2));
    bModelBtn.onClick = [this]()
        {
            showModelMenu(false);
        };

    addAndMakeVisible(bPartials);
    bPartials.addItem("4", 1);
    bPartials.addItem("8", 2);
    bPartials.addItem("16", 3);
    bPartials.addItem("32", 4);
    bPartials.addItem("64", 5);
    bPartials.addItem("1", 6);
    bPartials.addItem("2", 7);
    bPartials.setTooltip("Number of partials");
    bPartialsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "b_partials", bPartials);
    bPartials.setBounds(col+50+110+40-5, row, 60, 25);

    // because comboBoxes only work with ordered items
    // use an invisible button on top to show a menu with the items in custom order
    addAndMakeVisible(bPartialsBtn);
    bPartialsBtn.setAlpha(0.0f);
    bPartialsBtn.setBounds(bPartials.getBounds().expanded(2));
    bPartialsBtn.onClick = [this]()
        {
            showPartialsMenu(false);
        };

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
    keyboardComponent.setOctaveForMiddleC(3);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colour(globals::COLOR_ACTIVE));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour(globals::COLOR_ACTIVE).withAlpha(0.5f));
#if !defined(DEBUG)
    keyboardComponent.clearKeyMappings();
#endif

    // METER
    meter = std::make_unique<Meter>(p);
    addAndMakeVisible(*meter);
    meter->setBounds(bounds.getRight() - 85, 235 + 40, 60, 95 - 40);

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

    bool is_tube = a_model == OpenTube || a_model == ClosedTube;
    aDamp.get()->setVisible(!is_tube);
    aTone.get()->setVisible(!is_tube);
    aHit.get()->setVisible(!is_tube);
    aInharm.get()->setVisible(!is_tube);
    aRatio.get()->setVisible(!is_tube && (a_model == Beam || a_model == Membrane || a_model == Plate || a_model == Djembe));
    aRadius.get()->setVisible(is_tube);
    aPartials.setVisible(!is_tube);

    is_tube = b_model == OpenTube || b_model == ClosedTube;
    bDamp.get()->setVisible(!is_tube);
    bTone.get()->setVisible(!is_tube);
    bHit.get()->setVisible(!is_tube);
    bInharm.get()->setVisible(!is_tube);
    bRatio.get()->setVisible(!is_tube && (b_model == Beam || b_model == Membrane || b_model == Plate || b_model == Djembe));
    bRadius.get()->setVisible(is_tube);
    bPartials.setVisible(!is_tube);

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
    ktrackSlider.setVisible(isSampleMallet);
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
    pitchLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL));
    bendLabel.setColour(juce::Label::ColourIds::textColourId, Colour(COLOR_NEUTRAL));
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

    // draw settings button
    drawGear(g, settingsBtn.getBounds(), 10, 6, Colour(COLOR_ACTIVE), Colour(COLOR_BACKGROUND));
}

void RipplerXAudioProcessorEditor::drawGear(Graphics& g, Rectangle<int> bounds, float radius, int segs, Colour color, Colour background)
{
    float x = bounds.toFloat().getCentreX();
    float y = bounds.toFloat().getCentreY();
    float oradius = radius;
    float iradius = radius / 3.f;
    float cradius = iradius / 1.5f;
    float coffset = MathConstants<float>::twoPi;
    float inc = MathConstants<float>::twoPi / segs;

    g.setColour(color);
    g.fillEllipse(x - oradius, y - oradius, oradius * 2.f, oradius * 2.f);

    g.setColour(background);
    for (int i = 0; i < segs; i++) {
        float angle = coffset + i * inc;
        float cx = x + std::cos(angle) * oradius;
        float cy = y + std::sin(angle) * oradius;
        g.fillEllipse(cx - cradius, cy - cradius, cradius * 2, cradius * 2);
    }
    g.fillEllipse(x - iradius, y - iradius, iradius * 2.f, iradius * 2.f);
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
    noiseA.get()->repaint();
    noiseD.get()->repaint();
    noiseS.get()->repaint();
    noiseR.get()->repaint();
}

void RipplerXAudioProcessorEditor::resized()
{
}

void RipplerXAudioProcessorEditor::showSettingsMenu()
{
    bool stereoizer = (bool)audioProcessor.params.getRawParameterValue("stereoizer")->load();
    bool reuseVoices = (bool)audioProcessor.params.getRawParameterValue("reuse_voices")->load();
    bool fadeoutRepeats = (bool)audioProcessor.params.getRawParameterValue("fadeout_repeats")->load();

    PopupMenu menu;
    PopupMenu scaleMenu;
    scaleMenu.addItem(1, "100%", true, audioProcessor.scale == 1.0f);
    scaleMenu.addItem(2, "125%", true, audioProcessor.scale == 1.25f);
    scaleMenu.addItem(3, "150%", true, audioProcessor.scale == 1.5f);
    scaleMenu.addItem(4, "175%", true, audioProcessor.scale == 1.75f);
    scaleMenu.addItem(5, "200%", true, audioProcessor.scale == 2.0f);

    PopupMenu polyphonyMenu;
    polyphonyMenu.addItem(10, "Reuse voices on repeated notes", true, reuseVoices);
    polyphonyMenu.addItem(12, "Fadeout repeated notes", reuseVoices, fadeoutRepeats);

    menu.addSubMenu("UI Scale", scaleMenu);
    menu.addSubMenu("Polyphony", polyphonyMenu);
    menu.addItem(11, "Stereoizer", true, stereoizer);

    auto menuPos = localPointToGlobal(settingsBtn.getBounds().getBottomRight());
    menu.showMenuAsync(PopupMenu::Options()
        .withTargetScreenArea({ menuPos.getX() - 125, menuPos.getY(), 1, 1 }),
        [this, stereoizer, reuseVoices, fadeoutRepeats](int result) {
            if (result == 0) return;
            if (result == 1) audioProcessor.setScale(1.f);
            if (result == 2) audioProcessor.setScale(1.25f);
            if (result == 3) audioProcessor.setScale(1.5f);
            if (result == 4) audioProcessor.setScale(1.75f);
            if (result == 5) audioProcessor.setScale(2.f);
            if (result >= 1 && result <= 5) {
                setScaleFactor(audioProcessor.scale);
            }

            if (result == 10) {
                auto param = audioProcessor.params.getParameter("reuse_voices");
                param->setValueNotifyingHost(reuseVoices ? 0.f : 1.f);
            }
            if (result == 11) {
                auto param = audioProcessor.params.getParameter("stereoizer");
                param->setValueNotifyingHost(stereoizer ? 0.f : 1.f);
            }
            if (result == 12) {
                auto param = audioProcessor.params.getParameter("fadeout_repeats");
                param->setValueNotifyingHost(fadeoutRepeats ? 0.f : 1.f);
            }
        });
}

void RipplerXAudioProcessorEditor::showMalletMenu()
{
    auto malletType = (int)audioProcessor.params.getRawParameterValue("mallet_type")->load() + 1;

    PopupMenu mallets;
    mallets.addItem(1, "Impulse", true, malletType == 1);
    mallets.addSeparator();
    mallets.addItem(13, "Click 1", true, malletType == 13);
    mallets.addItem(14, "Click 2", true, malletType == 14);
    mallets.addItem(18, "Click 3", true, malletType == 18);
    mallets.addItem(24, "Strike", true, malletType == 24);
    mallets.addItem(23, "Wood", true, malletType == 23);
    mallets.addItem(21, "Metal 1", true, malletType == 21);
    mallets.addItem(22, "Metal 2", true, malletType == 22);
    mallets.addItem(25, "Perc 1", true, malletType == 25);
    mallets.addItem(26, "Perc 2", true, malletType == 26);
    mallets.addItem(19, "Blip", true, malletType == 19);
    mallets.addItem(20, "Blop", true, malletType == 20);
    mallets.addSeparator();
    mallets.addItem(1000, "Load File", true, malletType == 12);

    auto menuPos = localPointToGlobal(malletLabel.getBounds().getBottomLeft());
    mallets.showMenuAsync(PopupMenu::Options()
        .withTargetScreenArea({ menuPos.getX(), menuPos.getY(), 1, 1 }),
        [this](int result) {
            if (result == 0) return;
            auto param = audioProcessor.params.getParameter("mallet_type");

            if (result == 1000) {
                mFileChooser.reset(new juce::FileChooser(importAudioTitle, juce::File(), audioExtension));
                mFileChooser->launchAsync(juce::FileBrowserComponent::openMode |
                    juce::FileBrowserComponent::canSelectFiles,
                    [this, param](const juce::FileChooser& fc)
                    {
                        const auto url = fc.getURLResult();
                        File file = !url.isEmpty() ? url.getLocalFile() : File();

                        if (!file.existsAsFile())
                            return;

                        auto path = file.getFullPathName();
                        MessageManager::callAsync([this, path, param]() {
                            audioProcessor.malletSampler->loadSample(path);
                            param->setValueNotifyingHost(param->convertTo0to1(float(kUserFile)));
                        });
                    });
            }
            else {
                param->setValueNotifyingHost(param->convertTo0to1(float(result - 1)));
            }

        });
}

void RipplerXAudioProcessorEditor::showModelMenu(bool AorB)
{
    auto choice = (int)audioProcessor.params.getRawParameterValue(AorB ? "a_model" : "b_model")->load() + 1;

    PopupMenu mallets;
    mallets.addItem(1,  "String", true, choice == 1);
    mallets.addItem(2,  "Beam", true, choice == 2);
    mallets.addItem(3,  "Squared", true, choice == 3);
    mallets.addItem(11, "Bell", true, choice == 11);
    mallets.addItem(4,  "Membrane", true, choice == 4);
    mallets.addItem(5,  "Plate", true, choice == 5);
    mallets.addItem(6,  "Drumhead", true, choice == 6);
    mallets.addItem(12, "Djembe", true, choice == 12);
    mallets.addItem(7,  "Marimba", true, choice == 7);
    mallets.addItem(10, "Marimba2", true, choice == 10);
    mallets.addItem(8,  "OpenTube", true, choice == 8);
    mallets.addItem(9,  "ClosedTube", true, choice == 9);

    auto menuPos = localPointToGlobal((AorB ? aModel : bModel).getBounds().getBottomLeft());
    mallets.showMenuAsync(PopupMenu::Options()
        .withTargetComponent(*this)
        .withTargetScreenArea({ menuPos.getX(), menuPos.getY(), 1, 1 }),
        [this, AorB](int result) {
            if (result == 0) return;
            auto param = audioProcessor.params.getParameter(AorB ? "a_model" : "b_model");
            param->setValueNotifyingHost(param->convertTo0to1(float(result - 1)));
        });
}

void RipplerXAudioProcessorEditor::showPartialsMenu(bool AorB)
{
    auto choice = (int)audioProcessor.params.getRawParameterValue(AorB ? "a_partials" : "b_partials")->load() + 1;

    PopupMenu mallets;
    mallets.addItem(6, "1", true, choice == 6);
    mallets.addItem(7, "2", true, choice == 7);
    mallets.addItem(1, "4", true, choice == 1);
    mallets.addItem(2, "8", true, choice == 2);
    mallets.addItem(3, "16", true, choice == 3);
    mallets.addItem(4, "32", true, choice == 4);
    mallets.addItem(5, "64", true, choice == 5);

    auto menuPos = localPointToGlobal((AorB ? aPartials : bPartials).getBounds().getBottomLeft());
    mallets.showMenuAsync(PopupMenu::Options()
        .withTargetComponent(*this)
        .withTargetScreenArea({ menuPos.getX(), menuPos.getY(), 1, 1 }),
        [this, AorB](int result) {
            if (result == 0) return;
            auto param = audioProcessor.params.getParameter(AorB ? "a_partials" : "b_partials");
            param->setValueNotifyingHost(param->convertTo0to1(float(result - 1)));
        });
}