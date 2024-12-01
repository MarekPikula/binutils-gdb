#line 1 "m32c/m32c.opc"
/* m32c.opc --- semantics for m32c opcodes.		        -*- mode: c -*-

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

#define M32C_ONLY() /* FIXME: add something here */

#define GETBYTE() (op[opi++] = getbyte())

#define UNSUPPORTED() unsupported("unsupported", m32c_opcode_pc)
#define NOTYET() unsupported("unimplemented", m32c_opcode_pc)

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
IMM(int bytes)
{
  int rv = 0;
  switch (bytes)
    {
    case 1:
      rv = mem_get_qi (get_reg(pc));
      break;
    case 2:
      rv = mem_get_hi (get_reg(pc));
      break;
    case 3:
      rv = mem_get_psi (get_reg(pc));
      break;
    case 4:
      rv = mem_get_si (get_reg(pc));
      break;
    }
  regs.r_pc += bytes;
  return rv;
}

#define IMM4() (immm >= 8 ? 7 - immm : immm + 1)

#define NO_PREFIX() PREFIX(0,0,0)

/* Indicate which sorts of prefixes are allowed for the current
   opcode.  */
static void
prefix (int src_allowed, int dest_allowed, int index_bytewidth)
{
  /* At the moment, we don't do anything with this information.  We
     just wanted to get the information entered in some
     machine-readable form while we were going through all the
     opcodes.  */
}

#define MATH_OP(dc,s,c,op) \
{ \
  int ma, mb; \
  ma = get_src(dc); \
  mb = s & b2mask[dc.bytes]; \
  ll = (long long)ma op (long long)mb op c; \
  tprintf("0x%x " #op " 0x%x " #op " 0x%x = 0x%llx\n", ma, mb, c, ll); \
  ma = sign_ext (ma, dc.bytes * 8); \
  mb = sign_ext (s, dc.bytes * 8); \
  v = ma op mb op c; \
  tprintf("%d " #op " %d " #op " %d = %d\n", ma, mb, c, v); \
  set_oszc (v, dc.bytes, (1 op 1) ? (ll > b2mask[dc.bytes]) : (ll >= 0)); \
  put_dest (dc, v); \
}

#define LOGIC_OP(dc,s,op) \
{ \
  int ma, mb; \
  ma = get_src(dc); \
  mb = s & b2mask[dc.bytes]; \
  v = ma op mb; \
  tprintf("0x%x " #op " 0x%x = 0x%x\n", ma, mb, v); \
  set_sz (v, dc.bytes); \
  put_dest (dc, v); \
}

#define BIT_OP(dc,bit,expr)                                             \
  b = get_bit2 (dc, bitindex == -1 ? bit : bitindex);                   \
  v = expr;                                                             \
  tprintf ("b=%d, bit=%d, carry=%d, %s = %d\n",                         \
           b,  bitindex == -1 ? bit : bitindex, carry, #expr, v);       \
  put_bit2 (dc,  bitindex == -1 ? bit : bitindex, v);

#define BIT_OPC(dc,bit,expr)                                            \
  b = get_bit2 (dc,  bitindex == -1 ? bit : bitindex);                  \
  v = expr;                                                             \
  tprintf ("b=%d, bit=%d, carry=%d, %s = %d\n",                         \
           b,  bitindex == -1 ? bit : bitindex, carry, #expr, v);       \
  set_c (v);

#define carry (FLAG_C ? 1 : 0)

static void
cmp (int d, int s, int bytes)
{
  int a, b, f=0;
  a = d - s;
  b = sign_ext (d, bytes*8) - sign_ext (s, bytes*8);
  tprintf ("cmp: %x - %x = %08x, %x - %x = %d\n",
	   d, s, a,
	   sign_ext(d,bytes*8), sign_ext(s,bytes*8), b);

  if (b == 0)
    f |= FLAGBIT_Z;
  if (b & b2signbit[bytes])
    f |= FLAGBIT_S;
  if ((d & b2mask[bytes]) >= (s & b2mask[bytes]))
    f |= FLAGBIT_C;
  if (b < b2minsigned[bytes] || b > b2maxsigned[bytes])
    f |= FLAGBIT_O;

  set_flags (FLAGBIT_Z | FLAGBIT_S | FLAGBIT_O | FLAGBIT_C, f);
}

static void
dadd_op (int ddd, int dd, int sss, int ss, int imm, int add, int cy, int w)
{
  srcdest sc, dc;
  int a, b=0, res;

  prefix (0, 0, 0);

  if (!imm)
    {
      sc = decode_src23 (sss, ss, w+1);
      b = get_src (sc);
    }
  dc = decode_dest23 (ddd, dd, w+1);
  a = get_src (dc);
  if (imm)
    b = IMM(w+1);

  a = bcd2int(a, w);
  b = bcd2int(b, w);

  tprintf("decimal: %d %s %d", a, add?"+":"-", b);
  if (cy)
    tprintf(" c=%d", carry);

  if (add)
    {
      res = a + b;
      if (cy)
	res += carry;
      cy = res > (w ? 9999 : 99);
    }
  else
    {
      res = a - b;
      if (cy)
	res -= (1-carry);
      cy = res >= 0;
      if (res < 0)
	res += w ? 10000 : 100;
    }

  res = int2bcd (res, w);
  tprintf(" = %x\n", res);

  set_szc (res, w+1, cy);

  put_dest (dc, res);
}
#define DADDV(A,C) dadd_op(ddd, dd, sss, ss, 0, A, C, w)
#define DADDI(A,C) dadd_op(ddd, dd, 0, 0, 1, A, C, w)

static void
div_op (int sss, int ss, int u, int x, int bytes)
{
  srcdest sc;
  int s, v, a, b;

  if (sss == -1)
    s = IMM(bytes);
  else
    {
      sc = decode_dest23 (sss, ss, bytes);
      s = get_src (sc);
    }

  v = get_reg (bytes > 1 ? r2r0 : r0);

  if (!u)
    {
      /* FIXME? do we sign extend a0/a1 to .L?  Docs say zero extend.  */
      s = sign_ext (s, bytes*8);
      v = sign_ext (v, bytes*8);
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
  if ((!u && (a > b2maxsigned[bytes]
	      || a < b2minsigned[bytes]))
      || (u && (a > b2mask[bytes])))
    set_flags (FLAGBIT_O, FLAGBIT_O);
  else
    set_flags (FLAGBIT_O, 0);

  switch (bytes)
    {
    case 1:
      put_reg (r0l, a);
      put_reg (r0h, b);
      break;
    case 2:
      put_reg (r0, a);
      put_reg (r2, b);
      break;
    case 4:
      put_reg (r2r0, a);
      break;
    }
}

static void
index_op (int sss, int ss, int do_s, int do_d, int scale, int w)
{
  srcdest sc = decode_src23 (sss, ss, w+1);
  int v = get_src (sc) * scale;
  tprintf("%d = %d * %d, %d %d\n", v, get_src(sc), scale, do_s, do_d);
  decode_index (do_s * v, do_d * v);
}
#define INDEXOP(scale,do_s,do_d)                                \
  index_op (sss, ss, do_s, do_d, scale, w); goto next_opcode

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
shift_op (srcdest sd, int arith, int count, int setc)
{
  int mask = (sd.bytes == 2) ? 0xffff : 0xff;
  int msb = (sd.bytes == 2) ? 0x8000 : 0x80;
  int v = get_src (sd);
  int c = 0;
  int o = 0;

  if (sd.bytes == 4)
    {
      mask = 0xffffffffU;
      msb = 0x80000000U;
    }

  tprintf("%s %x by %d\n", arith ? "sha" : "shl", v, count);
  tprintf (": %s %d %d\n", bits(v, 8*sd.bytes), c, o);
  while (count > 0)
    {
      c = (v & msb) ? 1 : 0;
      v <<= 1;
      v &= mask;
      if (c != ((v & msb) ? 1 : 0))
	o = 1;
      tprintf (": %s %d %d\n", bits(v, 8*sd.bytes), c, o);
      count --;
    }
  while (count < 0)
    {
      c = v & 1;
      if (arith)
	v = (v & msb) | (v >> 1);
      else
	v = (v >> 1) & (msb - 1);
      tprintf (": %s %d %d\n", bits(v, 8*sd.bytes), c, o);
      count ++;
    }
  put_dest (sd, v);
  set_sz (v, sd.bytes);
  if (setc)
    set_c (c);
  set_flags (FLAGBIT_O, o ? FLAGBIT_O : 0);
}

static int pcs[16];
static int ipcs = 0;

int
decode_m32c (void)
{
  unsigned char op[40];
  int opi;
  int v, a, b;
  long long ll;
  srcdest sc, dc;
  int imm;
  int bitindex = -1;
  int t0, t1=0, t2, t3=0;
  int ta0, ta1, dif;

  step_result = M32C_MAKE_STEPPED ();

  decode_indirect (0, 0);
  decode_index (0, 0);

next_opcode:
  opi = 0;
  m32c_opcode_pc = get_reg (pc);

  tprintf("trace: decode pc = %06x\n", m32c_opcode_pc);

  if (m32c_opcode_pc == 0)
    {
      int i;
      printf("Abort: PC is zero, here from:\n");
      for (i=0; i<4; i++)
	printf("  0x%06x\n", pcs[(ipcs+15-i)%16]);
      return M32C_MAKE_HIT_BREAK ();
    }
  pcs[ipcs++] = m32c_opcode_pc;
  ipcs %= 16;


  GETBYTE();
  switch (op[0] & 0xff) {
    case 0x00:
        {
          /** 0000 0000				BRK */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 0000				BRK */",
                     op[0]);
            }
#line 679 "m32c/m32c.opc"
        
          /* We report the break to our caller with the PC still pointing at the 
             breakpoint instruction.  */
          put_reg (pc, m32c_opcode_pc);
          if (verbose)
            printf("[break]\n");
          if (in_gdb || (regs.r_intbl == 0 && regs.r_intbh == 0))
            return M32C_MAKE_HIT_BREAK ();
          if (mem_get_qi (0xFFFFE7) == 0xff)
            trigger_based_interrupt (0);
          else
            trigger_fixed_interrupt (0xFFFFE4);
        
        }
      break;
    case 0x01:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x80:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  op_semantics_1:
                    {
                      /** 0000 0001 1sss ddd w dd ss 0000	DADD.size src,dest */
#line 852 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 852 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 852 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 852 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 852 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 0000	DADD.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 852 "m32c/m32c.opc"
                    
                      DADDV(1,0);
                    
                    }
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  op_semantics_2:
                    {
                      /** 0000 0001 1sss ddd w dd ss 0010	DSUB.size src,dest */
#line 926 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 926 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 926 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 926 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 926 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 0010	DSUB.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 926 "m32c/m32c.opc"
                    
                      DADDV(0,0);
                    
                    }
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  op_semantics_3:
                    {
                      /** 0000 0001 1sss ddd w dd ss 0100	ADC.size src,dest */
#line 467 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 467 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 467 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 467 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 467 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 0100	ADC.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 467 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, w+1);
                      dc = decode_dest23 (ddd, dd, w+1);
                      b = get_src (sc);
                      MATH_OP (dc, b, carry, +);
                    
                    }
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  op_semantics_4:
                    {
                      /** 0000 0001 1sss ddd w dd ss 0110	SBB.size src,dest */
#line 1756 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 1756 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1756 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1756 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1756 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 0110	SBB.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 1756 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, w+1);
                      dc = decode_dest23 (ddd, dd, w+1);
                      MATH_OP (dc, get_src (sc), !carry, -);
                    
                    }
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  op_semantics_5:
                    {
                      /** 0000 0001 1sss ddd0 dd ss 0111	EXTS.B src,dest */
#line 955 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 955 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 955 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 955 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd0 dd ss 0111	EXTS.B src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 955 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      dc = decode_dest23 (ddd, dd, 2);
                      v = sign_ext (get_src (sc), 8);
                      put_dest (dc, v);
                      set_sz (v, 16);
                    
                    }
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  op_semantics_6:
                    {
                      /** 0000 0001 1sss ddd w dd ss 1000	DADC.size src,dest */
#line 844 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 844 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 844 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 844 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 844 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 1000	DADC.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 844 "m32c/m32c.opc"
                    
                      DADDV(1,1);
                    
                    }
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  op_semantics_7:
                    {
                      /** 0000 0001 1sss ddd w dd ss 1001	TST.size:G src,dest */
#line 2105 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 2105 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 2105 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 2105 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 2105 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 1001	TST.size:G src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 2105 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, w+1);
                      dc = decode_dest23 (ddd, dd, w+1);
                      b = get_src (sc);
                      a = get_src (dc);
                      v = a & b;
                      set_sz (v, w+1);
                    
                    }
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  op_semantics_8:
                    {
                      /** 0000 0001 1sss ddd w dd ss 1010	DSBB.size src,dest */
#line 918 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 918 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 918 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 918 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 918 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 1010	DSBB.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 918 "m32c/m32c.opc"
                    
                      DADDV(0,1);
                    
                    }
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  op_semantics_9:
                    {
                      /** 0000 0001 1sss ddd0 dd ss 1011	EXTZ src,dest */
#line 964 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 964 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 964 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 964 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd0 dd ss 1011	EXTZ src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 964 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      dc = decode_dest23 (ddd, dd, 2);
                      v = get_src (sc);
                      put_dest (dc, v);
                      set_sz (v, 16);
                    
                    }
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  op_semantics_10:
                    {
                      /** 0000 0001 1sss ddd w dd ss 1100	MIN.size src,dest */
#line 1211 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 1211 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1211 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1211 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1211 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 1100	MIN.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 1211 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      w++;
                      sc = decode_src23 (sss, ss, w);
                      dc = decode_dest23 (ddd, dd, w);
                      b = sign_ext (get_src (sc), w*8);
                      a = sign_ext (get_src (dc), w*8);
                      tprintf ("min %d %d\n", b, a);
                      if (b < a)
                        put_dest (dc, b);
                    
                    }
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  op_semantics_11:
                    {
                      /** 0000 0001 1sss ddd w dd ss 1101	MAX.size src,dest */
#line 1188 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x07;
#line 1188 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1188 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1188 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1188 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1sss ddd w dd ss 1101	MAX.size src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x,", dd);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 1188 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      w++;
                      sc = decode_src23 (sss, ss, w);
                      dc = decode_dest23 (ddd, dd, w);
                      b = sign_ext (get_src (sc), w*8);
                      a = sign_ext (get_src (dc), w*8);
                      tprintf ("max %d %d\n", b, a);
                      if (b > a)
                        put_dest (dc, b);
                    
                    }
                  break;
                case 0x0e:
                  op_semantics_12:
                    {
                      /** 0000 0001 1000 ddd w dd00 1110	DADC.size #IMM,dest */
#line 840 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 840 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 840 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd00 1110	DADC.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 840 "m32c/m32c.opc"
                    
                      DADDI(1,1);
                    
                    }
                  break;
                case 0x1e:
                  op_semantics_13:
                    {
                      /** 0000 0001 1000 ddd w dd01 1110	DADD.size #IMM,dest */
#line 848 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 848 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 848 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd01 1110	DADD.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 848 "m32c/m32c.opc"
                    
                      DADDI(1,0);
                    
                    }
                  break;
                case 0x2e:
                  op_semantics_14:
                    {
                      /** 0000 0001 1000 ddd w dd10 1110	ADC.size #IMM,dest */
#line 460 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 460 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 460 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd10 1110	ADC.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 460 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, w+1);
                      imm = IMM (w+1);
                      MATH_OP (dc, imm, carry, +);
                    
                    }
                  break;
                case 0x2f:
                  op_semantics_15:
                    {
                      /** 0000 0001 1000 ddd w dd10 1111	MIN.size #IMM,dest */
#line 1200 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1200 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1200 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd10 1111	MIN.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 1200 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      w++;
                      dc = decode_dest23 (ddd, dd, w);
                      imm = sign_ext (IMM(w), w*8);
                      a = sign_ext (get_src (dc), w*8);
                      tprintf ("min %d %d\n", imm, a);
                      if (imm < a)
                        put_dest (dc, imm);
                    
                    }
                  break;
                case 0x3e:
                  op_semantics_16:
                    {
                      /** 0000 0001 1000 ddd w dd11 1110	CLIP.size #IMM1,#IMM2,dest */
#line 759 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 759 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 759 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd11 1110	CLIP.size #IMM1,#IMM2,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 759 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, w+1);
                      a = sign_ext (IMM(w+1), w*8+8);
                      b = sign_ext (IMM(w+1), w*8+8);
                      v = sign_ext (get_src (dc), w*8+8);
                      tprintf("clip %d <= %d <= %d : ", a, v, b);
                      if (a > v)
                        v = a;
                      if (v > b)
                        v = b;
                      tprintf("%d\n", v);
                      put_dest (dc, v);
                    
                    }
                  break;
                case 0x3f:
                  op_semantics_17:
                    {
                      /** 0000 0001 1000 ddd w dd11 1111	MAX.size #IMM,dest */
#line 1177 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1177 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1177 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 ddd w dd11 1111	MAX.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 1177 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      w++;
                      dc = decode_dest23 (ddd, dd, w);
                      imm = sign_ext (IMM(w), w*8);
                      a = sign_ext (get_src (dc), w*8);
                      tprintf ("max %d %d\n", imm, a);
                      if (imm > a)
                        put_dest (dc, imm);
                    
                    }
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x81:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x0f:
                  op_semantics_18:
                    {
                      /** 0000 0001 1000 sss1 ss00 1111	MULU.L src,R2R0 */
#line 1476 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1476 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 sss1 ss00 1111	MULU.L src,R2R0 */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 1476 "m32c/m32c.opc"
                    
                      M32C_ONLY();
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 4);
                      a = get_src (sc);
                      b = get_reg (r2r0);
                      ll = (long long)a * (long long)b;
                      tprintf("%d * %d = %lld (%llx)\n", a, b, ll, ll);
                      if (ll < b2minsigned[4] || ll > b2maxsigned[4])
                        set_flags (FLAGBIT_O, FLAGBIT_O);
                      else
                        set_flags (FLAGBIT_O, 0);
                      put_reg (r2r0, (int)ll);
                    
                    }
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x1f:
                  op_semantics_19:
                    {
                      /** 0000 0001 1000 sss1 ss01 1111	MUL.L src,R2R0 */
#line 1425 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1425 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1000 sss1 ss01 1111	MUL.L src,R2R0 */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 1425 "m32c/m32c.opc"
                    
                      M32C_ONLY();
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 4);
                      a = sign_ext (get_src (sc), 32);
                      b = sign_ext (get_reg (r2r0), 32);
                      ll = (long long)a * (long long)b;
                      tprintf("%d * %d = %lld (%llx)\n", a, b, ll, ll);
                      if (ll < b2minsigned[4] || ll > b2maxsigned[4])
                        set_flags (FLAGBIT_O, FLAGBIT_O);
                      else
                        set_flags (FLAGBIT_O, 0);
                      put_reg (r2r0, (int)ll);
                    
                    }
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x82:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x83:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x0f:
                  goto op_semantics_18;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x1f:
                  goto op_semantics_19;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x84:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x85:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x0f:
                  goto op_semantics_18;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x1f:
                  goto op_semantics_19;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x86:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x87:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x0f:
                  goto op_semantics_18;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x1f:
                  goto op_semantics_19;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x88:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x89:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_12;
                  break;
                case 0x0f:
                  goto op_semantics_18;
                  break;
                case 0x1e:
                  goto op_semantics_13;
                  break;
                case 0x1f:
                  goto op_semantics_19;
                  break;
                case 0x2e:
                  goto op_semantics_14;
                  break;
                case 0x2f:
                  goto op_semantics_15;
                  break;
                case 0x3e:
                  goto op_semantics_16;
                  break;
                case 0x3f:
                  goto op_semantics_17;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x90:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  op_semantics_20:
                    {
                      /** 0000 0001 1001 ddd w dd00 1110	DSBB.size #IMM,dest */
#line 914 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 914 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 914 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1001 ddd w dd00 1110	DSBB.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 914 "m32c/m32c.opc"
                    
                      DADDI(0,1);
                    
                    }
                  break;
                case 0x1e:
                  op_semantics_21:
                    {
                      /** 0000 0001 1001 ddd w dd01 1110	DSUB.size #IMM,dest */
#line 922 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 922 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 922 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1001 ddd w dd01 1110	DSUB.size #IMM,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 922 "m32c/m32c.opc"
                    
                      DADDI(0,0);
                    
                    }
                  break;
                case 0x2e:
                  op_semantics_22:
                    {
                      /** 0000 0001 1001 ddd w dd10 1110	SBB.size #IMM, dest */
#line 1749 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1749 "m32c/m32c.opc"
                      int w ATTRIBUTE_UNUSED = op[1] & 0x01;
#line 1749 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1001 ddd w dd10 1110	SBB.size #IMM, dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  w = 0x%x,", w);
                          printf("  dd = 0x%x\n", dd);
                        }
#line 1749 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, w+1);
                      imm = IMM (w+1);
                      MATH_OP (dc, imm, !carry, -);
                    
                    }
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x91:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x92:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x93:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x94:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x95:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x96:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x97:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x98:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x07:
                case 0x17:
                case 0x27:
                case 0x37:
                  goto op_semantics_5;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                case 0x1b:
                case 0x2b:
                case 0x3b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0x99:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_20;
                  break;
                case 0x1e:
                  goto op_semantics_21;
                  break;
                case 0x2e:
                  goto op_semantics_22;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa0:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  op_semantics_23:
                    {
                      /** 0000 0001 1010 sss0 ss hl 1110	MOVdir src,R0L */
#line 1377 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1377 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1377 "m32c/m32c.opc"
                      int hl ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1010 sss0 ss hl 1110	MOVdir src,R0L */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  hl = 0x%x\n", hl);
                        }
#line 1377 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_dest23 (sss, ss, 1);
                      a = get_reg (r0l);
                      b = get_src (dc);
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
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa1:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0f:
                  op_semantics_24:
                    {
                      /** 0000 0001 1010 sss1 ss00 1111	DIVU.L src */
#line 892 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 892 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1010 sss1 ss00 1111	DIVU.L src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 892 "m32c/m32c.opc"
                    
                      M32C_ONLY();
                      prefix (0, 0, 0);
                      div_op (sss, ss, 1, 0, 4);
                    
                    }
                  break;
                case 0x1f:
                  op_semantics_25:
                    {
                      /** 0000 0001 1010 sss1 ss01 1111	DIV.L src */
#line 876 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 876 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1010 sss1 ss01 1111	DIV.L src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 876 "m32c/m32c.opc"
                    
                      M32C_ONLY();
                      prefix (0, 0, 0);
                      div_op (sss, ss, 0, 0, 4);
                    
                    }
                  break;
                case 0x2f:
                  op_semantics_26:
                    {
                      /** 0000 0001 1010 sss1 ss10 1111	DIVX.L src */
#line 908 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 908 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1010 sss1 ss10 1111	DIVX.L src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x\n", ss);
                        }
#line 908 "m32c/m32c.opc"
                    
                      M32C_ONLY();
                      prefix (0, 0, 0);
                      div_op (sss, ss, 0, 1, 4);
                    
                    }
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa2:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_23;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa3:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0f:
                  goto op_semantics_24;
                  break;
                case 0x1f:
                  goto op_semantics_25;
                  break;
                case 0x2f:
                  goto op_semantics_26;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa4:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_23;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa5:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0f:
                  goto op_semantics_24;
                  break;
                case 0x1f:
                  goto op_semantics_25;
                  break;
                case 0x2f:
                  goto op_semantics_26;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa6:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_23;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa7:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0f:
                  goto op_semantics_24;
                  break;
                case 0x1f:
                  goto op_semantics_25;
                  break;
                case 0x2f:
                  goto op_semantics_26;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa8:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_23;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xa9:
              GETBYTE();
              switch (op[2] & 0x3f) {
                case 0x00:
                case 0x10:
                case 0x20:
                case 0x30:
                  goto op_semantics_1;
                  break;
                case 0x02:
                case 0x12:
                case 0x22:
                case 0x32:
                  goto op_semantics_2;
                  break;
                case 0x04:
                case 0x14:
                case 0x24:
                case 0x34:
                  goto op_semantics_3;
                  break;
                case 0x06:
                case 0x16:
                case 0x26:
                case 0x36:
                  goto op_semantics_4;
                  break;
                case 0x08:
                case 0x18:
                case 0x28:
                case 0x38:
                  goto op_semantics_6;
                  break;
                case 0x09:
                case 0x19:
                case 0x29:
                case 0x39:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                case 0x1a:
                case 0x2a:
                case 0x3a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                case 0x1c:
                case 0x2c:
                case 0x3c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                case 0x1d:
                case 0x2d:
                case 0x3d:
                  goto op_semantics_11;
                  break;
                case 0x0f:
                  goto op_semantics_24;
                  break;
                case 0x1f:
                  goto op_semantics_25;
                  break;
                case 0x2f:
                  goto op_semantics_26;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb0:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  op_semantics_27:
                    {
                      /** 0000 0001 1011 ddd0 dd hl 1110	MOVdir R0L,dest */
#line 1362 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1362 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1362 "m32c/m32c.opc"
                      int hl ATTRIBUTE_UNUSED = (op[2] >> 4) & 0x03;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1011 ddd0 dd hl 1110	MOVdir R0L,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  dd = 0x%x,", dd);
                          printf("  hl = 0x%x\n", hl);
                        }
#line 1362 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, 1);
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
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb1:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb2:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_27;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb3:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb4:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_27;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb5:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb6:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_27;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb7:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb8:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                case 0x0e:
                  goto op_semantics_27;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xb9:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc0:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc1:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc2:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc3:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc4:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc5:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc6:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc7:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc8:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x07:
                  goto op_semantics_5;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0b:
                  goto op_semantics_9;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xc9:
              GETBYTE();
              switch (op[2] & 0x0f) {
                case 0x00:
                  goto op_semantics_1;
                  break;
                case 0x02:
                  goto op_semantics_2;
                  break;
                case 0x04:
                  goto op_semantics_3;
                  break;
                case 0x06:
                  goto op_semantics_4;
                  break;
                case 0x08:
                  goto op_semantics_6;
                  break;
                case 0x09:
                  goto op_semantics_7;
                  break;
                case 0x0a:
                  goto op_semantics_8;
                  break;
                case 0x0c:
                  goto op_semantics_10;
                  break;
                case 0x0d:
                  goto op_semantics_11;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd0:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  op_semantics_28:
                    {
                      /** 0000 0001 1101 sss0 ss00 0bit	BNTST src */
#line 660 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 660 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 660 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss00 0bit	BNTST src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 660 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_dest23 (sss, ss, 1);
                      b = get_bit2 (sc, bit);
                      set_zc (!b, !b);
                    
                    }
                  break;
                case 0x08:
                  op_semantics_29:
                    {
                      /** 0000 0001 1101 sss0 ss00 1bit	BAND src */
#line 607 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 607 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 607 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss00 1bit	BAND src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 607 "m32c/m32c.opc"
                    
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, b & carry);
                    
                    }
                  break;
                case 0x18:
                  op_semantics_30:
                    {
                      /** 0000 0001 1101 sss0 ss01 1bit	BNAND src */
#line 642 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 642 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 642 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss01 1bit	BNAND src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 642 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, (!b) & carry);
                    
                    }
                  break;
                case 0x20:
                  op_semantics_31:
                    {
                      /** 0000 0001 1101 sss0 ss10 0bit	BOR src */
#line 673 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 673 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 673 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss10 0bit	BOR src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 673 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, b | carry);
                    
                    }
                  break;
                case 0x28:
                  op_semantics_32:
                    {
                      /** 0000 0001 1101 sss0 ss10 1bit	BXOR src */
#line 753 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 753 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 753 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss10 1bit	BXOR src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 753 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, b ^ carry);
                    
                    }
                  break;
                case 0x30:
                  op_semantics_33:
                    {
                      /** 0000 0001 1101 sss0 ss11 0bit	BNOR src */
#line 648 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 648 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 648 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss11 0bit	BNOR src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 648 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, (!b) | carry);
                    
                    }
                  break;
                case 0x38:
                  op_semantics_34:
                    {
                      /** 0000 0001 1101 sss0 ss11 1bit	BNXOR src */
#line 667 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 667 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 667 "m32c/m32c.opc"
                      int bit ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss0 ss11 1bit	BNXOR src */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  bit = 0x%x\n", bit);
                        }
#line 667 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 1);
                      BIT_OPC (sc, bit, !b ^ carry);
                    
                    }
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd1:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  op_semantics_35:
                    {
                      /** 0000 0001 1101 sss1 ss00 0dst	LDC src,dest */
#line 1161 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1161 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1161 "m32c/m32c.opc"
                      int dst ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss1 ss00 0dst	LDC src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  dst = 0x%x\n", dst);
                        }
#line 1161 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 3);
                      dc = decode_cr_b (dst, CR_B_DMA0);
                      a = get_src (sc);
                      put_dest (dc, a);
                    
                    }
                  break;
                case 0x08:
                  op_semantics_36:
                    {
                      /** 0000 0001 1101 sss1 ss00 1dst	LDC src,dest */
#line 1145 "m32c/m32c.opc"
                      int sss ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1145 "m32c/m32c.opc"
                      int ss ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1145 "m32c/m32c.opc"
                      int dst ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 sss1 ss00 1dst	LDC src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  sss = 0x%x,", sss);
                          printf("  ss = 0x%x,", ss);
                          printf("  dst = 0x%x\n", dst);
                        }
#line 1145 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      sc = decode_src23 (sss, ss, 2);
                      dc = decode_cr_b (dst, CR_B_DCT0);
                      a = get_src (sc);
                      put_dest (dc, a);
                    
                    }
                  break;
                case 0x10:
                  op_semantics_37:
                    {
                      /** 0000 0001 1101 ddd1 dd01 0src	STC src,dest */
#line 1980 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1980 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1980 "m32c/m32c.opc"
                      int src ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 ddd1 dd01 0src	STC src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  dd = 0x%x,", dd);
                          printf("  src = 0x%x\n", src);
                        }
#line 1980 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, 4);
                      sc = decode_cr_b (src, CR_B_DMA0);
                      a = get_src (sc);
                      put_dest (dc, a);
                    
                    }
                  break;
                case 0x18:
                  op_semantics_38:
                    {
                      /** 0000 0001 1101 ddd1 dd01 1src	STC src,dest */
#line 1988 "m32c/m32c.opc"
                      int ddd ATTRIBUTE_UNUSED = (op[1] >> 1) & 0x07;
#line 1988 "m32c/m32c.opc"
                      int dd ATTRIBUTE_UNUSED = (op[2] >> 6) & 0x03;
#line 1988 "m32c/m32c.opc"
                      int src ATTRIBUTE_UNUSED = op[2] & 0x07;
                      if (trace) {
                          printf("\033[33m%s\033[0m  %02x %02x %02x\n",
                                 "/** 0000 0001 1101 ddd1 dd01 1src	STC src,dest */",
                                 op[0], op[1], op[2]);
                          printf("  ddd = 0x%x,", ddd);
                          printf("  dd = 0x%x,", dd);
                          printf("  src = 0x%x\n", src);
                        }
#line 1988 "m32c/m32c.opc"
                    
                      prefix (0, 0, 0);
                      dc = decode_dest23 (ddd, dd, 2);
                      sc = decode_cr_b (src, CR_B_DCT0);
                      a = get_src (sc);
                      put_dest (dc, a);
                    
                    }
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd2:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_28;
                  break;
                case 0x08:
                  goto op_semantics_29;
                  break;
                case 0x18:
                  goto op_semantics_30;
                  break;
                case 0x20:
                  goto op_semantics_31;
                  break;
                case 0x28:
                  goto op_semantics_32;
                  break;
                case 0x30:
                  goto op_semantics_33;
                  break;
                case 0x38:
                  goto op_semantics_34;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd3:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_35;
                  break;
                case 0x08:
                  goto op_semantics_36;
                  break;
                case 0x10:
                  goto op_semantics_37;
                  break;
                case 0x18:
                  goto op_semantics_38;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd4:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_28;
                  break;
                case 0x08:
                  goto op_semantics_29;
                  break;
                case 0x18:
                  goto op_semantics_30;
                  break;
                case 0x20:
                  goto op_semantics_31;
                  break;
                case 0x28:
                  goto op_semantics_32;
                  break;
                case 0x30:
                  goto op_semantics_33;
                  break;
                case 0x38:
                  goto op_semantics_34;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd5:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_35;
                  break;
                case 0x08:
                  goto op_semantics_36;
                  break;
                case 0x10:
                  goto op_semantics_37;
                  break;
                case 0x18:
                  goto op_semantics_38;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd6:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_28;
                  break;
                case 0x08:
                  goto op_semantics_29;
                  break;
                case 0x18:
                  goto op_semantics_30;
                  break;
                case 0x20:
                  goto op_semantics_31;
                  break;
                case 0x28:
                  goto op_semantics_32;
                  break;
                case 0x30:
                  goto op_semantics_33;
                  break;
                case 0x38:
                  goto op_semantics_34;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd7:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_35;
                  break;
                case 0x08:
                  goto op_semantics_36;
                  break;
                case 0x10:
                  goto op_semantics_37;
                  break;
                case 0x18:
                  goto op_semantics_38;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd8:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_28;
                  break;
                case 0x08:
                  goto op_semantics_29;
                  break;
                case 0x18:
                  goto op_semantics_30;
                  break;
                case 0x20:
                  goto op_semantics_31;
                  break;
                case 0x28:
                  goto op_semantics_32;
                  break;
                case 0x30:
                  goto op_semantics_33;
                  break;
                case 0x38:
                  goto op_semantics_34;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          case 0xd9:
              GETBYTE();
              switch (op[2] & 0x38) {
                case 0x00:
                  goto op_semantics_35;
                  break;
                case 0x08:
                  goto op_semantics_36;
                  break;
                case 0x10:
                  goto op_semantics_37;
                  break;
                case 0x18:
                  goto op_semantics_38;
                  break;
                default: UNSUPPORTED(); break;
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x02:
    case 0x03:
    case 0x12:
    case 0x13:
    case 0x22:
    case 0x23:
    case 0x32:
    case 0x33:
        {
          /** 00dd 001w				MOV.size:Z #0,dest */
#line 1264 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1264 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00dd 001w				MOV.size:Z #0,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 1264 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2 (dd, w+1);
          put_dest (dc, 0);
          set_sz (0, w+1);
        
        }
      break;
    case 0x04:
    case 0x05:
    case 0x14:
    case 0x15:
    case 0x24:
    case 0x25:
    case 0x34:
    case 0x35:
        {
          /** 00dd 010w				MOV.size:S #IMM,dest */
#line 1250 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1250 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00dd 010w				MOV.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 1250 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2 (dd, w+1);
          imm = IMM(w+1);
          put_dest (dc, imm);
          set_sz (imm, w+1);
        
        }
      break;
    case 0x06:
    case 0x07:
    case 0x16:
    case 0x17:
    case 0x26:
    case 0x27:
    case 0x36:
    case 0x37:
        {
          /** 00dd 011w				ADD.size:S #IMM,dest */
#line 502 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 502 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00dd 011w				ADD.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 502 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2(dd, w+1);
          imm = IMM (w+1);
          MATH_OP (dc, imm, 0, +);
        
        }
      break;
    case 0x08:
        {
          /** 0000 1000				BRK2 */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 1000				BRK2 */",
                     op[0]);
            }
#line 708 "m32c/m32c.opc"
        
          if (verbose)
            printf("[break2]\n");
          if (in_gdb)
            return M32C_MAKE_HIT_BREAK ();
          if (mem_get_qi (0xFFFFE7) == 0xff)
            trigger_based_interrupt (0);
          else
            trigger_fixed_interrupt (0xFFFFE4);
        
        }
      break;
    case 0x09:
        {
          /** 0000 1001				indirect dest */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0000 1001				indirect dest */",
                     op[0]);
            }
#line 435 "m32c/m32c.opc"
        
          decode_indirect (0, 1);
          goto next_opcode;
        
        }
      break;
    case 0x0a:
    case 0x0b:
    case 0x1a:
    case 0x1b:
    case 0x2a:
    case 0x2b:
    case 0x3a:
    case 0x3b:
      GETBYTE();
        {
          /** 00bb 101bit			BTST:S src */
#line 731 "m32c/m32c.opc"
          int bb ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 731 "m32c/m32c.opc"
          int bit ATTRIBUTE_UNUSED = (op[0] >> 6) & 0x07;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x %02x\n",
                     "/** 00bb 101bit			BTST:S src */",
                     op[0], op[1]);
              printf("  bb = 0x%x,", bb);
              printf("  bit = 0x%x\n", bit);
            }
#line 731 "m32c/m32c.opc"
        
          sc = decode_src23 (3, 3, 1); /* bit,base:19 */
          bit = get_bit2 (sc, bb*2 + bit);
          set_zc (!bit, bit);
        
        }
      break;
    case 0x0c:
    case 0x0d:
    case 0x1c:
    case 0x1d:
    case 0x2c:
    case 0x2d:
    case 0x3c:
    case 0x3d:
        {
          /** 00dd 110w				TST.size:S #IMM,dest */
#line 2096 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 2096 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00dd 110w				TST.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 2096 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          dc = decode_dest2 (dd, w+1);
          imm = IMM(w+1);
          a = get_src (dc);
          v = a & imm;
          set_sz (v, w+1);
        
        }
      break;
    case 0x0e:
    case 0x0f:
    case 0x1e:
    case 0x1f:
    case 0x2e:
    case 0x2f:
    case 0x3e:
    case 0x3f:
        {
          /** 00dd 111w				SUB.size:S #IMM,dest */
#line 2049 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 2049 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00dd 111w				SUB.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 2049 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2(dd, w+1);
          imm = IMM (w+1);
          MATH_OP (dc, imm, 0, -);
        
        }
      break;
    case 0x10:
    case 0x11:
    case 0x20:
    case 0x21:
    case 0x30:
    case 0x31:
        {
          /** 00DD 000w				MOV.size:S R0L/R0,dest */
#line 1307 "m32c/m32c.opc"
          int DD ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1307 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00DD 000w				MOV.size:S R0L/R0,dest */",
                     op[0]);
              printf("  DD = 0x%x,", DD);
              printf("  w = 0x%x\n", w);
            }
#line 1307 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2 (DD, w+1);
          v = get_reg (w ? r0 : r0l);
          put_dest (dc, v);
          set_sz (v, w+1);
        
        }
      break;
    case 0x18:
    case 0x19:
    case 0x28:
    case 0x29:
    case 0x38:
    case 0x39:
        {
          /** 00SS 100w				MOV.size:S src,R0L/R0 */
#line 1290 "m32c/m32c.opc"
          int SS ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1290 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 00SS 100w				MOV.size:S src,R0L/R0 */",
                     op[0]);
              printf("  SS = 0x%x,", SS);
              printf("  w = 0x%x\n", w);
            }
#line 1290 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          sc = decode_dest2 (SS, w+1);
          v = get_src (sc);
          put_reg (w ? r0 : r0l, v);
          set_sz (v, w+1);
        
        }
      break;
    case 0x40:
    case 0x50:
    case 0x51:
    case 0x60:
    case 0x61:
    case 0x70:
    case 0x71:
        {
          /** 01dd 000w				CMP.size:S src,R0/R0L */
#line 824 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 824 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01dd 000w				CMP.size:S src,R0/R0L */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 824 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2 (dd, w+1);
          a = get_reg (w ? r0 : r0l);
          b = get_src (dc);
          cmp (a, b, w+1);
        
        }
      break;
    case 0x41:
        {
          /** 0100 0001				indirect src */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0100 0001				indirect src */",
                     op[0]);
            }
#line 440 "m32c/m32c.opc"
        
          decode_indirect (1, 0);
          goto next_opcode;
        
        }
      break;
    case 0x42:
    case 0x43:
    case 0x52:
    case 0x53:
    case 0x62:
    case 0x63:
    case 0x72:
    case 0x73:
        {
          /** 01ii 001i				ADD.L:Q #IMM3,SP */
#line 539 "m32c/m32c.opc"
          int ii ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 539 "m32c/m32c.opc"
          int i ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01ii 001i				ADD.L:Q #IMM3,SP */",
                     op[0]);
              printf("  ii = 0x%x,", ii);
              printf("  i = 0x%x\n", i);
            }
#line 539 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          dc = reg_sd (sp);
          b = ii * 2 + i + 1;
          MATH_OP (dc, b, 0, +);
        
        }
      break;
    case 0x44:
    case 0x45:
    case 0x54:
    case 0x55:
    case 0x64:
    case 0x65:
    case 0x74:
    case 0x75:
        {
          /** 01dd 010w				OR.size:S #IMM,dest */
#line 1522 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1522 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01dd 010w				OR.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 1522 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2(dd, w+1);
          imm = IMM (w+1);
          LOGIC_OP (dc, imm, |);
        
        }
      break;
    case 0x46:
    case 0x47:
    case 0x56:
    case 0x57:
    case 0x66:
    case 0x67:
    case 0x76:
    case 0x77:
        {
          /** 01dd 011w				CMP.size:S #IMM,dest */
#line 798 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 798 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01dd 011w				CMP.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 798 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2 (dd, w+1);
          v = get_src (dc);
          imm = sign_ext (IMM(w+1),w*8+8);
          cmp (v, imm, w+1);
        
        }
      break;
    case 0x48:
    case 0x58:
    case 0x59:
    case 0x68:
    case 0x69:
    case 0x78:
    case 0x79:
        {
          /** 01ss 100d				MOV.L:S src,A0/A1 */
#line 1315 "m32c/m32c.opc"
          int ss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1315 "m32c/m32c.opc"
          int d ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01ss 100d				MOV.L:S src,A0/A1 */",
                     op[0]);
              printf("  ss = 0x%x,", ss);
              printf("  d = 0x%x\n", d);
            }
#line 1315 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          sc = decode_dest2 (ss, 4);
          v = get_src (sc);
          put_reg (d ? a1 : a0, v);
          set_sz (v, 4);
        
        }
      break;
    case 0x49:
        {
          /** 0100 1001				indirect src and dest */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 0100 1001				indirect src and dest */",
                     op[0]);
            }
#line 445 "m32c/m32c.opc"
        
          decode_indirect (1, 1);
          goto next_opcode;
        
        }
      break;
    case 0x4a:
    case 0x4b:
    case 0x5a:
    case 0x5b:
    case 0x6a:
    case 0x6b:
    case 0x7a:
    case 0x7b:
        {
          /** 01dd 101d				JMP.S label */
#line 1032 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1032 "m32c/m32c.opc"
          int d ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01dd 101d				JMP.S label */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  d = 0x%x\n", d);
            }
#line 1032 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          put_reg (pc, m32c_opcode_pc + (dd*2+d) + 2);
        
        }
      break;
    case 0x4c:
    case 0x4d:
    case 0x5c:
    case 0x5d:
    case 0x6c:
    case 0x6d:
    case 0x7c:
    case 0x7d:
        {
          /** 01dd 110w				AND.size:S #IMM,dest */
#line 592 "m32c/m32c.opc"
          int dd ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 592 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01dd 110w				AND.size:S #IMM,dest */",
                     op[0]);
              printf("  dd = 0x%x,", dd);
              printf("  w = 0x%x\n", w);
            }
#line 592 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          dc = decode_dest2(dd, w+1);
          imm = IMM (w+1);
          LOGIC_OP (dc, imm, &);
        
        }
      break;
    case 0x4e:
    case 0x4f:
    case 0x5e:
    case 0x5f:
    case 0x6e:
    case 0x6f:
    case 0x7e:
    case 0x7f:
        {
          /** 01ss 111w				MOV.size:S src,R1L/R1 */
#line 1298 "m32c/m32c.opc"
          int ss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x03;
#line 1298 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 01ss 111w				MOV.size:S src,R1L/R1 */",
                     op[0]);
              printf("  ss = 0x%x,", ss);
              printf("  w = 0x%x\n", w);
            }
#line 1298 "m32c/m32c.opc"
        
          prefix (0, 1, 0);
          sc = decode_dest2 (ss, w+1);
          v = get_src (sc);
          put_reg (w ? r1 : r1l, v);
          set_sz (v, w+1);
        
        }
      break;
    case 0x80:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            op_semantics_39:
              {
                /** 1sss ddd0 dd ss 0000		SUBX src,dest */
#line 2079 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 2079 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2079 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 2079 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd0 dd ss 0000		SUBX src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 2079 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, 1);
                dc = decode_dest23(ddd, dd, 4);
                b = sign_ext (get_src (sc), 8);
                MATH_OP (dc, b, 0, -);
              
              }
            break;
          case 0x01:
            op_semantics_40:
              {
                /** 1000 sss0 ss00 0001		JMPI.A src */
#line 1069 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1069 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 sss0 ss00 0001		JMPI.A src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1069 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 3);
                a = get_src (sc);
                put_reg (pc, a);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            op_semantics_41:
              {
                /** 1sss ddd0 dd ss 0010		ADDX src,dest */
#line 560 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 560 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 560 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 560 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd0 dd ss 0010		ADDX src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 560 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, 1);
                dc = decode_dest23(ddd, dd, 4);
                b = sign_ext (get_src (sc), 8);
                MATH_OP (dc, b, 0, +);
              
              }
            break;
          case 0x03:
          case 0x13:
            op_semantics_42:
              {
                /** 1000 sss0 ss0w 0011		INDEXB.size src */
#line 995 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 995 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 995 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 sss0 ss0w 0011		INDEXB.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 995 "m32c/m32c.opc"
                INDEXOP(1, 1, 1);
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            op_semantics_43:
              {
                /** 1sss ddd w dd ss 0100		MULU.size src,dest */
#line 1463 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1463 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1463 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1463 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1463 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 0100		MULU.size src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1463 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                w ++;
                sc = decode_src23 (sss, ss, w);
                dc = decode_dest23 (ddd, dd, w);
                a = get_src (sc);
                b = get_src (dc);
                tprintf("%d * %d = %d\n", a, b, a*b);
                v = a * b;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            op_semantics_44:
              {
                /** 1sss ddd w dd ss 0101		OR.size:G src,dest */
#line 1529 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1529 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1529 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1529 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1529 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 0101		OR.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1529 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, w+1);
                dc = decode_dest23(ddd, dd, w+1);
                b = get_src (sc);
                LOGIC_OP (dc, b, |);
              
              }
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            op_semantics_45:
              {
                /** 1sss ddd w dd ss 0110		CMP.size:G src,dest */
#line 806 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 806 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 806 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 806 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 806 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 0110		CMP.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 806 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23 (sss, ss, w+1);
                dc = decode_dest23 (ddd, dd, w+1);
                a = get_src (dc);
                b = get_src (sc);
                cmp (a, b, w+1);
              
              }
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            op_semantics_46:
              {
                /** 1sss ddd w dd ss 1000		ADD.size:G src,dest */
#line 516 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 516 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 516 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 516 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 516 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1000		ADD.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 516 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, w+1);
                dc = decode_dest23(ddd, dd, w+1);
                b = get_src (sc);
                MATH_OP (dc, b, 0, +);
              
              }
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            op_semantics_47:
              {
                /** 1sss ddd w dd ss 1001		XOR.size src,dest */
#line 2139 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 2139 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2139 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2139 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 2139 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1001		XOR.size src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 2139 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, w+1);
                dc = decode_dest23(ddd, dd, w+1);
                b = get_src (sc);
                LOGIC_OP (dc, b, ^);
              
              }
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            op_semantics_48:
              {
                /** 1sss ddd w dd ss 1010		SUB.size:G src,dest */
#line 2056 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 2056 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2056 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2056 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 2056 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1010		SUB.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 2056 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, w+1);
                dc = decode_dest23(ddd, dd, w+1);
                b = get_src (sc);
                MATH_OP (dc, b, 0, -);
              
              }
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            op_semantics_49:
              {
                /** 1sss ddd w dd ss 1011		MOV.size:G src,dest */
#line 1271 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1271 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1271 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1271 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1271 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1011		MOV.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1271 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23 (sss, ss, w+1);
                dc = decode_dest23 (ddd, dd, w+1);
                v = get_src (sc);
                put_dest (dc, v);
                set_sz (v, w+1);
              
              }
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            op_semantics_50:
              {
                /** 1sss ddd w dd ss 1100		MUL.size src,dest */
#line 1412 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1412 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1412 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1412 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1412 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1100		MUL.size src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1412 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                w ++;
                sc = decode_src23 (sss, ss, w);
                dc = decode_dest23 (ddd, dd, w);
                a = sign_ext (get_src (sc), w*8);
                b = sign_ext (get_src (dc), w*8);
                tprintf("%d * %d = %d\n", a, b, a*b);
                v = a * b;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            op_semantics_51:
              {
                /** 1sss ddd w dd ss 1101		AND.size:G src,dest */
#line 599 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 599 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 599 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 599 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 599 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd w dd ss 1101		AND.size:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 599 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, w+1);
                dc = decode_dest23(ddd, dd, w+1);
                b = get_src (sc);
                LOGIC_OP (dc, b, &);
              
              }
            break;
          case 0x0e:
            op_semantics_52:
              {
                /** 1000 sss w ss00 1110		DIVU.size src */
#line 887 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 887 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 887 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 sss w ss00 1110		DIVU.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  w = 0x%x,", w);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 887 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                div_op (sss, ss, 1, 0, w+1);
              
              }
            break;
          case 0x0f:
            op_semantics_53:
              {
                /** 1000 ddd w dd00 1111		MULU.size #IMM,dest */
#line 1451 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1451 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1451 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd00 1111		MULU.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1451 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                w ++;
                dc = decode_dest23 (ddd, dd, w);
                v = get_src (dc);
                imm = IMM(w);
                tprintf("%d * %d = %d\n", v, imm, v*imm);
                v *= imm;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x11:
            op_semantics_54:
              {
                /** 1000 ddd0 dd01 0001		ADDX #IMM,dest */
#line 553 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 553 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd0 dd01 0001		ADDX #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 553 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                MATH_OP (dc, imm, 0, +);
              
              }
            break;
          case 0x1e:
            op_semantics_55:
              {
                /** 1000 sss w ss01 1110		DIV.size src */
#line 871 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 871 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 871 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 sss w ss01 1110		DIV.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  w = 0x%x,", w);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 871 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                div_op (sss, ss, 0, 0, w+1);
              
              }
            break;
          case 0x1f:
            op_semantics_56:
              {
                /** 1000 ddd w dd01 1111		MUL.size #IMM,dest */
#line 1400 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1400 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1400 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd01 1111		MUL.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1400 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                w ++;
                dc = decode_dest23 (ddd, dd, w);
                v = sign_ext (get_src (dc), w*8);
                imm = sign_ext (IMM(w), w*8);
                tprintf("%d * %d = %d\n", v, imm, v*imm);
                v *= imm;
                dc = widen_sd (dc);
                put_dest (dc, v);
              
              }
            break;
          case 0x21:
            op_semantics_57:
              {
                /** 1000 ddd0 dd10 0001		SHLNC.L #IMM,dest */
#line 1862 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1862 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd0 dd10 0001		SHLNC.L #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1862 "m32c/m32c.opc"
              
                M32C_ONLY();
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                shift_op (dc, 0, imm, 0);
              
              }
            break;
          case 0x23:
          case 0x33:
            op_semantics_58:
              {
                /** 1000 sss0 ss1w 0011		INDEXW.size src */
#line 1007 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1007 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1007 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 sss0 ss1w 0011		INDEXW.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1007 "m32c/m32c.opc"
                INDEXOP(2, 1, 1);
              }
            break;
          case 0x2e:
            op_semantics_59:
              {
                /** 1000 ddd w dd10 1110		ADD.size:G #IMM,dest */
#line 481 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 481 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 481 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd10 1110		ADD.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 481 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, w+1);
                imm = IMM(w+1);
                MATH_OP (dc, imm, 0, +);
              
              }
            break;
          case 0x2f:
            op_semantics_60:
              {
                /** 1000 ddd w dd10 1111		OR.size:G #IMM,dest */
#line 1515 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1515 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1515 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd10 1111		OR.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1515 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, w+1);
                imm = IMM(w+1);
                LOGIC_OP (dc, imm, |);
              
              }
            break;
          case 0x31:
            op_semantics_61:
              {
                /** 1000 ddd0 dd11 0001		ADD.L:G #IMM,dest */
#line 488 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 488 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd0 dd11 0001		ADD.L:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 488 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, 4);
                imm = IMM(4);
                MATH_OP (dc, imm, 0, +);
              
              }
            break;
          case 0x3e:
            op_semantics_62:
              {
                /** 1000 ddd w dd11 1110		SUB.size:G #IMM,dest */
#line 2035 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2035 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2035 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd11 1110		SUB.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2035 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, w+1);
                imm = IMM(w+1);
                MATH_OP (dc, imm, 0, -);
              
              }
            break;
          case 0x3f:
            op_semantics_63:
              {
                /** 1000 ddd w dd11 1111		AND.size:G #IMM,dest */
#line 585 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 585 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 585 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1000 ddd w dd11 1111		AND.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 585 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, w+1);
                imm = IMM(w+1);
                LOGIC_OP (dc, imm, &);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x81:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            op_semantics_64:
              {
                /** 1sss ddd1 dd ss 0000		SUB.L:G src,dest */
#line 2064 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 2064 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2064 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 2064 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd1 dd ss 0000		SUB.L:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 2064 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, 4);
                dc = decode_dest23(ddd, dd, 4);
                b = get_src (sc);
                MATH_OP (dc, b, 0, -);
              
              }
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            op_semantics_65:
              {
                /** 1sss ddd1 dd ss 0001		CMP.L:G src,dest */
#line 815 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 815 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 815 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 815 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd1 dd ss 0001		CMP.L:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 815 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23 (sss, ss, 4);
                dc = decode_dest23 (ddd, dd, 4);
                a = get_src (dc);
                b = get_src (sc);
                cmp (a, b, 4);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            op_semantics_66:
              {
                /** 1sss ddd1 dd ss 0010		ADD.L:G src,dest */
#line 524 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 524 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 524 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 524 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd1 dd ss 0010		ADD.L:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 524 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23(sss, ss, 4);
                dc = decode_dest23(ddd, dd, 4);
                b = get_src (sc);
                MATH_OP (dc, b, 0, +);
              
              }
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            op_semantics_67:
              {
                /** 1sss ddd1 dd ss 0011		MOV.L:G src,dest */
#line 1280 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1280 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1280 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1280 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1sss ddd1 dd ss 0011		MOV.L:G src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1280 "m32c/m32c.opc"
              
                prefix (1, 1, 0);
                sc = decode_src23 (sss, ss, 4);
                dc = decode_dest23 (ddd, dd, 4);
                v = get_src (sc);
                put_dest (dc, v);
                set_sz (v, 4);
              
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x82:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_40;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_42;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x11:
            goto op_semantics_54;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x21:
            goto op_semantics_57;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_58;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x31:
            goto op_semantics_61;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x83:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x84:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_40;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_42;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x11:
            goto op_semantics_54;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x21:
            goto op_semantics_57;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_58;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x31:
            goto op_semantics_61;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x85:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x86:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_40;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_42;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x11:
            goto op_semantics_54;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x21:
            goto op_semantics_57;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_58;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x31:
            goto op_semantics_61;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x87:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x88:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_40;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_42;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x11:
            goto op_semantics_54;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x21:
            goto op_semantics_57;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_58;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x31:
            goto op_semantics_61;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x89:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_52;
            break;
          case 0x0f:
            goto op_semantics_53;
            break;
          case 0x1e:
            goto op_semantics_55;
            break;
          case 0x1f:
            goto op_semantics_56;
            break;
          case 0x2e:
            goto op_semantics_59;
            break;
          case 0x2f:
            goto op_semantics_60;
            break;
          case 0x3e:
            goto op_semantics_62;
            break;
          case 0x3f:
            goto op_semantics_63;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x8a:
    case 0x8b:
    case 0x9a:
    case 0x9b:
    case 0xaa:
    case 0xab:
    case 0xba:
    case 0xca:
    case 0xcb:
    case 0xda:
    case 0xdb:
    case 0xea:
    case 0xeb:
    case 0xfa:
    case 0xfb:
        {
          /** 1ccc 101c				Jcnd label */
#line 1025 "m32c/m32c.opc"
          int ccc ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x07;
#line 1025 "m32c/m32c.opc"
          int c ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1ccc 101c				Jcnd label */",
                     op[0]);
              printf("  ccc = 0x%x,", ccc);
              printf("  c = 0x%x\n", c);
            }
#line 1025 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          v = sign_ext (IMM(1), 8);
          if (condition_true (ccc*2+c))
            put_reg (pc, m32c_opcode_pc + 1 + v);
        
        }
      break;
    case 0x8c:
    case 0x8d:
    case 0xac:
    case 0xad:
        {
          /** 10i0 110d				ADD.L:S #IMM,A0/A1 */
#line 509 "m32c/m32c.opc"
          int i ATTRIBUTE_UNUSED = (op[0] >> 5) & 0x01;
#line 509 "m32c/m32c.opc"
          int d ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 10i0 110d				ADD.L:S #IMM,A0/A1 */",
                     op[0]);
              printf("  i = 0x%x,", i);
              printf("  d = 0x%x\n", d);
            }
#line 509 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          dc = reg_sd (d ? a1 : a0);
          imm = i ? 2 : 1;
          MATH_OP (dc, imm, 0, +);
        
        }
      break;
    case 0x8e:
        {
          /** 1000 1110				POPM dest */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1000 1110				POPM dest */",
                     op[0]);
            }
#line 1567 "m32c/m32c.opc"
        
          {
          static int map[] = { r0, r1, r2, r3, a0, a1, sb, fb };
          prefix (0, 0, 0);
          imm = IMM(1);
          tprintf("popm: %x\n", imm);
          for (a=0; a<4; a++)
            if (imm & (1<<a))
              {
        	v = mem_get_hi (get_reg (sp));
        	put_reg (map[a], v);
        	put_reg (sp, get_reg (sp) + 2);
              }
          for (; a<8; a++)
            if (imm & (1<<a))
              {
        	v = mem_get_si (get_reg (sp));
        	put_reg (map[a], v);
        	put_reg (sp, get_reg (sp) + 4);
              }
          }
        
        }
      break;
    case 0x8f:
        {
          /** 1000 1111				PUSHM src */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1000 1111				PUSHM src */",
                     op[0]);
            }
#line 1654 "m32c/m32c.opc"
        
          {
          static int map[] = { fb, sb, a1, a0, r3, r2, r1, r0 };
          imm = IMM(1);
          tprintf("pushm: %x\n", imm);
          for (a=0; a<4; a++)
            if (imm & (1<<a))
              {
        	put_reg (sp, get_reg (sp) - 4);
        	v = get_reg (map[a]);
        	mem_put_si (get_reg (sp), v);
              }
          for (; a<8; a++)
            if (imm & (1<<a))
              {
        	put_reg (sp, get_reg (sp) - 2);
        	v = get_reg (map[a]);
        	mem_put_hi (get_reg (sp), v);
              }
          }
        
        }
      break;
    case 0x90:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            op_semantics_68:
              {
                /** 1001 sss0 ss00 0001		JSRI.A src */
#line 1109 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1109 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 sss0 ss00 0001		JSRI.A src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1109 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 3);
                a = get_src (sc);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_si (get_reg (sp), get_reg (pc));
                put_reg (pc, a);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            op_semantics_69:
              {
                /** 1001 sss0 ss0w 0011		INDEXLS.size src */
#line 1005 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1005 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1005 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 sss0 ss0w 0011		INDEXLS.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1005 "m32c/m32c.opc"
                INDEXOP(4, 1, 0);
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            op_semantics_70:
              {
                /** 1001 ddd w dd00 1110		XOR.size #IMM,dest */
#line 2132 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2132 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2132 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd00 1110		XOR.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2132 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, w+1);
                imm = IMM(w+1);
                LOGIC_OP (dc, imm, ^);
              
              }
            break;
          case 0x0f:
            op_semantics_71:
              {
                /** 1001 ddd w dd00 1111		STZ.size #IMM,dest */
#line 2016 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2016 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2016 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd00 1111		STZ.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2016 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                imm = IMM(w+1);
                if (FLAG_Z)
                  put_dest (dc, imm);
              
              }
            break;
          case 0x11:
            op_semantics_72:
              {
                /** 1001 ddd0 dd01 0001		SUBX #IMM,dest */
#line 2072 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2072 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd0 dd01 0001		SUBX #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2072 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                MATH_OP (dc, imm, 0, -);
              
              }
            break;
          case 0x1e:
            op_semantics_73:
              {
                /** 1001 sss w ss01 1110		DIVX.size src */
#line 903 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 903 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 903 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 sss w ss01 1110		DIVX.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  w = 0x%x,", w);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 903 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                div_op (sss, ss, 0, 1, w+1);
              
              }
            break;
          case 0x1f:
            op_semantics_74:
              {
                /** 1001 ddd w dd01 1111		STNZ.size #IMM,dest */
#line 2008 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2008 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2008 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd01 1111		STNZ.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2008 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                imm = IMM(w+1);
                if (! FLAG_Z)
                  put_dest (dc, imm);
              
              }
            break;
          case 0x21:
            op_semantics_75:
              {
                /** 1001 ddd0 dd10 0001		SHL.L #IMM, dest */
#line 1841 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1841 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd0 dd10 0001		SHL.L #IMM, dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1841 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                shift_op (dc, 0, imm, 1);
              
              }
            break;
          case 0x23:
          case 0x33:
            op_semantics_76:
              {
                /** 1001 sss0 ss1w 0011		INDEXL.size src */
#line 1001 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1001 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1001 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 sss0 ss1w 0011		INDEXL.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1001 "m32c/m32c.opc"
                INDEXOP(4, 1, 1);
              }
            break;
          case 0x2e:
            op_semantics_77:
              {
                /** 1001 ddd w dd10 1110		CMP.size:G #IMM,dest */
#line 774 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 774 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 774 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd10 1110		CMP.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 774 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                v = get_src (dc);
                imm = IMM(w+1);
                cmp (v, imm, w+1);
              
              }
            break;
          case 0x2f:
            op_semantics_78:
              {
                /** 1001 ddd w dd10 1111		MOV.size:G #IMM,dest */
#line 1223 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1223 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1223 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd10 1111		MOV.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1223 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, w+1);
                imm = IMM(w+1);
                v = imm;
                tprintf("%x = %x\n", v, v);
                set_sz(v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x31:
            op_semantics_79:
              {
                /** 1001 ddd0 dd11 0001		SUB.L:G #IMM,dest */
#line 2042 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2042 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd0 dd11 0001		SUB.L:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2042 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, 4);
                imm = IMM(4);
                MATH_OP (dc, imm, 0, -);
              
              }
            break;
          case 0x3e:
            op_semantics_80:
              {
                /** 1001 ddd w dd11 1110		TST.size:G #IMM,dest */
#line 2087 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2087 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2087 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd11 1110		TST.size:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2087 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                imm = IMM(w+1);
                a = get_src (dc);
                v = a & imm;
                set_sz (v, w+1);
              
              }
            break;
          case 0x3f:
            op_semantics_81:
              {
                /** 1001 ddd w dd11 1111		STZX.size #IMM1,#IMM2,dest */
#line 2024 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2024 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2024 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1001 ddd w dd11 1111		STZX.size #IMM1,#IMM2,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 2024 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = IMM(w+1);
                b = IMM(w+1);
                if (FLAG_Z)
                  put_dest (dc, a);
                else
                  put_dest (dc, b);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x91:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x92:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_68;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_69;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x11:
            goto op_semantics_72;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x21:
            goto op_semantics_75;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_76;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x31:
            goto op_semantics_79;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x93:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x94:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_68;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_69;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x11:
            goto op_semantics_72;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x21:
            goto op_semantics_75;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_76;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x31:
            goto op_semantics_79;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x95:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x96:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_68;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_69;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x11:
            goto op_semantics_72;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x21:
            goto op_semantics_75;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_76;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x31:
            goto op_semantics_79;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x97:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x98:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_68;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_69;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x11:
            goto op_semantics_72;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x21:
            goto op_semantics_75;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_76;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x31:
            goto op_semantics_79;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x99:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_70;
            break;
          case 0x0f:
            goto op_semantics_71;
            break;
          case 0x1e:
            goto op_semantics_73;
            break;
          case 0x1f:
            goto op_semantics_74;
            break;
          case 0x2e:
            goto op_semantics_77;
            break;
          case 0x2f:
            goto op_semantics_78;
            break;
          case 0x3e:
            goto op_semantics_80;
            break;
          case 0x3f:
            goto op_semantics_81;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0x9c:
    case 0x9d:
    case 0xbc:
    case 0xbd:
        {
          /** 10w1 110d				MOV.size:S #IMM,a0/a1 */
#line 1258 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = (op[0] >> 5) & 0x01;
#line 1258 "m32c/m32c.opc"
          int d ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 10w1 110d				MOV.size:S #IMM,a0/a1 */",
                     op[0]);
              printf("  w = 0x%x,", w);
              printf("  d = 0x%x\n", d);
            }
#line 1258 "m32c/m32c.opc"
        
          imm = IMM(w ? 3 : 2);
          put_reg (d ? a1 : a0, imm);
          set_sz (imm & addr_mask, w+1);
        
        }
      break;
    case 0x9e:
        {
          /** 1001 1110				REIT */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1001 1110				REIT */",
                     op[0]);
            }
#line 1676 "m32c/m32c.opc"
        
          a = get_reg (sp);
          put_reg (pc, mem_get_si (a));
          a += 4;
          put_reg (flags, mem_get_hi (a));
          a += 2;
          put_reg (sp, a);
        
        }
      break;
    case 0x9f:
        {
          /** 1001 1111				FREIT */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1001 1111				FREIT */",
                     op[0]);
            }
#line 977 "m32c/m32c.opc"
        
          NOTYET();
        
        }
      break;
    case 0xa0:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            op_semantics_82:
              {
                /** 1010 sss0 ss00 0001		PUSH.L src */
#line 1620 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1620 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 sss0 ss00 0001		PUSH.L src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1620 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                sc = decode_dest23 (sss, ss, 4);
                a = get_src (sc);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_si (get_reg (sp), a);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            op_semantics_83:
              {
                /** 1010 sss0 ss0w 0011		INDEXBD.size src */
#line 997 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 997 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 997 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 sss0 ss0w 0011		INDEXBD.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 997 "m32c/m32c.opc"
                INDEXOP(1, 0, 1);
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            op_semantics_84:
              {
                /** 1010 ddd w dd00 1110		INC.size dest */
#line 985 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 985 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 985 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd00 1110		INC.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 985 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = get_src (dc);
                v = a+1;
                tprintf ("%x ++ = %x\n", a, v);
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x0f:
            op_semantics_85:
              {
                /** 1010 sss w ss00 1111		MOV.size:G src,dsp:8[SP] */
#line 1337 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1337 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1337 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 sss w ss00 1111		MOV.size:G src,dsp:8[SP] */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  w = 0x%x,", w);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1337 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_dest23 (sss, ss, w+1);
                imm = IMM(1);
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
          case 0x11:
            op_semantics_86:
              {
                /** 1010 ddd0 dd01 0001		CMPX #IMM,dest */
#line 832 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 832 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd0 dd01 0001		CMPX #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 832 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                v = get_src (dc);
                imm = sign_ext (IMM(1), 8);
                cmp (v, imm, 4);
              
              }
            break;
          case 0x1e:
            op_semantics_87:
              {
                /** 1010 ddd w dd01 1110		NOT.size dest */
#line 1505 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1505 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1505 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd01 1110		NOT.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1505 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = get_src (dc);
                v = ~a;
                tprintf("~ %x = %x\n", a, v);
                set_sz(v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x1f:
            op_semantics_88:
              {
                /** 1010 ddd w dd01 1111		ABS.size dest */
#line 450 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 450 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 450 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd01 1111		ABS.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 450 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                v = sign_ext (get_src (dc), w?16:8);
                a = v<0 ? -v : v;
                tprintf("abs(%d) = %d\n", v, a);
                set_osz(a, w+1);
                put_dest (dc, a);
              
              }
            break;
          case 0x21:
            op_semantics_89:
              {
                /** 1010 ddd0 dd10 0001		SHA.L #IMM,dest */
#line 1806 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1806 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd0 dd10 0001		SHA.L #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1806 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                shift_op (dc, 1, imm, 1);
              
              }
            break;
          case 0x23:
          case 0x33:
            op_semantics_90:
              {
                /** 1010 sss0 ss1w 0011		INDEXWD.size src */
#line 1009 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1009 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1009 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 sss0 ss1w 0011		INDEXWD.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1009 "m32c/m32c.opc"
                INDEXOP(2, 0, 1);
              }
            break;
          case 0x2e:
            op_semantics_91:
              {
                /** 1010 ddd w dd10 1110		RORC.size dest */
#line 1725 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1725 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1725 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd10 1110		RORC.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1725 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                rot_op (dc, 1, -1);
              
              }
            break;
          case 0x2f:
            op_semantics_92:
              {
                /** 1010 ddd w dd10 1111		NEG.size dest */
#line 1491 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1491 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1491 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd10 1111		NEG.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1491 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = sign_ext (get_src (dc), (w+1)*8);
                v = -a;
                tprintf("%d * -1 = %d\n", a, v);
                set_oszc(v, w+1, v==0);
                put_dest (dc, v);
              
              }
            break;
          case 0x31:
            op_semantics_93:
              {
                /** 1010 ddd0 dd11 0001		CMP.L:G #IMM32,dest */
#line 782 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 782 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd0 dd11 0001		CMP.L:G #IMM32,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 782 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                v = get_src (dc);
                imm = IMM(4);
                cmp (v, imm, 4);
              
              }
            break;
          case 0x3e:
            op_semantics_94:
              {
                /** 1010 ddd w dd11 1110		SHL.size R1H,dest */
#line 1848 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1848 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1848 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd11 1110		SHL.size R1H,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1848 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 0, a, 1);
              
              }
            break;
          case 0x3f:
            op_semantics_95:
              {
                /** 1010 ddd w dd11 1111		ROT.size R1H,dest */
#line 1737 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1737 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1737 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1010 ddd w dd11 1111		ROT.size R1H,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1737 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = sign_ext (get_reg (r1h), 8);
                rot_op (dc, a, -1);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa1:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa2:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_82;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_83;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x11:
            goto op_semantics_86;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x21:
            goto op_semantics_89;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_90;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x31:
            goto op_semantics_93;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa3:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa4:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_82;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_83;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x11:
            goto op_semantics_86;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x21:
            goto op_semantics_89;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_90;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x31:
            goto op_semantics_93;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa5:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa6:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_82;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_83;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x11:
            goto op_semantics_86;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x21:
            goto op_semantics_89;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_90;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x31:
            goto op_semantics_93;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa7:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa8:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_82;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_83;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x11:
            goto op_semantics_86;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x21:
            goto op_semantics_89;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_90;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x31:
            goto op_semantics_93;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xa9:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_84;
            break;
          case 0x0f:
            goto op_semantics_85;
            break;
          case 0x1e:
            goto op_semantics_87;
            break;
          case 0x1f:
            goto op_semantics_88;
            break;
          case 0x2e:
            goto op_semantics_91;
            break;
          case 0x2f:
            goto op_semantics_92;
            break;
          case 0x3e:
            goto op_semantics_94;
            break;
          case 0x3f:
            goto op_semantics_95;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xae:
    case 0xaf:
        {
          /** 1010 111w				PUSH.size #IMM */
#line 1590 "m32c/m32c.opc"
          int w ATTRIBUTE_UNUSED = op[0] & 0x01;
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1010 111w				PUSH.size #IMM */",
                     op[0]);
              printf("  w = 0x%x\n", w);
            }
#line 1590 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = IMM(w+1);
          tprintf("push%s: %x\n", w ? "hi" : "qi", imm);
          a = get_reg (sp) - 2;
          if (w)
            mem_put_hi (a, imm);
          else
            mem_put_qi (a, imm);
          put_reg (sp, a);
        
        }
      break;
    case 0xb0:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
          case 0x40:
          case 0x50:
          case 0x60:
          case 0x70:
          case 0x80:
          case 0x90:
          case 0xa0:
          case 0xb0:
          case 0xc0:
          case 0xd0:
          case 0xe0:
          case 0xf0:
            goto op_semantics_39;
            break;
          case 0x01:
          case 0x41:
          case 0x81:
          case 0xc1:
            op_semantics_96:
              {
                /** 1011 0sa0 ss00 0001		PUSHA src */
#line 1628 "m32c/m32c.opc"
                int sa ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x03;
#line 1628 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0sa0 ss00 0001		PUSHA src */",
                           op[0], op[1]);
                    printf("  sa = 0x%x,", sa);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1628 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_dest23 (sa, ss, 1);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_hi (get_reg (sp), sc.u.addr);
                tprintf("pushsi: %x\n", sc.u.addr);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
          case 0x42:
          case 0x52:
          case 0x62:
          case 0x72:
          case 0x82:
          case 0x92:
          case 0xa2:
          case 0xb2:
          case 0xc2:
          case 0xd2:
          case 0xe2:
          case 0xf2:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
              {
                /** 1011 0000 000w 0011		DIVU.size #IMM */
#line 882 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0000 000w 0011		DIVU.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 882 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                div_op (-1, 0, 1, 0, w+1);
              
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
          case 0x44:
          case 0x54:
          case 0x64:
          case 0x74:
          case 0x84:
          case 0x94:
          case 0xa4:
          case 0xb4:
          case 0xc4:
          case 0xd4:
          case 0xe4:
          case 0xf4:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
          case 0x45:
          case 0x55:
          case 0x65:
          case 0x75:
          case 0x85:
          case 0x95:
          case 0xa5:
          case 0xb5:
          case 0xc5:
          case 0xd5:
          case 0xe5:
          case 0xf5:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
          case 0x46:
          case 0x56:
          case 0x66:
          case 0x76:
          case 0x86:
          case 0x96:
          case 0xa6:
          case 0xb6:
          case 0xc6:
          case 0xd6:
          case 0xe6:
          case 0xf6:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
          case 0x48:
          case 0x58:
          case 0x68:
          case 0x78:
          case 0x88:
          case 0x98:
          case 0xa8:
          case 0xb8:
          case 0xc8:
          case 0xd8:
          case 0xe8:
          case 0xf8:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
          case 0x49:
          case 0x59:
          case 0x69:
          case 0x79:
          case 0x89:
          case 0x99:
          case 0xa9:
          case 0xb9:
          case 0xc9:
          case 0xd9:
          case 0xe9:
          case 0xf9:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
          case 0x4a:
          case 0x5a:
          case 0x6a:
          case 0x7a:
          case 0x8a:
          case 0x9a:
          case 0xaa:
          case 0xba:
          case 0xca:
          case 0xda:
          case 0xea:
          case 0xfa:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
          case 0x4b:
          case 0x5b:
          case 0x6b:
          case 0x7b:
          case 0x8b:
          case 0x9b:
          case 0xab:
          case 0xbb:
          case 0xcb:
          case 0xdb:
          case 0xeb:
          case 0xfb:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
          case 0x4c:
          case 0x5c:
          case 0x6c:
          case 0x7c:
          case 0x8c:
          case 0x9c:
          case 0xac:
          case 0xbc:
          case 0xcc:
          case 0xdc:
          case 0xec:
          case 0xfc:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
          case 0x4d:
          case 0x5d:
          case 0x6d:
          case 0x7d:
          case 0x8d:
          case 0x9d:
          case 0xad:
          case 0xbd:
          case 0xcd:
          case 0xdd:
          case 0xed:
          case 0xfd:
            goto op_semantics_51;
            break;
          case 0x0e:
          case 0x4e:
          case 0x8e:
          case 0xce:
            op_semantics_97:
              {
                /** 1011 ddd w dd00 1110		DEC.size dest */
#line 856 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 856 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 856 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd00 1110		DEC.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 856 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = get_src (dc);
                v = a-1;
                tprintf ("%x -- = %x\n", a, v);
                set_sz (v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x0f:
          case 0x4f:
          case 0x8f:
          case 0xcf:
            op_semantics_98:
              {
                /** 1011 ddd w dd00 1111		MOV.size:G dsp:8[SP], dest */
#line 1323 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1323 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1323 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd00 1111		MOV.size:G dsp:8[SP], dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1323 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                imm = IMM(1);
                dc = decode_dest23 (ddd, dd, w+1);
                a = get_reg (sp) + sign_ext (imm, 8);
                a &= addr_mask;
                if (w)
                  v = mem_get_hi (a);
                else
                  v = mem_get_qi (a);
                put_dest (dc, v);
                set_sz (v, w+1);
              
              }
            break;
          case 0x11:
          case 0x51:
          case 0x91:
          case 0xd1:
            op_semantics_99:
              {
                /** 1011 ddd0 dd01 0001		MOVX #IMM,dest */
#line 1392 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1392 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd0 dd01 0001		MOVX #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1392 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                put_dest (dc, imm);
                set_sz (imm, 1);
              
              }
            break;
          case 0x1e:
          case 0x5e:
          case 0x9e:
          case 0xde:
            op_semantics_100:
              {
                /** 1011 ddd w dd01 1110		ADCF.size dest */
#line 475 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 475 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 475 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd01 1110		ADCF.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 475 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                MATH_OP (dc, 0, carry, +);
              
              }
            break;
          case 0x23:
          case 0x33:
          case 0x63:
          case 0x73:
          case 0xa3:
          case 0xb3:
          case 0xe3:
          case 0xf3:
            op_semantics_101:
              {
                /** 1011 sss0 ss1w 0011		INDEXLD.size src */
#line 1003 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1003 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1003 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 sss0 ss1w 0011		INDEXLD.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1003 "m32c/m32c.opc"
                INDEXOP(4, 0, 1);
              }
            break;
          case 0x2e:
          case 0x6e:
          case 0xae:
          case 0xee:
            op_semantics_102:
              {
                /** 1011 ddd w dd10 1110		ROLC.size dest */
#line 1719 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1719 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1719 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd10 1110		ROLC.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1719 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                rot_op (dc, 1, 1);
              
              }
            break;
          case 0x2f:
          case 0x6f:
          case 0xaf:
          case 0xef:
            op_semantics_103:
              {
                /** 1011 ddd w dd10 1111		POP.size dest */
#line 1537 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1537 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1537 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd10 1111		POP.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1537 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                if (w)
                  a = mem_get_hi (get_reg (sp));
                else
                  a = mem_get_qi (get_reg (sp));
                put_reg (sp, get_reg (sp) + 2);
                tprintf("pop%s: %x\n", w ? "hi" : "qi", a);
                put_dest (dc, a);
              
              }
            break;
          case 0x31:
          case 0x71:
          case 0xb1:
          case 0xf1:
            op_semantics_104:
              {
                /** 1011 ddd0 dd11 0001		MOV.L:G #IMM,dest */
#line 1232 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1232 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd0 dd11 0001		MOV.L:G #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1232 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, 4);
                imm = IMM(4);
                v = imm;
                tprintf("%x = %x\n", v, v);
                set_sz(v, 4);
                put_dest (dc, v);
              
              }
            break;
          case 0x3e:
          case 0x7e:
          case 0xbe:
          case 0xfe:
            op_semantics_105:
              {
                /** 1011 ddd w dd11 1110		SHA.size R1H,dest */
#line 1813 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1813 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1813 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 ddd w dd11 1110		SHA.size R1H,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1813 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 1, a, 1);
              
              }
            break;
          case 0x43:
          case 0x53:
              {
                /** 1011 0000 010w 0011		DIV.size #IMM */
#line 866 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0000 010w 0011		DIV.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 866 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                div_op (-1, 0, 0, 0, w+1);
              
              }
            break;
          case 0x83:
          case 0x93:
              {
                /** 1011 0000 100w 0011		SMOVF.size */
#line 1907 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0000 100w 0011		SMOVF.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1907 "m32c/m32c.opc"
              
                v = get_reg (a0);
                a = get_reg (a1);
                b = get_reg (r3);
                if (b) for (;b;)
                  {
                    if (w)
              	mem_put_hi(a, mem_get_hi (v));
                    else
              	mem_put_qi(a, mem_get_qi (v));
                    v += w ? 2 : 1;
                    a += w ? 2 : 1;
                    b --;
                  }
                put_reg (a0, v);
                put_reg (a1, a);
                put_reg (r3, b);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb1:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_97;
            break;
          case 0x0f:
            goto op_semantics_98;
            break;
          case 0x1e:
            goto op_semantics_100;
            break;
          case 0x2e:
            goto op_semantics_102;
            break;
          case 0x2f:
            goto op_semantics_103;
            break;
          case 0x3e:
            goto op_semantics_105;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb2:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
          case 0x40:
          case 0x50:
          case 0x60:
          case 0x70:
          case 0x80:
          case 0x90:
          case 0xa0:
          case 0xb0:
          case 0xc0:
          case 0xd0:
          case 0xe0:
          case 0xf0:
            goto op_semantics_39;
            break;
          case 0x01:
          case 0x41:
          case 0x81:
          case 0xc1:
            goto op_semantics_96;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
          case 0x42:
          case 0x52:
          case 0x62:
          case 0x72:
          case 0x82:
          case 0x92:
          case 0xa2:
          case 0xb2:
          case 0xc2:
          case 0xd2:
          case 0xe2:
          case 0xf2:
            goto op_semantics_41;
            break;
          case 0x03:
              {
                /** 1011 0010 0000 0011		WAIT */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0010 0000 0011		WAIT */",
                           op[0], op[1]);
                  }
#line 2119 "m32c/m32c.opc"
              
                ;
              
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
          case 0x44:
          case 0x54:
          case 0x64:
          case 0x74:
          case 0x84:
          case 0x94:
          case 0xa4:
          case 0xb4:
          case 0xc4:
          case 0xd4:
          case 0xe4:
          case 0xf4:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
          case 0x45:
          case 0x55:
          case 0x65:
          case 0x75:
          case 0x85:
          case 0x95:
          case 0xa5:
          case 0xb5:
          case 0xc5:
          case 0xd5:
          case 0xe5:
          case 0xf5:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
          case 0x46:
          case 0x56:
          case 0x66:
          case 0x76:
          case 0x86:
          case 0x96:
          case 0xa6:
          case 0xb6:
          case 0xc6:
          case 0xd6:
          case 0xe6:
          case 0xf6:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
          case 0x48:
          case 0x58:
          case 0x68:
          case 0x78:
          case 0x88:
          case 0x98:
          case 0xa8:
          case 0xb8:
          case 0xc8:
          case 0xd8:
          case 0xe8:
          case 0xf8:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
          case 0x49:
          case 0x59:
          case 0x69:
          case 0x79:
          case 0x89:
          case 0x99:
          case 0xa9:
          case 0xb9:
          case 0xc9:
          case 0xd9:
          case 0xe9:
          case 0xf9:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
          case 0x4a:
          case 0x5a:
          case 0x6a:
          case 0x7a:
          case 0x8a:
          case 0x9a:
          case 0xaa:
          case 0xba:
          case 0xca:
          case 0xda:
          case 0xea:
          case 0xfa:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
          case 0x4b:
          case 0x5b:
          case 0x6b:
          case 0x7b:
          case 0x8b:
          case 0x9b:
          case 0xab:
          case 0xbb:
          case 0xcb:
          case 0xdb:
          case 0xeb:
          case 0xfb:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
          case 0x4c:
          case 0x5c:
          case 0x6c:
          case 0x7c:
          case 0x8c:
          case 0x9c:
          case 0xac:
          case 0xbc:
          case 0xcc:
          case 0xdc:
          case 0xec:
          case 0xfc:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
          case 0x4d:
          case 0x5d:
          case 0x6d:
          case 0x7d:
          case 0x8d:
          case 0x9d:
          case 0xad:
          case 0xbd:
          case 0xcd:
          case 0xdd:
          case 0xed:
          case 0xfd:
            goto op_semantics_51;
            break;
          case 0x0e:
          case 0x4e:
          case 0x8e:
          case 0xce:
            goto op_semantics_97;
            break;
          case 0x0f:
          case 0x4f:
          case 0x8f:
          case 0xcf:
            goto op_semantics_98;
            break;
          case 0x11:
          case 0x51:
          case 0x91:
          case 0xd1:
            goto op_semantics_99;
            break;
          case 0x1e:
          case 0x5e:
          case 0x9e:
          case 0xde:
            goto op_semantics_100;
            break;
          case 0x23:
          case 0x33:
          case 0x63:
          case 0x73:
          case 0xa3:
          case 0xb3:
          case 0xe3:
          case 0xf3:
            goto op_semantics_101;
            break;
          case 0x2e:
          case 0x6e:
          case 0xae:
          case 0xee:
            goto op_semantics_102;
            break;
          case 0x2f:
          case 0x6f:
          case 0xaf:
          case 0xef:
            goto op_semantics_103;
            break;
          case 0x31:
          case 0x71:
          case 0xb1:
          case 0xf1:
            goto op_semantics_104;
            break;
          case 0x3e:
          case 0x7e:
          case 0xbe:
          case 0xfe:
            goto op_semantics_105;
            break;
          case 0x43:
          case 0x53:
              {
                /** 1011 0010 010w 0011		DIVX.size #IMM */
#line 898 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0010 010w 0011		DIVX.size #IMM */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 898 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                div_op (-1, 0, 0, 1, w+1);
              
              }
            break;
          case 0x83:
          case 0x93:
              {
                /** 1011 0010 100w 0011		SIN.size */
#line 1870 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0010 100w 0011		SIN.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1870 "m32c/m32c.opc"
              
                v = get_reg (a0);
                a = get_reg (a1);
                b = get_reg (r3);
                if (b) for (;b;)
                  {
                    if (w)
              	mem_put_hi(a, mem_get_hi (v));
                    else
              	mem_put_qi(a, mem_get_qi (v));
                    a += w ? 2 : 1;
                    b --;
                  }
                put_reg (a0, v);
                put_reg (a1, a);
                put_reg (r3, b);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb3:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_97;
            break;
          case 0x0f:
            goto op_semantics_98;
            break;
          case 0x1e:
            goto op_semantics_100;
            break;
          case 0x2e:
            goto op_semantics_102;
            break;
          case 0x2f:
            goto op_semantics_103;
            break;
          case 0x3e:
            goto op_semantics_105;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb4:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
          case 0x40:
          case 0x50:
          case 0x60:
          case 0x70:
          case 0x80:
          case 0x90:
          case 0xa0:
          case 0xb0:
          case 0xc0:
          case 0xd0:
          case 0xe0:
          case 0xf0:
            goto op_semantics_39;
            break;
          case 0x01:
          case 0x41:
          case 0x81:
          case 0xc1:
            goto op_semantics_96;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
          case 0x42:
          case 0x52:
          case 0x62:
          case 0x72:
          case 0x82:
          case 0x92:
          case 0xa2:
          case 0xb2:
          case 0xc2:
          case 0xd2:
          case 0xe2:
          case 0xf2:
            goto op_semantics_41;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
          case 0x44:
          case 0x54:
          case 0x64:
          case 0x74:
          case 0x84:
          case 0x94:
          case 0xa4:
          case 0xb4:
          case 0xc4:
          case 0xd4:
          case 0xe4:
          case 0xf4:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
          case 0x45:
          case 0x55:
          case 0x65:
          case 0x75:
          case 0x85:
          case 0x95:
          case 0xa5:
          case 0xb5:
          case 0xc5:
          case 0xd5:
          case 0xe5:
          case 0xf5:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
          case 0x46:
          case 0x56:
          case 0x66:
          case 0x76:
          case 0x86:
          case 0x96:
          case 0xa6:
          case 0xb6:
          case 0xc6:
          case 0xd6:
          case 0xe6:
          case 0xf6:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
          case 0x48:
          case 0x58:
          case 0x68:
          case 0x78:
          case 0x88:
          case 0x98:
          case 0xa8:
          case 0xb8:
          case 0xc8:
          case 0xd8:
          case 0xe8:
          case 0xf8:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
          case 0x49:
          case 0x59:
          case 0x69:
          case 0x79:
          case 0x89:
          case 0x99:
          case 0xa9:
          case 0xb9:
          case 0xc9:
          case 0xd9:
          case 0xe9:
          case 0xf9:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
          case 0x4a:
          case 0x5a:
          case 0x6a:
          case 0x7a:
          case 0x8a:
          case 0x9a:
          case 0xaa:
          case 0xba:
          case 0xca:
          case 0xda:
          case 0xea:
          case 0xfa:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
          case 0x4b:
          case 0x5b:
          case 0x6b:
          case 0x7b:
          case 0x8b:
          case 0x9b:
          case 0xab:
          case 0xbb:
          case 0xcb:
          case 0xdb:
          case 0xeb:
          case 0xfb:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
          case 0x4c:
          case 0x5c:
          case 0x6c:
          case 0x7c:
          case 0x8c:
          case 0x9c:
          case 0xac:
          case 0xbc:
          case 0xcc:
          case 0xdc:
          case 0xec:
          case 0xfc:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
          case 0x4d:
          case 0x5d:
          case 0x6d:
          case 0x7d:
          case 0x8d:
          case 0x9d:
          case 0xad:
          case 0xbd:
          case 0xcd:
          case 0xdd:
          case 0xed:
          case 0xfd:
            goto op_semantics_51;
            break;
          case 0x0e:
          case 0x4e:
          case 0x8e:
          case 0xce:
            goto op_semantics_97;
            break;
          case 0x0f:
          case 0x4f:
          case 0x8f:
          case 0xcf:
            goto op_semantics_98;
            break;
          case 0x11:
          case 0x51:
          case 0x91:
          case 0xd1:
            goto op_semantics_99;
            break;
          case 0x1e:
          case 0x5e:
          case 0x9e:
          case 0xde:
            goto op_semantics_100;
            break;
          case 0x23:
          case 0x33:
          case 0x63:
          case 0x73:
          case 0xa3:
          case 0xb3:
          case 0xe3:
          case 0xf3:
            goto op_semantics_101;
            break;
          case 0x2e:
          case 0x6e:
          case 0xae:
          case 0xee:
            goto op_semantics_102;
            break;
          case 0x2f:
          case 0x6f:
          case 0xaf:
          case 0xef:
            goto op_semantics_103;
            break;
          case 0x31:
          case 0x71:
          case 0xb1:
          case 0xf1:
            goto op_semantics_104;
            break;
          case 0x3e:
          case 0x7e:
          case 0xbe:
          case 0xfe:
            goto op_semantics_105;
            break;
          case 0x83:
          case 0x93:
              {
                /** 1011 0100 100w 0011		SOUT.size */
#line 1945 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0100 100w 0011		SOUT.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1945 "m32c/m32c.opc"
              
                v = get_reg (a0);
                a = get_reg (a1);
                b = get_reg (r3);
                for (;b;)
                  {
                    if (w)
              	mem_put_hi(a, mem_get_hi (v));
                    else
              	mem_put_qi(a, mem_get_qi (v));
                    v += w ? 2 : 1;
                    b --;
                  }
                put_reg (a0, v);
                put_reg (a1, a);
                put_reg (r3, b);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb5:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_97;
            break;
          case 0x0f:
            goto op_semantics_98;
            break;
          case 0x1e:
            goto op_semantics_100;
            break;
          case 0x2e:
            goto op_semantics_102;
            break;
          case 0x2f:
            goto op_semantics_103;
            break;
          case 0x3e:
            goto op_semantics_105;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb6:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
          case 0x40:
          case 0x50:
          case 0x60:
          case 0x70:
          case 0x80:
          case 0x90:
          case 0xa0:
          case 0xb0:
          case 0xc0:
          case 0xd0:
          case 0xe0:
          case 0xf0:
            goto op_semantics_39;
            break;
          case 0x01:
          case 0x41:
          case 0x81:
          case 0xc1:
            goto op_semantics_96;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
          case 0x42:
          case 0x52:
          case 0x62:
          case 0x72:
          case 0x82:
          case 0x92:
          case 0xa2:
          case 0xb2:
          case 0xc2:
          case 0xd2:
          case 0xe2:
          case 0xf2:
            goto op_semantics_41;
            break;
          case 0x03:
              {
                /** 1011 0110 0000 0011		ADD.L:S #IMM8,SP */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 0000 0011		ADD.L:S #IMM8,SP */",
                           op[0], op[1]);
                  }
#line 546 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = reg_sd (sp);
                b = sign_ext (IMM(1), 8);
                MATH_OP (dc, b, 0, +);
              
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
          case 0x44:
          case 0x54:
          case 0x64:
          case 0x74:
          case 0x84:
          case 0x94:
          case 0xa4:
          case 0xb4:
          case 0xc4:
          case 0xd4:
          case 0xe4:
          case 0xf4:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
          case 0x45:
          case 0x55:
          case 0x65:
          case 0x75:
          case 0x85:
          case 0x95:
          case 0xa5:
          case 0xb5:
          case 0xc5:
          case 0xd5:
          case 0xe5:
          case 0xf5:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
          case 0x46:
          case 0x56:
          case 0x66:
          case 0x76:
          case 0x86:
          case 0x96:
          case 0xa6:
          case 0xb6:
          case 0xc6:
          case 0xd6:
          case 0xe6:
          case 0xf6:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
          case 0x48:
          case 0x58:
          case 0x68:
          case 0x78:
          case 0x88:
          case 0x98:
          case 0xa8:
          case 0xb8:
          case 0xc8:
          case 0xd8:
          case 0xe8:
          case 0xf8:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
          case 0x49:
          case 0x59:
          case 0x69:
          case 0x79:
          case 0x89:
          case 0x99:
          case 0xa9:
          case 0xb9:
          case 0xc9:
          case 0xd9:
          case 0xe9:
          case 0xf9:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
          case 0x4a:
          case 0x5a:
          case 0x6a:
          case 0x7a:
          case 0x8a:
          case 0x9a:
          case 0xaa:
          case 0xba:
          case 0xca:
          case 0xda:
          case 0xea:
          case 0xfa:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
          case 0x4b:
          case 0x5b:
          case 0x6b:
          case 0x7b:
          case 0x8b:
          case 0x9b:
          case 0xab:
          case 0xbb:
          case 0xcb:
          case 0xdb:
          case 0xeb:
          case 0xfb:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
          case 0x4c:
          case 0x5c:
          case 0x6c:
          case 0x7c:
          case 0x8c:
          case 0x9c:
          case 0xac:
          case 0xbc:
          case 0xcc:
          case 0xdc:
          case 0xec:
          case 0xfc:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
          case 0x4d:
          case 0x5d:
          case 0x6d:
          case 0x7d:
          case 0x8d:
          case 0x9d:
          case 0xad:
          case 0xbd:
          case 0xcd:
          case 0xdd:
          case 0xed:
          case 0xfd:
            goto op_semantics_51;
            break;
          case 0x0e:
          case 0x4e:
          case 0x8e:
          case 0xce:
            goto op_semantics_97;
            break;
          case 0x0f:
          case 0x4f:
          case 0x8f:
          case 0xcf:
            goto op_semantics_98;
            break;
          case 0x11:
          case 0x51:
          case 0x91:
          case 0xd1:
            goto op_semantics_99;
            break;
          case 0x13:
              {
                /** 1011 0110 0001 0011		ADD.L:G #IMM16,SP */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 0001 0011		ADD.L:G #IMM16,SP */",
                           op[0], op[1]);
                  }
#line 532 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = reg_sd (sp);
                b = sign_ext (IMM(2), 16);
                MATH_OP (dc, b, 0, +);
              
              }
            break;
          case 0x1e:
          case 0x5e:
          case 0x9e:
          case 0xde:
            goto op_semantics_100;
            break;
          case 0x23:
          case 0x33:
          case 0x63:
          case 0x73:
          case 0xa3:
          case 0xb3:
          case 0xe3:
          case 0xf3:
            goto op_semantics_101;
            break;
          case 0x2e:
          case 0x6e:
          case 0xae:
          case 0xee:
            goto op_semantics_102;
            break;
          case 0x2f:
          case 0x6f:
          case 0xaf:
          case 0xef:
            goto op_semantics_103;
            break;
          case 0x31:
          case 0x71:
          case 0xb1:
          case 0xf1:
            goto op_semantics_104;
            break;
          case 0x3e:
          case 0x7e:
          case 0xbe:
          case 0xfe:
            goto op_semantics_105;
            break;
          case 0x53:
              {
                /** 1011 0110 0101 0011		PUSH.L #IMM32 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 0101 0011		PUSH.L #IMM32 */",
                           op[0], op[1]);
                  }
#line 1614 "m32c/m32c.opc"
              
                imm = IMM(4);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_si (get_reg (sp), imm);
              
              }
            break;
          case 0x83:
          case 0x93:
              {
                /** 1011 0110 100w 0011		SMOVB.size */
#line 1888 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 100w 0011		SMOVB.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1888 "m32c/m32c.opc"
              
                v = get_reg (a0);
                a = get_reg (a1);
                b = get_reg (r3);
                if (b) for (;b;)
                  {
                    if (w)
              	mem_put_hi(a, mem_get_hi (v));
                    else
              	mem_put_qi(a, mem_get_qi (v));
                    v -= w ? 2 : 1;
                    a -= w ? 2 : 1;
                    b --;
                  }
                put_reg (a0, v);
                put_reg (a1, a);
                put_reg (r3, b);
              
              }
            break;
          case 0xc3:
              {
                /** 1011 0110 1100 0011		LDCTX */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 1100 0011		LDCTX */",
                           op[0], op[1]);
                  }
#line 1169 "m32c/m32c.opc"
              
                NOTYET();
              
              }
            break;
          case 0xd3:
              {
                /** 1011 0110 1101 0011		STCX abs16,abs24 */
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 0110 1101 0011		STCX abs16,abs24 */",
                           op[0], op[1]);
                  }
#line 2004 "m32c/m32c.opc"
              
                NOTYET();
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb7:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_97;
            break;
          case 0x0f:
            goto op_semantics_98;
            break;
          case 0x1e:
            goto op_semantics_100;
            break;
          case 0x2e:
            goto op_semantics_102;
            break;
          case 0x2f:
            goto op_semantics_103;
            break;
          case 0x3e:
            goto op_semantics_105;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb8:
        GETBYTE();
        switch (op[1] & 0xff) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
          case 0x40:
          case 0x50:
          case 0x60:
          case 0x70:
          case 0x80:
          case 0x90:
          case 0xa0:
          case 0xb0:
          case 0xc0:
          case 0xd0:
          case 0xe0:
          case 0xf0:
            goto op_semantics_39;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
          case 0x42:
          case 0x52:
          case 0x62:
          case 0x72:
          case 0x82:
          case 0x92:
          case 0xa2:
          case 0xb2:
          case 0xc2:
          case 0xd2:
          case 0xe2:
          case 0xf2:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
              {
                /** 1011 1000 000w 0011		SSTR.size */
#line 1963 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 1000 000w 0011		SSTR.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1963 "m32c/m32c.opc"
              
                a = get_reg (a1);
                b = get_reg (r3);
                v = get_reg (w ? r0 : r0l);
                for (;b;)
                  {
                    if (w)
              	mem_put_hi(a, v);
                    else
              	mem_put_qi(a, v);
                    a += w ? 2 : 1;
                    b --;
                  }
                put_reg (a1, a);
                put_reg (r3, b);
              
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
          case 0x44:
          case 0x54:
          case 0x64:
          case 0x74:
          case 0x84:
          case 0x94:
          case 0xa4:
          case 0xb4:
          case 0xc4:
          case 0xd4:
          case 0xe4:
          case 0xf4:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
          case 0x45:
          case 0x55:
          case 0x65:
          case 0x75:
          case 0x85:
          case 0x95:
          case 0xa5:
          case 0xb5:
          case 0xc5:
          case 0xd5:
          case 0xe5:
          case 0xf5:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
          case 0x46:
          case 0x56:
          case 0x66:
          case 0x76:
          case 0x86:
          case 0x96:
          case 0xa6:
          case 0xb6:
          case 0xc6:
          case 0xd6:
          case 0xe6:
          case 0xf6:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
          case 0x48:
          case 0x58:
          case 0x68:
          case 0x78:
          case 0x88:
          case 0x98:
          case 0xa8:
          case 0xb8:
          case 0xc8:
          case 0xd8:
          case 0xe8:
          case 0xf8:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
          case 0x49:
          case 0x59:
          case 0x69:
          case 0x79:
          case 0x89:
          case 0x99:
          case 0xa9:
          case 0xb9:
          case 0xc9:
          case 0xd9:
          case 0xe9:
          case 0xf9:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
          case 0x4a:
          case 0x5a:
          case 0x6a:
          case 0x7a:
          case 0x8a:
          case 0x9a:
          case 0xaa:
          case 0xba:
          case 0xca:
          case 0xda:
          case 0xea:
          case 0xfa:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
          case 0x4b:
          case 0x5b:
          case 0x6b:
          case 0x7b:
          case 0x8b:
          case 0x9b:
          case 0xab:
          case 0xbb:
          case 0xcb:
          case 0xdb:
          case 0xeb:
          case 0xfb:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
          case 0x4c:
          case 0x5c:
          case 0x6c:
          case 0x7c:
          case 0x8c:
          case 0x9c:
          case 0xac:
          case 0xbc:
          case 0xcc:
          case 0xdc:
          case 0xec:
          case 0xfc:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
          case 0x4d:
          case 0x5d:
          case 0x6d:
          case 0x7d:
          case 0x8d:
          case 0x9d:
          case 0xad:
          case 0xbd:
          case 0xcd:
          case 0xdd:
          case 0xed:
          case 0xfd:
            goto op_semantics_51;
            break;
          case 0x0e:
          case 0x4e:
          case 0x8e:
          case 0xce:
            goto op_semantics_97;
            break;
          case 0x0f:
          case 0x4f:
          case 0x8f:
          case 0xcf:
            goto op_semantics_98;
            break;
          case 0x11:
          case 0x51:
          case 0x91:
          case 0xd1:
            goto op_semantics_99;
            break;
          case 0x1e:
          case 0x5e:
          case 0x9e:
          case 0xde:
            goto op_semantics_100;
            break;
          case 0x23:
          case 0x33:
          case 0x63:
          case 0x73:
          case 0xa3:
          case 0xb3:
          case 0xe3:
          case 0xf3:
            goto op_semantics_101;
            break;
          case 0x2e:
          case 0x6e:
          case 0xae:
          case 0xee:
            goto op_semantics_102;
            break;
          case 0x2f:
          case 0x6f:
          case 0xaf:
          case 0xef:
            goto op_semantics_103;
            break;
          case 0x31:
          case 0x71:
          case 0xb1:
          case 0xf1:
            goto op_semantics_104;
            break;
          case 0x3e:
          case 0x7e:
          case 0xbe:
          case 0xfe:
            goto op_semantics_105;
            break;
          case 0x43:
          case 0x53:
              {
                /** 1011 1000 010w 0011		RMPA.size */
#line 1685 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 1000 010w 0011		RMPA.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1685 "m32c/m32c.opc"
              
                {
                int count = get_reg (r3);
                int list1 = get_reg (a0);
                int list2 = get_reg (a1);
                long long sum = get_reg_ll (r3r1r2r0) & 0xffffff;
              
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
                    tprintf("%lld + %d * %d = ", sum, a, b);
                    sum += a * b;
                    tprintf("%lld\n", sum);
                    list1 += w ? 2 : 1;
                    list2 += w ? 2 : 1;
                    count --;
                  }
                put_reg (r3, count);
                put_reg (a0, list1);
                put_reg (a1, list2);
                put_reg (r2r0, (int)(sum & 0xffffffffU));
                put_reg (r1, (int)(sum >> 32));
                }
              
              }
            break;
          case 0x83:
          case 0x93:
              {
                /** 1011 1000 100w 0011		SMOVU.size */
#line 1926 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 1000 100w 0011		SMOVU.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1926 "m32c/m32c.opc"
              
                v = get_reg (a0);
                a = get_reg (a1);
                do
                  {
                    if (w)
              	mem_put_hi(a, (t0 = mem_get_hi (v)));
                    else
              	mem_put_qi(a, (t0 = mem_get_qi (v)));
                    v += w ? 2 : 1;
                    a += w ? 2 : 1;
                    if (t0 == 0
              	  || (w && ((t0 & 0xff) == 0 || (t0 & 0xff00) == 0)))
              	break;
                  } while (1);
                put_reg (a0, v);
                put_reg (a1, a);
              
              }
            break;
          case 0xc3:
          case 0xd3:
              {
                /** 1011 1000 110w 0011		SCMPU.size */
#line 1772 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 1000 110w 0011		SCMPU.size */",
                           op[0], op[1]);
                    printf("  w = 0x%x\n", w);
                  }
#line 1772 "m32c/m32c.opc"
              
                ta0 = get_reg (a0);
                ta1 = get_reg (a1);
              
                for (;;)
                  {
                    t0 = mem_get_qi (ta0);
                    t2 = mem_get_qi (ta1);
                    if (w)
              	{
              	  t1 = mem_get_qi (ta0 + 1);
              	  t3 = mem_get_qi (ta1 + 1);
              	}
                    dif = t0 - t2;
                    if (dif == 0 && t0 != 0 && w)
              	dif = t1 - t3;
                    set_oszc (dif, 1, dif > 0);
              
                    ta0 += w ? 2 : 1;
                    ta1 += w ? 2 : 1;
              
                    if (t0 == 0 || t0 != t2)
              	break;
                    if (w && (t1 == 0 || t1 != t3))
              	break;
                  }
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xb9:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_97;
            break;
          case 0x0f:
            goto op_semantics_98;
            break;
          case 0x1e:
            goto op_semantics_100;
            break;
          case 0x2e:
            goto op_semantics_102;
            break;
          case 0x2f:
            goto op_semantics_103;
            break;
          case 0x3e:
            goto op_semantics_105;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xbb:
        {
          /** 1011 1011				JMP.B label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1011 1011				JMP.B label */",
                     op[0]);
            }
#line 1037 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = sign_ext (IMM(1), 8);
          if (imm == -1)
            {
              if (verbose)
        	printf("[jmp-to-self detected as exit]\n");
              return M32C_MAKE_HIT_BREAK ();
            }
          put_reg (pc, m32c_opcode_pc + 1 + imm);
        
        }
      break;
    case 0xbe:
        GETBYTE();
        switch (op[1] & 0x03) {
          case 0x00:
              {
                /** 1011 1110 vector00			INT #IMM */
#line 1014 "m32c/m32c.opc"
                int vector ATTRIBUTE_UNUSED = (op[1] >> 2) & 0x3f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1011 1110 vector00			INT #IMM */",
                           op[0], op[1]);
                    printf("  vector = 0x%x\n", vector);
                  }
#line 1014 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                trigger_based_interrupt (vector);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xbf:
        {
          /** 1011 1111				INTO */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1011 1111				INTO */",
                     op[0]);
            }
#line 1019 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          if (FLAG_O)
            trigger_fixed_interrupt (0xffffe0);
        
        }
      break;
    case 0xc0:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            op_semantics_106:
              {
                /** 1100 ddd0 dd00 0001		SHL.L R1H,dest */
#line 1855 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1855 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 ddd0 dd00 0001		SHL.L R1H,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1855 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 0, a, 1);
              
              }
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            op_semantics_107:
              {
                /** 1100 sss0 ss0w 0011		INDEXBS.size src */
#line 999 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 999 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 999 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss0 ss0w 0011		INDEXBS.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 999 "m32c/m32c.opc"
                INDEXOP(1, 1, 0);
              }
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            op_semantics_108:
              {
                /** 1100 sss w ss00 1110		PUSH.size src */
#line 1602 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1602 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1602 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss w ss00 1110		PUSH.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  w = 0x%x,", w);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1602 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                sc = decode_dest23 (sss, ss, w+1);
                a = get_src (sc);
                put_reg (sp, get_reg (sp) - 2);
                if (w)
                  mem_put_hi (get_reg (sp), a);
                else
                  mem_put_qi (get_reg (sp), a);
                tprintf("push%s: %x\n", w ? "hi" : "qi", a);
              
              }
            break;
          case 0x11:
            op_semantics_109:
              {
                /** 1100 ddd0 dd01 0001		SHA.L	R1H,dest */
#line 1820 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1820 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 ddd0 dd01 0001		SHA.L	R1H,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1820 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                a = sign_ext (get_reg (r1h), 8);
                shift_op (dc, 1, a, 1);
              
              }
            break;
          case 0x1e:
            op_semantics_110:
              {
                /** 1100 ddd w dd01 1110		EXTS.size dest */
#line 946 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 946 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 946 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 ddd w dd01 1110		EXTS.size dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 946 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                v = sign_ext (get_src (dc), (w+1)*8);
                dc = widen_sd (dc);
                put_dest (dc, v);
                set_sz (v, (w+1)*2);
              
              }
            break;
          case 0x21:
            op_semantics_111:
              {
                /** 1100 ddd0 dd10 0001		SHANC.L #IMM,dest */
#line 1827 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1827 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 ddd0 dd10 0001		SHANC.L #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 1827 "m32c/m32c.opc"
              
                M32C_ONLY();
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 4);
                imm = sign_ext (IMM(1), 8);
                shift_op (dc, 1, imm, 0);
              
              }
            break;
          case 0x23:
          case 0x33:
            op_semantics_112:
              {
                /** 1100 sss0 ss1w 0011		INDEXWS.size src */
#line 1011 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1011 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1011 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = (op[1] >> 4) & 0x01;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss0 ss1w 0011		INDEXWS.size src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  w = 0x%x\n", w);
                  }
#line 1011 "m32c/m32c.opc"
                INDEXOP(2, 1, 0);
              
              }
            break;
          case 0x2e:
            op_semantics_113:
              {
                /** 1100 ddd w dd10 1110		BITINDEX.size src */
#line 617 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 617 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 617 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 ddd w dd10 1110		BITINDEX.size src */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x\n", dd);
                  }
#line 617 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                bitindex = get_src (dc);
                tprintf ("bitindex set to %d\n", bitindex);
                goto next_opcode;
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc1:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x0f:
            op_semantics_114:
              {
                /** 1100 sss1 ss00 1111		JMPI.W src */
#line 1061 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1061 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss1 ss00 1111		JMPI.W src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1061 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 2);
                a = get_src (sc);
                a = sign_ext (a, 16);
                put_reg (pc, m32c_opcode_pc + a);
              
              }
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x1f:
            op_semantics_115:
              {
                /** 1100 sss1 ss01 1111		JSRI.W src */
#line 1099 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1099 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss1 ss01 1111		JSRI.W src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1099 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 2);
                a = get_src (sc);
                a = sign_ext (a, 16);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_si (get_reg (sp), get_reg (pc));
                put_reg (pc, m32c_opcode_pc + a);
              
              }
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          case 0x3e:
            op_semantics_116:
              {
                /** 1100 sss1 ss11 1110		MULEX src */
#line 1440 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1440 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1100 sss1 ss11 1110		MULEX src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x\n", ss);
                  }
#line 1440 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                sc = decode_dest23 (sss, ss, 2);
                a = sign_ext (get_src (sc), 16);
                b = sign_ext (get_reg (r2r0), 32);
                ll = (long long)a * (long long)b;
                tprintf("%d * %d = %lld (%llx)\n", a, b, ll, ll);
                put_reg (r2r0, (int)ll);
                put_reg (r1, (int)(ll >> 32));
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc2:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_106;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_107;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x11:
            goto op_semantics_109;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x21:
            goto op_semantics_111;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_112;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc3:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x0f:
            goto op_semantics_114;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x1f:
            goto op_semantics_115;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          case 0x3e:
            goto op_semantics_116;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc4:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_106;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_107;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x11:
            goto op_semantics_109;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x21:
            goto op_semantics_111;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_112;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc5:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x0f:
            goto op_semantics_114;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x1f:
            goto op_semantics_115;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          case 0x3e:
            goto op_semantics_116;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc6:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_106;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_107;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x11:
            goto op_semantics_109;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x21:
            goto op_semantics_111;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_112;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc7:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x0f:
            goto op_semantics_114;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x1f:
            goto op_semantics_115;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          case 0x3e:
            goto op_semantics_116;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc8:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_39;
            break;
          case 0x01:
            goto op_semantics_106;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_41;
            break;
          case 0x03:
          case 0x13:
            goto op_semantics_107;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x11:
            goto op_semantics_109;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x21:
            goto op_semantics_111;
            break;
          case 0x23:
          case 0x33:
            goto op_semantics_112;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xc9:
        GETBYTE();
        switch (op[1] & 0x3f) {
          case 0x00:
          case 0x10:
          case 0x20:
          case 0x30:
            goto op_semantics_64;
            break;
          case 0x01:
          case 0x11:
          case 0x21:
          case 0x31:
            goto op_semantics_65;
            break;
          case 0x02:
          case 0x12:
          case 0x22:
          case 0x32:
            goto op_semantics_66;
            break;
          case 0x03:
          case 0x13:
          case 0x23:
          case 0x33:
            goto op_semantics_67;
            break;
          case 0x04:
          case 0x14:
          case 0x24:
          case 0x34:
            goto op_semantics_43;
            break;
          case 0x05:
          case 0x15:
          case 0x25:
          case 0x35:
            goto op_semantics_44;
            break;
          case 0x06:
          case 0x16:
          case 0x26:
          case 0x36:
            goto op_semantics_45;
            break;
          case 0x08:
          case 0x18:
          case 0x28:
          case 0x38:
            goto op_semantics_46;
            break;
          case 0x09:
          case 0x19:
          case 0x29:
          case 0x39:
            goto op_semantics_47;
            break;
          case 0x0a:
          case 0x1a:
          case 0x2a:
          case 0x3a:
            goto op_semantics_48;
            break;
          case 0x0b:
          case 0x1b:
          case 0x2b:
          case 0x3b:
            goto op_semantics_49;
            break;
          case 0x0c:
          case 0x1c:
          case 0x2c:
          case 0x3c:
            goto op_semantics_50;
            break;
          case 0x0d:
          case 0x1d:
          case 0x2d:
          case 0x3d:
            goto op_semantics_51;
            break;
          case 0x0e:
            goto op_semantics_108;
            break;
          case 0x0f:
            goto op_semantics_114;
            break;
          case 0x1e:
            goto op_semantics_110;
            break;
          case 0x1f:
            goto op_semantics_115;
            break;
          case 0x2e:
            goto op_semantics_113;
            break;
          case 0x3e:
            goto op_semantics_116;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xcc:
        {
          /** 1100 1100				JMP.A label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 1100				JMP.A label */",
                     op[0]);
            }
#line 1055 "m32c/m32c.opc"
          
          prefix (0, 0, 0);
          imm = IMM(3);
          put_reg (pc, imm);
        
        }
      break;
    case 0xcd:
        {
          /** 1100 1101				JSR.A label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 1101				JSR.A label */",
                     op[0]);
            }
#line 1091 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = IMM(3);
          put_reg (sp, get_reg (sp) - 4);
          mem_put_si (get_reg (sp), get_reg (pc));
          put_reg (pc, imm);
        
        }
      break;
    case 0xce:
        {
          /** 1100 1110				JMP.W label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 1110				JMP.W label */",
                     op[0]);
            }
#line 1049 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = sign_ext (IMM(2), 16);
          put_reg (pc, m32c_opcode_pc + 1 + imm);
        
        }
      break;
    case 0xcf:
        {
          /** 1100 1111				JSR.W label */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1100 1111				JSR.W label */",
                     op[0]);
            }
#line 1083 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = sign_ext (IMM(2), 16);
          put_reg (sp, get_reg (sp) - 4);
          mem_put_si (get_reg (sp), get_reg (pc));
          put_reg (pc, m32c_opcode_pc + imm + 1);
        
        }
      break;
    case 0xd0:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            op_semantics_117:
              {
                /** 1101 sss0 ss00 0bit		BTST:G src */
#line 724 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 724 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 724 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 sss0 ss00 0bit		BTST:G src */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 724 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 1);
                b = get_bit2 (sc, bit);
                set_zc (!b, b);
              
              }
            break;
          case 0x08:
            op_semantics_118:
              {
                /** 1101 ddd w dd00 1src		XCHG.size src,dest */
#line 2123 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 2123 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 2123 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 2123 "m32c/m32c.opc"
                int src ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd w dd00 1src		XCHG.size src,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  src = 0x%x\n", src);
                  }
#line 2123 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, w+1);
                sc = decode_src3 (src, w+1);
                a = get_src (dc);
                b = get_src (sc);
                put_dest (dc, b);
                put_dest (sc, a);
              
              }
            break;
          case 0x10:
            op_semantics_119:
              {
                /** 1101 ddd0 dd01 0bit		BMcnd dest */
#line 625 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 625 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 625 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd01 0bit		BMcnd dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 625 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, 1);
                if (condition_true (IMM (1)))
                  put_bit2 (dc, bit, 1);
                else
                  put_bit2 (dc, bit, 0);
              
              }
            break;
          case 0x18:
            op_semantics_120:
              {
                /** 1101 ddd0 dd01 1bit		BNOT dest */
#line 654 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 654 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 654 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd01 1bit		BNOT dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 654 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, 1);
                BIT_OP (dc, bit, !b);
              
              }
            break;
          case 0x20:
            op_semantics_121:
              {
                /** 1101 ddd0 dd10 0bit		BTSTC dest */
#line 737 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 737 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 737 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd10 0bit		BTSTC dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 737 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_dest23 (ddd, dd, 1);
                b = get_bit2 (sc, bit);
                set_zc (!b, b);
                put_bit2 (sc, bit, 0);
              
              }
            break;
          case 0x28:
            op_semantics_122:
              {
                /** 1101 ddd0 dd10 1bit		BTSTS dest */
#line 745 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 745 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 745 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd10 1bit		BTSTS dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 745 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_dest23 (ddd, dd, 1);
                b = get_bit2 (sc, bit);
                set_zc (!b, b);
                put_bit2 (sc, bit, 1);
              
              }
            break;
          case 0x30:
            op_semantics_123:
              {
                /** 1101 ddd0 dd11 0bit		BCLR dest */
#line 612 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 612 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 612 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd11 0bit		BCLR dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 612 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, 1);
                BIT_OP (dc, bit, 0);
              
              }
            break;
          case 0x38:
            op_semantics_124:
              {
                /** 1101 ddd0 dd11 1bit		BSET dest */
#line 719 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 719 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 719 "m32c/m32c.opc"
                int bit ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd0 dd11 1bit		BSET dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  bit = 0x%x\n", bit);
                  }
#line 719 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, 1);
                BIT_OP (dc, bit, 1);
              
              }
            break;
        }
      break;
    case 0xd1:
        GETBYTE();
        switch (op[1] & 0xf8) {
          case 0x00:
          case 0x40:
          case 0x80:
          case 0xc0:
            op_semantics_125:
              {
                /** 1101 sss1 ss00 0dst		LDC src,dest */
#line 1153 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1153 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1153 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 sss1 ss00 0dst		LDC src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1153 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 3);
                dc = decode_cr_b (dst, CR_B_INTB);
                a = get_src (sc);
                put_dest (dc, a);
              
              }
            break;
          case 0x08:
          case 0x48:
          case 0x88:
          case 0xc8:
            goto op_semantics_118;
            break;
          case 0x10:
          case 0x50:
          case 0x90:
          case 0xd0:
            op_semantics_126:
              {
                /** 1101 ddd1 dd01 0src		STC src,dest */
#line 1996 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1996 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1996 "m32c/m32c.opc"
                int src ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd1 dd01 0src		STC src,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  src = 0x%x\n", src);
                  }
#line 1996 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, 4);
                sc = decode_cr_b (src, CR_B_INTB);
                a = get_src (sc);
                put_dest (dc, a);
              
              }
            break;
          case 0x18:
          case 0x58:
          case 0x98:
          case 0xd8:
            op_semantics_127:
              {
                /** 1101 sss1 ss01 1dst		MOVA src,dest */
#line 1351 "m32c/m32c.opc"
                int sss ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1351 "m32c/m32c.opc"
                int ss ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1351 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 sss1 ss01 1dst		MOVA src,dest */",
                           op[0], op[1]);
                    printf("  sss = 0x%x,", sss);
                    printf("  ss = 0x%x,", ss);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1351 "m32c/m32c.opc"
              
                {
                static reg_id map[8] = { r2r0, r3r1, a0, a1 };
                prefix (0, 0, 0);
                sc = decode_src23 (sss, ss, 1);
                if (!sc.mem || !map[dst])
                  UNSUPPORTED();
                put_reg (map[dst], sc.u.addr);
                }
              
              }
            break;
          case 0x28:
              {
                /** 1101 0001 0010 1src		PUSHC src */
#line 1645 "m32c/m32c.opc"
                int src ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0001 0010 1src		PUSHC src */",
                           op[0], op[1]);
                    printf("  src = 0x%x\n", src);
                  }
#line 1645 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_cr_b (src, CR_B_INTB);
                a = get_src (sc);
                put_reg (sp, get_reg (sp) - 4);
                mem_put_si (get_reg (sp), a);
                tprintf("pushsi: %x\n", a);
              
              }
            break;
          case 0x30:
          case 0x38:
          case 0x70:
          case 0x78:
          case 0xb0:
          case 0xb8:
          case 0xf0:
          case 0xf8:
            op_semantics_128:
              {
                /** 1101 ddd1 dd11 cond		SCcond dest */
#line 1763 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1763 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1763 "m32c/m32c.opc"
                int cond ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 ddd1 dd11 cond		SCcond dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  dd = 0x%x,", dd);
                    printf("  cond = 0x%x\n", cond);
                  }
#line 1763 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, 2);
                if (condition_true (cond))
                  put_dest (dc, 1);
                else
                  put_dest (dc, 0);
              
              }
            break;
          case 0xa8:
              {
                /** 1101 0001 1010 1src		PUSHC src */
#line 1636 "m32c/m32c.opc"
                int src ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0001 1010 1src		PUSHC src */",
                           op[0], op[1]);
                    printf("  src = 0x%x\n", src);
                  }
#line 1636 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                sc = decode_cr_b (src, CR_B_DCT0);
                a = get_src (sc);
                put_reg (sp, get_reg (sp) - 2);
                mem_put_hi (get_reg (sp), a);
                tprintf("pushhi: %x\n", a);
              
              }
            break;
          case 0xe8:
              {
                /** 1101 0001 1110 1dst		FSET dest */
#line 981 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0001 1110 1dst		FSET dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 981 "m32c/m32c.opc"
                
                set_flags (1 << dst, 1 << dst);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xd2:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            goto op_semantics_117;
            break;
          case 0x08:
            goto op_semantics_118;
            break;
          case 0x10:
            goto op_semantics_119;
            break;
          case 0x18:
            goto op_semantics_120;
            break;
          case 0x20:
            goto op_semantics_121;
            break;
          case 0x28:
            goto op_semantics_122;
            break;
          case 0x30:
            goto op_semantics_123;
            break;
          case 0x38:
            goto op_semantics_124;
            break;
        }
      break;
    case 0xd3:
        GETBYTE();
        switch (op[1] & 0xf8) {
          case 0x00:
          case 0x40:
          case 0x80:
          case 0xc0:
            goto op_semantics_125;
            break;
          case 0x08:
          case 0x48:
          case 0x88:
          case 0xc8:
            goto op_semantics_118;
            break;
          case 0x10:
          case 0x50:
          case 0x90:
          case 0xd0:
            goto op_semantics_126;
            break;
          case 0x18:
          case 0x58:
          case 0x98:
          case 0xd8:
            goto op_semantics_127;
            break;
          case 0x28:
              {
                /** 1101 0011 0010 1dst		POPC dest */
#line 1558 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0011 0010 1dst		POPC dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1558 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_cr_b (dst, CR_B_INTB);
                a = mem_get_si (get_reg (sp));
                put_reg (sp, get_reg (sp) + 4);
                tprintf("popsi: %x\n", a);
                put_dest (dc, a);
              
              }
            break;
          case 0x30:
          case 0x38:
          case 0x70:
          case 0x78:
          case 0xb0:
          case 0xb8:
          case 0xf0:
          case 0xf8:
            goto op_semantics_128;
            break;
          case 0xa8:
              {
                /** 1101 0011 1010 1dst		POPC dest */
#line 1549 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0011 1010 1dst		POPC dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1549 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_cr_b (dst, CR_B_DCT0);
                a = mem_get_hi (get_reg (sp));
                put_reg (sp, get_reg (sp) + 2);
                tprintf("pophi: %x\n", a);
                put_dest (dc, a);
              
              }
            break;
          case 0xe8:
              {
                /** 1101 0011 1110 1dst		FCLR dest */
#line 973 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0011 1110 1dst		FCLR dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 973 "m32c/m32c.opc"
              
                set_flags (1 << dst, 0);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xd4:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            goto op_semantics_117;
            break;
          case 0x08:
            goto op_semantics_118;
            break;
          case 0x10:
            goto op_semantics_119;
            break;
          case 0x18:
            goto op_semantics_120;
            break;
          case 0x20:
            goto op_semantics_121;
            break;
          case 0x28:
            goto op_semantics_122;
            break;
          case 0x30:
            goto op_semantics_123;
            break;
          case 0x38:
            goto op_semantics_124;
            break;
        }
      break;
    case 0xd5:
        GETBYTE();
        switch (op[1] & 0xf8) {
          case 0x00:
          case 0x40:
          case 0x80:
          case 0xc0:
            goto op_semantics_125;
            break;
          case 0x08:
          case 0x48:
          case 0x88:
          case 0xc8:
            goto op_semantics_118;
            break;
          case 0x10:
          case 0x50:
          case 0x90:
          case 0xd0:
            goto op_semantics_126;
            break;
          case 0x18:
          case 0x58:
          case 0x98:
          case 0xd8:
            goto op_semantics_127;
            break;
          case 0x28:
              {
                /** 1101 0101 0010 1dst		LDC #IMM24,dest */
#line 1133 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0101 0010 1dst		LDC #IMM24,dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1133 "m32c/m32c.opc"
              
                imm = IMM(3);
                dc = decode_cr_b (dst, CR_B_INTB);
                put_dest (dc, imm);
              
              }
            break;
          case 0x30:
          case 0x38:
          case 0x70:
          case 0x78:
          case 0xb0:
          case 0xb8:
          case 0xf0:
          case 0xf8:
            goto op_semantics_128;
            break;
          case 0x68:
              {
                /** 1101 0101 0110 1dst		LDC #IMM24,dest */
#line 1139 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0101 0110 1dst		LDC #IMM24,dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1139 "m32c/m32c.opc"
              
                imm = IMM(3);
                dc = decode_cr_b (dst, CR_B_DMA0);
                put_dest (dc, imm);
              
              }
            break;
          case 0xa8:
              {
                /** 1101 0101 1010 1dst		LDC #IMM16,dest */
#line 1127 "m32c/m32c.opc"
                int dst ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0101 1010 1dst		LDC #IMM16,dest */",
                           op[0], op[1]);
                    printf("  dst = 0x%x\n", dst);
                  }
#line 1127 "m32c/m32c.opc"
              
                imm = IMM(2);
                dc = decode_cr_b (dst, CR_B_DCT0);
                put_dest (dc, imm);
              
              }
            break;
          case 0xe8:
              {
                /** 1101 0101 1110 1flg		LDIPL #IMM */
#line 1173 "m32c/m32c.opc"
                int flg ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 0101 1110 1flg		LDIPL #IMM */",
                           op[0], op[1]);
                    printf("  flg = 0x%x\n", flg);
                  }
#line 1173 "m32c/m32c.opc"
              
                set_flags (0x7000, flg*0x1000);
              
              }
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xd6:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            goto op_semantics_117;
            break;
          case 0x08:
            goto op_semantics_118;
            break;
          case 0x10:
            goto op_semantics_119;
            break;
          case 0x18:
            goto op_semantics_120;
            break;
          case 0x20:
            goto op_semantics_121;
            break;
          case 0x28:
            goto op_semantics_122;
            break;
          case 0x30:
            goto op_semantics_123;
            break;
          case 0x38:
            goto op_semantics_124;
            break;
        }
      break;
    case 0xd7:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            goto op_semantics_125;
            break;
          case 0x08:
            goto op_semantics_118;
            break;
          case 0x10:
            goto op_semantics_126;
            break;
          case 0x18:
            goto op_semantics_127;
            break;
          case 0x30:
          case 0x38:
            goto op_semantics_128;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xd8:
        GETBYTE();
        switch (op[1] & 0x38) {
          case 0x00:
            goto op_semantics_117;
            break;
          case 0x08:
            goto op_semantics_118;
            break;
          case 0x10:
            goto op_semantics_119;
            break;
          case 0x18:
            goto op_semantics_120;
            break;
          case 0x20:
            goto op_semantics_121;
            break;
          case 0x28:
            goto op_semantics_122;
            break;
          case 0x30:
            goto op_semantics_123;
            break;
          case 0x38:
            goto op_semantics_124;
            break;
        }
      break;
    case 0xd9:
        GETBYTE();
        switch (op[1] & 0xb8) {
          case 0x00:
          case 0x80:
            goto op_semantics_125;
            break;
          case 0x08:
          case 0x88:
            goto op_semantics_118;
            break;
          case 0x10:
          case 0x90:
            goto op_semantics_126;
            break;
          case 0x18:
          case 0x98:
            goto op_semantics_127;
            break;
          case 0x28:
              {
                /** 1101 1001 0c10 1cnd		BMcnd C */
#line 634 "m32c/m32c.opc"
                int c ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x01;
#line 634 "m32c/m32c.opc"
                int cnd ATTRIBUTE_UNUSED = op[1] & 0x07;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1101 1001 0c10 1cnd		BMcnd C */",
                           op[0], op[1]);
                    printf("  c = 0x%x,", c);
                    printf("  cnd = 0x%x\n", cnd);
                  }
#line 634 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                if (condition_true (c * 8 + cnd))
                  set_c (1);
                else
                  set_c (0);
              
              }
            break;
          case 0x30:
          case 0x38:
          case 0xb0:
          case 0xb8:
            goto op_semantics_128;
            break;
          default: UNSUPPORTED(); break;
        }
      break;
    case 0xdc:
        {
          /** 1101 1100				JMPS #IMM8 */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 1100				JMPS #IMM8 */",
                     op[0]);
            }
#line 1076 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = IMM(1);
          a = 0xff0000 + mem_get_hi (0xfffe00 - imm * 2);
          put_reg (pc, a);
        
        }
      break;
    case 0xdd:
        {
          /** 1101 1101				JSRS #IMM8 */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 1101				JSRS #IMM8 */",
                     op[0]);
            }
#line 1118 "m32c/m32c.opc"
        
          prefix (0, 0, 0);
          imm = IMM(1);
          a = 0xff0000 + mem_get_hi (0xfffe00 - imm * 2);
          put_reg (sp, get_reg (sp) - 4);
          mem_put_si (get_reg (sp), get_reg (pc));
          put_reg (pc, a);
        
        }
      break;
    case 0xde:
        {
          /** 1101 1110				NOP */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 1110				NOP */",
                     op[0]);
            }
#line 1501 "m32c/m32c.opc"
        
          tprintf("nop\n");
        
        }
      break;
    case 0xdf:
        {
          /** 1101 1111				RTS */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1101 1111				RTS */",
                     op[0]);
            }
#line 1744 "m32c/m32c.opc"
        
          put_reg (pc, mem_get_si (get_reg (sp)));
          put_reg (sp, get_reg (sp) + 4);
        
        }
      break;
    case 0xe0:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            op_semantics_129:
              {
                /** 1110 ddd w dd00 immm		SHL.size #IMM, dest */
#line 1835 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1835 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1835 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1835 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 ddd w dd00 immm		SHL.size #IMM, dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1835 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                shift_op (dc, 0, IMM4(), 1);
              
              }
            break;
          case 0x10:
            op_semantics_130:
              {
                /** 1110 ddd w dd01 immm		CMP.size:Q #IMM,dest */
#line 790 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 790 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 790 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 790 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 ddd w dd01 immm		CMP.size:Q #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 790 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                v = get_src (dc);
                immm = sign_ext (immm, 4);
                cmp (v, immm, w+1);
              
              }
            break;
          case 0x20:
            op_semantics_131:
              {
                /** 1110 ddd w dd10 immm		ROT.size #IMM, dest */
#line 1731 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1731 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1731 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1731 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1110 ddd w dd10 immm		ROT.size #IMM, dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1731 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                rot_op (dc, IMM4(), -1);
              
              }
            break;
          case 0x30:
            op_semantics_132:
              {
                /** 111L ddd w dd11 immm		ADD.size:Q #IMM,dest */
#line 495 "m32c/m32c.opc"
                int L ATTRIBUTE_UNUSED = (op[0] >> 4) & 0x01;
#line 495 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 495 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 495 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 495 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 111L ddd w dd11 immm		ADD.size:Q #IMM,dest */",
                           op[0], op[1]);
                    printf("  L = 0x%x,", L);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 495 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23(ddd, dd, L ? 4 : (w+1));
                imm = sign_ext (immm, 4);
                MATH_OP (dc, imm, 0, +);
              
              }
            break;
        }
      break;
    case 0xe1:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe2:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe3:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe4:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe5:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe6:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe7:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe8:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xe9:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_129;
            break;
          case 0x10:
            goto op_semantics_130;
            break;
          case 0x20:
            goto op_semantics_131;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xec:
        {
          /** 1110 1100				ENTER #IMM */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1110 1100				ENTER #IMM */",
                     op[0]);
            }
#line 930 "m32c/m32c.opc"
        
          imm = IMM(1);
          put_reg (sp, get_reg (sp) - 4);
          mem_put_si (get_reg (sp), get_reg (fb));
          put_reg (fb, get_reg (sp));
          put_reg (sp, get_reg (sp) - imm);
        
        }
      break;
    case 0xf0:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            op_semantics_133:
              {
                /** 1111 ddd w dd00 immm		SHA.size #IMM,dest */
#line 1800 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1800 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1800 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1800 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1111 ddd w dd00 immm		SHA.size #IMM,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1800 "m32c/m32c.opc"
              
                prefix (0, 1, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                shift_op (dc, 1, IMM4(), 1);
              
              }
            break;
          case 0x10:
            op_semantics_134:
              {
                /** 1111 ddd w dd01 immm		ADJNZ.size #IMM,dest,label */
#line 568 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 568 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 568 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 568 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1111 ddd w dd01 immm		ADJNZ.size #IMM,dest,label */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 568 "m32c/m32c.opc"
              
                prefix (0, 0, 0);
                dc = decode_dest23 (ddd, dd, w+1);
                v = get_src (dc);
                imm = sign_ext(immm, 4);
                tprintf("%d + %d = %d\n", v, imm, v+imm);
                v += imm;
                put_dest (dc, v);
                a = sign_ext (IMM(1), 8);
                if ((v & (w ? 0xffff : 0xff)) != 0)
                  {
                    tprintf("jmp: %x + 2 + %d = ", get_reg (pc), a);
                    put_reg (pc, m32c_opcode_pc + 2 + a);
                    tprintf("%x\n", get_reg (pc));
                  }
              
              }
            break;
          case 0x20:
            op_semantics_135:
              {
                /** 1111 ddd w dd10 immm		MOV.size:Q #IMM4,dest */
#line 1241 "m32c/m32c.opc"
                int ddd ATTRIBUTE_UNUSED = (op[0] >> 1) & 0x07;
#line 1241 "m32c/m32c.opc"
                int w ATTRIBUTE_UNUSED = op[0] & 0x01;
#line 1241 "m32c/m32c.opc"
                int dd ATTRIBUTE_UNUSED = (op[1] >> 6) & 0x03;
#line 1241 "m32c/m32c.opc"
                int immm ATTRIBUTE_UNUSED = op[1] & 0x0f;
                if (trace) {
                    printf("\033[33m%s\033[0m  %02x %02x\n",
                           "/** 1111 ddd w dd10 immm		MOV.size:Q #IMM4,dest */",
                           op[0], op[1]);
                    printf("  ddd = 0x%x,", ddd);
                    printf("  w = 0x%x,", w);
                    printf("  dd = 0x%x,", dd);
                    printf("  immm = 0x%x\n", immm);
                  }
#line 1241 "m32c/m32c.opc"
              
                dc = decode_dest23 (ddd, dd, w+1);
                imm = sign_ext (immm, 4);
                v = imm;
                tprintf("%x = %d\n", v, v);
                set_sz(v, w+1);
                put_dest (dc, v);
              
              }
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf1:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf2:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf3:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf4:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf5:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf6:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf7:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf8:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xf9:
        GETBYTE();
        switch (op[1] & 0x30) {
          case 0x00:
            goto op_semantics_133;
            break;
          case 0x10:
            goto op_semantics_134;
            break;
          case 0x20:
            goto op_semantics_135;
            break;
          case 0x30:
            goto op_semantics_132;
            break;
        }
      break;
    case 0xfc:
        {
          /** 1111 1100				EXITD */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1100				EXITD */",
                     op[0]);
            }
#line 938 "m32c/m32c.opc"
        
          put_reg (sp, get_reg (fb));
          put_reg (fb, mem_get_si (get_reg (sp)));
          put_reg (sp, get_reg (sp) + 4);
          put_reg (pc, mem_get_si (get_reg (sp)));
          put_reg (sp, get_reg (sp) + 4);
        
        }
      break;
    case 0xfe:
        {
          /** 1111 1110				GBRK */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1110				GBRK */",
                     op[0]);
            }
#line 693 "m32c/m32c.opc"
        
          /* This alternate break, which is not part of the chip's opcode set,
           is here in case you need to debug a program that itself uses the
           chip's BRK opcode.  You'll need to modify your copy of GDB to use
           this opcode instead of the real BRK.  */
        
          /* GDB Break. */
          /* We report the break to our caller with the PC still pointing at the 
             breakpoint instruction.  */
          put_reg (pc, m32c_opcode_pc);
          if (verbose)
            printf("[gdb break]\n");
          return M32C_MAKE_HIT_BREAK ();
        
        }
      break;
    case 0xff:
        {
          /** 1111 1111				UND */
          if (trace) {
              printf("\033[33m%s\033[0m  %02x\n",
                     "/** 1111 1111				UND */",
                     op[0]);
            }
#line 2115 "m32c/m32c.opc"
        
          trigger_fixed_interrupt (0xffffdc);
        
        }
      break;
    default: UNSUPPORTED(); break;
  }
#line 2147 "m32c/m32c.opc"

  return step_result;
}
