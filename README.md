<h1 align="center">
  <img src="doc/logo.png" width="200" style="padding: 5px;" />
  <br>
</h1>
<div align="center">

[![Windows Support](https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)](https://github.com/tiagolr/ripplerx/releases)
[![Ubuntu Support](https://img.shields.io/badge/Linux-E95420?style=for-the-badge&logo=linux&logoColor=white)](https://github.com/tiagolr/ripplerx/releases)
[![Mac Support](https://img.shields.io/badge/MACOS-adb8c5?style=for-the-badge&logo=macos&logoColor=white)](https://github.com/tiagolr/ripplerx/releases)

</div>
<div align="center">

[![GitHub package.json version](https://img.shields.io/github/v/release/tiagolr/ripplerx?color=%40&label=latest)](https://github.com/tiagolr/ripplerx/releases/latest)
![GitHub issues](https://img.shields.io/github/issues-raw/tiagolr/ripplerx)
![GitHub all releases](https://img.shields.io/github/downloads/tiagolr/ripplerx/total)
![Github license](https://img.shields.io/github/license/tiagolr/ripplerx)

</div>

RipplerX is a physically modeled synth, capable of sounds similar to AAS Chromaphone and Ableton Collision.

<div align="center">

![Screenshot](./doc/ripplerx.png)

![Screenshot](./doc/ripplerx-dark.png)

</div>

## Features

  * **Cross-platform**, available on Windows, Linux, and macOS.
  * **Dual resonators** with serial and parallel coupling.
  * **9 Models** of acoustic resonators: String, Beam, Squared, Membrane, Drumhead, Plate, Marimba, Open tube, and Closed tube.
  * **Inharmonicity, Tone, Ratio, and Material** sliders to shape the timbre.
  * **Noise and mallet** generators.
  * Up to **64 partials** per resonator.
  * Built with **JUCE**.

## Download

* [Download latest release](https://github.com/tiagolr/ripplerx/releases)
* Current builds include VST3 and LV2 for Windows, Linux, and macOS, plus AU for macOS.
* Clap is planned when there is official [JUCE support](https://juce.com/blog/juce-roadmap-update-q3-2024/).

## About

RipplerX is a port of [Rippler2](https://github.com/tiagolr/tilr_jsfx?tab=readme-ov-file#rippler-2)
for the Reaper DAW. It started as a research project into physically modelled drums and ended up a
synth heavily based on AAS Chromaphone and Saike's [Partials](https://github.com/JoepVanlier/JSFX?tab=readme-ov-file#partials---modal-synthesis-effect).

The first version was based off Partials since it is one of the few open source modal synths I could find. From there,
the main reference became Chromaphone and Collision. The controls and models are almost a one-to-one match.

RipplerX has additional controls for Inharmonicity and model Ratio and a distinct sound.
Chromaphone is a more complete synth with more features; it has a more sophisticated mallet generator, physics-based serial coupling, a better UI, etc.
If you like this synth, you should definitely check it out.

The value of RipplerX is as an open source alternative to AAS plugins.
It also provides an entry point for developers or researchers into modal synthesis.

**Acknowledgments**

* Saike for the open source Partials synth, which the original Rippler was based on.
* AAS for Chromaphone synth and manual with hints about the models and parameters used.
* Nathan Ho and others for articles about [modal synthesis](https://nathan.ho.name/posts/exploring-modal-synthesis).
* DSP coders like Tale and Scott Stillwell for providing open source JSFXs and libs.

## macOS

macOS builds are untested and unsigned. Please let me know of any issues by opening a ticket. Don't expect a prompt fix as I don't have access to that OS.

Because the builds are unsigned, you may have to run the following commands:

```bash
sudo xattr -dr com.apple.quarantine /path/to/your/plugins/RipplerX.component
sudo xattr -dr com.apple.quarantine /path/to/your/plugins/RipplerX.vst3
sudo xattr -dr com.apple.quarantine /path/to/your/plugins/RipplerX.lv3
```

The commands above will recursively remove the quarantine flag from the plugins.

## Audio In

From version 1.4.0 Rippler supports Audio In. This means that you can feed an audio signal into the plugin resonators and excite them when pressing keys.

This feature works in many DAWs *but not all*. Here are some working examples:

- **Reaper** – Play an audio clip in the same track as Rippler. The sound will only go through when pressing keys.
- **Ableton Live** – Add an audio clip, on the mixer click the button below **Audio to** and send it to the plugin.
- **Bitwig** – Add an audio clip, on the Rippler device window click **Show plug-in sidechain** and select the clip track.
- **Cubase** – This is done by track sidechaining. Unfortunately, I can no longer find the video for this tutorial.
- **Logic** – Track sidechaining should also work for the AU version.

Because not all DAWs support this feature (looking at you *FL Studio*), an FX version may be added in the future. Meanwhile, sidechaining into the synth is the simplest way to use this.

## Microtuning

Microtuning is supported via MTS-ESP. You can install a player like [MTS-ESP MINI](https://oddsound.com/mtsespmini.php) and run it alongside RipplerX. The two plugins will communicate and automatically tune Rippler to the config file loaded.

Tuning files can be found, for example, at https://www.huygens-fokker.org/docs/scales.zip

For more details and a list of players, also check this section of the [Surge tuning guide](https://surge-synthesizer.github.io/tuning-guide/#mts-esp).

## Build

```bash
git clone --recurse-submodules https://github.com/tiagolr/ripplerx.git

# windows
cmake -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release -S . -B ./build

# linux
sudo apt update
sudo apt-get install libx11-dev libfreetype-dev libfontconfig1-dev libasound2-dev libxrandr-dev libxinerama-dev libxcursor-dev
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -S . -B ./build
cmake --build ./build --config Release

# macOS
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" -S . -B ./build
cmake --build ./build --config Release
```
