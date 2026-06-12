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

## Full image + game code (phase 3 — done)

`lynxexec` boots the cart with the real boot ROM + a modeled cart-read interface
and snapshots RAM at the loader's `JMP ($004E)` hand-off:

```
lynxexec "Chip's Challenge (USA, Europe).lnx" lynxboot.img ram.bin
# -> game entry $18B7, 64KB ram.bin
m65c02recomp recompbin ram.bin 0x0000 out/ 0x18B7 0x1C40
# -> discovers + emits the game's functions (reset, IRQ $1C40, main loop, ...)
```

- **Game entry: `$18B7`** (RAM image). Startup sets the stack, programs
  Suzy/Mikey, maps vectors into RAM (`MAPCTL |= $08`), installs the IRQ handler
  at **`$1C40`**, and enters the main loop at **`$1C6E`**.
- The recompiled game C compiles + links against the runtime.
- Needs `lynxboot.img` (the 512-byte boot ROM) — not committed; supply your own.

Full detail: [`lynxrecomp/docs/IMAGE.md`](../lynxrecomp/docs/IMAGE.md).

## Runtime peripherals (phase 4 — in place)

The toolkit runtime now implements the hardware the recompiled game drives:
Mikey timers + interrupts (frame cadence), Mikey video readout (framebuffer +
palette → RGB), the Suzy sprite blitter (SCB walk, packed/literal, 1–4 bpp,
flip), and the Suzy math unit (multiply/divide). Each is unit-tested. See
[`lynxrecomp/ROADMAP.md`](../lynxrecomp/ROADMAP.md) phase 4.

## First pixels (phase 5)

The execution driver runs Chip's Challenge to a rendered frame:

```
lynxrun "Chip's Challenge (USA, Europe).lnx" lynxboot.img frame.ppm
```

It boots the cart, runs the game against the real runtime peripherals (CPU
interpreter + blitter + timers/IRQs + video), and renders the **credits screen**
(`screenshots/credits.png`) — readable text on the Epyx circuit-board
background. Two hardware bugs were found and fixed along the way: Suzy/video DMA
must bypass the MAPCTL overlay (write DRAM directly), and the timer link chain
is `0→2→4→1→3→5→7` (so VBL fires). Details: [`lynxrecomp/docs/RUN.md`](../lynxrecomp/docs/RUN.md).

It also runs the full attract sequence — the Epyx circuit-board intro, then the
title screen with scrolling "GAME DESIGN BY CHUCK SOMMERVILLE" credits — and
input is wired (arrow keys / buttons → Suzy's JOYSTICK). `lynxrun --play` opens a
live window on Windows; `lynxrun --capture` dumps a frame sequence.

![Chip's Challenge title screen, rendered by lynxrun](screenshots/title.png)
![Chip's Challenge credits, rendered by lynxrun](screenshots/credits.png)

## Running the *recompiled* C (the static-recompilation goal)

`games/chipschallenge/src/main.c` runs the emitted `lynx_func_*` C directly (not
the interpreter): it loads a post-init RAM image, registers the recompiled
functions, and calls the entry; the runtime's cooperative ticks drive the timers
and deliver the VBL interrupt into the recompiled handler. **The recompiled C
renders the title screen** (`screenshots/recompiled-title.png`).

Workflow (needs your cart dump + `lynxboot.img`, neither committed):

```
# 1. post-init RAM snapshot (jump tables built)
lynxrun --snapshot "Chip's Challenge (USA, Europe).lnx" lynxboot.img ram.bin 60
# 2. recompile, seeding the computed-jump gaps -> 317 functions, 0 gaps
m65c02recomp recompbin ram.bin 0x0000 games/chipschallenge/generated \
    0x18B7 0x2D0E 0x2E2C 0x2701 0x7D1B
# 3. build + run the host (renders to a PPM)
cmake --build build --config Release
./build/games/chipschallenge/Release/chipschallenge.exe ram.bin 400 title.ppm
```

(The generated C is ROM-derived and not committed - a placeholder keeps the
build green; regenerate it as above.)

![Chip's Challenge title, rendered by the recompiled C](screenshots/recompiled-title.png)

## Open items

- Run from the *entry* snapshot (re-run reset from `$18B7` on a clean boot) at
  full fidelity - currently the recompiled run uses the post-init snapshot.
- Audio; blitter scaling/tilt + collision; interactive input in the host.
