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

## Open items

- Phase 3: reconstruct the full cart→RAM image + true game entry (model the
  loader + boot-ROM cart-read, or snapshot a reference emulator).
- Seed `chipschallenge.hints.txt` from `$0200` and the timer-IRQ setup once the
  full image is mapped.
