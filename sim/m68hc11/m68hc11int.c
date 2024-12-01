/* File generated automatically by gencode. */
#include "m68hc11-sim.h"

static const unsigned char cycles_page1[256] = {
  /*   0 */  5,  2, 41, 41,  3,  3,  2,  2,  3,  3,  2,  2,  2,  2,  2,  2,
  /*  16 */  2,  2,  6,  6,  6,  6,  2,  2,  0,  2,  0,  2,  7,  7,  7,  7,
  /*  32 */  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
  /*  48 */  3,  3,  4,  4,  3,  3,  3,  3,  5,  5,  3, 12,  4, 10, 14, 14,
  /*  64 */  2,  2,  0,  2,  2,  0,  2,  2,  2,  2,  2,  0,  2,  2,  0,  2,
  /*  80 */  2,  0,  0,  2,  2,  0,  2,  2,  2,  2,  2,  0,  2,  2,  0,  2,
  /*  96 */  6,  0,  0,  6,  6,  0,  6,  6,  6,  6,  6,  0,  6,  6,  3,  6,
  /* 112 */  6,  0,  0,  6,  6,  0,  6,  6,  6,  6,  6,  0,  6,  6,  3,  6,
  /* 128 */  2,  2,  2,  4,  2,  2,  2,  0,  2,  2,  2,  2,  4,  6,  3,  3,
  /* 144 */  3,  3,  3,  5,  3,  3,  3,  3,  3,  3,  3,  3,  5,  5,  4,  4,
  /* 160 */  4,  4,  4,  6,  4,  4,  4,  4,  4,  4,  4,  4,  6,  6,  5,  5,
  /* 176 */  4,  4,  4,  6,  4,  4,  4,  4,  4,  4,  4,  4,  5,  6,  5,  5,
  /* 192 */  2,  2,  2,  4,  2,  2,  2,  0,  2,  2,  2,  2,  3,  0,  3,  2,
  /* 208 */  3,  3,  3,  5,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,
  /* 224 */  4,  4,  4,  6,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5,
  /* 240 */  4,  4,  4,  6,  4,  4,  4,  4,  4,  4,  4,  4,  5,  5,  5,  5
};


static const unsigned char cycles_page2[256] = {
  /*   0 */  0,  0,  0,  0,  0,  0,  0,  0,  4,  4,  0,  0,  0,  0,  0,  0,
  /*  16 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  8,
  /*  32 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  48 */  4,  0,  0,  0,  0,  4,  0,  0,  6,  0,  4,  0,  5,  0,  0,  0,
  /*  64 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  80 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  96 */  7,  0,  0,  7,  7,  0,  7,  7,  7,  7,  7,  0,  7,  7,  4,  7,
  /* 112 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 128 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  4,
  /* 144 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,
  /* 160 */  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  5,  7,  6,  6,  6,
  /* 176 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,
  /* 192 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,
  /* 208 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5,
  /* 224 */  5,  5,  5,  7,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,
  /* 240 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6
};


static const unsigned char cycles_page3[256] = {
  /*   0 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  16 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  32 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  48 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  64 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  80 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  96 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 112 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 128 */  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 144 */  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 160 */  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,
  /* 176 */  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 192 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 208 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 224 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,
  /* 240 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


static const unsigned char cycles_page4[256] = {
  /*   0 */  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  16 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  32 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  48 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  64 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  80 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  96 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 112 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 128 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 144 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 160 */  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  0,  0,
  /* 176 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 192 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 208 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 224 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,
  /* 240 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


static void
cpu_page3_interp (sim_cpu *cpu)
    {
  unsigned char op;
  uint16_t addr, src16, dst16;
  op = cpu_fetch8 (cpu);
  cpu_add_cycles (cpu, cycles_page3[op]);
  switch (op)
      {
    case 0x83: /* cmpd #,d */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x93: /* cmpd *,d */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xa3: /* cmpd (x),d */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xac: /* cmpy (x),y */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xb3: /* cmpd (),d */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xee: /* ldy (x)->y */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xef: /* sty y->(x) */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
static void
cpu_page4_interp (sim_cpu *cpu)
    {
  unsigned char op;
  uint16_t addr, src16, dst16;
  op = cpu_fetch8 (cpu);
  cpu_add_cycles (cpu, cycles_page4[op]);
  switch (op)
      {
    case 0x03: /* syscall  */
      cpu_special (cpu, M6811_EMUL_SYSCALL);
      break;
    case 0xa3: /* cmpd (y),d */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xac: /* cmpx (y),x */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xee: /* ldx (y)->x */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xef: /* stx x->(y) */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
static void
cpu_page2_interp (sim_cpu *cpu)
    {
  unsigned char op;
  uint16_t addr, src16, dst16;
  uint8_t src8;

  uint8_t dst8;

  op = cpu_fetch8 (cpu);
  cpu_add_cycles (cpu, cycles_page2[op]);
  switch (op)
      {
    case 0x08: /* iny y->y */
      src16 = cpu_get_y (cpu);
      dst16 = src16 + 1;
      cpu_set_y (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x09: /* dey y->y */
      src16 = cpu_get_y (cpu);
      dst16 = src16 - 1;
      cpu_set_y (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x1c: /* bset (y),#->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1d: /* bclr (y),#->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1e: /* brset (y),#,r */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x1f: /* brclr (y),#,r */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if ((src8 & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x30: /* tsy sp->y */
      src16 = cpu_get_sp (cpu);
      dst16 = src16 + 1;
      ;
      cpu_set_y (cpu, dst16);
      break;
    case 0x35: /* tys y->sp */
      src16 = cpu_get_y (cpu);
      dst16 = src16 - 1;
      ;
      cpu_set_sp (cpu, dst16);
      break;
    case 0x38: /* puly (sp)->y */
      src16 = cpu_m68hc11_pop_uint16 (cpu);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      break;
    case 0x3a: /* aby b,y->y */
      src8 = cpu_get_b (cpu);
      dst16 = cpu_get_y (cpu);
      dst16 = dst16 + (uint16_t) src8;
      cpu_set_y (cpu, dst16);
      break;
    case 0x3c: /* pshy y->(sp) */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      cpu_m68hc11_push_uint16 (cpu, dst16);
      break;
    case 0x60: /* neg (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = - src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x63: /* com (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = ~src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x64: /* lsr (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 >> 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x66: /* ror (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x67: /* asr (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x68: /* asl (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 << 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x69: /* rol (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x6a: /* dec (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 - 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6c: /* inc (y)->(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 + 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6d: /* tst (y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0x6e: /* jmp &(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x6f: /* clr ->(y) */
      dst8 = 0;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x8c: /* cmpy #,y */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x8f: /* xgdy y->y */
      src16 = cpu_get_y (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_set_d (cpu, src16);
      cpu_set_y (cpu, dst16);
      break;
    case 0x9c: /* cmpy *,y */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xa0: /* suba (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xa1: /* cmpa (y),a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xa2: /* sbca (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xa3: /* subd (y),d->d */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xa4: /* anda (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa5: /* bita (y),a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa6: /* ldaa (y)->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa7: /* staa a->(y) */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa8: /* eora (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa9: /* adca (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xaa: /* oraa (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xab: /* adda (y),a->a */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xac: /* cmpy (y),y */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xad: /* jsr &(y) */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      cpu_m68hc11_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0xae: /* lds (y)->sp */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xaf: /* sts sp->(y) */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xbc: /* cmpy (),y */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xce: /* ldy #->y */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xde: /* ldy *->y */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xdf: /* sty y->* */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xe0: /* subb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xe1: /* cmpb (y),b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xe2: /* sbcb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xe3: /* addd (y),d->d */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xe4: /* andb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe5: /* bitb (y),b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe6: /* ldab (y)->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe7: /* stab b->(y) */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe8: /* eorb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe9: /* adcb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xea: /* orab (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xeb: /* addb (y),b->b */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xec: /* ldd (y)->d */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xed: /* std d->(y) */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xee: /* ldy (y)->y */
      addr = cpu_get_y (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xef: /* sty y->(y) */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = cpu_get_y (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xfe: /* ldy ()->y */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xff: /* sty y->() */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
void
cpu_interp_m6811 (sim_cpu *cpu)
    {
  unsigned char op;
  uint16_t addr, src16, dst16;
  uint8_t src8;

  uint8_t dst8;

  op = cpu_fetch8 (cpu);
  cpu_add_cycles (cpu, cycles_page1[op]);
  switch (op)
      {
    case 0x00: /* test  */
      cpu_special (cpu, M6811_TEST);
      break;
    case 0x01: /* nop  */
      break;
    case 0x02: /* idiv x,d->x */
      src16 = cpu_get_x (cpu);
      dst16 = cpu_get_d (cpu);
      if (src16 == 0)
        {
          dst16 = 0xffff;
        }
      else
        {
          cpu_set_d (cpu, dst16 % src16);
          dst16 = dst16 / src16;
        };
      cpu_set_x (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      cpu_set_ccr_V (cpu, 0);
      cpu_set_ccr_C (cpu, src16 == 0);
      break;
    case 0x03: /* fdiv x,d->x */
      src16 = cpu_get_x (cpu);
      dst16 = cpu_get_d (cpu);
      if (src16 <= dst16 )
        {
          dst16 = 0xffff;

          cpu_set_ccr_Z (cpu, 0);

          cpu_set_ccr_V (cpu, 1);

          cpu_set_ccr_C (cpu, dst16 == 0);

        }
      else
        {
          unsigned long l = (unsigned long) (dst16) << 16;

          cpu_set_d (cpu, (uint16_t) (l % (unsigned long) (src16)));

          dst16 = (uint16_t) (l / (unsigned long) (src16));

          cpu_set_ccr_V (cpu, 0);

          cpu_set_ccr_C (cpu, 0);

          cpu_set_ccr_Z (cpu, dst16 == 0);

        };
      cpu_set_x (cpu, dst16);
      break;
    case 0x04: /* lsrd d->d */
      src16 = cpu_get_d (cpu);
      dst16 = src16 >> 1;
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, src16 & 1);
      cpu_ccr_update_shift16 (cpu, dst16);
      break;
    case 0x05: /* asld d->d */
      src16 = cpu_get_d (cpu);
      dst16 = src16 << 1;
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, (src16&0x8000) >> 15);
      cpu_ccr_update_shift16 (cpu, dst16);
      break;
    case 0x06: /* tap a->ccr */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      cpu_set_ccr (cpu, dst8);
      break;
    case 0x07: /* tpa ccr->a */
      src8 = cpu_get_ccr (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x08: /* inx x->x */
      src16 = cpu_get_x (cpu);
      dst16 = src16 + 1;
      cpu_set_x (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x09: /* dex x->x */
      src16 = cpu_get_x (cpu);
      dst16 = src16 - 1;
      cpu_set_x (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x0a: /* clv  */
      cpu_set_ccr_V (cpu, 0);
      break;
    case 0x0b: /* sev  */
      cpu_set_ccr_V (cpu, 1);
      break;
    case 0x0c: /* clc  */
      cpu_set_ccr_C (cpu, 0);
      break;
    case 0x0d: /* sec  */
      cpu_set_ccr_C (cpu, 1);
      break;
    case 0x0e: /* cli  */
      cpu_set_ccr_I (cpu, 0);
      break;
    case 0x0f: /* sei  */
      cpu_set_ccr_I (cpu, 1);
      break;
    case 0x10: /* sba b,a->a */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x11: /* cba b,a */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0x12: /* brset *,#,r */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x13: /* brclr *,#,r */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if ((src8 & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x14: /* bset *,#->* */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x15: /* bclr *,#->* */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x16: /* tab a->b */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x17: /* tba b->a */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x18: /* page2  */
      cpu_page2_interp (cpu);
      break;
    case 0x19: /* daa  */
      cpu_special (cpu, M6811_DAA);
      break;
    case 0x1a: /* page3  */
      cpu_page3_interp (cpu);
      break;
    case 0x1b: /* aba b,a->a */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x1c: /* bset (x),#->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1d: /* bclr (x),#->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1e: /* brset (x),#,r */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x1f: /* brclr (x),#,r */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if ((src8 & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x20: /* bra r */
      addr = cpu_fetch_relbranch (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x21: /* brn r */
      addr = cpu_fetch_relbranch (cpu);
      break;
    case 0x22: /* bhi r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr (cpu) & (M6811_C_BIT|M6811_Z_BIT)) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x23: /* bls r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr (cpu) & (M6811_C_BIT|M6811_Z_BIT)))
        cpu_set_pc (cpu, addr);
      break;
    case 0x24: /* bcc r */
      addr = cpu_fetch_relbranch (cpu);
      if (!cpu_get_ccr_C (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x25: /* bcs r */
      addr = cpu_fetch_relbranch (cpu);
      if (cpu_get_ccr_C (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x26: /* bne r */
      addr = cpu_fetch_relbranch (cpu);
      if (!cpu_get_ccr_Z (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x27: /* beq r */
      addr = cpu_fetch_relbranch (cpu);
      if (cpu_get_ccr_Z (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x28: /* bvc r */
      addr = cpu_fetch_relbranch (cpu);
      if (!cpu_get_ccr_V (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x29: /* bvs r */
      addr = cpu_fetch_relbranch (cpu);
      if (cpu_get_ccr_V (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2a: /* bpl r */
      addr = cpu_fetch_relbranch (cpu);
      if (!cpu_get_ccr_N (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2b: /* bmi r */
      addr = cpu_fetch_relbranch (cpu);
      if (cpu_get_ccr_N (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2c: /* bge r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu)) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x2d: /* blt r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu)))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2e: /* bgt r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr_Z (cpu) | (cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu))) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x2f: /* ble r */
      addr = cpu_fetch_relbranch (cpu);
      if ((cpu_get_ccr_Z (cpu) | (cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu))))
        cpu_set_pc (cpu, addr);
      break;
    case 0x30: /* tsx sp->x */
      src16 = cpu_get_sp (cpu);
      dst16 = src16 + 1;
      ;
      cpu_set_x (cpu, dst16);
      break;
    case 0x31: /* ins sp->sp */
      src16 = cpu_get_sp (cpu);
      dst16 = src16 + 1;
      cpu_set_sp (cpu, dst16);
      break;
    case 0x32: /* pula (sp)->a */
      src8 = cpu_m68hc11_pop_uint8 (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x33: /* pulb (sp)->b */
      src8 = cpu_m68hc11_pop_uint8 (cpu);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0x34: /* des sp->sp */
      src16 = cpu_get_sp (cpu);
      dst16 = src16 - 1;
      cpu_set_sp (cpu, dst16);
      break;
    case 0x35: /* txs x->sp */
      src16 = cpu_get_x (cpu);
      dst16 = src16 - 1;
      ;
      cpu_set_sp (cpu, dst16);
      break;
    case 0x36: /* psha a->(sp) */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      cpu_m68hc11_push_uint8 (cpu, dst8);
      break;
    case 0x37: /* pshb b->(sp) */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      cpu_m68hc11_push_uint8 (cpu, dst8);
      break;
    case 0x38: /* pulx (sp)->x */
      src16 = cpu_m68hc11_pop_uint16 (cpu);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      break;
    case 0x39: /* rts  */
      addr = cpu_m68hc11_pop_uint16 (cpu);
      cpu_set_pc (cpu, addr);
      cpu_return (cpu);
      break;
    case 0x3a: /* abx b,x->x */
      src8 = cpu_get_b (cpu);
      dst16 = cpu_get_x (cpu);
      dst16 = dst16 + (uint16_t) src8;
      cpu_set_x (cpu, dst16);
      break;
    case 0x3b: /* rti  */
      cpu_special (cpu, M6811_RTI);
      break;
    case 0x3c: /* pshx x->(sp) */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      cpu_m68hc11_push_uint16 (cpu, dst16);
      break;
    case 0x3d: /* mul b,a->d */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      dst16 = ((uint16_t) src8 & 0x0FF) * ((uint16_t) dst8 & 0x0FF);
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, src8 & 0x80);
      break;
    case 0x3e: /* wai  */
      cpu_special (cpu, M6811_WAI);
      break;
    case 0x3f: /* swi  */
      cpu_special (cpu, M6811_SWI);
      break;
    case 0x40: /* nega a->a */
      src8 = cpu_get_a (cpu);
      dst8 = - src8;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x41: /* syscall  */
      cpu_special (cpu, M6811_EMUL_SYSCALL);
      break;
    case 0x43: /* coma a->a */
      src8 = cpu_get_a (cpu);
      dst8 = ~src8;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x44: /* lsra a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 >> 1;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x46: /* rora a->a */
      src8 = cpu_get_a (cpu);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x47: /* asra a->a */
      src8 = cpu_get_a (cpu);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x48: /* asla a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 << 1;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x49: /* rola a->a */
      src8 = cpu_get_a (cpu);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x4a: /* deca a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 - 1;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x4c: /* inca a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 + 1;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x4d: /* tsta a */
      src8 = cpu_get_a (cpu);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0x4f: /* clra ->a */
      dst8 = 0;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x50: /* negb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = - src8;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x53: /* comb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = ~src8;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x54: /* lsrb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 >> 1;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x56: /* rorb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x57: /* asrb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x58: /* aslb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 << 1;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x59: /* rolb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x5a: /* decb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 - 1;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x5c: /* incb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 + 1;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x5d: /* tstb b */
      src8 = cpu_get_b (cpu);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0x5f: /* clrb ->b */
      dst8 = 0;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x60: /* neg (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = - src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x63: /* com (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = ~src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x64: /* lsr (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 >> 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x66: /* ror (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x67: /* asr (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x68: /* asl (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 << 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x69: /* rol (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x6a: /* dec (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 - 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6c: /* inc (x)->(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 + 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6d: /* tst (x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0x6e: /* jmp &(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x6f: /* clr ->(x) */
      dst8 = 0;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x70: /* neg ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = - src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x73: /* com ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = ~src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x74: /* lsr ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 >> 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x76: /* ror ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x77: /* asr ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x78: /* asl ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 << 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x79: /* rol ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x7a: /* dec ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 - 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x7c: /* inc ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 + 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x7d: /* tst () */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0x7e: /* jmp &() */
      addr = cpu_fetch16 (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x7f: /* clr ->() */
      dst8 = 0;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x80: /* suba #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x81: /* cmpa #,a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0x82: /* sbca #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0x83: /* subd #,d->d */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0x84: /* anda #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x85: /* bita #,a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x86: /* ldaa #->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x88: /* eora #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x89: /* adca #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0x8a: /* oraa #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x8b: /* adda #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x8c: /* cmpx #,x */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x8d: /* bsr r */
      addr = cpu_fetch_relbranch (cpu);
      cpu_m68hc11_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0x8e: /* lds #->sp */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x8f: /* xgdx x->x */
      src16 = cpu_get_x (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_set_d (cpu, src16);
      cpu_set_x (cpu, dst16);
      break;
    case 0x90: /* suba *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x91: /* cmpa *,a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0x92: /* sbca *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0x93: /* subd *,d->d */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0x94: /* anda *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x95: /* bita *,a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x96: /* ldaa *->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x97: /* staa a->* */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x98: /* eora *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x99: /* adca *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0x9a: /* oraa *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x9b: /* adda *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x9c: /* cmpx *,x */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x9d: /* jsr * */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      cpu_m68hc11_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0x9e: /* lds *->sp */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x9f: /* sts sp->* */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xa0: /* suba (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xa1: /* cmpa (x),a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xa2: /* sbca (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xa3: /* subd (x),d->d */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xa4: /* anda (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa5: /* bita (x),a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa6: /* ldaa (x)->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa7: /* staa a->(x) */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa8: /* eora (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa9: /* adca (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xaa: /* oraa (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xab: /* adda (x),a->a */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xac: /* cmpx (x),x */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xad: /* jsr &(x) */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      cpu_m68hc11_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0xae: /* lds (x)->sp */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xaf: /* sts sp->(x) */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xb0: /* suba (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xb1: /* cmpa (),a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xb2: /* sbca (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xb3: /* subd (),d->d */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xb4: /* anda (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xb5: /* bita (),a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xb6: /* ldaa ()->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xb7: /* staa a->() */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xb8: /* eora (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xb9: /* adca (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_a (cpu, dst8);
      break;
    case 0xba: /* oraa (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xbb: /* adda (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xbc: /* cmpx (),x */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xbd: /* jsr &() */
      addr = cpu_fetch16 (cpu);
      cpu_m68hc11_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0xbe: /* lds ()->sp */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xbf: /* sts sp->() */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xc0: /* subb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xc1: /* cmpb #,b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xc2: /* sbcb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xc3: /* addd #,d->d */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xc4: /* andb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xc5: /* bitb #,b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xc6: /* ldab #->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xc8: /* eorb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xc9: /* adcb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xca: /* orab #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xcb: /* addb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xcc: /* ldd #->d */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xcd: /* page4  */
      cpu_page4_interp (cpu);
      break;
    case 0xce: /* ldx #->x */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xcf: /* stop  */
      cpu_special (cpu, M6811_STOP);
      break;
    case 0xd0: /* subb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xd1: /* cmpb *,b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xd2: /* sbcb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xd3: /* addd *,d->d */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xd4: /* andb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xd5: /* bitb *,b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xd6: /* ldab *->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xd7: /* stab b->* */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xd8: /* eorb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xd9: /* adcb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xda: /* orab *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xdb: /* addb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xdc: /* ldd *->d */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xdd: /* std d->* */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xde: /* ldx *->x */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xdf: /* stx x->* */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xe0: /* subb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xe1: /* cmpb (x),b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xe2: /* sbcb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xe3: /* addd (x),d->d */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xe4: /* andb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe5: /* bitb (x),b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe6: /* ldab (x)->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe7: /* stab b->(x) */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe8: /* eorb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe9: /* adcb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8 + 1, dst8, src8);

            dst8 = dst8 + src8 + 1;

          }
        else
          {
            cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);

            dst8 = dst8 + src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xea: /* orab (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xeb: /* addb (x),b->b */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xec: /* ldd (x)->d */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xed: /* std d->(x) */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xee: /* ldx (x)->x */
      addr = cpu_get_x (cpu) + (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xef: /* stx x->(x) */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = cpu_get_x (cpu) + cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xf0: /* subb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xf1: /* cmpb (),b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xf2: /* sbcb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      if (cpu_get_ccr_C (cpu))
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8 - 1, dst8, src8);

            dst8 = dst8 - src8 - 1;

          }
        else
          {
            cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);

            dst8 = dst8 - src8;

          };
      cpu_set_b (cpu, dst8);
      break;
    case 0xf3: /* addd (),d->d */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xf4: /* andb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xf5: /* bitb (),b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xf6: /* ldab ()->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xf7: /* stab b->() */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xf8: /* eorb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xf9: /* adcb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xfa: /* orab (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xfb: /* addb (),b->b */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xfc: /* ldd ()->d */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xfd: /* std d->() */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xfe: /* ldx ()->x */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xff: /* stx x->() */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
