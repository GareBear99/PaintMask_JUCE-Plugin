# PaintMask

PaintMask is a visual MIDI instrument where painted strokes, splashes, and geometry on a canvas generate music.

This package is the **full current project handoff** generated in-chat:
- a JUCE/CMake source scaffold
- a browser prototype
- release-prep docs
- preset/addon templates

## What is included
- `Source/` — JUCE source scaffold for the plugin
- `prototype/` — standalone HTML prototype you can open in a browser
- `docs/` — production notes, UX guidance, release checklist, module map, and host-test matrix
- `presets/` — demo PaintMask state
- `addons/` — theme/addon template
- `.github/ISSUE_TEMPLATE/` — repo issue templates
- `licenses/` — license note

## Honest status
This package is **not a compiled production binary**.
It is the strongest complete source/prototype/release-prep bundle produced here.

To reach a shipping plugin, the remaining external steps are:
1. Point CMake at your JUCE checkout.
2. Compile locally for Standalone / AU / VST3.
3. Fix any environment-specific compile issues.
4. Host-test in Logic, Ableton, Reaper, etc.
5. Tune performance and polish.

## Build
Typical flow:

```bash
mkdir build
cd build
cmake .. -DJUCE_DIR=/path/to/JUCE
cmake --build .
```

## Business model
- 30-day full Studio trial
- Lite fallback after trial
- Studio at $1/month

## Author
Gary Doman
