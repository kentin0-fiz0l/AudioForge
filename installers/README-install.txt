AudioForge: free, open-source audio plugins (MIT License)
https://github.com/kentin0-fiz0l/AudioForge

This folder holds the plugins themselves. The installer for your platform puts them in the
right place for you; to install by hand instead, copy them as below, then restart your DAW
and rescan plugins.

macOS
  VST3/*.vst3          ->  /Library/Audio/Plug-Ins/VST3/        (or ~/Library/Audio/Plug-Ins/VST3/)
  AU/*.component       ->  /Library/Audio/Plug-Ins/Components/  (or ~/Library/Audio/Plug-Ins/Components/)
  Standalone/*.app     ->  /Applications/
  The plugins are not signed with an Apple Developer ID. If macOS blocks a copied plugin,
  run:  xattr -dr com.apple.quarantine <the .vst3, .component or .app>

Windows
  VST3\*.vst3          ->  C:\Program Files\Common Files\VST3\
  Standalone\*.exe     ->  anywhere you like
  The files are not code-signed, so Windows SmartScreen may warn: choose "More info", then "Run anyway".

Linux (x86_64)
  VST3/*.vst3          ->  ~/.vst3/   (or /usr/lib/vst3/)
  Standalone/*         ->  anywhere on your PATH, e.g. ~/.local/bin/
  Or run ./install.sh, which does exactly that for your user.

The MIDI tools (Arpeggiator, Chord Generator, Harmonizer and others) are MIDI effects.
Ableton Live does not load third-party MIDI effects; use them in Logic Pro, Reaper, Bitwig or Cubase.
