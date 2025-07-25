Multidumper
===========

Multidumper is a program that takes emulated video game music files and dumps them to multiple WAV files, one per channel.

This is a fork of https://bitbucket.org/losnoco/multidumper with the following changes:

1. Added new options:
   - `--sampling_rate=<number>` to choose the sampling rate
   - `--no_progress` to disable printing progress to the console (only useful when wrapped by a GUI)
   - `--loop_count=<number>` to pick the number of loops
   - `--play_length=<ms>` to pick the total duration (for formats like NSF)
   - `--fade_length=<ms>` to pick the fade duration for looped tracks
   - `--gap_length=<ms>` to add silence after non-looped tracks
   - `--default_filter` to enable game_music_emu's default filters
   - `--treble_filter=<number>` to set game_music_emu's treble filter
   - `--bass_filter=<number>` to set game_music_emu's bass filter
2. Fixed bugs:
   - File length was truncated to a multiple of 512 samples
3. Updated dependencies to latest versions:
   - ZLib
   - game_music_emu
     - Using Nuked core for YM2612
   - highly_experimental
4. I make builds for WIndows and release them!
