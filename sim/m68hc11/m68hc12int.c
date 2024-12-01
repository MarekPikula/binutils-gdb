/* File generated automatically by gencode. */
#include "m68hc11-sim.h"

static const unsigned char cycles_page1[256] = {
  /*   0 */  5,  5,  1,  1,  3,  3,  3,  4,  1,  1,  6, 10,  4,  4,  4,  4,
  /*  16 */  1, 11,  3,  3,  1,  4,  4,  4,  0,  2,  2,  2,  4,  4,  5,  5,
  /*  32 */  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
  /*  48 */  3,  3,  3,  3,  2,  2,  2,  2,  3,  2,  3,  2,  0,  5,  8,  9,
  /*  64 */  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  8,  8,  4,  4,  4,  4,
  /*  80 */  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,
  /*  96 */  3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,
  /* 112 */  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,
  /* 128 */  1,  1,  1,  2,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
  /* 144 */  3,  3,  3,  3,  3,  3,  3,  1,  3,  3,  3,  3,  3,  3,  3,  3,
  /* 160 */  3,  3,  3,  3,  3,  3,  3,  1,  3,  3,  3,  3,  3,  3,  3,  3,
  /* 176 */  3,  3,  3,  3,  3,  3,  3,  1,  3,  3,  3,  3,  3,  3,  3,  3,
  /* 192 */  1,  1,  1,  2,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,
  /* 208 */  3,  3,  3,  3,  3,  3,  3,  1,  3,  3,  3,  3,  3,  3,  3,  3,
  /* 224 */  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
  /* 240 */  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3
};


static const unsigned char cycles_page2[256] = {
  /*   0 */  4,  5,  5,  5,  6,  5,  0,  3,  4,  5,  5,  4,  6,  5,  2,  2,
  /*  16 */ 12, 12, 13,  3, 12, 12,  2,  2,  4,  4,  4,  4,  4,  4,  4,  4,
  /*  32 */  4,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
  /*  48 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1,  8,  0,  9, 10,
  /*  64 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  80 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*  96 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 112 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 128 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 144 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 160 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 176 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 192 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 208 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 224 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /* 240 */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};


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
    case 0x00: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x01: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x02: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x03: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x04: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x05: /* movw  */
      cpu_move16 (cpu, op);
      break;
    case 0x07: /* daa  */
      cpu_special (cpu, M6811_DAA);
      break;
    case 0x08: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x09: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x0a: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x0b: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x0c: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x0d: /* movb  */
      cpu_move8 (cpu, op);
      break;
    case 0x0e: /* tab a->b */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x0f: /* tba b->a */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x10: /* idiv x,d->x */
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
    case 0x11: /* fdiv x,d->x */
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
    case 0x12: /* emacs  */
      cpu_special (cpu, M6812_EMACS);
      break;
    case 0x13: /* emuls  */
      cpu_special (cpu, M6812_EMULS);
      break;
    case 0x14: /* edivs  */
      cpu_special (cpu, M6812_EDIVS);
      break;
    case 0x15: /* idivs  */
      cpu_special (cpu, M6812_IDIVS);
      break;
    case 0x16: /* sba b,a->a */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x17: /* cba b,a */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0x18: /* maxa [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      if (dst8 < src8) dst8 = src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x19: /* mina [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      if (dst8 > src8) dst8 = src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x1a: /* emaxd [],d->d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      if (dst16 < src16) dst16 = src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0x1b: /* emind [],d->d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      if (dst16 > src16) dst16 = src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0x1c: /* maxm [],a->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      if (dst8 < src8) dst8 = src8;
      memory_write8 (cpu, addr, dst8);
      break;
    case 0x1d: /* minm [],a->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      if (dst8 > src8) dst8 = src8;
      memory_write8 (cpu, addr, dst8);
      break;
    case 0x1e: /* emaxm [],d->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      if (dst16 < src16) dst16 = src16;
      memory_write16 (cpu, addr, dst16);
      break;
    case 0x1f: /* eminm [],d->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      if (dst16 > src16) dst16 = src16;
      memory_write16 (cpu, addr, dst16);
      break;
    case 0x20: /* lbra R */
      addr = cpu_fetch_relbranch16 (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x21: /* lbrn R */
      addr = cpu_fetch_relbranch16 (cpu);
      break;
    case 0x22: /* lbhi R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr (cpu) & (M6811_C_BIT|M6811_Z_BIT)) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x23: /* lbls R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr (cpu) & (M6811_C_BIT|M6811_Z_BIT)))
        cpu_set_pc (cpu, addr);
      break;
    case 0x24: /* lbcc R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (!cpu_get_ccr_C (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x25: /* lbcs R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (cpu_get_ccr_C (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x26: /* lbne R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (!cpu_get_ccr_Z (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x27: /* lbeq R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (cpu_get_ccr_Z (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x28: /* lbvc R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (!cpu_get_ccr_V (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x29: /* lbvs R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (cpu_get_ccr_V (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2a: /* lbpl R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (!cpu_get_ccr_N (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2b: /* lbmi R */
      addr = cpu_fetch_relbranch16 (cpu);
      if (cpu_get_ccr_N (cpu))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2c: /* lbge R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu)) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x2d: /* lblt R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu)))
        cpu_set_pc (cpu, addr);
      break;
    case 0x2e: /* lbgt R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr_Z (cpu) | (cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu))) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x2f: /* lble R */
      addr = cpu_fetch_relbranch16 (cpu);
      if ((cpu_get_ccr_Z (cpu) | (cpu_get_ccr_N (cpu) ^ cpu_get_ccr_V (cpu))))
        cpu_set_pc (cpu, addr);
      break;
    case 0x3a: /* rev  */
      cpu_special (cpu, M6812_REV);
      ;
      break;
    case 0x3b: /* revw  */
      cpu_special (cpu, M6812_REVW);
      ;
      break;
    case 0x3c: /* wav  */
      cpu_special (cpu, M6812_WAV);
      ;
      break;
    case 0x3e: /* stop  */
      cpu_special (cpu, M6811_STOP);
      break;
    case 0x3f: /* etbl [] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      cpu_special (cpu, M6812_ETBL);
      ;
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
void
cpu_interp_m6812 (sim_cpu *cpu)
    {
  unsigned char op;
  uint16_t addr, src16, dst16;
  uint8_t src8;

  uint8_t dst8;

  op = cpu_fetch8 (cpu);
  cpu_add_cycles (cpu, cycles_page1[op]);
  switch (op)
      {
    case 0x00: /* bgnd  */
      cpu_special (cpu, M6812_BGND);
      break;
    case 0x01: /* mem  */
      cpu_special (cpu, M6812_MEM);
      break;
    case 0x02: /* iny y->y */
      src16 = cpu_get_y (cpu);
      dst16 = src16 + 1;
      cpu_set_y (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x03: /* dey y->y */
      src16 = cpu_get_y (cpu);
      dst16 = src16 - 1;
      cpu_set_y (cpu, dst16);
      cpu_set_ccr_Z (cpu, dst16 == 0);
      break;
    case 0x04: /* dbeq  */
      cpu_dbcc (cpu);
      break;
    case 0x05: /* jmp &[] */
      addr = cpu_get_indexed_operand_addr (cpu, 0);
      cpu_set_pc (cpu, addr);
      break;
    case 0x06: /* jmp &() */
      addr = cpu_fetch16 (cpu);
      cpu_set_pc (cpu, addr);
      break;
    case 0x07: /* bsr r */
      addr = cpu_fetch_relbranch (cpu);
      cpu_m68hc12_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
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
    case 0x0a: /* rtc  */
      cpu_special (cpu, M6812_RTC);
      break;
    case 0x0b: /* rti  */
      cpu_special (cpu, M6812_RTI);
      break;
    case 0x0c: /* bset [],#->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x0d: /* bclr [],#->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x0e: /* brset [],#,r */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x0f: /* brclr [],#,r */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if ((src8 & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x10: /* andcc #,ccr->ccr */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_ccr (cpu);
      dst8 = dst8 & src8;
      cpu_set_ccr (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x11: /* ediv  */
      cpu_special (cpu, M6812_EDIV);
      break;
    case 0x12: /* mul b,a->d */
      src8 = cpu_get_b (cpu);
      dst8 = cpu_get_a (cpu);
      dst16 = ((uint16_t) src8 & 0x0FF) * ((uint16_t) dst8 & 0x0FF);
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, src8 & 0x80);
      break;
    case 0x13: /* emul  */
        { uint32_t src1 = (uint32_t) cpu_get_d (cpu);
          uint32_t src2 = (uint32_t) cpu_get_y (cpu);
          src1 *= src2;
          cpu_set_d (cpu, src1);
          cpu_set_y (cpu, src1 >> 16);
          cpu_set_ccr_Z (cpu, src1 == 0);
          cpu_set_ccr_C (cpu, src1 & 0x08000);
          cpu_set_ccr_N (cpu, src1 & 0x80000000);
        };
      break;
    case 0x14: /* orcc #,ccr->ccr */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_ccr (cpu);
      dst8 = dst8 | src8;
      cpu_set_ccr (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x15: /* jsr &[] */
      addr = cpu_get_indexed_operand_addr (cpu, 0);
      cpu_m68hc12_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0x16: /* jsr &() */
      addr = cpu_fetch16 (cpu);
      cpu_m68hc12_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0x17: /* jsr * */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      cpu_m68hc12_push_uint16 (cpu, cpu_get_pc (cpu));
      cpu_call (cpu, addr);
      break;
    case 0x18: /* page2  */
      cpu_page2_interp (cpu);
      break;
    case 0x19: /* leay &[]->y */
      addr = cpu_get_indexed_operand_addr (cpu, 0);
      dst16 = addr;
      cpu_set_y (cpu, dst16);
      break;
    case 0x1a: /* leax &[]->x */
      addr = cpu_get_indexed_operand_addr (cpu, 0);
      dst16 = addr;
      cpu_set_x (cpu, dst16);
      break;
    case 0x1b: /* leas &[]->sp */
      addr = cpu_get_indexed_operand_addr (cpu, 0);
      dst16 = addr;
      cpu_set_sp (cpu, dst16);
      break;
    case 0x1c: /* bset (),#->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1d: /* bclr (),#->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x1e: /* brset (),#,r */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x1f: /* brclr (),#,r */
      addr = cpu_fetch16 (cpu);
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
    case 0x30: /* pulx (sp)->x */
      src16 = cpu_m68hc12_pop_uint16 (cpu);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      break;
    case 0x31: /* puly (sp)->y */
      src16 = cpu_m68hc12_pop_uint16 (cpu);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      break;
    case 0x32: /* pula (sp)->a */
      src8 = cpu_m68hc12_pop_uint8 (cpu);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x33: /* pulb (sp)->b */
      src8 = cpu_m68hc12_pop_uint8 (cpu);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0x34: /* pshx x->(sp) */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      cpu_m68hc12_push_uint16 (cpu, dst16);
      break;
    case 0x35: /* pshy y->(sp) */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      cpu_m68hc12_push_uint16 (cpu, dst16);
      break;
    case 0x36: /* psha a->(sp) */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      cpu_m68hc12_push_uint8 (cpu, dst8);
      break;
    case 0x37: /* pshb b->(sp) */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      cpu_m68hc12_push_uint8 (cpu, dst8);
      break;
    case 0x38: /* pulc (sp)->ccr */
      src8 = cpu_m68hc12_pop_uint8 (cpu);
      dst8 = src8;
      cpu_set_ccr (cpu, dst8);
      break;
    case 0x39: /* pshc ccr->(sp) */
      src8 = cpu_get_ccr (cpu);
      dst8 = src8;
      cpu_m68hc12_push_uint8 (cpu, dst8);
      break;
    case 0x3a: /* puld (sp)->d */
      src16 = cpu_m68hc12_pop_uint16 (cpu);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0x3b: /* pshd d->(sp) */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      cpu_m68hc12_push_uint16 (cpu, dst16);
      break;
    case 0x3d: /* rts  */
      addr = cpu_m68hc12_pop_uint16 (cpu);
      cpu_set_pc (cpu, addr);
      cpu_return (cpu);
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
    case 0x41: /* coma a->a */
      src8 = cpu_get_a (cpu);
      dst8 = ~src8;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x42: /* inca a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 + 1;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x43: /* deca a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 - 1;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x44: /* lsra a->a */
      src8 = cpu_get_a (cpu);
      dst8 = src8 >> 1;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x45: /* rola a->a */
      src8 = cpu_get_a (cpu);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      cpu_set_a (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
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
    case 0x49: /* lsrd d->d */
      src16 = cpu_get_d (cpu);
      dst16 = src16 >> 1;
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, src16 & 1);
      cpu_ccr_update_shift16 (cpu, dst16);
      break;
    case 0x4a: /* call  */
      cpu_special (cpu, M6812_CALL);
      break;
    case 0x4b: /* call  */
      cpu_special (cpu, M6812_CALL_INDIRECT);
      break;
    case 0x4c: /* bset *,#->* */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = dst8 | src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x4d: /* bclr *,#->* */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      dst8 = (~dst8) & src8;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x4e: /* brset *,#,r */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if (((~src8) & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x4f: /* brclr *,#,r */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_fetch8 (cpu);
      addr = cpu_fetch_relbranch (cpu);
      if ((src8 & dst8) == 0)
        cpu_set_pc (cpu, addr);
      break;
    case 0x50: /* negb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = - src8;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x51: /* comb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = ~src8;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x52: /* incb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 + 1;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x53: /* decb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 - 1;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x54: /* lsrb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = src8 >> 1;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x55: /* rolb b->b */
      src8 = cpu_get_b (cpu);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      cpu_set_b (cpu, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
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
    case 0x59: /* asld d->d */
      src16 = cpu_get_d (cpu);
      dst16 = src16 << 1;
      cpu_set_d (cpu, dst16);
      cpu_set_ccr_C (cpu, (src16&0x8000) >> 15);
      cpu_ccr_update_shift16 (cpu, dst16);
      break;
    case 0x5a: /* staa a->* */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x5b: /* stab b->* */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x5c: /* std d->* */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x5d: /* sty y->* */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x5e: /* stx x->* */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x5f: /* sts sp->* */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = (uint16_t) cpu_fetch8 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x60: /* neg []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = - src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x61: /* com []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = ~src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x62: /* inc []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 + 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x63: /* dec []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 - 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x64: /* lsr []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 >> 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x65: /* rol []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x66: /* ror []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (cpu_get_ccr_C (cpu) << 7);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x67: /* asr []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 >> 1) | (src8 & 0x80);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x68: /* asl []->[] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 << 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x69: /* clr ->[] */
      dst8 = 0;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x6a: /* staa a->[] */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6b: /* stab b->[] */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x6c: /* std d->[] */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x6d: /* sty y->[] */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x6e: /* stx x->[] */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x6f: /* sts sp->[] */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = cpu_get_indexed_operand_addr (cpu,0);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x70: /* neg ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = - src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 == 0);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x71: /* com ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = ~src8;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, 1);
      cpu_ccr_update_tst8 (cpu, dst8);
      ;
      break;
    case 0x72: /* inc ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 + 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x73: /* dec ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 - 1;
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x74: /* lsr ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8 >> 1;
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, src8 & 1);
      cpu_ccr_update_shift8 (cpu, dst8);
      break;
    case 0x75: /* rol ()->() */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = (src8 << 1) | (cpu_get_ccr_C (cpu));
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr_C (cpu, (src8 & 0x80) >> 7);
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
    case 0x79: /* clr ->() */
      dst8 = 0;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
      break;
    case 0x7a: /* staa a->() */
      src8 = cpu_get_a (cpu);
      dst8 = src8;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x7b: /* stab b->() */
      src8 = cpu_get_b (cpu);
      dst8 = src8;
      addr = cpu_fetch16 (cpu);
      memory_write8 (cpu, addr, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0x7c: /* std d->() */
      src16 = cpu_get_d (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x7d: /* sty y->() */
      src16 = cpu_get_y (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x7e: /* stx x->() */
      src16 = cpu_get_x (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x7f: /* sts sp->() */
      src16 = cpu_get_sp (cpu);
      dst16 = src16;
      addr = cpu_fetch16 (cpu);
      memory_write16 (cpu, addr, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0x80: /* suba #,a->a */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x81: /* cpa #,a */
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
    case 0x87: /* clra ->a */
      dst8 = 0;
      cpu_set_a (cpu, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
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
    case 0x8c: /* cpd #,d */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x8d: /* cpy #,y */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x8e: /* cpx #,x */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x8f: /* cps #,sp */
      src16 = cpu_fetch16 (cpu);
      dst16 = cpu_get_sp (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x90: /* suba *,a->a */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0x91: /* cpa *,a */
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
    case 0x97: /* tsta a */
      src8 = cpu_get_a (cpu);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
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
    case 0x9c: /* cpd *,d */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x9d: /* cpy *,y */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x9e: /* cpx *,x */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0x9f: /* cps *,sp */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_sp (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xa0: /* suba [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xa1: /* cpa [],a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xa2: /* sbca [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
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
    case 0xa3: /* subd [],d->d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xa4: /* anda [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa5: /* bita [],a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa6: /* ldaa []->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa7: /* nop  */
      break;
    case 0xa8: /* eora [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xa9: /* adca [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
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
    case 0xaa: /* oraa [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      dst8 = dst8 | src8;
      cpu_set_a (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xab: /* adda [],a->a */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xac: /* cpd [],d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xad: /* cpy [],y */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xae: /* cpx [],x */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xaf: /* cps [],sp */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_sp (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xb0: /* suba (),a->a */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_a (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_a (cpu, dst8);
      break;
    case 0xb1: /* cpa (),a */
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
    case 0xb7: /* exg # */
      src8 = cpu_fetch8 (cpu);
      cpu_exg (cpu, src8);
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
    case 0xbc: /* cpd (),d */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xbd: /* cpy (),y */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_y (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xbe: /* cpx (),x */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_x (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xbf: /* cps (),sp */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_sp (cpu);
      cpu_ccr_update_sub16 (cpu, dst16 - src16, dst16, src16);
      dst16 = dst16 - src16;
      break;
    case 0xc0: /* subb #,b->b */
      src8 = cpu_fetch8 (cpu);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xc1: /* cpb #,b */
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
    case 0xc7: /* clrb ->b */
      dst8 = 0;
      cpu_set_b (cpu, dst8);
      cpu_set_ccr (cpu, (cpu_get_ccr (cpu) & (M6811_S_BIT|M6811_X_BIT|M6811_H_BIT| M6811_I_BIT)) | M6811_Z_BIT);
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
    case 0xcd: /* ldy #->y */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xce: /* ldx #->x */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xcf: /* lds #->sp */
      src16 = cpu_fetch16 (cpu);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xd0: /* subb *,b->b */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xd1: /* cpb *,b */
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
    case 0xd7: /* tstb b */
      src8 = cpu_get_b (cpu);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
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
    case 0xdd: /* ldy *->y */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xde: /* ldx *->x */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xdf: /* lds *->sp */
      addr = (uint16_t) cpu_fetch8 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xe0: /* subb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xe1: /* cpb [],b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_sub8 (cpu, dst8 - src8, dst8, src8);
      dst8 = dst8 - src8;
      break;
    case 0xe2: /* sbcb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
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
    case 0xe3: /* addd [],d->d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = cpu_get_d (cpu);
      cpu_ccr_update_add16 (cpu, dst16 + src16, dst16, src16);
      dst16 = dst16 + src16;
      cpu_set_d (cpu, dst16);
      break;
    case 0xe4: /* andb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe5: /* bitb [],b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 & src8;
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe6: /* ldab []->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe7: /* tst [] */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
      break;
    case 0xe8: /* eorb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 ^ src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xe9: /* adcb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
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
    case 0xea: /* orab [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      dst8 = dst8 | src8;
      cpu_set_b (cpu, dst8);
      cpu_ccr_update_tst8 (cpu, dst8);
      break;
    case 0xeb: /* addb [],b->b */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src8 = memory_read8 (cpu, addr);
      dst8 = cpu_get_b (cpu);
      cpu_ccr_update_add8 (cpu, dst8 + src8, dst8, src8);
      dst8 = dst8 + src8;
      cpu_set_b (cpu, dst8);
      break;
    case 0xec: /* ldd []->d */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_d (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xed: /* ldy []->y */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xee: /* ldx []->x */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xef: /* lds []->sp */
      addr = cpu_get_indexed_operand_addr (cpu,0);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
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
    case 0xf1: /* cpb (),b */
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
    case 0xf7: /* tst () */
      addr = cpu_fetch16 (cpu);
      src8 = memory_read8 (cpu, addr);
      cpu_set_ccr_C (cpu, 0);
      cpu_ccr_update_tst8 (cpu, src8);
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
    case 0xfd: /* ldy ()->y */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_y (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xfe: /* ldx ()->x */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_x (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    case 0xff: /* lds ()->sp */
      addr = cpu_fetch16 (cpu);
      src16 = memory_read16 (cpu, addr);
      dst16 = src16;
      cpu_set_sp (cpu, dst16);
      cpu_ccr_update_tst16 (cpu, dst16);
      break;
    default:
      cpu_special (cpu, M6811_ILLEGAL);
      break;
  }
}
