#pragma once

#include <JuceHeader.h>

class RipplerXAudioProcessor;

class About : public juce::Component {
public:
    About() 
    {
        addAndMakeVisible(githublink);
        githublink.setURL(URL("https://github.com/tiagolr/ripplerx"));
        githublink.setButtonText("github.com/tiagolr/ripplerx");
    }

    ~About() override {}

    HyperlinkButton githublink;

    void mouseDown(const juce::MouseEvent& e) override;
    void paint(Graphics& g) override;
};