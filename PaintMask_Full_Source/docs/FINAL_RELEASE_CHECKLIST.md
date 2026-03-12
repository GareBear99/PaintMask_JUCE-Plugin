# Final Release Checklist

## Build
- [ ] JUCE path set and CMake configure passes
- [ ] Debug build succeeds
- [ ] Release build succeeds
- [ ] Standalone launches
- [ ] VST3 generated
- [ ] AU generated (macOS)

## Core behavior
- [ ] Brush paints notes correctly
- [ ] Splash paints clustered events correctly
- [ ] Line tool maps to time/pitch as designed
- [ ] Circle and Triangle geometry read correctly
- [ ] Mask removes or gates events correctly
- [ ] Studio-only tools lock behind trial/subscription gates

## Trial / licensing
- [ ] First launch starts 30-day Studio trial
- [ ] Trial countdown persists locally
- [ ] Expired trial drops to Lite without breaking sessions
- [ ] Studio unlock restores locked tools
- [ ] Messaging is non-hostile and clear

## MIDI / state
- [ ] MIDI out works in major hosts
- [ ] State save/load round-trips cleanly
- [ ] Presets load deterministically
- [ ] Seeded content reproduces as expected

## Polish
- [ ] Cursor, icons, and hover states feel deliberate
- [ ] Scan beam is readable and attractive
- [ ] RGB mapping feels musical, not arbitrary
- [ ] Empty canvas state still feels premium
- [ ] Trial and Lite badges are tasteful

## Distribution
- [ ] README finalized
- [ ] Repo description and tags set
- [ ] Release notes written
- [ ] Screenshots/GIFs exported
- [ ] Binaries packaged
- [ ] Signing/notarization complete where needed
