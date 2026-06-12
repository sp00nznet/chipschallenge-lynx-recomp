# chipschallenge-lynx-recomp

**A static recompilation of *Chip's Challenge* (Atari Lynx, 1989) — and the
first game ever brought up on [`lynxrecomp`](https://github.com/sp00nznet/lynxrecomp).**

*Chip's Challenge* was the Lynx launch puzzle game: a tile-grid maze where Chip
collects chips, dodges monsters, and juggles keys and boots across 100+ levels.
It's an ideal first recompilation target — deterministic tile logic, flat sprite
blits, and almost none of the hardware sprite-scaling that makes the Lynx's
pseudo-3D games hard.

This repository is the **reference game**: it pulls in the `lynxrecomp` toolkit
as a submodule, adds the per-game host (window/input/audio) and the
recompiler-generated C, and builds a native executable. The reusable recompiler
itself lives in [`lynxrecomp`](https://github.com/sp00nznet/lynxrecomp) — fork
that to recompile a different Lynx game.

> **No ROM here.** The cartridge dump is `.gitignore`d. Supply your own
> `Chip's Challenge (USA, Europe).lnx`. Verified against:
> sha1 `92af2d566c6bbf626a21dbfa41af16d73e0a774d`.

## Status — phase 1 (scaffold)

This is the project skeleton. It establishes the structure, builds the toolkit,
and documents the path; it does **not** yet boot the game — that waits on the
encrypted-boot work (`lynxrecomp/docs/BOOT.md`) and the emitter (phase 2–3).

What's in place:

- `lynxrecomp` wired in as a submodule; CMake builds the toolkit + this game.
- Per-game host driver stub (`games/chipschallenge/src/main.c`) — the entry
  point, frame loop and HLE hook surface that the generated code links into.
- A `generated/` placeholder so the target compiles before real C is emitted.
- `chipschallenge.hints.txt` for the analyzer (force-code/data, names, HLE).
- `scripts/extract_rom.ps1` to pull the `.lnx` out of a romset zip into `roms/`.

See [`lynxrecomp/ROADMAP.md`](https://github.com/sp00nznet/lynxrecomp/blob/main/ROADMAP.md)
for the full plan.

## Getting started

```powershell
git clone --recurse-submodules https://github.com/sp00nznet/chipschallenge-lynx-recomp
cd chipschallenge-lynx-recomp

# (already cloned without --recurse-submodules?)
git submodule update --init --recursive

# put your dump at roms/Chip's Challenge (USA, Europe).lnx, then:
cmake -S . -B build
cmake --build build --config Release

# inspect the ROM with the toolkit:
./build/lynxrecomp/tools/m65c02recomp/Release/m65c02recomp.exe info "roms/Chip's Challenge (USA, Europe).lnx"
```

## Layout

```
lynxrecomp/                      the toolkit (git submodule)
games/chipschallenge/
  src/main.c                     per-game host: entry, frame loop, HLE hooks
  generated/                     recompiler output (gitignored; placeholder kept)
  chipschallenge.hints.txt       analyzer hints for this title
scripts/extract_rom.ps1          romset-zip -> roms/<game>.lnx
docs/                            game-specific notes
```

## License

MIT — see [`LICENSE`](LICENSE). *Chip's Challenge* is a trademark of its
respective owner; this project ships no game data and is an independent
preservation/recompilation effort.
