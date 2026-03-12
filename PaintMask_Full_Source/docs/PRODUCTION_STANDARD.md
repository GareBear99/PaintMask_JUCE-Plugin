# PaintMask Production Standard

## Quality bar
PaintMask only counts as production-ready when all of the following are true:

1. Standalone build launches cleanly.
2. VST3 validates in at least Reaper and Ableton.
3. AU validates in Logic on macOS.
4. Trial -> Lite fallback behaves correctly across restarts.
5. Saved state files load identically across versions.
6. Canvas interaction stays responsive at target FPS.
7. MIDI output matches the painted content deterministically.
8. No audio spikes, denormals, or runaway note floods.
9. Undo/redo behaves safely.
10. Crashes fail loudly with actionable logs.

## Minimum release targets
- macOS: AU, VST3, Standalone
- Windows: VST3, Standalone

## Performance goals
- Idle CPU: low single digits
- Heavy canvas session: responsive editor, no visible hitching
- Scan/playback: deterministic and sample-safe
