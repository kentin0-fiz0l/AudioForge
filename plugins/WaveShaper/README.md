# WaveShaper

Waveshaping distortion plugin with multiple curve shapes and tone control.

## Features

- **5 Waveshaping Modes**:
  - Soft Clip - Smooth saturation
  - Hard Clip - Digital clipping
  - Tanh - Hyperbolic tangent curve
  - Cubic - Polynomial waveshaping
  - Asymmetric - Asymmetric distortion

- **Drive Control**: 0-100% drive amount (up to 11x gain)
- **Mix Control**: Dry/wet blend (0-100%)
- **Tone Control**: Low-pass filter to tame harsh highs (500Hz-20kHz)
- **Output Gain**: -12 to +12 dB output level adjustment
- **DC Blocker**: Automatic DC offset removal
- **Metering**: Input and output level meters

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Drive | 0-100% | 0% | Amount of drive/distortion |
| Mix | 0-100% | 100% | Dry/wet blend |
| Tone | 0-100% | 100% | Low-pass filter cutoff |
| Shape | 5 modes | Soft Clip | Waveshaping curve type |
| Output | -12 to +12 dB | 0 dB | Output level |

## Usage

1. **Choose a Shape**: Select the waveshaping curve that fits your sound
2. **Add Drive**: Increase drive to add distortion/saturation
3. **Adjust Tone**: Roll off highs if the sound is too harsh
4. **Blend with Mix**: Mix in dry signal for parallel processing
5. **Set Output**: Adjust output gain to compensate for level changes

## Technical Details

- **Sample Rate**: 44.1kHz - 192kHz
- **Bit Depth**: 32-bit float processing
- **Latency**: Zero latency
- **Format**: VST3
- **Channels**: Stereo

## Tips

- Start with low drive amounts and increase gradually
- Use the Mix control for parallel distortion effects
- The Tone control helps tame digital harshness
- Asymmetric mode adds even harmonics for warmth
- Soft Clip is great for gentle saturation
- Hard Clip creates more aggressive distortion

## Version History

- **v0.1.0** - Initial release
  - 5 waveshaping modes
  - Drive, Mix, Tone controls
  - Input/output metering
  - DC blocker
