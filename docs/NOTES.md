# Chip's Challenge — game-specific notes

Title-specific findings live here as bring-up proceeds. General Lynx hardware,
the recompiler internals, and the boot-decryption problem are documented in the
toolkit instead: see [`lynxrecomp/docs/`](../lynxrecomp/docs/).

## ROM

- File: `Chip's Challenge (USA, Europe).lnx`
- Container: BLL `.lnx`, 64-byte header + 131072-byte cart image (128 KiB)
- Header: cart name `chipchal.lnx`, manufacturer `Atari`, bank0 page size 512
- sha1: `92af2d566c6bbf626a21dbfa41af16d73e0a774d`
- md5: `ad3d091ba4250d0354bc6d3fd8c9a882`

## Why this title first

Tile-grid puzzle logic is deterministic and light on Suzy: flat sprite blits,
no hardware scaling/tilt, modest audio. That isolates the bring-up to the CPU
translation, the timer/IRQ frame cadence, and a basic blitter — without the
math-unit-heavy pseudo-3D path the racing/shooter titles need.

## Boot (phase 2 — done)

The cart's encrypted boot block decrypts cleanly with the toolkit:

```
m65c02recomp loader "Chip's Challenge (USA, Europe).lnx"
```

- First byte `$FB` → 5 encrypted blocks → a **250-byte secondary loader at
  `$0200`** (verified: byte-identical to an independent reference, disassembles
  as coherent 65SC02).
- The loader clears the palette, programs Suzy, sets `DISPADR=$0400`
  (framebuffer), and calls boot-ROM cart-read helpers (`$FE00`/`$FE4A`) to stream
  the plaintext game into RAM.
- Effective entry / analysis seed: **`$0200`**.

Full detail: [`lynxrecomp/docs/BOOT.md`](../lynxrecomp/docs/BOOT.md).

## Recompilation (phase 3 — pipeline works)

The toolkit now decrypts, discovers functions, and emits runnable C:

```
m65c02recomp recomp "Chip's Challenge (USA, Europe).lnx" out/
```

On the loader this recovers 5 functions (entry `$0200`, plus `$020E`, `$02BD`,
`$02C9`, `$02DE`) and emits readable `lynx_func_<addr>` C. The recovered routine
`$02C9` (palette/SERCTL init) has been compiled and **executed** with correct
hardware effects — the first recompiled-and-run Chip's Challenge code.

> The generated C is derived from the ROM, so it is not committed here; generate
> it locally from your own dump. The `generated/` placeholder keeps the build
> green until then.

## Open items

- Reconstruct the full cart→RAM image + true game entry (the loader's
  `JMP ($004E)` jumps to the game; that vector is set by the boot-ROM cart-read).
  Model the loader + boot-ROM, or snapshot a reference emulator — then discovery
  reaches the game's own code, not just the loader.
- Seed `chipschallenge.hints.txt` from `$0200` and the timer-IRQ setup once the
  full image is mapped.
