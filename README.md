# Capture, View & Record (CVR)

CVR is a small application that allows to view & play video and audio from capture cards with low latency. Optionally, AV can also be recorded to disk.

The intent to write CVR was to be able to connect video game consoles via HDMI being able to actually play and also (but not just) record gameplay. Other applications are usually designed to record video and only provide a low-priority, high-latency capture preview which is unsuitable for playing games without splitting output to a dedicated monitor beforehand.

## Prerequisites

To use CVR, you currently need:

* Linux
* a capture card by [Blackmagic Design](http://www.blackmagicdesign.com/) which is supported by their [Decklink SDK](http://www.blackmagicdesign.com/support/sdks)
* the Decklink SDK for compilation, plus drivers for your capture card (if you are on Gentoo, check out media-libs/blackmagic-desktop-video from [our overlay](http://gentoo-overlay.megacoffee.net/) for just the runtime library & kernel module)
* PulseAudio
* Qt 4.8
* a video card that supports XVideo in UYVY pixel mode

## Current state

CVR is in very early development. Since it's only done by one developer with just one capture card, there haven't been any tests with any cards other than Blackmagic Design Intensity Pro or on other computers.

What works so far:
* live, low-latency playback of AV through XVideo and PulseAudio
* recording (simple & time-shifted)

Current restrictions:
* hard-coded input format 720p59.94 (suitable for Microsoft's Xbox 360 and Xbox One), no configuration available yet (but planned)

For more details, please see the issue tracker.

## License

MIT license for own code, may degrade to restrictive licenses on compilation.

## Why no precompiled binaries?

Due to unresolved questions regarding SDK licensing, you currently need to build CVR yourself.

## Tested hardware

The following setup is currently verified to work:

* Intel Core2Duo 3,0GHz
* Nvidia GeForce 8800GT, Nvidia GeForce GTX 750 Ti
* Gentoo Linux 64 Bit
* Blackmagic Design Intensity Pro (others from Blackmagic Design should work as well thanks to the universal SDK)

## A quick note on buying a capture card (for game consoles)

I started writing CVR for the Xbox One release because I didn't have a TV nor monitor with HDMI input/audio output, so I wouldn't have been able to connect the Xbox One. Only after I bought my capture card I noticed that at least the Xbox doesn't output anything else than 720p59.94 or 1080p59.94 but the capture card is only capable of up to 1080i60 or 1080p30 (either processing isn't fast enough for 1080p60 or bandwidth of that card is too low), so I had to lower the resolution of my Xbox 360 to 720p.

Another issue I noticed was the color encoding: My card doesn't do RGB from HDMI, only color-compressed YUV where 2 pixels in width share one color. This means you may see visible artifacts on clean graphics and text (wrong color/grey) and may cause compression artifacts on recorded video. It's hard to notice on rapid gameplay footage, however. Just be more cautious than I was about technical specifications when you consider buying new capture hardware. ;)

## Developer documentation

* [Encoding](docs/encoding.md)
