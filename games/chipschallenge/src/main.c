/* main.c - Chip's Challenge host driver (phase 1).
 *
 * This is the per-game glue around the lynxrecomp runtime: it initialises the
 * hardware model, will load the recompiled entry point, run the frame loop
 * (CPU work -> timer step -> Suzy blit -> present), and pump host input into
 * Suzy's JOYSTICK register. Phase 1 is a skeleton: the recompiled entry and the
 * video/audio host are not wired yet (see lynxrecomp/ROADMAP.md), so this builds
 * and initialises but does not run game logic.
 *
 * Host-specific bits (window, audio, real input) are intentionally kept here in
 * the game repo, not in the toolkit, so the toolkit stays portable.
 */
#include <stdio.h>
#include <stdlib.h>

#include "lynxrecomp/cpu.h"
#include "lynxrecomp/mem.h"
#include "lynxrecomp/suzy.h"
#include "lynxrecomp/mikey.h"
#include "lynxrecomp/timer.h"
#include "lynxrecomp/input.h"
#include "recomp_funcs.h"

static void runtime_init(void) {
    lynx_mem_init();
    lynx_suzy_init();
    lynx_mikey_init();
    lynx_timer_init();
    lynx_input_set(0x00, 0x00);
    lynx_cpu_reset();
}

/* One emulated frame. Phase 2+: call the recompiled main-loop body, advance the
 * timers by the cycles it consumed, service IRQs, run the Suzy blit, and hand
 * the framebuffer to the host. */
static void run_frame(void) {
    /* TODO(phase 2+): lynx_func_<entry>();  + timer/IRQ/video servicing. */
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <Chip's Challenge (USA, Europe).lnx>\n", argv[0]);
        return 2;
    }

    printf("chipschallenge-lynx-recomp (phase 1 scaffold)\n");
    printf("rom: %s\n", argv[1]);
    printf("note: this scaffold initialises the lynxrecomp runtime but does not\n"
           "      yet execute game code - see lynxrecomp/docs/BOOT.md.\n");

    runtime_init();
    (void)run_frame;   /* not yet driven */

    return 0;
}
