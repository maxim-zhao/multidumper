Multidumper
===========

Multidumper is a program that takes emulated video game music files and dumps them to multiple WAV files, one per channel.

This is a fork of https://bitbucket.org/losnoco/multidumper with the following changes:

1. Added new options:
   - `--sampling_rate=<number>` to choose the sampling rate
   - `--no_progress` to disable printing progress to the console (only useful when wrapped by a GUI)
   - `--loop_count=<number>` to pick the number of loops
   - `--max_threads=<number>` to pick the number of threads to use (defaults to all your CPU cores)
   - `--play_length=<ms>` to pick the total duration (for formats like NSF)
   - `--fade_length=<ms>` to pick the fade duration for looped tracks
   - `--gap_length=<ms>` to add silence after non-looped tracks
   - Filtering options:
     - `--default_filter` to enable any default filtering - by default, filtering is disabled for backwards compatibility
     - `--bass_filter=<number>` to set a bass filter
     - `--treble_filter=<number>` to set a treble filter
   - Emulation core options
     - `--ym2413_core=<emu2413|mame|nuked>` to select the emulation core for YM2413
     - `--ym2612_core=<mame|gens|nuked>` to select the emulation core for YM2612
2. Fixed bugs:
   - File length was truncated to a multiple of 512 samples
3. Updated dependencies to latest versions:
   - ZLib
   - highly_experimental
   - emu2413
4. Added new, up to date emulation cores:
   - [Nuked OPLL](https://github.com/nukeykt/Nuked-OPLL)
   - [Nuked OPN2](https://github.com/nukeykt/Nuked-OPN2)
5. I make builds for Windows and release them! This repo automatically builds using Github Actions and makes a release.

For compatibility reasons, any new options default to what older versions did.

Some notes on Multidumper's source
----------------------------------

I think the original developer, kode54, deliberately removed the project after having some problems with some users online. I can understand that, but I think in the absence of an alternative tool, it's worth preserving the code in order to provide builds.

My understanding of the codebase is that he originally built the game_music_emu library to support multiple common video game music formats like NSF and VGM, back when VGM only supported a few chips. To this day, the current main fork of game_music_emu does not support YM2413 or YM2151 chips in VGM files, which is half of the chips it supported in 2003.

Later, he took the code from VGMPlay and modified it a lot to support multi-threaded usage, and hooked that up to game_music_emu's VGM handler. A lot of other changes were made to game_music_emu around this vgmplay branch which seem to have been "lost" from official libgme version.

Then things stalled for a long time. As a result, the chip emulators represent a snapshot of what was the state of the art quite a long time ago - as such, the emulation is not very good in some areas.

I suspect a long-term replacement ought to instead use libvgm...