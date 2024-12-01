#line 1 "m32c/r8c.opc"
/* r8c.opc --- semantics for r8c opcodes.		        -*- mode: c -*-

Copyright (C) 2005-2024 Free Software Foundation, Inc.
Contributed by Red Hat, Inc.

This file is part of the GNU simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This must come before any other includes.  */
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>

#include "ansidecl.h"
#include "cpu.h"
#include "mem.h"
#include "misc.h"
#include "int.h"

#define tprintf if (trace) printf

static unsigned char
getbyte (void)
{
  int tsave = trace;
  unsigned char b;

  if (trace == 1)
    trace = 0;
  b = mem_get_pc ();
  regs.r_pc ++;
  trace = tsave;
  return b;
}

#define M16C_ONLY() /* FIXME: add something here */

#define GETBYTE() (op[opi++] = getbyte())

#define UNSUPPORTED() unsupported("unsupported", orig_pc)
#define NOTYET() unsupported("unimplemented", orig_pc)

static void
unsupported (char *tag, int orig_pc)
{
  int i;
  printf("%s opcode at %08x\n", tag, orig_pc);
  regs.r_pc = orig_pc;
  for (i=0; i<2; i++)
    {
      int b = mem_get_pc();
      printf(" %s", bits(b>>4, 4));
      printf(" %s", bits(b, 4));
      regs.r_pc ++;
    }
  printf("\n");
  regs.r_pc = orig_pc;
  for (i=0; i<6; i++)
    {
      printf(" %02x", mem_get_pc ());
      regs.r_pc ++;
    }
  printf("\n");
  exit(1);
}

static int
IMM (int bw)
{
  int rv = getbyte ();
  if (bw)
    rv = rv + 256 * getbyte();
  if (bw == 2)
    rv = rv + 65536 * getbyte();
  return rv;
}

#define IMM4() (immm >= 8 ? 7 - immm : immm + 1)

#define UNARY_SOP \
  dc = decode_srcdest4 (dest, w); \
  v = sign_ext (get_src (dc), w?16:8);

#define UNARY_UOP \
  dc = decode_srcdest4 (dest, w); \
  v = get_src (dc);

#define BINARY_SOP \
  sc = decode_srcdest4 (srcx, w); \
  dc = decode_srcdest4 (dest, w); \
  a = sign_ext (get_src (sc), w?16:8); \
  b = sign_ext (get_src (dc), w?16:8);

#define BINARY_UOP \
  sc = decode_srcdest4 (srcx, w); \
  dc = decode_srcdest4 (dest, w); \
  a = get_src (sc); \
  b = get_src (dc);

#define carry (FLAG_C ? 1 : 0)

static void
cmp (int d, int s, int w)
{
  int a, b, f=0;
  int mask = w ? 0xffff : 0xff;
  a = d - s;
  b = sign_ext (d, w?16:8) - sign_ext (s, w?16:8);
  tprintf ("cmp: %x - %x = %08x, %x - %x = %d\n",
	   d, s, a,
	   sign_ext(d,w?16:8), sign_ext(s,w?16:8), b);

  if (b == 0)
    f |= FLAGBIT_Z;
  if (b & (w ? 0x8000 : 0x80))
    f |= FLAGBIT_S;
  if ((d & mask) >= (s & mask))
    f |= FLAGBIT_C;
  if (b < (w ? -32768 : -128) || b > (w ? 32767 : 127))
    f |= FLAGBIT_O;

  set_flags (FLAGBIT_Z | FLAGBIT_S | FLAGBIT_O | FLAGBIT_C, f);
}

static void
div_op (int s, int u, int x, int w)
{
  srcdest sc;
  int v, a, b;

  if (s == -1)
    s = IMM(w);
  else
    {
      sc = decode_srcdest4 (s, w);
      s = get_src (sc);
    }

  v = get_reg (w ? r2r0 : r0);

  if (!u)
    {
      s = sign_ext (s, w ? 16 : 8);
      v = sign_ext (v, w ? 16 : 8);
    }

  if (s == 0)
    {
      set_flags (FLAGBIT_O, FLAGBIT_O);
      return;
    }

  if (u)
    {
      a = (unsigned int)v / (unsigned int)s;
      b = (unsigned int)v % (unsigned int)s;
    }
  else
    {
      a = v / s;
      b = v % s;
    }
  if (x)
    {
      if ((s > 0 && b < 0)
	  || (s < 0 && b > 0))
	{
	  a --;
	  b += s;
	}
    }
  tprintf ("%d / %d = %d rem %d\n", v, s, a, b);
  if ((!u && (a > (w ? 32767 : 127)
	     || a < (w ? -32768 : -129)))
      || (u && (a > (w ? 65536 : 255))))
    set_flags (FLAGBIT_O, FLAGBIT_O);
  else
    set_flags (FLAGBIT_O, 0);

  put_reg (w ? r0 : r0l, a);
  put_reg (w ? r2 : r0h, b);
}

static void
rot_op (srcdest sd, int rotc, int count)
{
  int mask = (sd.bytes == 2) ? 0xffff : 0xff;
  int msb = (sd.bytes == 2) ? 0x8000 : 0x80;
  int v = get_src (sd);
  int c = carry, ct;

  tprintf("%s %x by %d\n", rotc ? "rotc" : "rot", v, count);
  tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
  while (count > 0)
    {
      ct = (v & msb) ? 1 : 0;
      v <<= 1;
      v |= rotc ? c : ct;
      v &= mask;
      c = ct;
      tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
      count --;
    }
  while (count < 0)
    {
      ct = v & 1;
      v >>= 1;
      v |= (rotc ? c : ct) * msb;
      c = ct;
      tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
      count ++;
    }
  put_dest (sd, v);
  set_szc (v, sd.bytes, c);
}

static void
shift_op (srcdest sd, int arith, int count)
{
  int mask = (sd.bytes == 2) ? 0xffff : 0xff;
  int msb = (sd.bytes == 2) ? 0x8000 : 0x80;
  int v = get_src (sd);
  int c = 0;

  if (sd.bytes == 4)
    {
      mask = 0xffffffffU;
      msb = 0x80000000U;
      if (count > 16 || count < -16)
	{
	  fprintf(stderr, "Error: SI shift of %d undefined\n", count);
	  exit(1);
	}
      if (count > 16)
	count = (count - 1) % 16 + 1;
      if (count < -16)
	count = -((-count - 1) % 16 + 1);
    }

  tprintf("%s %x by %d\n", arith ? "sha" : "shl", v, count);
  tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
  while (count > 0)
    {
      c = (v & msb) ? 1 : 0;
      v <<= 1;
      v &= mask;
      tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
      count --;
    }
  while (count < 0)
    {
      c = v & 1;
      if (arith)
	v = (v & msb) | (v >> 1);
      else
	v = (v >> 1) & (msb - 1);
      tprintf (": %s %d\n", bits(v, 8*sd.bytes), c);
      count ++;
    }
  put_dest (sd, v);
  set_szc (v, sd.bytes, c);
}

#define MATH_OP(dc,s,c,op,carryrel) \
  a = get_src(dc); \
  b = s & b2mask[dc.bytes]; \
  v2 = a op b op c; \
  tprintf("0x%x " #op " 0x%x " #op " 0x%x = 0x%x\n", a, b, c, v2); \
  a = sign_ext (a, dc.bytes * 8); \
  b = sign_ext (s, dc.bytes * 8); \
  v = a op b op c; \
  tprintf("%d " #op " %d " #op " %d = %d\n", a, b, c, v); \
  set_oszc (v, dc.bytes, v2 carryrel); \
  put_dest (dc, v2);

#define BIT_OP(field,expr) \
  dc = decode_bit (field); \
  b = get_bit (dc); \
  v = expr; \
  tprintf ("b=%d, carry=%d, %s = %d\n", b, carry, #expr, v); \
  put_bit (dc, v);

#define BIT_OPC(field,expr) \
  dc = decode_bit (field); \
  b = get_bit (dc); \
  v = expr; \
  tprintf ("b=%d, carry=%d, %s = %d\n", b, carry, #expr, v); \
  set_c (v);

/* The "BMcnd dest" opcode uses a different encoding for the */
/* condition than other opcodes.  */
static int bmcnd_cond_map[] = {
  0, 1, 2, 3, 8, 9, 10, 11, 4, 5, 6, 7, 12, 13, 14, 15
};

int
decode_r8c (void)
{
  unsigned char op[40];
  int opi = 0;
  int v, v2, a, b;
  int orig_pc = get_reg (pc);
  srcdest sc, dc;
  int imm;

  step_result = M32C_MAKE_STEPPED ();

  tprintf("trace: decode pc = %05x\n", orig_pc);


  GETBYTE();
  switch (op[0] & 0xff) {
    case 0x00:
        {
          /** 0000 0000            BRK */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 0000            BRK */",
                     op[0]);
            }
#line 508 "m32c/r8c.opc"
        
          /* We report the break to our caller with the PC still pointing at the 
             breakpoint instruction.  */
          put_reg (pc, orig_pc);
          if (verbose)
            printf("[break]\n");
          return M32C_MAKE_HIT_BREAK ();
        
        }
      break;
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x05:
    case 0x06:
    case 0x07:
        {
          /** 0000 0s ds           MOV.B:S R0L/R0H,dest */
#line 943 "m32c/r8c.opc"
          int s ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 943 "m32c/r8c.opc"
          int ds ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 0s ds           MOV.B:S R0L/R0H,dest */",
                     op[0]);
              printf("  s = 0x%x,", s);
              printf("  ds = 0x%x\n", ds);
            }
#line 943 "m32c/r8c.opc"
        
          if (ds == 0)
            UNSUPPORTED();
          dc = decode_src2 (ds, 0, s);
          v = get_reg (s ? r0h : r0l);
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0x04:
        {
          /** 0000 0100            NOP */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 0100            NOP */",
                     op[0]);
            }
#line 1067 "m32c/r8c.opc"
        
          tprintf("nop\n");
        
        }
      break;
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x0f:
        {
          /** 0000 1d sr           MOV.B:S src,R0L/R0H */
#line 952 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 952 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 1d sr           MOV.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 952 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          v = get_src (sc);
          set_sz (v, 1);
          put_reg (d ? r0h : r0l, v);
        
        }
      break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
        {
          /** 0001 0d sr           AND.B:S src,R0L/R0H */
#line 437 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 437 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0001 0d sr           AND.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 437 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          dc = decode_dest1 (d, 0);
          a = get_src (sc);
          b = get_src (dc);
          v = a & b;
          tprintf("%x & %x = %x\n", a, b, v);
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
        {
          /** 0001 1d sr           OR.B:S src,R0L/R0H */
#line 1115 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 1115 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0001 1d sr           OR.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 1115 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          dc = decode_dest1 (d, 0);
          a = get_src (sc);
          b = get_src (dc);
          v = a | b;
          tprintf("%x | %x = %x\n", a, b, v);
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
        {
          /** 0010 0d sr           ADD.B:S src,R0L/R0H */
#line 376 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 376 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0010 0d sr           ADD.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 376 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          dc = decode_dest1 (d, 0);
          b = get_src (sc);
          MATH_OP (dc, b, 0, +, > 0xff);
        
        }
      break;
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
        {
          /** 0010 1d sr           SUB.B:S src,R0L/R0H */
#line 1548 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 1548 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0010 1d sr           SUB.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 1548 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          dc = decode_dest1 (d, 0);
          b = get_src (sc);
          MATH_OP (dc, b, 0, -, >= 0);
        
        }
      break;
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
        {
          /** 0011 0d sr           MOV.B:S src,dest */
#line 936 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 936 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0011 0d sr           MOV.B:S src,dest */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 936 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          v = get_src (sc);
          set_sz (v, 1);
          put_reg (d ? a1 : a0, v);
        
        }
      break;
    case 0x38:
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
        {
          /** 0011 1d sr           CMP.B:S src,R0L/R0H */
#line 581 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 2) & 0x01;
#line 581 "m32c/r8c.opc"
          int sr ATTRIBUTE_UNUSED = op[0] & 0x03;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0011 1d sr           CMP.B:S src,R0L/R0H */",
                     op[0]);
              printf("  d = 0x%x,", d);
              printf("  sr = 0x%x\n", sr);
            }
#line 581 "m32c/r8c.opc"
        
          sc = decode_src2 (sr, 0, d);
          dc = decode_dest1 (d, 0);
          a = get_src (sc);
          b = get_src (dc);
          cmp (b, a, 0);
        
        }
      break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
        {
          /** 0100 0bit            BCLR:S bit,base:11[SB] */
#line 457 "m32c/r8c.opc"
          int bit ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0100 0bit            BCLR:S bit,base:11[SB] */",
                     op[0]);
              printf("  bit = 0x%x\n", bit);
            }
#line 457 "m32c/r8c.opc"
        
          dc = decode_bit11 (bit);
          put_bit (dc, 0);
        
        }
      break;
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
        {
          /** 0100 1bit            BSET:S bit,base:11[SB] */
#line 522 "m32c/r8c.opc"
          int bit ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0100 1bit            BSET:S bit,base:11[SB] */",
                     op[0]);
              printf("  bit = 0x%x\n", bit);
            }
#line 522 "m32c/r8c.opc"
        
          dc = decode_bit11 (bit);
          put_bit (dc, 1);
        
        }
      break;
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0x56:
    case 0x57:
        {
          /** 0101 0bit            BNOT:S bit,base:11[SB] */
#line 489 "m32c/r8c.opc"
          int bit ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0101 0bit            BNOT:S bit,base:11[SB] */",
                     op[0]);
              printf("  bit = 0x%x\n", bit);
            }
#line 489 "m32c/r8c.opc"
        
          dc = decode_bit11 (bit);
          put_bit (dc, !get_bit (dc));
        
        }
      break;
    case 0x58:
    case 0x59:
    case 0x5a:
    case 0x5b:
    case 0x5c:
    case 0x5d:
    case 0x5e:
    case 0x5f:
        {
          /** 0101 1bit            BTST:S bit,base:11[SB] */
#line 533 "m32c/r8c.opc"
          int bit ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0101 1bit            BTST:S bit,base:11[SB] */",
                     op[0]);
              printf("  bit = 0x%x\n", bit);
            }
#line 533 "m32c/r8c.opc"
        
          dc = decode_bit11 (bit);
          b = get_bit (dc);
          set_zc (!b, b);
        
        }
      break;
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
        {
          /** 0110 0dsp            JMP.S label */
#line 747 "m32c/r8c.opc"
          int dsp ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0110 0dsp            JMP.S label */",
                     op[0]);
              printf("  dsp = 0x%x\n", dsp);
            }
#line 747 "m32c/r8c.opc"
        
          put_reg (pc, orig_pc + 2 + dsp);
        
        }
      break;
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
        {
          /** 0110 1cnd            Jcnd label */
#line 735 "m32c/r8c.opc"
          int cnd ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0110 1cnd            Jcnd label */",
                     op[0]);
              printf("  cnd = 0x%x\n", cnd);
            }
#line 735 "m32c/r8c.opc"
        
          v = sign_ext (IMM(0), 8);
          if (condition_true (cnd))
            put_reg (pc, orig_pc + 1 + v);
        
        }
      break;
    case 0x70:
    case 0x71:
      GETBYTE();
        {
          /** 0111 000w srcx dest  MULU.size src,dest */
#line 1051 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1051 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1051 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 0111 000w srcx dest  MULU.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1051 "m32c/r8c.opc"
        
          BINARY_UOP;
          v = a * b;
          tprintf("%u * %u = %u\n", a, b, v);
          dc = widen_sd (dc);
          put_dest (dc, v);
        
        }
      break;
    case 0x72:
    case 0x73:
      GETBYTE();
        {
          /** 0111 001w srcx dest  MOV.size:G src,dest */
#line 928 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 928 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 928 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 0111 001w srcx dest  MOV.size:G src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 928 "m32c/r8c.opc"
        
          sc = decode_srcdest4 (srcx, w);
          dc = decode_srcdest4 (dest, w);
          v = get_src (sc);
          set_sz (v, w+1);
          put_dest (dc, v);
        
        }
      break;
    case 0x74:
        GETBYTE();
        switch (op[1] & 0xf0) {
          case 0x00:
            op_semantics_1:
              {
                /** 0111 010w 0000 srcx  STE.size src,abs20 */
#line 1454 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1454 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0000 srcx  STE.size src,abs20 */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1454 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, w);
                a = IMM(2);
                v = get_src (sc);
                if (w)
                  mem_put_hi (a, v);
                else
                  mem_put_qi (a, v);
                if (srcx == 4 || srcx == 5)
                  {
                    v = get_reg (sc.u.reg);
                    set_sz (v, 2);
                  }
                else
                  set_sz (v, w+1);
                  
              }
            break;
          case 0x10:
            op_semantics_2:
              {
                /** 0111 010w 0001 srcx  STE.size src,disp20[a0] */
#line 1471 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1471 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0001 srcx  STE.size src,disp20[a0] */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1471 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, w);
                a = get_reg(a0) + IMM(2);
                v = get_src (sc);
                if (w)
                  mem_put_hi (a, v);
                else
                  mem_put_qi (a, v);
                if (srcx == 4 || srcx == 5)
                  {
                    v = get_reg (sc.u.reg);
                    set_sz (v, 2);
                  }
                else
                  set_sz (v, w+1);
              
              }
            break;
          case 0x20:
            op_semantics_3:
              {
                /** 0111 010w 0010 srcx  STE.size src,[a1a0] */
#line 1488 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1488 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0010 srcx  STE.size src,[a1a0] */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1488 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, w);
                a = get_reg(a1a0);
                v = get_src (sc);
                if (w)
                  mem_put_hi (a, v);
                else
                  mem_put_qi (a, v);
                if (srcx == 4 || srcx == 5)
                  {
                    v = get_reg (sc.u.reg);
                    set_sz (v, 2);
                  }
                else
                  set_sz (v, w+1);
              
              }
            break;
          case 0x30:
            op_semantics_4:
              {
                /** 0111 010w 0011 srcx  MOV.size:G src, disp8[SP] */
#line 972 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 972 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0011 srcx  MOV.size:G src, disp8[SP] */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 972 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, w);
                imm = IMM(0);
                a = get_reg (sp) + sign_ext (imm, 8);
                a &= addr_mask;
                v = get_src (sc);
                if (w)
                  mem_put_hi (a, v);
                else
                  mem_put_qi (a, v);
                set_sz (v, w+1);
              
              }
            break;
          case 0x40:
            op_semantics_5:
              {
                /** 0111 010w 0100 srcx  PUSH.size:G src */
#line 1196 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1196 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0100 srcx  PUSH.size:G src */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1196 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, w);
                v = get_src (sc);
                if (w)
                  {
                    put_reg (sp, get_reg (sp) - 2);
                    mem_put_hi (get_reg (sp), v);
                    tprintf("pushhi: %x\n", v);
                  }
                else
                  {
                    put_reg (sp, get_reg (sp) - 1);
                    mem_put_qi (get_reg (sp), v);
                    tprintf("pushqi: %x\n", v);
                  }
              
              }
            break;
          case 0x50:
            op_semantics_6:
              {
                /** 0111 010w 0101 dest  NEG.size dest */
#line 1059 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1059 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0101 dest  NEG.size dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1059 "m32c/r8c.opc"
              
                UNARY_SOP;
                tprintf("%d * -1 = %d\n", v, -v);
                v = -v;
                set_oszc (v, w+1, v == 0);
                put_dest (dc, v);
              
              }
            break;
          case 0x60:
            op_semantics_7:
              {
                /** 0111 010w 0110 dest  ROT.size R1H,dest */
#line 1316 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1316 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0110 dest  ROT.size R1H,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1316 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                rot_op (dc, 0, sign_ext (get_reg (r1h), 8));
              
              }
            break;
          case 0x70:
            op_semantics_8:
              {
                /** 0111 010w 0111 dest  NOT.size:G */
#line 1071 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1071 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 0111 dest  NOT.size:G */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1071 "m32c/r8c.opc"
              
                UNARY_UOP;
                tprintf("~ %x = %x\n", v, ~v);
                v = ~v;
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x80:
            op_semantics_9:
              {
                /** 0111 010w 1000 dest  LDE.size abs20,dest */
#line 853 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 853 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1000 dest  LDE.size abs20,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 853 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(2);
                if (w)
                  v = mem_get_hi (imm);
                else
                  v = mem_get_qi (imm);
                put_dest (dc, v);
              
              }
            break;
          case 0x90:
            op_semantics_10:
              {
                /** 0111 010w 1001 dest  LDE.size dsp:20[a0], dest */
#line 863 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 863 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1001 dest  LDE.size dsp:20[a0], dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 863 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(2) + get_reg (a0);
                if (w)
                  v = mem_get_hi (imm);
                else
                  v = mem_get_qi (imm);
                put_dest (dc, v);
              
              }
            break;
          case 0xa0:
            op_semantics_11:
              {
                /** 0111 010w 1010 dest  LDE.size [a1a0],dest */
#line 873 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 873 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1010 dest  LDE.size [a1a0],dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 873 "m32c/r8c.opc"
                
                dc = decode_srcdest4 (dest, w);
                imm = get_reg (a1a0);
                if (w)
                  v = mem_get_hi (imm);
                else
                  v = mem_get_qi (imm);
                put_dest (dc, v);
              
              }
            break;
          case 0xb0:
            op_semantics_12:
              {
                /** 0111 010w 1011 dest  MOV.size:G dsp:8[SP], dest */
#line 959 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 959 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1011 dest  MOV.size:G dsp:8[SP], dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 959 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(0);
                a = get_reg (sp) + sign_ext (imm, 8);
                a &= addr_mask;
                if (w)
                  v = mem_get_hi (a);
                else
                  v = mem_get_qi (a);
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0xc0:
            op_semantics_13:
              {
                /** 0111 010w 1100 dest  MOV.size:G #IMM,dest */
#line 887 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 887 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1100 dest  MOV.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 887 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(w);
                v = imm;
                tprintf("%x = %x\n", v, v);
                set_sz(v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0xd0:
            op_semantics_14:
              {
                /** 0111 010w 1101 dest  POP.size:G dest */
#line 1126 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1126 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1101 dest  POP.size:G dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1126 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                if (w)
                  {
                    v = mem_get_hi (get_reg (sp));
                    put_reg (sp, get_reg (sp) + 2);
                    tprintf("pophi: %x\n", v);
                  }
                else
                  {
                    v = mem_get_qi (get_reg (sp));
                    put_reg (sp, get_reg (sp) + 1);
                    tprintf("popqi: %x\n", v);
                  }
                put_dest (dc, v);
              
              }
            break;
          case 0xe0:
            op_semantics_15:
              {
                /** 0111 010w 1110 dest  SHL.size R1H,dest */
#line 1366 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1366 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1110 dest  SHL.size R1H,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1366 "m32c/r8c.opc"
              
                dc = decode_srcdest4(dest, w);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 0, a);
              
              }
            break;
          case 0xf0:
            op_semantics_16:
              {
                /** 0111 010w 1111 dest  SHA.size R1H,dest */
#line 1344 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1344 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 010w 1111 dest  SHA.size R1H,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1344 "m32c/r8c.opc"
              
                dc = decode_srcdest4(dest, w);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 1, a);
              
              }
            break;
        }
      break;
    case 0x75:
        GETBYTE();
        switch (op[1] & 0xf0) {
          case 0x00:
            goto op_semantics_1;
            break;
          case 0x10:
            goto op_semantics_2;
            break;
          case 0x20:
            goto op_semantics_3;
            break;
          case 0x30:
            goto op_semantics_4;
            break;
          case 0x40:
            goto op_semantics_5;
            break;
          case 0x50:
            goto op_semantics_6;
            break;
          case 0x60:
            goto op_semantics_7;
            break;
          case 0x70:
            goto op_semantics_8;
            break;
          case 0x80:
            goto op_semantics_9;
            break;
          case 0x90:
            goto op_semantics_10;
            break;
          case 0xa0:
            goto op_semantics_11;
            break;
          case 0xb0:
            goto op_semantics_12;
            break;
          case 0xc0:
            goto op_semantics_13;
            break;
          case 0xd0:
            goto op_semantics_14;
            break;
          case 0xe0:
            goto op_semantics_15;
            break;
          case 0xf0:
            goto op_semantics_16;
            break;
        }
      break;
    case 0x76:
        GETBYTE();
        switch (op[1] & 0xf0) {
          case 0x00:
            op_semantics_17:
              {
                /** 0111 011w 0000 dest  TST.size #IMM, dest */
#line 1555 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1555 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0000 dest  TST.size #IMM, dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1555 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                tprintf ("%x & %x = %x\n", v, imm, v & imm);
                v &= imm;
                set_sz (v, w+1);
              
              }
            break;
          case 0x10:
            op_semantics_18:
              {
                /** 0111 011w 0001 dest  XOR.size #IMM,dest */
#line 1587 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1587 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0001 dest  XOR.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1587 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                tprintf ("%x ^ %x = %x\n", v, imm, v ^ imm);
                v ^= imm;
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x20:
            op_semantics_19:
              {
                /** 0111 011w 0010 dest  AND.size:G #IMM,dest */
#line 410 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 410 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0010 dest  AND.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 410 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                tprintf ("%x & %x = %x\n", v, imm, v & imm);
                v &= imm;
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x30:
            op_semantics_20:
              {
                /** 0111 011w 0011 dest  OR.size:G #IMM,dest */
#line 1088 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1088 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0011 dest  OR.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1088 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                tprintf ("%x | %x = %x\n", v, imm, v | imm);
                v |= imm;
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x40:
            op_semantics_21:
              {
                /** 0111 011w 0100 dest  ADD.size:G #imm,dest */
#line 351 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 351 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0100 dest  ADD.size:G #imm,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 351 "m32c/r8c.opc"
              
                dc = decode_srcdest4(dest, w);
                imm = IMM(w);
                MATH_OP (dc, imm, 0, +, > (w?0xffff:0xff));
              
              }
            break;
          case 0x50:
            op_semantics_22:
              {
                /** 0111 011w 0101 dest  SUB.size:G #IMM,dest */
#line 1529 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1529 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0101 dest  SUB.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1529 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(w);
                MATH_OP (dc, imm, 0, -, >= 0);
              
              }
            break;
          case 0x60:
            op_semantics_23:
              {
                /** 0111 011w 0110 dest  ADC.size #IMM,dest */
#line 333 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 333 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0110 dest  ADC.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 333 "m32c/r8c.opc"
              
                dc = decode_srcdest4(dest, w);
                imm = IMM(w);
                MATH_OP (dc, imm, carry, +, > (w?0xffff:0xff));
              
              }
            break;
          case 0x70:
            op_semantics_24:
              {
                /** 0111 011w 0111 dest  SBB.size #IMM,dest */
#line 1326 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1326 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 0111 dest  SBB.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1326 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                imm = IMM(w);
                MATH_OP (dc, imm, !carry, -, >= 0);
              
              }
            break;
          case 0x80:
            op_semantics_25:
              {
                /** 0111 011w 1000 dest  CMP.size:G #IMM,dest */
#line 557 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 557 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1000 dest  CMP.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 557 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                cmp (v, imm, w);
              
              }
            break;
          case 0x90:
            op_semantics_26:
              {
                /** 0111 011w 1001 srcx  DIVX.size src */
#line 667 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 667 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1001 srcx  DIVX.size src */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 667 "m32c/r8c.opc"
              
                div_op (srcx, 0, 1, w);
              
              }
            break;
          case 0xa0:
            op_semantics_27:
              {
                /** 0111 011w 1010 dest  ROLC.size dest */
#line 1301 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1301 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1010 dest  ROLC.size dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1301 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                rot_op (dc, 1, 1);
              
              }
            break;
          case 0xb0:
            op_semantics_28:
              {
                /** 0111 011w 1011 dest  RORC.size dest */
#line 1306 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1306 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1011 dest  RORC.size dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1306 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, w);
                rot_op (dc, 1, -1);
              
              }
            break;
          case 0xc0:
            op_semantics_29:
              {
                /** 0111 011w 1100 srcx  DIVU.size src */
#line 659 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 659 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1100 srcx  DIVU.size src */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 659 "m32c/r8c.opc"
              
                div_op (srcx, 1, 0, w);
              
              }
            break;
          case 0xd0:
            op_semantics_30:
              {
                /** 0111 011w 1101 srcx  DIV.size src */
#line 651 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 651 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1101 srcx  DIV.size src */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 651 "m32c/r8c.opc"
              
                div_op (srcx, 0, 0, w);
              
              }
            break;
          case 0xe0:
            op_semantics_31:
              {
                /** 0111 011w 1110 dest  ADCF.size dest */
#line 346 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 346 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1110 dest  ADCF.size dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 346 "m32c/r8c.opc"
              
                dc = decode_srcdest4(dest, w);
                MATH_OP (dc, 0, carry, +, > (w?0xffff:0xff));
              
              }
            break;
          case 0xf0:
            op_semantics_32:
              {
                /** 0111 011w 1111 dest  ABS.size dest */
#line 325 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 325 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 011w 1111 dest  ABS.size dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 325 "m32c/r8c.opc"
              
                UNARY_SOP;
                a = v<0 ? -v : v;
                tprintf("abs(%d) = %d\n", v, a);
                set_osz(a, w+1);
                put_dest (dc, a);
              
              }
            break;
        }
      break;
    case 0x77:
        GETBYTE();
        switch (op[1] & 0xf0) {
          case 0x00:
            goto op_semantics_17;
            break;
          case 0x10:
            goto op_semantics_18;
            break;
          case 0x20:
            goto op_semantics_19;
            break;
          case 0x30:
            goto op_semantics_20;
            break;
          case 0x40:
            goto op_semantics_21;
            break;
          case 0x50:
            goto op_semantics_22;
            break;
          case 0x60:
            goto op_semantics_23;
            break;
          case 0x70:
            goto op_semantics_24;
            break;
          case 0x80:
            goto op_semantics_25;
            break;
          case 0x90:
            goto op_semantics_26;
            break;
          case 0xa0:
            goto op_semantics_27;
            break;
          case 0xb0:
            goto op_semantics_28;
            break;
          case 0xc0:
            goto op_semantics_29;
            break;
          case 0xd0:
            goto op_semantics_30;
            break;
          case 0xe0:
            goto op_semantics_31;
            break;
          case 0xf0:
            goto op_semantics_32;
            break;
        }
      break;
    case 0x78:
    case 0x79:
      GETBYTE();
        {
          /** 0111 100w srcx dest  MUL.size src,dest */
#line 1034 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1034 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1034 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 0111 100w srcx dest  MUL.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1034 "m32c/r8c.opc"
        
          BINARY_SOP;
          v = a * b;
          tprintf("%d * %d = %d\n", a, b, v);
          dc = widen_sd (dc);
          put_dest (dc, v);
        
        }
      break;
    case 0x7a:
        GETBYTE();
        switch (op[1] & 0xc0) {
          case 0x00:
            op_semantics_33:
              {
                /** 0111 101w 00sr dest  XCHG.size src,dest */
#line 1578 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1578 "m32c/r8c.opc"
                int sr ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
#line 1578 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 101w 00sr dest  XCHG.size src,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  sr = 0x%x,", sr);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1578 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (sr, w);
                dc = decode_srcdest4 (dest, w);
                a = get_src (sc);
                b = get_src (dc);
                put_dest (dc, a);
                put_dest (sc, b);
              
              }
            break;
          case 0x80:
          case 0xc0:
              {
                /** 0111 1010 1reg srcx  LDC src,dest */
#line 843 "m32c/r8c.opc"
                int reg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 843 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1010 1reg srcx  LDC src,dest */",
                           op[0], op[1]);
                    printf("  reg = 0x%x,", reg);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 843 "m32c/r8c.opc"
              
                dc = decode_cr (reg);
                sc = decode_srcdest4 (srcx,1);
                put_dest (dc, get_src (sc));
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x7b:
        GETBYTE();
        switch (op[1] & 0xc0) {
          case 0x00:
            goto op_semantics_33;
            break;
          case 0x80:
          case 0xc0:
              {
                /** 0111 1011 1src dest  STC src,dest */
#line 1438 "m32c/r8c.opc"
                int src ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 1438 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1011 1src dest  STC src,dest */",
                           op[0], op[1]);
                    printf("  src = 0x%x,", src);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1438 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, 1);
                sc = decode_cr (src);
                put_dest (dc, get_src(sc));
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x7c:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x01:
          case 0x02:
          case 0x03:
          case 0x04:
          case 0x05:
          case 0x06:
          case 0x07:
          case 0x08:
          case 0x09:
          case 0x0a:
          case 0x0b:
          case 0x0c:
          case 0x0d:
          case 0x0e:
          case 0x0f:
          case 0x10:
          case 0x11:
          case 0x12:
          case 0x13:
          case 0x14:
          case 0x15:
          case 0x16:
          case 0x17:
          case 0x18:
          case 0x19:
          case 0x1a:
          case 0x1b:
          case 0x1c:
          case 0x1d:
          case 0x1e:
          case 0x1f:
          case 0x20:
          case 0x21:
          case 0x22:
          case 0x23:
          case 0x24:
          case 0x25:
          case 0x26:
          case 0x27:
          case 0x28:
          case 0x29:
          case 0x2a:
          case 0x2b:
          case 0x2c:
          case 0x2d:
          case 0x2e:
          case 0x2f:
          case 0x30:
          case 0x31:
          case 0x32:
          case 0x33:
          case 0x34:
          case 0x35:
          case 0x36:
          case 0x37:
          case 0x38:
          case 0x39:
          case 0x3a:
          case 0x3b:
          case 0x3c:
          case 0x3d:
          case 0x3e:
          case 0x3f:
              {
                /** 0111 1100 00hl srcx  MOVdir src,R0L */
#line 1009 "m32c/r8c.opc"
                int hl ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
#line 1009 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 00hl srcx  MOVdir src,R0L */",
                           op[0], op[1]);
                    printf("  hl = 0x%x,", hl);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1009 "m32c/r8c.opc"
              
                if (srcx == 0 || srcx == 4 || srcx == 5)
                  UNSUPPORTED();
                sc = decode_srcdest4 (srcx, 0);
                a = get_reg (r0l);
                b = get_src (sc);
                switch (hl)
                  {
                  case 0: a = (a & 0xf0) | (b & 0x0f); break;
                  case 1: a = (a & 0xf0) | ((b>>4) & 0x0f); break;
                  case 2: a = (a & 0x0f) | ((b & 0x0f)<<4); break;
                  case 3: a = (a & 0x0f) | (b & 0xf0); break;
                  }
                put_reg (r0l, a);
              
              }
            break;
          case 0x40:
          case 0x41:
          case 0x42:
          case 0x43:
          case 0x44:
          case 0x45:
          case 0x46:
          case 0x47:
          case 0x48:
          case 0x49:
          case 0x4a:
          case 0x4b:
          case 0x4c:
          case 0x4d:
          case 0x4e:
          case 0x4f:
            op_semantics_34:
              {
                /** 0111 110w 0100 dest  MULU.size #IMM,dest */
#line 1042 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1042 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 0100 dest  MULU.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1042 "m32c/r8c.opc"
              
                UNARY_UOP;
                imm = IMM(w);
                tprintf("%u * %u = %u\n", v, imm, v*imm);
                v *= imm;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x50:
          case 0x51:
          case 0x52:
          case 0x53:
          case 0x54:
          case 0x55:
          case 0x56:
          case 0x57:
          case 0x58:
          case 0x59:
          case 0x5a:
          case 0x5b:
          case 0x5c:
          case 0x5d:
          case 0x5e:
          case 0x5f:
            op_semantics_35:
              {
                /** 0111 110w 0101 dest  MUL.size #IMM,dest */
#line 1025 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1025 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 0101 dest  MUL.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1025 "m32c/r8c.opc"
              
                UNARY_SOP;
                imm = sign_ext (IMM(w), w?16:8);
                tprintf("%d * %d = %d\n", v, imm, v*imm);
                v *= imm;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x60:
          case 0x61:
          case 0x62:
          case 0x63:
          case 0x64:
          case 0x65:
          case 0x66:
          case 0x67:
          case 0x68:
          case 0x69:
          case 0x6a:
          case 0x6b:
          case 0x6c:
          case 0x6d:
          case 0x6e:
          case 0x6f:
              {
                /** 0111 1100 0110 dest  EXTS.B dest */
#line 687 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 0110 dest  EXTS.B dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 687 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, 0);
                v = sign_ext (get_src (dc), 8);
                dc = widen_sd (dc);
                put_dest (dc, v);
                set_sz (v, 1);
              
              }
            break;
          case 0x80:
          case 0x81:
          case 0x82:
          case 0x83:
          case 0x84:
          case 0x85:
          case 0x86:
          case 0x87:
          case 0x88:
          case 0x89:
          case 0x8a:
          case 0x8b:
          case 0x8c:
          case 0x8d:
          case 0x8e:
          case 0x8f:
          case 0x90:
          case 0x91:
          case 0x92:
          case 0x93:
          case 0x94:
          case 0x95:
          case 0x96:
          case 0x97:
          case 0x98:
          case 0x99:
          case 0x9a:
          case 0x9b:
          case 0x9c:
          case 0x9d:
          case 0x9e:
          case 0x9f:
          case 0xa0:
          case 0xa1:
          case 0xa2:
          case 0xa3:
          case 0xa4:
          case 0xa5:
          case 0xa6:
          case 0xa7:
          case 0xa8:
          case 0xa9:
          case 0xaa:
          case 0xab:
          case 0xac:
          case 0xad:
          case 0xae:
          case 0xaf:
          case 0xb0:
          case 0xb1:
          case 0xb2:
          case 0xb3:
          case 0xb4:
          case 0xb5:
          case 0xb6:
          case 0xb7:
          case 0xb8:
          case 0xb9:
          case 0xba:
          case 0xbb:
          case 0xbc:
          case 0xbd:
          case 0xbe:
          case 0xbf:
              {
                /** 0111 1100 10hl dest  MOVdir R0L,dest */
#line 993 "m32c/r8c.opc"
                int hl ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
#line 993 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 10hl dest  MOVdir R0L,dest */",
                           op[0], op[1]);
                    printf("  hl = 0x%x,", hl);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 993 "m32c/r8c.opc"
              
                if (dest == 0 || dest == 4 || dest == 5)
                  UNSUPPORTED();
                dc = decode_srcdest4 (dest, 0);
                a = get_src (dc);
                b = get_reg (r0l);
                switch (hl)
                  {
                  case 0: a = (a & 0xf0) | (b & 0x0f); break;
                  case 1: a = (a & 0xf0) | ((b>>4) & 0x0f); break;
                  case 2: a = (a & 0x0f) | ((b & 0x0f)<<4); break;
                  case 3: a = (a & 0x0f) | (b & 0xf0); break;
                  }
                put_dest (dc, a);
              
              }
            break;
          case 0xc0:
          case 0xc1:
          case 0xc2:
          case 0xc3:
          case 0xc4:
          case 0xc5:
          case 0xc6:
          case 0xc7:
          case 0xc8:
          case 0xc9:
          case 0xca:
          case 0xcb:
          case 0xcc:
          case 0xcd:
          case 0xce:
          case 0xcf:
              {
                /** 0111 1100 1100 dest  STC PC,dest */
#line 1444 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 1100 dest  STC PC,dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 1444 "m32c/r8c.opc"
              
                dc = decode_srcdest4 (dest, 1);
                dc.bytes = 3;
                put_dest (dc, orig_pc);
              
              }
            break;
          case 0xe0:
            op_semantics_36:
              {
                /** 0111 110w 1110 0000  DIVU.size #IMM */
#line 655 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 0000  DIVU.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 655 "m32c/r8c.opc"
              
                div_op (-1, 1, 0, w);
              
              }
            break;
          case 0xe1:
            op_semantics_37:
              {
                /** 0111 110w 1110 0001  DIV.size #IMM */
#line 647 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 0001  DIV.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 647 "m32c/r8c.opc"
              
                div_op (-1, 0, 0, w);
              
              }
            break;
          case 0xe2:
            op_semantics_38:
              {
                /** 0111 110w 1110 0010  PUSH.size:G #IMM */
#line 1180 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 0010  PUSH.size:G #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1180 "m32c/r8c.opc"
              
                imm = IMM(w);
                if (w)
                  {
                    put_reg (sp, get_reg (sp) - 2);
                    mem_put_hi (get_reg (sp), imm);
                    tprintf("pushhi %04x\n", imm);
                  }
                else
                  {
                    put_reg (sp, get_reg (sp) - 1);
                    mem_put_qi (get_reg (sp), imm);
                    tprintf("pushqi %02x\n", imm);
                  }
              
              }
            break;
          case 0xe3:
            op_semantics_39:
              {
                /** 0111 110w 1110 0011  DIVX.size #IMM */
#line 663 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 0011  DIVX.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 663 "m32c/r8c.opc"
              
                div_op (-1, 0, 1, w);
              
              }
            break;
          case 0xe4:
          case 0xe5:
          case 0xe6:
          case 0xe7:
          case 0xec:
          case 0xed:
          case 0xee:
          case 0xef:
            op_semantics_40:
              {
                /** 0111 110w 1110 i1c s  DADC,DADD,DSBB,DSUB */
#line 589 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 589 "m32c/r8c.opc"
                int i ATTRIBUTE_UNUSED = (op[1] >> 3) & 0x01;
#line 589 "m32c/r8c.opc"
                int c ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x01;
#line 589 "m32c/r8c.opc"
                int s ATTRIBUTE_UNUSED = op[1] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 i1c s  DADC,DADD,DSBB,DSUB */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  i = 0x%x,", i);
                    printf("  c = 0x%x,", c);
                    printf("  s = 0x%x\n", s);
                  }
#line 589 "m32c/r8c.opc"
              
                /* w = width, i = immediate, c = carry, s = subtract */
              
                {
                int src = i ? IMM(w) : get_reg (w ? r1 : r0h);
                int dest = get_reg (w ? r0 : r0l);
                int res;
              
                src = bcd2int(src, w);
                dest = bcd2int(dest, w);
              
                tprintf("decimal: %d %s %d", dest, s?"-":"+", src);
                if (c)
                  tprintf(" c=%d", carry);
              
                if (!s)
                  {
                    res = dest + src;
                    if (c)
              	res += carry;
                    c = res > (w ? 9999 : 99);
                  }
                else
                  {
                    res = dest - src;
                    if (c)
              	res -= (1-carry);
                    c = res >= 0;
                    if (res < 0)
              	res += w ? 10000 : 100;
                  }
              
                res = int2bcd (res, w);
                tprintf(" = %x\n", res);
              
                set_szc (res, w+1, c);
              
                put_reg (w ? r0 : r0l, res);
                }
              
              }
            break;
          case 0xe8:
          case 0xe9:
            op_semantics_41:
              {
                /** 0111 110w 1110 100z  SMOVB.size */
#line 1383 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1383 "m32c/r8c.opc"
                int z ATTRIBUTE_UNUSED = op[1] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 100z  SMOVB.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x,", w);
                    printf("  z = 0x%x\n", z);
                  }
#line 1383 "m32c/r8c.opc"
              
                {
                int count = get_reg (r3);
                int s1 = get_reg (a0) + (get_reg (r1h) << 16);
                int s2 = get_reg (a1);
                int inc = (w ? 2 : 1) * (z ? -1 : 1);
              
                while (count)
                  {
                    if (w)
              	{
              	  v = mem_get_hi (s1);
              	  mem_put_hi (s2, v);
              	}
                    else
              	{
              	  v = mem_get_qi (s1);
              	  mem_put_qi (s2, v);
              	}
                    s1 += inc;
                    s2 += inc;
                    count --;
                  }
                put_reg (r3, count);
                put_reg (a0, s1 & 0xffff);
                put_reg (a1, s2);
                put_reg (r1h, s1 >> 16);
                }
              
              }
            break;
          case 0xea:
            op_semantics_42:
              {
                /** 0111 110w 1110 1010  SSTR.size */
#line 1413 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 1010  SSTR.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1413 "m32c/r8c.opc"
              
                {
                int count = get_reg (r3);
                int s1 = get_reg (a1);
                v = get_reg (w ? r0 : r0l);
              
                while (count)
                  {
                    if (w)
              	{
              	  mem_put_hi (s1, v);
              	  s1 += 2;
              	}
                    else
              	{
              	  mem_put_qi (s1, v);
              	  s1 += 1;
              	}
                    count --;
                  }
                put_reg (r3, count);
                put_reg (a1, s1);
                }
              
              }
            break;
          case 0xeb:
            op_semantics_43:
              {
                /** 0111 110w 1110 1011  ADD.size:G #IMM,sp */
#line 383 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1110 1011  ADD.size:G #IMM,sp */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 383 "m32c/r8c.opc"
              
                dc = reg_sd (sp);
                imm = sign_ext (IMM(w), w?16:8);
                MATH_OP (dc, imm, 0, +, > 0xffff);
              
              }
            break;
          case 0xf0:
              {
                /** 0111 1100 1111 0000  LDCTX abs16,abs20 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 1111 0000  LDCTX abs16,abs20 */",
                           op[0], op[1]);
                  }
#line 849 "m32c/r8c.opc"
              
                NOTYET();
              
              }
            break;
          case 0xf1:
            op_semantics_44:
              {
                /** 0111 110w 1111 0001  RMPA.size */
#line 1268 "m32c/r8c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 110w 1111 0001  RMPA.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1268 "m32c/r8c.opc"
              
                {
                int count = get_reg (r3);
                int list1 = get_reg (a0);
                int list2 = get_reg (a1);
                int sum = get_reg (w ? r2r0 : r0);
              
                while (count)
                  {
                    if (w)
              	{
              	  a = sign_ext (mem_get_hi (list1), 16);
              	  b = sign_ext (mem_get_hi (list2), 16);
              	}
                    else
              	{
              	  a = sign_ext (mem_get_qi (list1), 8);
              	  b = sign_ext (mem_get_qi (list2), 8);
              	}
                    tprintf("%d + %d * %d = ", sum, a, b);
                    sum += a * b;
                    tprintf("%d\n", sum);
                    list1 += w ? 2 : 1;
                    list2 += w ? 2 : 1;
                    count --;
                  }
                put_reg (r3, count);
                put_reg (a0, list1);
                put_reg (a1, list2);
                put_reg (w ? r2r0 : r0, sum);
                }
              
              }
            break;
          case 0xf2:
              {
                /** 0111 1100 1111 0010  ENTER #IMM8 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 1111 0010  ENTER #IMM8 */",
                           op[0], op[1]);
                  }
#line 671 "m32c/r8c.opc"
              
                imm = IMM(0);
                put_reg (sp, get_reg (sp) - 2);
                mem_put_hi (get_reg (sp), get_reg (fb));
                put_reg (fb, get_reg (sp));
                put_reg (sp, get_reg (sp) - imm);
              
              }
            break;
          case 0xf3:
              {
                /** 0111 1100 1111 0011  EXTS.W R0 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1100 1111 0011  EXTS.W R0 */",
                           op[0], op[1]);
                  }
#line 695 "m32c/r8c.opc"
              
                v = sign_ext (get_reg (r0), 16);
                put_reg (r2r0, v);
                set_sz (v, 2);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x7d:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x01:
          case 0x02:
          case 0x03:
          case 0x04:
          case 0x05:
          case 0x06:
          case 0x07:
          case 0x08:
          case 0x09:
          case 0x0a:
          case 0x0b:
          case 0x0c:
          case 0x0d:
          case 0x0e:
          case 0x0f:
              {
                /** 0111 1101 0000 srcx  JMPI.A src */
#line 779 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 0000 srcx  JMPI.A src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 779 "m32c/r8c.opc"
              
                sc = decode_jumpdest (srcx, 0);
                a = get_src (sc);
                put_reg (pc, a);
              
              }
            break;
          case 0x10:
          case 0x11:
          case 0x12:
          case 0x13:
          case 0x14:
          case 0x15:
          case 0x16:
          case 0x17:
          case 0x18:
          case 0x19:
          case 0x1a:
          case 0x1b:
          case 0x1c:
          case 0x1d:
          case 0x1e:
          case 0x1f:
              {
                /** 0111 1101 0001 srcx  JSRI.A src */
#line 817 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 0001 srcx  JSRI.A src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 817 "m32c/r8c.opc"
              
                sc = decode_jumpdest (srcx, 0);
                a = get_src (sc);
              
                put_reg (sp, get_reg (sp) - 3);
                mem_put_psi (get_reg (sp), get_reg (pc));
                put_reg (pc, a);
              
              }
            break;
          case 0x20:
          case 0x21:
          case 0x22:
          case 0x23:
          case 0x24:
          case 0x25:
          case 0x26:
          case 0x27:
          case 0x28:
          case 0x29:
          case 0x2a:
          case 0x2b:
          case 0x2c:
          case 0x2d:
          case 0x2e:
          case 0x2f:
              {
                /** 0111 1101 0010 srcx  JMPI.W src */
#line 772 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 0010 srcx  JMPI.W src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 772 "m32c/r8c.opc"
              
                sc = decode_jumpdest (srcx, 1);
                a = get_src (sc);
                a = sign_ext (a, 16);
                put_reg (pc, orig_pc + a);
              
              }
            break;
          case 0x30:
          case 0x31:
          case 0x32:
          case 0x33:
          case 0x34:
          case 0x35:
          case 0x36:
          case 0x37:
          case 0x38:
          case 0x39:
          case 0x3a:
          case 0x3b:
          case 0x3c:
          case 0x3d:
          case 0x3e:
          case 0x3f:
              {
                /** 0111 1101 0011 srcx  JSRI.W src */
#line 807 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 0011 srcx  JSRI.W src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 807 "m32c/r8c.opc"
              
                sc = decode_jumpdest (srcx, 1);
                a = get_src (sc);
                a = sign_ext (a, 16);
              
                put_reg (sp, get_reg (sp) - 3);
                mem_put_psi (get_reg (sp), get_reg (pc));
                put_reg (pc, orig_pc + a);
              
              }
            break;
          case 0x40:
          case 0x41:
          case 0x42:
          case 0x43:
          case 0x44:
          case 0x45:
          case 0x46:
          case 0x47:
          case 0x48:
          case 0x49:
          case 0x4a:
          case 0x4b:
          case 0x4c:
          case 0x4d:
          case 0x4e:
          case 0x4f:
            goto op_semantics_34;
            break;
          case 0x50:
          case 0x51:
          case 0x52:
          case 0x53:
          case 0x54:
          case 0x55:
          case 0x56:
          case 0x57:
          case 0x58:
          case 0x59:
          case 0x5a:
          case 0x5b:
          case 0x5c:
          case 0x5d:
          case 0x5e:
          case 0x5f:
            goto op_semantics_35;
            break;
          case 0x90:
          case 0x91:
          case 0x92:
          case 0x93:
          case 0x94:
          case 0x95:
          case 0x96:
          case 0x97:
          case 0x98:
          case 0x99:
          case 0x9a:
          case 0x9b:
          case 0x9c:
          case 0x9d:
          case 0x9e:
          case 0x9f:
              {
                /** 0111 1101 1001 srcx  PUSHA src */
#line 1227 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1001 srcx  PUSHA src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 1227 "m32c/r8c.opc"
              
                sc = decode_srcdest4 (srcx, 0);
                put_reg (sp, get_reg (sp) - 2);
                mem_put_hi (get_reg (sp), sc.u.addr);
                tprintf("pushhi: %x\n", sc.u.addr);
              
              }
            break;
          case 0xa0:
          case 0xa1:
          case 0xa2:
          case 0xa3:
          case 0xa4:
          case 0xa5:
          case 0xa6:
          case 0xa7:
              {
                /** 0111 1101 1010 0flg  LDIPL #IMM */
#line 883 "m32c/r8c.opc"
                int flg ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1010 0flg  LDIPL #IMM */",
                           op[0], op[1]);
                    printf("  flg = 0x%x\n", flg);
                  }
#line 883 "m32c/r8c.opc"
              
                set_flags (0x700, flg*0x100);
              
              }
            break;
          case 0xb0:
          case 0xb1:
          case 0xb2:
          case 0xb3:
          case 0xb4:
          case 0xb5:
          case 0xb6:
          case 0xb7:
          case 0xb8:
          case 0xb9:
          case 0xba:
          case 0xbb:
          case 0xbc:
          case 0xbd:
          case 0xbe:
          case 0xbf:
              {
                /** 0111 1101 1011 immm  ADD.size:Q #IMM,sp */
#line 389 "m32c/r8c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1011 immm  ADD.size:Q #IMM,sp */",
                           op[0], op[1]);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 389 "m32c/r8c.opc"
              
                dc = reg_sd (sp);
                imm = sign_ext (immm, 4);
                MATH_OP (dc, imm, 0, +, > 0xffff);
              
              }
            break;
          case 0xc0:
          case 0xc1:
          case 0xc2:
          case 0xc3:
          case 0xc4:
          case 0xc5:
          case 0xc6:
          case 0xc7:
          case 0xc8:
          case 0xc9:
          case 0xca:
          case 0xcb:
          case 0xcc:
          case 0xcd:
          case 0xce:
          case 0xcf:
              {
                /** 0111 1101 1100 cond  Jcnd label */
#line 741 "m32c/r8c.opc"
                int cond ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1100 cond  Jcnd label */",
                           op[0], op[1]);
                    printf("  cond = 0x%x\n", cond);
                  }
#line 741 "m32c/r8c.opc"
              
                v = sign_ext (IMM(0), 8);
                if (condition_true (cond))
                  put_reg (pc, orig_pc + 2 + v);
              
              }
            break;
          case 0xd0:
          case 0xd1:
          case 0xd2:
          case 0xd3:
          case 0xd4:
          case 0xd5:
          case 0xd6:
          case 0xd7:
          case 0xd8:
          case 0xd9:
          case 0xda:
          case 0xdb:
          case 0xdc:
          case 0xdd:
          case 0xde:
          case 0xdf:
              {
                /** 0111 1101 1101 cond  BMcnd C  */
#line 470 "m32c/r8c.opc"
                int cond ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1101 cond  BMcnd C  */",
                           op[0], op[1]);
                    printf("  cond = 0x%x\n", cond);
                  }
#line 470 "m32c/r8c.opc"
              
                if (condition_true (cond))
                  set_c (1);
                else
                  set_c (0);
              
              }
            break;
          case 0xe0:
            goto op_semantics_36;
            break;
          case 0xe1:
            goto op_semantics_37;
            break;
          case 0xe2:
            goto op_semantics_38;
            break;
          case 0xe3:
            goto op_semantics_39;
            break;
          case 0xe4:
          case 0xe5:
          case 0xe6:
          case 0xe7:
          case 0xec:
          case 0xed:
          case 0xee:
          case 0xef:
            goto op_semantics_40;
            break;
          case 0xe8:
          case 0xe9:
            goto op_semantics_41;
            break;
          case 0xea:
            goto op_semantics_42;
            break;
          case 0xeb:
            goto op_semantics_43;
            break;
          case 0xf0:
              {
                /** 0111 1101 1111 0000  STCTX abs16,abs20 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1111 0000  STCTX abs16,abs20 */",
                           op[0], op[1]);
                  }
#line 1450 "m32c/r8c.opc"
              
                NOTYET();
              
              }
            break;
          case 0xf1:
            goto op_semantics_44;
            break;
          case 0xf2:
              {
                /** 0111 1101 1111 0010  EXITD */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1111 0010  EXITD */",
                           op[0], op[1]);
                  }
#line 679 "m32c/r8c.opc"
              
                put_reg (sp, get_reg (fb));
                put_reg (fb, mem_get_hi (get_reg (sp)));
                put_reg (sp, get_reg (sp) + 2);
                put_reg (pc, mem_get_psi (get_reg (sp)));
                put_reg (sp, get_reg (sp) + 3);
              
              }
            break;
          case 0xf3:
              {
                /** 0111 1101 1111 0011  WAIT */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1101 1111 0011  WAIT */",
                           op[0], op[1]);
                  }
#line 1574 "m32c/r8c.opc"
              
                tprintf("waiting...\n");
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x7e:
        GETBYTE();
        switch (op[1] & 0xf0) {
          case 0x00:
              {
                /** 0111 1110 0000 dest  BTSTC dest */
#line 539 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0000 dest  BTSTC dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 539 "m32c/r8c.opc"
              
                dc = decode_bit (dest);
                b = get_bit (dc);
                set_zc (!b, b);
                put_bit (dc, 0);
              
              }
            break;
          case 0x10:
              {
                /** 0111 1110 0001 dest  BTSTS dest */
#line 546 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0001 dest  BTSTS dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 546 "m32c/r8c.opc"
              
                dc = decode_bit (dest);
                b = get_bit (dc);
                set_zc (!b, b);
                put_bit (dc, 1);
              
              }
            break;
          case 0x20:
              {
                /** 0111 1110 0010 dest  BMcnd dest  */
#line 462 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0010 dest  BMcnd dest  */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 462 "m32c/r8c.opc"
              
                dc = decode_bit (dest);
                if (condition_true (bmcnd_cond_map [IMM (0) & 15]))
                  put_bit (dc, 1);
                else
                  put_bit (dc, 0);
              
              }
            break;
          case 0x30:
              {
                /** 0111 1110 0011 srcx  BNTST src */
#line 494 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0011 srcx  BNTST src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 494 "m32c/r8c.opc"
              
                dc = decode_bit (srcx);
                b = get_bit (dc);
                set_zc (!b, !b);
              
              }
            break;
          case 0x40:
              {
                /** 0111 1110 0100 srcx  BAND src */
#line 448 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0100 srcx  BAND src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 448 "m32c/r8c.opc"
              
                BIT_OPC (srcx, b & carry);
              
              }
            break;
          case 0x50:
              {
                /** 0111 1110 0101 srcx  BNAND src */
#line 477 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0101 srcx  BNAND src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 477 "m32c/r8c.opc"
              
                BIT_OPC (srcx, (!b) & carry);
              
              }
            break;
          case 0x60:
              {
                /** 0111 1110 0110 srcx  BOR src */
#line 504 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0110 srcx  BOR src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 504 "m32c/r8c.opc"
              
                BIT_OPC (srcx, b | carry);
              
              }
            break;
          case 0x70:
              {
                /** 0111 1110 0111 srcx  BNOR src */
#line 481 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 0111 srcx  BNOR src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 481 "m32c/r8c.opc"
              
                BIT_OPC (srcx, (!b) | carry);
              
              }
            break;
          case 0x80:
              {
                /** 0111 1110 1000 dest  BCLR:G dest */
#line 452 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1000 dest  BCLR:G dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 452 "m32c/r8c.opc"
              
                dc = decode_bit (dest);
                put_bit (dc, 0);
              
              }
            break;
          case 0x90:
              {
                /** 0111 1110 1001 dest  BSET:G dest */
#line 517 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1001 dest  BSET:G dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 517 "m32c/r8c.opc"
              
                dc = decode_bit (dest);
                put_bit (dc, 1);
              
              }
            break;
          case 0xa0:
              {
                /** 0111 1110 1010 dest  BNOT:G dest */
#line 485 "m32c/r8c.opc"
                int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1010 dest  BNOT:G dest */",
                           op[0], op[1]);
                    printf("  dest = 0x%x\n", dest);
                  }
#line 485 "m32c/r8c.opc"
              
                BIT_OP (dest, !b);
              
              }
            break;
          case 0xb0:
              {
                /** 0111 1110 1011 srcx  BTST:G src */
#line 527 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1011 srcx  BTST:G src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 527 "m32c/r8c.opc"
              
                dc = decode_bit (srcx);
                b = get_bit (dc);
                set_zc (!b, b);
              
              }
            break;
          case 0xc0:
              {
                /** 0111 1110 1100 srcx  BXOR src */
#line 553 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1100 srcx  BXOR src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 553 "m32c/r8c.opc"
              
                BIT_OPC (srcx, b ^ carry);
              
              }
            break;
          case 0xd0:
              {
                /** 0111 1110 1101 srcx  BNXOR src */
#line 500 "m32c/r8c.opc"
                int srcx ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 0111 1110 1101 srcx  BNXOR src */",
                           op[0], op[1]);
                    printf("  srcx = 0x%x\n", srcx);
                  }
#line 500 "m32c/r8c.opc"
              
                BIT_OPC (srcx, !b ^ carry);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x80:
    case 0x81:
      GETBYTE();
        {
          /** 1000 000w srcx dest  TST.size src,dest */
#line 1563 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1563 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1563 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1000 000w srcx dest  TST.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1563 "m32c/r8c.opc"
        
          BINARY_UOP;
          tprintf ("%x & %x = %x\n", a, b, a & b);
          v = a & b;
          set_sz (v, w+1);
        
        }
      break;
    case 0x82:
    case 0x8a:
        {
          /** 1000 s010            PUSH.B:S src */
#line 1213 "m32c/r8c.opc"
          int s ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1000 s010            PUSH.B:S src */",
                     op[0]);
              printf("  s = 0x%x\n", s);
            }
#line 1213 "m32c/r8c.opc"
        
          v = get_reg (s ? r0h : r0l);
          put_reg (sp, get_reg (sp) - 1);
          mem_put_qi (get_reg (sp), v);
          tprintf("pushqi: %x\n", v);
        
        }
      break;
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
        {
          /** 1000 0dst            ADD.B:S #IMM8,dst */
#line 363 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1000 0dst            ADD.B:S #IMM8,dst */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 363 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          MATH_OP (dc, imm, 0, +, > 0xff);
        
        }
      break;
    case 0x88:
    case 0x89:
      GETBYTE();
        {
          /** 1000 100w srcx dest  XOR.size src,dest */
#line 1596 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1596 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1596 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1000 100w srcx dest  XOR.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1596 "m32c/r8c.opc"
        
          BINARY_UOP;
          tprintf ("%x ^ %x = %x\n", a, b, a ^ b);
          v = a ^ b;
          set_sz (v, w+1);
          put_dest (dc, v);
          
          /**                      OP */
        }
      break;
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f:
        {
          /** 1000 1dst            SUB.B:S #IMM8,dest */
#line 1535 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1000 1dst            SUB.B:S #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1535 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          MATH_OP (dc, imm, 0, -, >= 0);
        
        }
      break;
    case 0x90:
    case 0x91:
      GETBYTE();
        {
          /** 1001 000w srcx dest  AND.size:G src.dest */
#line 429 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 429 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 429 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1001 000w srcx dest  AND.size:G src.dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 429 "m32c/r8c.opc"
        
          BINARY_UOP;
          tprintf ("%x & %x = %x\n", a, b, a & b);
          v = a & b;
          set_sz (v, w+1);
          put_dest (dc, v);
        
        }
      break;
    case 0x92:
    case 0x9a:
        {
          /** 1001 d010            POP.B:S dest */
#line 1143 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1001 d010            POP.B:S dest */",
                     op[0]);
              printf("  d = 0x%x\n", d);
            }
#line 1143 "m32c/r8c.opc"
        
          v = mem_get_qi (get_reg (sp));
          put_reg (d ? r0h : r0l, v);
          put_reg (sp, get_reg (sp) + 1);
          tprintf("popqi: %x\n", v);
        
        }
      break;
    case 0x93:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
        {
          /** 1001 0dst            AND.B:S #IMM8,dest */
#line 419 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1001 0dst            AND.B:S #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 419 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          tprintf("%x & %x = %x\n", v, imm, v & imm);
          v &= imm;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0x98:
    case 0x99:
      GETBYTE();
        {
          /** 1001 100w srcx dest  OR.size:G src,dest */
#line 1107 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1107 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1107 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1001 100w srcx dest  OR.size:G src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1107 "m32c/r8c.opc"
        
          BINARY_UOP;
          tprintf ("%x | %x = %x\n", a, b, a | b);
          v = a | b;
          set_sz (v, w+1);
          put_dest (dc, v);
        
        }
      break;
    case 0x9b:
    case 0x9c:
    case 0x9d:
    case 0x9e:
    case 0x9f:
        {
          /** 1001 1dst            OR.B:S #IMM8,dest */
#line 1097 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1001 1dst            OR.B:S #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1097 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          tprintf("%x | %x = %x\n", v, imm, v|imm);
          v |= imm;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xa0:
    case 0xa1:
      GETBYTE();
        {
          /** 1010 000w srcx dest  ADD.size:G src,dest */
#line 369 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 369 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 369 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1010 000w srcx dest  ADD.size:G src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 369 "m32c/r8c.opc"
        
          sc = decode_srcdest4(srcx, w);
          dc = decode_srcdest4(dest, w);
          b = get_src (sc);
          MATH_OP (dc, b, 0, +, > (w?0xffff:0xff));
        
        }
      break;
    case 0xa2:
    case 0xaa:
    case 0xe2:
    case 0xea:
        {
          /** 1w10 d010            MOV.size:S #IMM,dest */
#line 913 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = (op[0] >> 6) & 0x01;
#line 913 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1w10 d010            MOV.size:S #IMM,dest */",
                     op[0]);
              printf("  w = 0x%x,", w);
              printf("  d = 0x%x\n", d);
            }
#line 913 "m32c/r8c.opc"
        
          /* Note that for w, 0=W and 1=B unlike the usual meaning.  */
          v = IMM(1-w);
          tprintf("%x = %x\n", v, v);
          set_sz (v, 2-w);
          put_reg (d ? a1 : a0, v);
        
        }
      break;
    case 0xa3:
    case 0xa4:
    case 0xa5:
    case 0xa6:
    case 0xa7:
        {
          /** 1010 0dst            INC.B dest */
#line 709 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1010 0dst            INC.B dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 709 "m32c/r8c.opc"
        
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          tprintf("%x ++ = %x\n", v, v+1);
          v ++;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xa8:
    case 0xa9:
      GETBYTE();
        {
          /** 1010 100w srcx dest  SUB.size:G src,dest */
#line 1541 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1541 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1541 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1010 100w srcx dest  SUB.size:G src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1541 "m32c/r8c.opc"
        
          sc = decode_srcdest4(srcx, w);
          dc = decode_srcdest4(dest, w);
          b = get_src (sc);
          MATH_OP (dc, b, 0, -, >= 0);
        
        }
      break;
    case 0xab:
    case 0xac:
    case 0xad:
    case 0xae:
    case 0xaf:
        {
          /** 1010 1dst            DEC.B dest */
#line 630 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1010 1dst            DEC.B dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 630 "m32c/r8c.opc"
        
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          tprintf("%x -- = %x\n", v, v-1);
          v --;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xb0:
    case 0xb1:
      GETBYTE();
        {
          /** 1011 000w srcx dest  ADC.size src,dest */
#line 339 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 339 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 339 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1011 000w srcx dest  ADC.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 339 "m32c/r8c.opc"
        
          sc = decode_srcdest4(srcx, w);
          dc = decode_srcdest4(dest, w);
          b = get_src (sc);
          MATH_OP (dc, b, carry, +, > (w?0xffff:0xff));
        
        }
      break;
    case 0xb2:
    case 0xba:
        {
          /** 1011 d010            INC.W dest */
#line 718 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1011 d010            INC.W dest */",
                     op[0]);
              printf("  d = 0x%x\n", d);
            }
#line 718 "m32c/r8c.opc"
        
          v = get_reg (d ? a1 : a0);
          tprintf("%x ++ = %x\n", v, v+1);
          v ++;
          set_sz (v, 2);
          put_reg (d ? a1 : a0, v);
        
        }
      break;
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
        {
          /** 1011 0dst            MOV.B:Z #0,dest */
#line 921 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1011 0dst            MOV.B:Z #0,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 921 "m32c/r8c.opc"
        
          dc = decode_dest3 (dst, 0);
          v = 0;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xb8:
    case 0xb9:
      GETBYTE();
        {
          /** 1011 100w srcx dest  SBB.size src,dest */
#line 1332 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1332 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1332 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1011 100w srcx dest  SBB.size src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 1332 "m32c/r8c.opc"
        
          sc = decode_srcdest4(srcx, w);
          dc = decode_srcdest4(dest, w);
          b = get_src (sc);
          MATH_OP (dc, b, !carry, -, >= 0);
        
        }
      break;
    case 0xbb:
    case 0xbc:
    case 0xbd:
    case 0xbe:
    case 0xbf:
        {
          /** 1011 1dst            NOT.B:S dest */
#line 1079 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1011 1dst            NOT.B:S dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1079 "m32c/r8c.opc"
        
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          tprintf("~ %x = %x\n", v, ~v);
          v = ~v;
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xc0:
    case 0xc1:
      GETBYTE();
        {
          /** 1100 000w srcx dest  CMP.size:G src,dest */
#line 576 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 576 "m32c/r8c.opc"
          int srcx ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 576 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1100 000w srcx dest  CMP.size:G src,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  srcx = 0x%x,", srcx);
              printf("  dest = 0x%x\n", dest);
            }
#line 576 "m32c/r8c.opc"
        
          BINARY_UOP;
          cmp(b, a, w);
        
        }
      break;
    case 0xc2:
    case 0xca:
        {
          /** 1100 s010            PUSH.W:S src */
#line 1220 "m32c/r8c.opc"
          int s ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 s010            PUSH.W:S src */",
                     op[0]);
              printf("  s = 0x%x\n", s);
            }
#line 1220 "m32c/r8c.opc"
        
          v = get_reg (s ? a1 : a0);
          put_reg (sp, get_reg (sp) - 2);
          mem_put_hi (get_reg (sp), v);
          tprintf("pushhi: %x\n", v);
        
        }
      break;
    case 0xc3:
    case 0xc4:
    case 0xc5:
    case 0xc6:
    case 0xc7:
        {
          /** 1100 0dst            MOV.B:S #IMM8,dest */
#line 904 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 0dst            MOV.B:S #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 904 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          v = imm;
          tprintf("%x = %x\n", v, v);
          set_sz (v, 1);
          put_dest (dc, v);
        
        }
      break;
    case 0xc8:
    case 0xc9:
      GETBYTE();
        {
          /** 1100 100w immm dest  ADD.size:Q #IMM,dest */
#line 357 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 357 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 357 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1100 100w immm dest  ADD.size:Q #IMM,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 357 "m32c/r8c.opc"
        
          dc = decode_srcdest4(dest, w);
          imm = sign_ext (immm, 4);
          MATH_OP (dc, imm, 0, +, > (w?0xffff:0xff));
        
        }
      break;
    case 0xcb:
    case 0xcc:
    case 0xcd:
    case 0xce:
    case 0xcf:
        {
          /** 1100 1dst            STZ #IMM8,dest */
#line 1512 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 1dst            STZ #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1512 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3(dst, 0);
          if (FLAG_Z)
            put_dest (dc, imm);
        
        }
      break;
    case 0xd0:
    case 0xd1:
      GETBYTE();
        {
          /** 1101 000w immm dest  CMP.size:Q #IMM,dest */
#line 563 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 563 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 563 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1101 000w immm dest  CMP.size:Q #IMM,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 563 "m32c/r8c.opc"
        
          UNARY_UOP;
          immm = sign_ext (immm, 4);
          cmp (v, immm, w);
        
        }
      break;
    case 0xd2:
    case 0xda:
        {
          /** 1101 d010            POP.W:S dest */
#line 1150 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 d010            POP.W:S dest */",
                     op[0]);
              printf("  d = 0x%x\n", d);
            }
#line 1150 "m32c/r8c.opc"
        
          v = mem_get_hi (get_reg (sp));
          put_reg (d ? a1 : a0, v);
          put_reg (sp, get_reg (sp) + 2);
          tprintf("pophi: %x\n", v);
        
        }
      break;
    case 0xd3:
    case 0xd4:
    case 0xd5:
    case 0xd6:
    case 0xd7:
        {
          /** 1101 0dst            STNZ #IMM8,dest */
#line 1505 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 0dst            STNZ #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1505 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3(dst, 0);
          if (!FLAG_Z)
            put_dest (dc, imm);
        
        }
      break;
    case 0xd8:
    case 0xd9:
      GETBYTE();
        {
          /** 1101 100w immm dest  MOV.size:Q #IMM,dest */
#line 896 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 896 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 896 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1101 100w immm dest  MOV.size:Q #IMM,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 896 "m32c/r8c.opc"
        
          dc = decode_srcdest4 (dest, w);
          v = sign_ext (immm, 4);
          tprintf ("%x = %x\n", v, v);
          set_sz (v, w+1);
          put_dest (dc, v);
        
        }
      break;
    case 0xdb:
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xdf:
        {
          /** 1101 1dst            STZX #IMM81,#IMM82,dest */
#line 1519 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 1dst            STZX #IMM81,#IMM82,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 1519 "m32c/r8c.opc"
        
          a = IMM(0);
          dc = decode_dest3(dst, 0);
          b = IMM(0);
          if (FLAG_Z)
            put_dest (dc, a);
          else
            put_dest (dc, b);
        
        }
      break;
    case 0xe0:
    case 0xe1:
      GETBYTE();
        {
          /** 1110 000w immm dest  ROT.size #IMM,dest */
#line 1311 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1311 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1311 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1110 000w immm dest  ROT.size #IMM,dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 1311 "m32c/r8c.opc"
        
          dc = decode_srcdest4 (dest, w);
          rot_op (dc, 0, IMM4());
        
        }
      break;
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
        {
          /** 1110 0dst            CMP.B:S #IMM8,dest */
#line 569 "m32c/r8c.opc"
          int dst ATTRIBUTE_UNUSED = op[0] & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 0dst            CMP.B:S #IMM8,dest */",
                     op[0]);
              printf("  dst = 0x%x\n", dst);
            }
#line 569 "m32c/r8c.opc"
        
          imm = IMM(0);
          dc = decode_dest3 (dst, 0);
          v = get_src (dc);
          cmp (v, imm, 0);
        
        }
      break;
    case 0xe8:
    case 0xe9:
      GETBYTE();
        {
          /** 1110 100w immm dest  SHL.size #IMM, dest */
#line 1361 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1361 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1361 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1110 100w immm dest  SHL.size #IMM, dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 1361 "m32c/r8c.opc"
        
          dc = decode_srcdest4(dest, w);
          shift_op (dc, 0, IMM4());
        
        }
      break;
    case 0xeb:
        GETBYTE();
        switch (op[1] & 0xef) {
          case 0x00:
          case 0x20:
          case 0x40:
          case 0x60:
              {
                /** 1110 1011 0reg 0000  LDC #IMM16,dest */
#line 837 "m32c/r8c.opc"
                int reg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0reg 0000  LDC #IMM16,dest */",
                           op[0], op[1]);
                    printf("  reg = 0x%x\n", reg);
                  }
#line 837 "m32c/r8c.opc"
              
                dc = decode_cr (reg);
                imm = IMM(1);
                put_dest (dc, imm);
              
              }
            break;
          case 0x01:
              {
                /** 1110 1011 000d 0001  SHL.L R1H,dest */
#line 1377 "m32c/r8c.opc"
                int d ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 000d 0001  SHL.L R1H,dest */",
                           op[0], op[1]);
                    printf("  d = 0x%x\n", d);
                  }
#line 1377 "m32c/r8c.opc"
              
                dc = reg_sd (d ? r3r1 : r2r0);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 0, a);
              
              }
            break;
          case 0x02:
          case 0x22:
          case 0x42:
          case 0x62:
              {
                /** 1110 1011 0src 0010  PUSHC src */
#line 1234 "m32c/r8c.opc"
                int src ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0src 0010  PUSHC src */",
                           op[0], op[1]);
                    printf("  src = 0x%x\n", src);
                  }
#line 1234 "m32c/r8c.opc"
              
                sc = decode_cr (src);
                put_reg (sp, get_reg (sp) - 2);
                v = get_src (sc);
                mem_put_hi (get_reg (sp), v);
                tprintf("pushc: %x\n", v);
              
              }
            break;
          case 0x03:
          case 0x23:
          case 0x43:
          case 0x63:
              {
                /** 1110 1011 0reg 0011  POPC dest */
#line 1157 "m32c/r8c.opc"
                int reg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0reg 0011  POPC dest */",
                           op[0], op[1]);
                    printf("  reg = 0x%x\n", reg);
                  }
#line 1157 "m32c/r8c.opc"
              
                dc = decode_cr (reg);
                v = mem_get_hi (get_reg (sp));
                put_dest (dc, v);
                put_reg (sp, get_reg (sp) + 2);
                tprintf("popc: %x\n", v);
              
              }
            break;
          case 0x04:
          case 0x24:
          case 0x44:
          case 0x64:
              {
                /** 1110 1011 0flg 0100  FSET dest */
#line 705 "m32c/r8c.opc"
                int flg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0flg 0100  FSET dest */",
                           op[0], op[1]);
                    printf("  flg = 0x%x\n", flg);
                  }
#line 705 "m32c/r8c.opc"
              
                set_flags (1 << flg, 1 << flg);
              
              }
            break;
          case 0x05:
          case 0x25:
          case 0x45:
          case 0x65:
              {
                /** 1110 1011 0flg 0101  FCLR dest */
#line 701 "m32c/r8c.opc"
                int flg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0flg 0101  FCLR dest */",
                           op[0], op[1]);
                    printf("  flg = 0x%x\n", flg);
                  }
#line 701 "m32c/r8c.opc"
              
                set_flags (1 << flg, 0);
              
              }
            break;
          case 0x08:
          case 0x09:
          case 0x0a:
          case 0x0b:
          case 0x0c:
          case 0x0d:
          case 0x0e:
          case 0x0f:
          case 0x28:
          case 0x29:
          case 0x2a:
          case 0x2b:
          case 0x2c:
          case 0x2d:
          case 0x2e:
          case 0x2f:
          case 0x48:
          case 0x49:
          case 0x4a:
          case 0x4b:
          case 0x4c:
          case 0x4d:
          case 0x4e:
          case 0x4f:
          case 0x68:
          case 0x69:
          case 0x6a:
          case 0x6b:
          case 0x6c:
          case 0x6d:
          case 0x6e:
          case 0x6f:
              {
                /** 1110 1011 0reg 1src  MOVA src,dest */
#line 985 "m32c/r8c.opc"
                int reg ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 985 "m32c/r8c.opc"
                int src ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 0reg 1src  MOVA src,dest */",
                           op[0], op[1]);
                    printf("  reg = 0x%x,", reg);
                    printf("  src = 0x%x\n", src);
                  }
#line 985 "m32c/r8c.opc"
              
                {
                static reg_id map[] = { r0, r1, r2, r3, a0, a1, 0, 0 };
                sc = decode_srcdest4 (8 + src, 0);
                put_reg (map[reg], sc.u.addr);
                }
              
              }
            break;
          case 0x21:
              {
                /** 1110 1011 001d 0001  SHA.L R1H,dest */
#line 1355 "m32c/r8c.opc"
                int d ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 001d 0001  SHA.L R1H,dest */",
                           op[0], op[1]);
                    printf("  d = 0x%x\n", d);
                  }
#line 1355 "m32c/r8c.opc"
              
                dc = reg_sd (d ? r3r1 : r2r0);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 1, a);
              
              }
            break;
          case 0x80:
          case 0x81:
          case 0x82:
          case 0x83:
          case 0x84:
          case 0x85:
          case 0x86:
          case 0x87:
          case 0x88:
          case 0x89:
          case 0x8a:
          case 0x8b:
          case 0x8c:
          case 0x8d:
          case 0x8e:
          case 0x8f:
              {
                /** 1110 1011 100d immm  SHL.L #IMM,dest */
#line 1372 "m32c/r8c.opc"
                int d ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
#line 1372 "m32c/r8c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 100d immm  SHL.L #IMM,dest */",
                           op[0], op[1]);
                    printf("  d = 0x%x,", d);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1372 "m32c/r8c.opc"
              
                dc = reg_sd (d ? r3r1 : r2r0);
                shift_op (dc, 0, IMM4());
              
              }
            break;
          case 0xa0:
          case 0xa1:
          case 0xa2:
          case 0xa3:
          case 0xa4:
          case 0xa5:
          case 0xa6:
          case 0xa7:
          case 0xa8:
          case 0xa9:
          case 0xaa:
          case 0xab:
          case 0xac:
          case 0xad:
          case 0xae:
          case 0xaf:
              {
                /** 1110 1011 101d immm  SHA.L #IMM, dest */
#line 1350 "m32c/r8c.opc"
                int d ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
#line 1350 "m32c/r8c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 101d immm  SHA.L #IMM, dest */",
                           op[0], op[1]);
                    printf("  d = 0x%x,", d);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1350 "m32c/r8c.opc"
              
                dc = reg_sd (d ? r3r1 : r2r0);
                shift_op (dc, 1, IMM4());
              
              }
            break;
          case 0xc0:
          case 0xc1:
          case 0xc2:
          case 0xc3:
          case 0xc4:
          case 0xc5:
          case 0xc6:
          case 0xc7:
          case 0xc8:
          case 0xc9:
          case 0xca:
          case 0xcb:
          case 0xcc:
          case 0xcd:
          case 0xce:
          case 0xcf:
          case 0xe0:
          case 0xe1:
          case 0xe2:
          case 0xe3:
          case 0xe4:
          case 0xe5:
          case 0xe6:
          case 0xe7:
          case 0xe8:
          case 0xe9:
          case 0xea:
          case 0xeb:
          case 0xec:
          case 0xed:
          case 0xee:
          case 0xef:
              {
                /** 1110 1011 11vector   INT #imm */
#line 726 "m32c/r8c.opc"
                int vector ATTRIBUTE_UNUSED = op[1] & 0x3f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 1011 11vector   INT #imm */",
                           op[0], op[1]);
                    printf("  vector = 0x%x\n", vector);
                  }
#line 726 "m32c/r8c.opc"
              
                trigger_based_interrupt (vector);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xec:
        {
          /** 1110 1100            PUSHM src */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 1100            PUSHM src */",
                     op[0]);
            }
#line 1242 "m32c/r8c.opc"
        
          {
          static int map[] = { fb, sb, a1, a0, r3, r2, r1, r0 };
          imm = IMM(0);
          tprintf("pushm: %x\n", imm);
          for (a=0; a<8; a++)
            if (imm & (1<<a))
              {
        	put_reg (sp, get_reg (sp) - 2);
        	v = get_reg (map[a]);
        	mem_put_hi (get_reg (sp), v);
              }
          }
        
        }
      break;
    case 0xed:
        {
          /** 1110 1101            POPM dest */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 1101            POPM dest */",
                     op[0]);
            }
#line 1165 "m32c/r8c.opc"
        
          {
          static int map[] = { r0, r1, r2, r3, a0, a1, sb, fb };
          imm = IMM(0);
          tprintf("popm: %x\n", imm);
          for (a=0; a<8; a++)
            if (imm & (1<<a))
              {
        	v = mem_get_hi (get_reg (sp));
        	put_reg (map[a], v);
        	put_reg (sp, get_reg (sp) + 2);
              }
          }
        
        }
      break;
    case 0xee:
        {
          /** 1110 1110            JMPS #IMM8 */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 1110            JMPS #IMM8 */",
                     op[0]);
            }
#line 785 "m32c/r8c.opc"
        
          M16C_ONLY();
        
          imm = IMM(0);
          a = 0xf0000 + mem_get_hi (0xffffe - imm * 2);
          put_reg (pc, a);
        
        }
      break;
    case 0xef:
        {
          /** 1110 1111            JSRS #IMM8 */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 1111            JSRS #IMM8 */",
                     op[0]);
            }
#line 826 "m32c/r8c.opc"
        
          M16C_ONLY();
        
          imm = IMM(0);
          a = 0xf0000 + mem_get_hi (0xffffe - imm * 2);
        
          put_reg (sp, get_reg (sp) - 3);
          mem_put_psi (get_reg (sp), get_reg (pc));
          put_reg (pc, a);
        
        }
      break;
    case 0xf0:
    case 0xf1:
      GETBYTE();
        {
          /** 1111 000w immm dest  SHA.size #IMM, dest */
#line 1339 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1339 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 1339 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1111 000w immm dest  SHA.size #IMM, dest */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 1339 "m32c/r8c.opc"
        
          dc = decode_srcdest4(dest, w);
          shift_op (dc, 1, IMM4());
        
        }
      break;
    case 0xf2:
    case 0xfa:
        {
          /** 1111 d010            DEC.W dest */
#line 639 "m32c/r8c.opc"
          int d ATTRIBUTE_UNUSED = (op[0] >> 3) & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 d010            DEC.W dest */",
                     op[0]);
              printf("  d = 0x%x\n", d);
            }
#line 639 "m32c/r8c.opc"
        
          v = get_reg (d ? a1 : a0);
          tprintf("%x -- = %x\n", v, v-1);
          v --;
          set_sz (v, 2);
          put_reg (d ? a1 : a0, v);
        
        }
      break;
    case 0xf3:
        {
          /** 1111 0011            RTS */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 0011            RTS */",
                     op[0]);
            }
#line 1321 "m32c/r8c.opc"
        
          put_reg (pc, mem_get_psi (get_reg (sp)));
          put_reg (sp, get_reg (sp) + 3);
        
        }
      break;
    case 0xf4:
        {
          /** 1111 0100            JMP.W label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 0100            JMP.W label */",
                     op[0]);
            }
#line 762 "m32c/r8c.opc"
        
          imm = sign_ext (IMM(1), 16);
          put_reg (pc, orig_pc + 1 + imm);
        
        }
      break;
    case 0xf5:
        {
          /** 1111 0101            JSR.W label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 0101            JSR.W label */",
                     op[0]);
            }
#line 793 "m32c/r8c.opc"
        
          imm = sign_ext (IMM(1), 16);
          put_reg (sp, get_reg (sp) - 3);
          mem_put_psi (get_reg (sp), get_reg (pc));
          put_reg (pc, orig_pc + imm + 1);
        
        }
      break;
    case 0xf6:
        {
          /** 1111 0110            INTO */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 0110            INTO */",
                     op[0]);
            }
#line 730 "m32c/r8c.opc"
        
          if (FLAG_O)
            trigger_fixed_interrupt (0xffe0);
        
        }
      break;
    case 0xf8:
    case 0xf9:
      GETBYTE();
        {
          /** 1111 100w immm dest  ADJNZ.size #IMM,dest,label */
#line 395 "m32c/r8c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 395 "m32c/r8c.opc"
          int immm ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x0f;
#line 395 "m32c/r8c.opc"
          int dest ATTRIBUTE_UNUSED = op[1] & 0x0f;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 1111 100w immm dest  ADJNZ.size #IMM,dest,label */",
                     op[0], op[1]);
              printf("  w = 0x%x,", w);
              printf("  immm = 0x%x,", immm);
              printf("  dest = 0x%x\n", dest);
            }
#line 395 "m32c/r8c.opc"
        
          UNARY_UOP;
          imm = sign_ext(immm, 4);
          tprintf("%x + %d = %x\n", v, imm, v+imm);
          v += imm;
          put_dest (dc, v);
          a = sign_ext (IMM(0), 8);
          if ((v & (w ? 0xffff : 0xff)) != 0)
            {
              tprintf("jmp: %x + 2 + %d = ", get_reg (pc), a);
              put_reg (pc, orig_pc + 2 + a);
              tprintf("%x\n", get_reg (pc));
            }
        
        }
      break;
    case 0xfb:
        {
          /** 1111 1011            REIT */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1011            REIT */",
                     op[0]);
            }
#line 1257 "m32c/r8c.opc"
        
          a = get_reg (sp);
          v = (mem_get_hi (a)
               + 4096 * (mem_get_qi (a+3) & 0xf0));
          b = (mem_get_qi (a+2)
               + 256 * (mem_get_qi (a+3) & 0xff));
          put_reg (pc, v);
          put_reg (flags, b);
          put_reg (sp, get_reg (sp) + 4);
        
        }
      break;
    case 0xfc:
        {
          /** 1111 1100            JMP.A label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1100            JMP.A label */",
                     op[0]);
            }
#line 767 "m32c/r8c.opc"
          
          imm = IMM(2);
          put_reg (pc, imm);
        
        }
      break;
    case 0xfd:
        {
          /** 1111 1101            JSR.A label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1101            JSR.A label */",
                     op[0]);
            }
#line 800 "m32c/r8c.opc"
        
          imm = IMM(2);
          put_reg (sp, get_reg (sp) - 3);
          mem_put_psi (get_reg (sp), get_reg (pc));
          put_reg (pc, imm);
        
        }
      break;
    case 0xfe:
        {
          /** 1111 1110            JMP.B label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1110            JMP.B label */",
                     op[0]);
            }
#line 751 "m32c/r8c.opc"
        
          imm = sign_ext (IMM(0), 8);
          if (imm == -1)
            {
              if (verbose)
        	printf("[jmp-to-self detected as exit]\n");
              return M32C_MAKE_HIT_BREAK ();
            }
          put_reg (pc, orig_pc + 1 + imm);
        
        }
      break;
    case 0xff:
        {
          /** 1111 1111            UND */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1111            UND */",
                     op[0]);
            }
#line 1570 "m32c/r8c.opc"
        
          trigger_fixed_interrupt (0xffdc);
        
        }
      break;
    default: UNSUPPORTED(); break;
  }
#line 1605 "m32c/r8c.opc"

  return step_result;
}
