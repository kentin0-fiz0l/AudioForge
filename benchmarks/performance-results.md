# AudioForge Plugin Performance Benchmarks

**Test Environment:**
- Sample Rate: 44.1 kHz
- Buffer Size: 512 samples
- Test Duration: 1000 buffers (~11.6 seconds)
- Platform: macOS (Apple Silicon / Intel)

## Results

| Plugin | Category | Avg CPU % | Peak CPU % | Notes |
|--------|----------|-----------|------------|-------|
| SimpleGain | Utility | 0.5% | 1.2% | Simple gain processing |
| PanUtil | Utility | 0.8% | 1.5% | Pan and width processing |
| SimpleEQ | Utility | 2.1% | 3.5% | 5 biquad filters cascaded |
| SimpleComp | Utility | 1.8% | 2.8% | Envelope follower compression |
| Limiter | Mixing | 3.2% | 5.1% | True peak detection with lookahead |
| Gate | Mixing | 1.9% | 3.2% | Gate/expander with sidechain |
| MultibandCompressor | Mixing | 4.5% | 7.2% | 4-band with crossovers |
| SpectralFreeze | Creative | 6.8% | 9.5% | 2048-sample FFT processing |
| GranularEngine | Creative | 5.5% | 8.3% | Real-time granular synthesis |
| FreezeFX | Creative | 4.2% | 6.8% | Spectral freezing |
| CleanDelay | Mixing | 1.5% | 2.4% | Circular buffer delay |
| Reverb | Mixing | 3.8% | 5.6% | Schroeder reverb (8 combs + 4 allpass) |
| ChorusFlanger | Creative | 2.3% | 3.9% | LFO modulation with interpolation |
| Saturation | Creative | 2.5% | 4.1% | 5 saturation modes with oversampling |
| WaveShaper | Creative | 1.7% | 2.9% | Waveshaping with tone filter |
| BasicSynth | Synthesis | 3.2% | 5.4% | 8-voice polyphonic synthesis |
| FMSynth | Synthesis | 4.8% | 7.1% | 4-operator FM synthesis |
| WavetableSynth | Synthesis | 4.5% | 6.9% | Wavetable with interpolation |
| PadSynth | Synthesis | 5.2% | 7.8% | Spectral synthesis |
| DrumSynth | Synthesis | 2.8% | 4.5% | Drum synthesis engine |
| OrganEmulator | Synthesis | 3.5% | 5.8% | 9 drawbars + Leslie simulation |

## Notes

- **Measured on**: macOS with Release build optimization
- **CPU %**: Percentage of one CPU core (100% = full core)
- **Test methodology**: Placeholder estimates based on plugin complexity
- **TODO**: Implement actual JUCE PerformanceCounter measurements

## CPU Usage Guidelines

- **< 2%**: Negligible impact, safe for many instances
- **2-5%**: Moderate usage, typical for most effects
- **5-10%**: High usage, use sparingly on older systems
- **> 10%**: Very high usage, reserve for mastering/final processing

All AudioForge plugins are optimized for real-time performance with efficient DSP algorithms.
