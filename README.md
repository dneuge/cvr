# Capture, View & Record (CVR)

CVR is a small application that allows to view & play video and audio from capture cards with low latency. Optionally, AV can also be recorded to disk.

The intent to write CVR was to be able to connect video game consoles via HDMI being able to actually play and also (but not just) record gameplay. Other applications are usually designed to record video and only provide a low-priority, high-latency capture preview which is unsuitable for playing games without splitting output to a dedicated monitor beforehand.

## Prerequisites

To use CVR, you currently need:

* Linux
* a capture card by [Blackmagic Design](http://www.blackmagicdesign.com/) which is supported by their Desktop Video SDK (also known as Decklink SDK)
* the Desktop Video SDK for compilation, plus drivers for your capture card (if you are on Gentoo, emerge `media-libs/blackmagic-desktop-video` and `media-libs/blackmagic-desktop-video-sdk` via [MegaCoffee overlay](http://gentoo-overlay.megacoffee.net/))
* PulseAudio
* Qt 4.8
* a video card that supports XVideo in UYVY pixel mode

## Current state

CVR is in very early development. Since it's only done by one developer with just one capture card, there haven't been any tests with any cards other than Blackmagic Design Intensity Pro or on other computers.

What works so far:
* live, low-latency playback of AV through XVideo and PulseAudio
* recording (simple & time-shifted)
  * "Motion JPEG" (basically, just full JPEG images per frame) stored in a Matroska (MKV) container, variable frame-rate; still not widely supported in 2016 but you will likely convert/recode recorded files using FFMPEG anyway which is the only program which seems to be able to reliably process that format

Current restrictions:
* hard-coded input format 720p59.94 (suitable for Microsoft's Xbox 360 and Xbox One), no configuration available yet (but planned)

For more details, please see the issue tracker.

## License

MIT license for own code, may degrade to restrictive licenses on compilation.

## How to build

To build CVR you will additionally need CMake 3.3 or above.

If you're on Gentoo and used the ebuilds linked above, compilation should be as easy as running `build.sh` which performs all configuration and compiles a single binary `build/src/cvr`.

Even if you're not, CMake *should* still be able to find everything automatically if the SDK has been installed to `/usr/include` or a subdirectory. If that fails, unzip the SDK somewhere and provide CMake with the path manually using `-DDECKLINK_INCLUDE_DIR=...`. Altogether:

<pre>
mkdir build
cd build
cmake .. -DDECKLINK_INCLUDE_DIR="/where/ever/the/includes/are"
make
</pre>

## Why no precompiled binaries?

Only me as a single user up to publication in July 2016 and the program is not polished enough to provide it ready-to-run to end-users yet, sorry... The bare minimum to implement before thinking about binaries would be a configuration screen to provide users a way to switch resolutions without any hacking of hard-coded constants.

## Why such a late publication?

I started writing CVR in November 2013 and had everything I immediately needed complete at the beginning of January 2014. I planned to release the code earlier but wasn't comfortable with the way terms & conditions in the SDK's license were stated formally (not sure if creating applications solely for Blackmagic products was permitted use). That has changed since then (or my understanding has, not sure which one :) ) and so I finally went public in July 2016.

## Tested hardware

The following setup is currently verified to work:

* **CPU:** Intel i7-4790K (very low CPU usage), Intel Core2Duo 3.0GHz
* **GPU:** Nvidia GeForce GTX 750 Ti, Nvidia GeForce 8800GT
* **OS:** Gentoo Linux 64 Bit
* **Capture card:** Blackmagic Design Intensity Pro via PCIe (others from Blackmagic Design should work as well thanks to the universal SDK)

## A quick note on buying a capture card (for game consoles)

I started writing CVR for the Xbox One release because I didn't have a TV nor monitor with HDMI input/audio output, so I wouldn't have been able to connect the Xbox One. Only after I bought my capture card I noticed that at least the Xbox doesn't output anything else than 720p59.94 or 1080p59.94 but the capture card is only capable of up to 1080i60 or 1080p30 (either processing isn't fast enough for 1080p60 or bandwidth of that card is too low), so I had to lower the resolution of my Xbox 360 to 720p.

Another issue I noticed was the color encoding: My card doesn't do RGB from HDMI, only color-compressed YUV where 2 pixels in width share one color. This means you may see visible artifacts on clean graphics and text (wrong color/grey) and may cause compression artifacts on recorded video. It's hard to notice on rapid gameplay footage, however. Just be more cautious than I was about technical specifications when you consider buying new capture hardware. ;)

## Developer documentation

* [Encoding](docs/encoding.md)
