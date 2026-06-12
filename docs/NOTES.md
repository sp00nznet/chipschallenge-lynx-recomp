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

## Open items

- Phase 2: recover the post-boot code image + load map (see
  [`lynxrecomp/docs/BOOT.md`](../lynxrecomp/docs/BOOT.md)).
- Seed `chipschallenge.hints.txt` from the reset/IRQ/NMI vectors once the image
  is mapped.
