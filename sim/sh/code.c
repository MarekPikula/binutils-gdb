{
/* REG_xy = [r4, r5, r0, r1].  */
#define REG_xy(R) ((R)==0 ? 4 : (R)==2 ? 5 : (R)==1 ?  0 :  1)
/* REG_yx = [r6, r7, r2, r3].  */
#define REG_yx(R) ((R)==0 ? 6 : (R)==1 ? 7 : (R)==2 ?  2 :  3)
/* DSP_ax = [a0, a1, x0, x1].  */
#define DSP_ax(R) ((R)==0 ? 7 : (R)==2 ? 5 : (R)==1 ?  8 :  9)
/* DSP_ay = [a0, a1, y0, y1].  */
#define DSP_ay(R) ((R)==0 ? 7 : (R)==1 ? 5 : (R)==2 ? 10 : 11)
/* DSP_xy = [x0, x1, y0, y1].  */
#define DSP_xy(R) ((R)==0 ? 8 : (R)==2 ? 9 : (R)==1 ? 10 : 11)
/* DSP_yx = [y0, y1, x0, x1].  */
#define DSP_yx(R) ((R)==0 ? 10 : (R)==1 ? 11 : (R)==2 ? 8 : 9)
  switch (jump_table[iword]) {
  /* movi20 #<imm20>,<REG_N> 0000nnnni8*10000 */
  case 1:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff);
      TL (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        R[n] = ((i << 24) >> 12) | RIAT (nip);
        SET_NIP (nip + 2);	/* Consume 2 more bytes.  */
      }
      CDEF (n);
      break;
    }
  /* movi20s #<imm20>,<REG_N> 0000nnnni8*10001 */
  case 2:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff);
      TL (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        R[n] = ((((i & 0xf0) << 24) >> 12) | RIAT (nip)) << 8;
        SET_NIP (nip + 2);	/* Consume 2 more bytes.  */
      }
      CDEF (n);
      break;
    }
  /* stc <CREG_M>,<REG_N> 0000nnnnmmmm0010 */
  case 3:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        R[n] = CREG (m);
      }
      CDEF (n);
      break;
    }
  /* braf <REG_N> 0000nnnn00100011 */
  case 4:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (PC + 4 + R[n]);
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* bsrf <REG_N> 0000nnnn00000011 */
  case 5:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        PR = PH2T (PC) + 4;
        SET_NIP (PC + 4 + R[n]);
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* movca.l R0, @<REG_N> 0000nnnn11000011 */
  case 6:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      CREF (0);
      {
        /* We don't simulate cache, so this insn is identical to mov.  */
        MA (1);
        WLAT (R[n], R[0]);
      }
      break;
    }
  /* movco.l R0, @<REG_N> 0000nnnn01110011 */
  case 7:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      CREF (0);
      {
        /* LDST -> T */
        SET_SR_T (LDST);
        /* if (T) R0 -> (Rn) */
        if (T)
          WLAT (R[n], R[0]);
        /* 0 -> LDST */
        SET_LDST (0);
      }
      break;
    }
  /* movli.l @<REG_N>, R0 0000nnnn01100011 */
  case 8:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        /* 1 -> LDST */
        SET_LDST (1);
        /* (Rn) -> R0 */
        R[0] = RLAT (R[n]);
        /* if (interrupt/exception) 0 -> LDST */
        /* (we don't simulate asynchronous interrupts/exceptions) */
      }
      CDEF (0);
      break;
    }
  /* icbi @<REG_N> 0000nnnn11100011 */
  case 9:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        /* Except for the effect on the cache - which is not simulated -
           this is like a nop.  */
      }
      break;
    }
  /* ocbi @<REG_N> 0000nnnn10010011 */
  case 10:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        (void) RSBAT (R[n]); /* Take exceptions like byte load, otherwise noop.  */
        /* FIXME: Cache not implemented */
      }
      break;
    }
  /* ocbp @<REG_N> 0000nnnn10100011 */
  case 11:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        (void) RSBAT (R[n]); /* Take exceptions like byte load, otherwise noop.  */
        /* FIXME: Cache not implemented */
      }
      break;
    }
  /* ocbwb @<REG_N> 0000nnnn10110011 */
  case 12:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        (void) RSBAT (R[n]); /* Take exceptions like byte load, otherwise noop.  */
        /* FIXME: Cache not implemented */
      }
      break;
    }
  /* pref @<REG_N> 0000nnnn10000011 */
  case 13:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        /* Except for the effect on the cache - which is not simulated -
           this is like a nop.  */
      }
      break;
    }
  /* prefi @<REG_N> 0000nnnn11010011 */
  case 14:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        /* Except for the effect on the cache - which is not simulated -
           this is like a nop.  */
      }
      break;
    }
  /* mov.b <REG_M>,@(R0,<REG_N>) 0000nnnnmmmm0100 */
  case 15:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      CREF (0);
      {
        MA (1);
        WBAT (R[n] + R0, R[m]);
      }
      break;
    }
  /* mov.w <REG_M>,@(R0,<REG_N>) 0000nnnnmmmm0101 */
  case 16:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (0);
      CREF (n);
      {
        MA (1);
        WWAT (R0 + R[n], R[m]);
      }
      break;
    }
  /* mov.l <REG_M>,@(R0,<REG_N>) 0000nnnnmmmm0110 */
  case 17:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      CREF (0);
      {
        MA (1);
        WLAT (R0 + R[n], R[m]);
      }
      break;
    }
  /* mul.l <REG_M>,<REG_N> 0000nnnnmmmm0111 */
  case 18:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        MACL = ((int) R[n]) * ((int) R[m]);
      }
      break;
    }
  /* nott 0000000001101000 */
  case 19:
    {
      {
        SET_SR_T (T == 0);
      }
      break;
    }
  /* clrmac 0000000000101000 */
  case 20:
    {
      {
        MACH = 0;
        MACL = 0;
      }
      break;
    }
  /* clrs 0000000001001000 */
  case 21:
    {
      {
        SET_SR_S (0);
      }
      break;
    }
  /* clrt 0000000000001000 */
  case 22:
    {
      {
        SET_SR_T (0);
      }
      break;
    }
  /* clrdmxy 0000000010001000 */
  case 23:
    {
      {
        saved_state.asregs.sr &= ~(SR_MASK_DMX | SR_MASK_DMY);
      }
      break;
    }
  /* ldtlb 0000000000111000 */
  case 24:
    {
      {
        /* We don't implement cache or tlb, so this is a noop.  */
      }
      break;
    }
  /* setdmx 0000000010011000 */
  case 25:
    {
      {
        saved_state.asregs.sr |=  SR_MASK_DMX;saved_state.asregs.sr &= ~SR_MASK_DMY;
      }
      break;
    }
  /* setdmy 0000000011001000 */
  case 26:
    {
      {
        saved_state.asregs.sr |=  SR_MASK_DMY;saved_state.asregs.sr &= ~SR_MASK_DMX;
      }
      break;
    }
  /* sets 0000000001011000 */
  case 27:
    {
      {
        SET_SR_S (1);
      }
      break;
    }
  /* sett 0000000000011000 */
  case 28:
    {
      {
        SET_SR_T (1);
      }
      break;
    }
  /* div0u 0000000000011001 */
  case 29:
    {
      {
        SET_SR_M (0);
        SET_SR_Q (0);
        SET_SR_T (0);
      }
      break;
    }
  /* movt <REG_N> 0000nnnn00101001 */
  case 30:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        R[n] = T;
      }
      CDEF (n);
      break;
    }
  /* movrt <REG_N> 0000nnnn00111001 */
  case 31:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        R[n] = (T == 0);
      }
      break;
    }
  /* nop 0000000000001001 */
  case 32:
    {
      {
        /* nop */
      }
      break;
    }
  /* stc SGR,<REG_N> 0000nnnn00111010 */
  case 33:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (SR_MD)
          R[n] = SGR; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* stc DBR,<REG_N> 0000nnnn11111010 */
  case 34:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (SR_MD)
          R[n] = DBR; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* stc TBR,<REG_N> 0000nnnn01001010 */
  case 35:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (SR_MD)
          R[n] = TBR; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* sts <SREG_M>,<REG_N> 0000nnnnssss1010 */
  case 36:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TL (n);
      {
        R[n] = SREG (m);
      }
      CDEF (n);
      break;
    }
  /* resbank 0000000001011011 */
  case 37:
    {
      {
        int i;
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (BO) {	/* Bank Overflow */
          for (i = 0; i <= 14; i++) {
            R[i] = RLAT (R[15]);
            MA (1);
            R[15] += 4;
          }
          PR = RLAT (R[15]);
          R[15] += 4;
          MA (1);
          GBR = RLAT (R[15]);
          R[15] += 4;
          MA (1);
          MACH = RLAT (R[15]);
          R[15] += 4;
          MA (1);
          MACL = RLAT (R[15]);
          R[15] += 4;
          MA (1);
        }
        else if (BANKN == 0)	/* Bank Underflow */
          RAISE_EXCEPTION (SIGILL);
        else {
          SET_BANKN (BANKN - 1);
          for (i = 0; i <= 14; i++)
            R[i] = saved_state.asregs.regstack[BANKN].regs[i];
          MACH = saved_state.asregs.regstack[BANKN].regs[15];
          PR   = saved_state.asregs.regstack[BANKN].regs[17];
          GBR  = saved_state.asregs.regstack[BANKN].regs[18];
          MACL = saved_state.asregs.regstack[BANKN].regs[19];
        }
      }
      break;
    }
  /* synco 0000000010101011 */
  case 38:
    {
      {
        /* Except for the effect on the pipeline - which is not simulated -
           this is like a nop.  */
      }
      break;
    }
  /* rte 0000000000101011 */
  case 39:
    {
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_SR (SSR);
        SET_NIP (PT2H (SPC));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* rts 0000000000001011 */
  case 40:
    {
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (PT2H (PR));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* rts/n 0000000001101011 */
  case 41:
    {
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (PT2H (PR));
      }
      break;
    }
  /* rtv/n <REG_N> 0000nnnn01111011 */
  case 42:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        R0 = R[n];
        L (0);
        SET_NIP (PT2H (PR));
      }
      CDEF (0);
      break;
    }
  /* sleep 0000000000011011 */
  case 43:
    {
      {
        nip += trap (sd, 0xc3, &R0, PC, memory, maskl, maskw, endianw);
      }
      break;
    }
  /* mov.b @(R0,<REG_M>),<REG_N> 0000nnnnmmmm1100 */
  case 44:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (0);
      CREF (m);
      {
        MA (1);
        R[n] = RSBAT (R0 + R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov.w @(R0,<REG_M>),<REG_N> 0000nnnnmmmm1101 */
  case 45:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (0);
      {
        MA (1);
        R[n] = RSWAT (R0 + R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov.l @(R0,<REG_M>),<REG_N> 0000nnnnmmmm1110 */
  case 46:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (0);
      {
        MA (1);
        R[n] = RLAT (R0 + R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mac.l @<REG_M>+,@<REG_N>+ 0000nnnnmmmm1111 */
  case 47:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        macl (&R0, memory, n, m);
      }
      CDEF (n);
      CDEF (m);
      break;
    }
  /* mov.l <REG_M>,@(<disp>,<REG_N>) 0001nnnnmmmmi4*4 */
  case 48:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf) << 2;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        MA (1);
        WLAT (i + R[n], R[m]);
      }
      break;
    }
  /* mov.b <REG_M>,@<REG_N> 0010nnnnmmmm0000 */
  case 49:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        MA (1);
        WBAT (R[n], R[m]);
      }
      break;
    }
  /* mov.w <REG_M>,@<REG_N> 0010nnnnmmmm0001 */
  case 50:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        MA (1);
        WWAT (R[n], R[m]);
      }
      break;
    }
  /* mov.l <REG_M>,@<REG_N> 0010nnnnmmmm0010 */
  case 51:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        MA (1);
        WLAT (R[n], R[m]);
      }
      break;
    }
  /* mov.b <REG_M>,@-<REG_N> 0010nnnnmmmm0100 */
  case 52:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        int t = R[m];
        MA (1);
        R[n] -= 1;
        WBAT (R[n], t);
      }
      CDEF (n);
      break;
    }
  /* mov.w <REG_M>,@-<REG_N> 0010nnnnmmmm0101 */
  case 53:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        int t = R[m];
        MA (1);
        R[n] -= 2;
        WWAT (R[n], t);
      }
      CDEF (n);
      break;
    }
  /* mov.l <REG_M>,@-<REG_N> 0010nnnnmmmm0110 */
  case 54:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        int t = R[m];
        MA (1) ;
        R[n] -= 4;
        WLAT (R[n], t);
      }
      CDEF (n);
      break;
    }
  /* div0s <REG_M>,<REG_N> 0010nnnnmmmm0111 */
  case 55:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_Q ((R[n] & sbit) != 0);
        SET_SR_M ((R[m] & sbit) != 0);
        SET_SR_T (M != Q);
      }
      break;
    }
  /* tst <REG_M>,<REG_N> 0010nnnnmmmm1000 */
  case 56:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T ((R[n] & R[m]) == 0);
      }
      break;
    }
  /* and <REG_M>,<REG_N> 0010nnnnmmmm1001 */
  case 57:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        R[n] &= R[m];
      }
      CDEF (n);
      break;
    }
  /* xor <REG_M>,<REG_N> 0010nnnnmmmm1010 */
  case 58:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        R[n] ^= R[m];
      }
      CDEF (n);
      break;
    }
  /* or <REG_M>,<REG_N> 0010nnnnmmmm1011 */
  case 59:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] |= R[m];
      }
      CDEF (n);
      break;
    }
  /* cmp/str <REG_M>,<REG_N> 0010nnnnmmmm1100 */
  case 60:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        ult = R[n] ^ R[m];
        SET_SR_T (((ult & 0xff000000) == 0)
                  | ((ult & 0xff0000) == 0)
                  | ((ult & 0xff00) == 0)
                  | ((ult & 0xff) == 0));
      }
      break;
    }
  /* xtrct <REG_M>,<REG_N> 0010nnnnmmmm1101 */
  case 61:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        R[n] = (((R[n] >> 16) & 0xffff)
                | ((R[m] << 16) & 0xffff0000));
      }
      CDEF (n);
      break;
    }
  /* mulu <REG_M>,<REG_N> 0010nnnnmmmm1110 */
  case 62:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        MACL = (((unsigned int) (unsigned short) R[n])
                * ((unsigned int) (unsigned short) R[m]));
      }
      break;
    }
  /* muls <REG_M>,<REG_N> 0010nnnnmmmm1111 */
  case 63:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        MACL = ((int) (short) R[n]) * ((int) (short) R[m]);
      }
      break;
    }
  /* cmp/eq <REG_M>,<REG_N> 0011nnnnmmmm0000 */
  case 64:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T (R[n] == R[m]);
      }
      break;
    }
  /* fmov.s @(disp12,<REG_N>), <FREG_M> 0011nnnnmmmm0001 */
  case 65:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        /* and fmov.s <FREG_N>, @(disp12,<FREG_M>)
           and mov.bwl <REG_N>, @(disp12,<REG_M>)
           and mov.bwl @(disp12,<REG_N>),<REG_M>
           and movu.bw @(disp12,<REG_N>),<REG_M>.  */
        int word2 = RIAT (nip);
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (nip + 2);	/* Consume 2 more bytes.  */
        MA (1);
        do_long_move_insn (word2 & 0xf000, word2 & 0x0fff, m, n, &thislock);
      }
      break;
    }
  /* cmp/hs <REG_M>,<REG_N> 0011nnnnmmmm0010 */
  case 66:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T (UR[n] >= UR[m]);
      }
      break;
    }
  /* cmp/ge <REG_M>,<REG_N> 0011nnnnmmmm0011 */
  case 67:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T (R[n] >= R[m]);
      }
      break;
    }
  /* div1 <REG_M>,<REG_N> 0011nnnnmmmm0100 */
  case 68:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        div1 (&R0, m, n/*, T*/);
      }
      CDEF (n);
      break;
    }
  /* dmulu.l <REG_M>,<REG_N> 0011nnnnmmmm0101 */
  case 69:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        dmul_u (R[n], R[m]);
      }
      break;
    }
  /* cmp/hi <REG_M>,<REG_N> 0011nnnnmmmm0110 */
  case 70:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T (UR[n] > UR[m]);
      }
      break;
    }
  /* cmp/gt <REG_M>,<REG_N> 0011nnnnmmmm0111 */
  case 71:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        SET_SR_T (R[n] > R[m]);
      }
      break;
    }
  /* sub <REG_M>,<REG_N> 0011nnnnmmmm1000 */
  case 72:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        R[n] -= R[m];
      }
      CDEF (n);
      break;
    }
  /* bit32 #imm3,@(disp12,<REG_N>) 0011nnnni8*11001 */
  case 73:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff);
      TL (n);
      CREF (n);
      {
        /* 32-bit logical bit-manipulation instructions.  */
        int word2 = RIAT (nip);
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        i >>= 4;	/* BOGUS: Using only three bits of 'i'.  */
        /* MSB of 'i' must be zero.  */
        if (i > 7)
          RAISE_EXCEPTION (SIGILL);
        MA (1);
        do_blog_insn (1 << i, (word2 & 0xfff) + R[n],
                      (word2 >> 12) & 0xf, memory, maskb);
        SET_NIP (nip + 2);	/* Consume 2 more bytes.  */
      }
      break;
    }
  /* subc <REG_M>,<REG_N> 0011nnnnmmmm1010 */
  case 74:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        ult = R[n] - T;
        SET_SR_T (ult > R[n]);
        R[n] = ult - R[m];
        SET_SR_T (T || (R[n] > ult));
      }
      CDEF (n);
      break;
    }
  /* subv <REG_M>,<REG_N> 0011nnnnmmmm1011 */
  case 75:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        ult = R[n] - R[m];
        SET_SR_T (((R[n] ^ R[m]) & (ult ^ R[n])) >> 31);
        R[n] = ult;
      }
      CDEF (n);
      break;
    }
  /* add <REG_M>,<REG_N> 0011nnnnmmmm1100 */
  case 76:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        R[n] += R[m];
      }
      CDEF (n);
      break;
    }
  /* dmuls.l <REG_M>,<REG_N> 0011nnnnmmmm1101 */
  case 77:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        dmul_s (R[n], R[m]);
      }
      break;
    }
  /* addc <REG_M>,<REG_N> 0011nnnnmmmm1110 */
  case 78:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        ult = R[n] + T;
        SET_SR_T (ult < R[n]);
        R[n] = ult + R[m];
        SET_SR_T (T || (R[n] < ult));
      }
      CDEF (n);
      break;
    }
  /* addv <REG_M>,<REG_N> 0011nnnnmmmm1111 */
  case 79:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        ult = R[n] + R[m];
        SET_SR_T ((~(R[n] ^ R[m]) & (ult ^ R[n])) >> 31);
        R[n] = ult;
      }
      CDEF (n);
      break;
    }
  /* mulr R0,<REG_N> 0100nnnn10000000 */
  case 80:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (0);
      CREF (n);
      {
        R[n] = (R[n] * R0) & 0xffffffff;
        L (n);
      }
      CDEF (n);
      break;
    }
  /* movmu.l <REG_N>,@-R15 0100nnnn11110000 */
  case 81:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (15);
      {
        int i = n;
        do {
          CREF (i);
        } while (i-- > 0);
      }
      {
        /* Push pr, R14...Rn (if n==15, push pr).  */
        int i = 15;

        do {
          MA (1);
          R[15] -= 4;
          if (i == 15)
            WLAT (R[15], PR);
          else
            WLAT (R[15], R[i]);
        } while (i-- > n);
      }
      CDEF (15);
      break;
    }
  /* dt <REG_N> 0100nnnn00010000 */
  case 82:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n]--;
        SET_SR_T (R[n] == 0);
      }
      CDEF (n);
      break;
    }
  /* shal <REG_N> 0100nnnn00100000 */
  case 83:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] < 0);
        R[n] <<= 1;
      }
      CDEF (n);
      break;
    }
  /* shll <REG_N> 0100nnnn00000000 */
  case 84:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] < 0);
        R[n] <<= 1;
      }
      CDEF (n);
      break;
    }
  /* clips.b <REG_N> 0100nnnn10010001 */
  case 85:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (R[n] < -128 || R[n] > 127) {
          L (n);
          SET_SR_CS (1);
          if (R[n] > 127)
            R[n] = 127;
          else if (R[n] < -128)
            R[n] = -128;
        }
      }
      CDEF (n);
      break;
    }
  /* clipu.b <REG_N> 0100nnnn10000001 */
  case 86:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (R[n] < -256 || R[n] > 255) {
          L (n);
          SET_SR_CS (1);
          R[n] = 255;
        }
      }
      CDEF (n);
      break;
    }
  /* stbank R0,@<REG_N> 0100nnnn11100001 */
  case 87:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (0);
      CREF (n);
      {
        int regn = (R[n] >> 2) & 0x1f;
        int bankn = (R[n] >> 7) & 0x1ff;
        if (regn > 19)
          regn = 19;	/* FIXME what should happen? */
        saved_state.asregs.regstack[bankn].regs[regn] = R0;
      }
      break;
    }
  /* movml.l <REG_N>,@-R15 0100nnnn11110001 */
  case 88:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (15);
      {
        int i = n;
        do {
          CREF (i);
        } while (i-- > 0);
      }
      {
        /* Push Rn...R0 (if n==15, push pr and R14...R0).  */
        do {
          MA (1);
          R[15] -= 4;
          if (n == 15)
            WLAT (R[15], PR);
          else
            WLAT (R[15], R[n]);
        } while (n-- > 0);
      }
      CDEF (15);
      break;
    }
  /* cmp/pz <REG_N> 0100nnnn00010001 */
  case 89:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] >= 0);
      }
      break;
    }
  /* shar <REG_N> 0100nnnn00100001 */
  case 90:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] & 1);
        R[n] = R[n] >> 1;
      }
      CDEF (n);
      break;
    }
  /* shlr <REG_N> 0100nnnn00000001 */
  case 91:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] & 1);
        R[n] = UR[n] >> 1;
      }
      CDEF (n);
      break;
    }
  /* stc.l SGR,@-<REG_N> 0100nnnn00110010 */
  case 92:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
        { /* priv mode */
          MA (1);
          R[n] -= 4;
          WLAT (R[n], SGR);
        }
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* stc.l DBR,@-<REG_N> 0100nnnn11110010 */
  case 93:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
        { /* priv mode */
          MA (1);
          R[n] -= 4;
          WLAT (R[n], DBR);
        }
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* sts.l <SREG_M>,@-<REG_N> 0100nnnnssss0010 */
  case 94:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        WLAT (R[n], SREG (m));
      }
      CDEF (n);
      break;
    }
  /* stc.l <CREG_M>,@-<REG_N> 0100nnnnmmmm0011 */
  case 95:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        WLAT (R[n], CREG (m));
      }
      CDEF (n);
      break;
    }
  /* divs R0,<REG_N> 0100nnnn10010100 */
  case 96:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (0);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (R0 == 0)
          R[n] = 0x7fffffff;
        else if (R0 == -1 && R[n] == 0x80000000)
          R[n] = 0x7fffffff;
        else R[n] /= R0;
        L (n);
      }
      CDEF (n);
      break;
    }
  /* divu R0,<REG_N> 0100nnnn10000100 */
  case 97:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (0);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (R0 == 0)
          R[n] = 0xffffffff;
        /* FIXME: The result may be implementation-defined if it is outside */
        /* the range of signed int (i.e. if R[n] was negative and R0 == 1).  */
        else R[n] = R[n] / (unsigned int) R0;
        L (n);
      }
      CDEF (n);
      break;
    }
  /* movmu.l @R15+,<REG_N> 0100nnnn11110100 */
  case 98:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (15);
      {
        int i = n;
        do {
          CREF (i);
        } while (i++ < 14);
      }
      {
        /* Pop Rn...R14, pr (if n==15, pop pr).  */
        do {
          MA (1);
          if (n == 15)
            PR = RLAT (R[15]);
          else
            R[n] = RLAT (R[15]);
          R[15] += 4;
        } while (n++ < 15);
      }
      CDEF (15);
      break;
    }
  /* ldrc <REG_N> 0100nnnn00110100 */
  case 99:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_RC (R[n]);
        loop = get_loop_bounds_ext (RS, RE, memory, mem_end, maskw, endianw);
        CHECK_INSN_PTR (insn_ptr);
        RE |= 1;
      }
      break;
    }
  /* rotcl <REG_N> 0100nnnn00100100 */
  case 100:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        ult = R[n] < 0;
        R[n] = (R[n] << 1) | T;
        SET_SR_T (ult);
      }
      CDEF (n);
      break;
    }
  /* rotl <REG_N> 0100nnnn00000100 */
  case 101:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] < 0);
        R[n] <<= 1;
        R[n] |= T;
      }
      CDEF (n);
      break;
    }
  /* setrc <REG_N> 0100nnnn00010100 */
  case 102:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_RC (R[n]);
      }
      break;
    }
  /* clips.w <REG_N> 0100nnnn10010101 */
  case 103:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (R[n] < -32768 || R[n] > 32767) {
          L (n);
          SET_SR_CS (1);
          if (R[n] > 32767)
            R[n] = 32767;
          else if (R[n] < -32768)
            R[n] = -32768;
        }
      }
      CDEF (n);
      break;
    }
  /* clipu.w <REG_N> 0100nnnn10000101 */
  case 104:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (R[n] < -65536 || R[n] > 65535) {
          L (n);
          SET_SR_CS (1);
          R[n] = 65535;
        }
      }
      CDEF (n);
      break;
    }
  /* ldbank @<REG_N>,R0 0100nnnn11100101 */
  case 105:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        int regn = (R[n] >> 2) & 0x1f;
        int bankn = (R[n] >> 7) & 0x1ff;
        if (regn > 19)
          regn = 19;	/* FIXME what should happen? */
        R0 = saved_state.asregs.regstack[bankn].regs[regn];
        L (0);
      }
      CDEF (0);
      break;
    }
  /* movml.l @R15+,<REG_N> 0100nnnn11110101 */
  case 106:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (15);
      {
        int i = n;
        do {
          CREF (i);
        } while (i++ < 14);
      }
      {
        /* Pop R0...Rn (if n==15, pop R0...R14 and pr).  */
        int i = 0;

        do {
          MA (1);
          if (i == 15)
            PR = RLAT (R[15]);
          else
            R[i] = RLAT (R[15]);
          R[15] += 4;
        } while (i++ < n);
      }
      CDEF (15);
      break;
    }
  /* cmp/pl <REG_N> 0100nnnn00010101 */
  case 107:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] > 0);
      }
      break;
    }
  /* rotcr <REG_N> 0100nnnn00100101 */
  case 108:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        ult = R[n] & 1;
        R[n] = (UR[n] >> 1) | (T << 31);
        SET_SR_T (ult);
      }
      CDEF (n);
      break;
    }
  /* rotr <REG_N> 0100nnnn00000101 */
  case 109:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR_T (R[n] & 1);
        R[n] = UR[n] >> 1;
        R[n] |= (T << 31);
      }
      CDEF (n);
      break;
    }
  /* ldc.l @<REG_N>+,DBR 0100nnnn11110110 */
  case 110:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
        { /* priv mode */
          MA (1);
          DBR = RLAT (R[n]);
          R[n] += 4;
        }
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* ldc.l @<REG_N>+,SGR 0100nnnn00110110 */
  case 111:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
        { /* priv mode */
          MA (1);
          SGR = RLAT (R[n]);
          R[n] += 4;
        }
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      CDEF (n);
      break;
    }
  /* lds.l @<REG_N>+,<SREG_M> 0100nnnnssss0110 */
  case 112:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        SREG (m) = RLAT (R[n]);
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* lds.l @<REG_N>+,FPSCR 0100nnnn01100110 */
  case 113:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        SET_FPSCR (RLAT (R[n]));
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* ldc.l @<REG_N>+,<CREG_M> 0100nnnnmmmm0111 */
  case 114:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        MA (1);
        CREG (m) = RLAT (R[n]);
        R[n] += 4;
        /* FIXME: user mode */
      }
      CDEF (n);
      break;
    }
  /* ldc.l @<REG_N>+,SR 0100nnnn00000111 */
  case 115:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        SET_SR (RLAT (R[n]));
        R[n] += 4;
        /* FIXME: user mode */
      }
      CDEF (n);
      break;
    }
  /* ldc.l @<REG_N>+,MOD 0100nnnn01010111 */
  case 116:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        SET_MOD (RLAT (R[n]));
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* shll2 <REG_N> 0100nnnn00001000 */
  case 117:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] <<= 2;
      }
      CDEF (n);
      break;
    }
  /* shll8 <REG_N> 0100nnnn00011000 */
  case 118:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] <<= 8;
      }
      CDEF (n);
      break;
    }
  /* shll16 <REG_N> 0100nnnn00101000 */
  case 119:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] <<= 16;
      }
      CDEF (n);
      break;
    }
  /* movua.l @<REG_N>,R0 0100nnnn10101001 */
  case 120:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        int regn = R[n];
        int e = target_little_endian ? 3 : 0;
        MA (1);
        R[0] = (RBAT (regn + (0^e)) << 24) + (RBAT (regn + (1^e)) << 16) +
          (RBAT (regn + (2^e)) << 8) + RBAT (regn + (3^e));
        L (0);
      }
      CDEF (0);
      break;
    }
  /* movua.l @<REG_N>+,R0 0100nnnn11101001 */
  case 121:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        int regn = R[n];
        int e = target_little_endian ? 3 : 0;
        MA (1);
        R[0] = (RBAT (regn + (0^e)) << 24) + (RBAT (regn + (1^e)) << 16) +
          (RBAT (regn + (2^e)) << 8) + RBAT (regn + (3^e));
        R[n] += 4;
        L (0);
      }
      CDEF (0);
      CDEF (n);
      break;
    }
  /* shlr2 <REG_N> 0100nnnn00001001 */
  case 122:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] = UR[n] >> 2;
      }
      CDEF (n);
      break;
    }
  /* shlr8 <REG_N> 0100nnnn00011001 */
  case 123:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] = UR[n] >> 8;
      }
      CDEF (n);
      break;
    }
  /* shlr16 <REG_N> 0100nnnn00101001 */
  case 124:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        R[n] = UR[n] >> 16;
      }
      CDEF (n);
      break;
    }
  /* ldc <REG_N>,DBR 0100nnnn11111010 */
  case 125:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
          DBR = R[n]; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      break;
    }
  /* ldc <REG_N>,SGR 0100nnnn00111010 */
  case 126:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
          SGR = R[n]; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      break;
    }
  /* ldc <REG_N>,TBR 0100nnnn01001010 */
  case 127:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        if (SR_MD)
          TBR = R[n]; /* priv mode */
        else
          RAISE_EXCEPTION (SIGILL); /* user mode */
      }
      break;
    }
  /* lds <REG_N>,<SREG_M> 0100nnnnssss1010 */
  case 128:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TL (n);
      CREF (n);
      {
        SREG (m) = R[n];
      }
      break;
    }
  /* lds <REG_N>,FPSCR 0100nnnn01101010 */
  case 129:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_FPSCR (R[n]);
      }
      break;
    }
  /* jmp @<REG_N> 0100nnnn00101011 */
  case 130:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (PT2H (R[n]));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* jsr @<REG_N> 0100nnnn00001011 */
  case 131:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        PR = PH2T (PC + 4);
        if (~doprofile)
          gotcall (PR, R[n]);
        SET_NIP (PT2H (R[n]));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* jsr/n @<REG_N> 0100nnnn01001011 */
  case 132:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        PR = PH2T (PC + 2);
        if (~doprofile)
          gotcall (PR, R[n]);
        SET_NIP (PT2H (R[n]));
      }
      break;
    }
  /* mov.b @-<REG_N>,R0 0100nnnn11001011 */
  case 133:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        R[n] -= 1;
        R0 = RSBAT (R[n]);
        L (0);
      }
      CDEF (0);
      CDEF (n);
      break;
    }
  /* mov.b R0,@<REG_N>+ 0100nnnn10001011 */
  case 134:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      CREF (0);
      {
        MA (1);
        WBAT (R[n], R0);
        R[n] += 1;
      }
      CDEF (n);
      break;
    }
  /* mov.l @-<REG_N>,R0 0100nnnn11101011 */
  case 135:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        R0 = RLAT (R[n]);
        L (0);
      }
      CDEF (0);
      CDEF (n);
      break;
    }
  /* mov.l R0,@<REG_N>+ 0100nnnn10101011 */
  case 136:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      CREF (0);
      {
        MA (1) ;
        WLAT (R[n], R0);
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* mov.w @-<REG_N>,R0 0100nnnn11011011 */
  case 137:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        R[n] -= 2;
        R0 = RSWAT (R[n]);
        L (0);
      }
      CDEF (0);
      CDEF (n);
      break;
    }
  /* mov.w R0,@<REG_N>+ 0100nnnn10011011 */
  case 138:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (0);
      CREF (n);
      {
        MA (1);
        WWAT (R[n], R0);
        R[n] += 2;
      }
      CDEF (n);
      break;
    }
  /* tas.b @<REG_N> 0100nnnn00011011 */
  case 139:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        MA (1);
        ult = RBAT (R[n]);
        SET_SR_T (ult == 0);
        WBAT (R[n],ult|0x80);
      }
      break;
    }
  /* shad <REG_M>,<REG_N> 0100nnnnmmmm1100 */
  case 140:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        R[n] = (R[m] < 0) ? (R[m]&0x1f ? R[n] >> ((-R[m])&0x1f) : R[n] >> 31) : (R[n] << (R[m] & 0x1f));
      }
      CDEF (n);
      break;
    }
  /* shld <REG_M>,<REG_N> 0100nnnnmmmm1101 */
  case 141:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      CREF (n);
      {
        R[n] = (R[m] < 0) ? (R[m]&0x1f ? UR[n] >> ((-R[m])&0x1f) : 0): (R[n] << (R[m] & 0x1f));
      }
      CDEF (n);
      break;
    }
  /* ldc <REG_N>,<CREG_M> 0100nnnnmmmm1110 */
  case 142:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        CREG (m) = R[n];
        /* FIXME: user mode */
      }
      break;
    }
  /* ldc <REG_N>,SR 0100nnnn00001110 */
  case 143:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_SR (R[n]);
        /* FIXME: user mode */
      }
      break;
    }
  /* ldc <REG_N>,MOD 0100nnnn01011110 */
  case 144:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      CREF (n);
      {
        SET_MOD (R[n]);
      }
      break;
    }
  /* mac.w @<REG_M>+,@<REG_N>+ 0100nnnnmmmm1111 */
  case 145:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        macw (&R0, memory, n, m, endianw);
      }
      CDEF (n);
      CDEF (m);
      break;
    }
  /* mov.l @(<disp>,<REG_M>),<REG_N> 0101nnnnmmmmi4*4 */
  case 146:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf) << 2;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RLAT (i + R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov.b @<REG_M>,<REG_N> 0110nnnnmmmm0000 */
  case 147:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RSBAT (R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov.w @<REG_M>,<REG_N> 0110nnnnmmmm0001 */
  case 148:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RSWAT (R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov.l @<REG_M>,<REG_N> 0110nnnnmmmm0010 */
  case 149:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RLAT (R[m]);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov <REG_M>,<REG_N> 0110nnnnmmmm0011 */
  case 150:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = R[m];
      }
      CDEF (n);
      break;
    }
  /* mov.b @<REG_M>+,<REG_N> 0110nnnnmmmm0100 */
  case 151:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RSBAT (R[m]);
        R[m] += 1;
        L (n);
      }
      CDEF (n);
      CDEF (m);
      break;
    }
  /* mov.w @<REG_M>+,<REG_N> 0110nnnnmmmm0101 */
  case 152:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        MA (1);
        R[n] = RSWAT (R[m]);
        R[m] += 2;
        L (n);
      }
      CDEF (n);
      CDEF (m);
      break;
    }
  /* mov.l @<REG_M>+,<REG_N> 0110nnnnmmmm0110 */
  case 153:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        MA (1);
        R[n] = RLAT (R[m]);
        R[m] += 4;
        L (n);
      }
      CDEF (n);
      CDEF (m);
      break;
    }
  /* not <REG_M>,<REG_N> 0110nnnnmmmm0111 */
  case 154:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = ~R[m];
      }
      CDEF (n);
      break;
    }
  /* swap.b <REG_M>,<REG_N> 0110nnnnmmmm1000 */
  case 155:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        R[n] = ((R[m] & 0xffff0000)
                | ((R[m] << 8) & 0xff00)
                | ((R[m] >> 8) & 0x00ff));
      }
      CDEF (n);
      break;
    }
  /* swap.w <REG_M>,<REG_N> 0110nnnnmmmm1001 */
  case 156:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      CREF (m);
      {
        R[n] = (((R[m] << 16) & 0xffff0000)
                | ((R[m] >> 16) & 0x00ffff));
      }
      CDEF (n);
      break;
    }
  /* negc <REG_M>,<REG_N> 0110nnnnmmmm1010 */
  case 157:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        ult = -T;
        SET_SR_T (ult > 0);
        R[n] = ult - R[m];
        SET_SR_T (T || (R[n] > ult));
      }
      CDEF (n);
      break;
    }
  /* neg <REG_M>,<REG_N> 0110nnnnmmmm1011 */
  case 158:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = - R[m];
      }
      CDEF (n);
      break;
    }
  /* extu.b <REG_M>,<REG_N> 0110nnnnmmmm1100 */
  case 159:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = (R[m] & 0xff);
      }
      CDEF (n);
      break;
    }
  /* extu.w <REG_M>,<REG_N> 0110nnnnmmmm1101 */
  case 160:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = (R[m] & 0xffff);
      }
      CDEF (n);
      break;
    }
  /* exts.b <REG_M>,<REG_N> 0110nnnnmmmm1110 */
  case 161:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = SEXT (R[m]);
      }
      CDEF (n);
      break;
    }
  /* exts.w <REG_M>,<REG_N> 0110nnnnmmmm1111 */
  case 162:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        R[n] = SEXTW (R[m]);
      }
      CDEF (n);
      break;
    }
  /* add #<imm>,<REG_N> 0111nnnni8*1.... */
  case 163:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff);
      TL (n);
      {
        R[n] += SEXT (i);
        if (i == 0) {
          UNDEF(n); /* see #ifdef PARANOID */
          break;
        }
      }
      CDEF (n);
      break;
    }
  /* bf <bdisp8> 10001011i8p1.... */
  case 164:
    {
      int i = (iword & 0xff);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (!T) {
          SET_NIP (PC + 4 + (SEXT (i) * 2));
          cycles += 2;
        }
      }
      break;
    }
  /* bf.s <bdisp8> 10001111i8p1.... */
  case 165:
    {
      int i = (iword & 0xff);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (!T) {
          SET_NIP (PC + 4 + (SEXT (i) * 2));
          cycles += 2;
          Delay_Slot (PC + 2);
        }
      }
      break;
    }
  /* bt <bdisp8> 10001001i8p1.... */
  case 166:
    {
      int i = (iword & 0xff);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (T) {
          SET_NIP (PC + 4 + (SEXT (i) * 2));
          cycles += 2;
        }
      }
      break;
    }
  /* bt.s <bdisp8> 10001101i8p1.... */
  case 167:
    {
      int i = (iword & 0xff);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (T) {
          SET_NIP (PC + 4 + (SEXT (i) * 2));
          cycles += 2;
          Delay_Slot (PC + 2);
        }
      }
      break;
    }
  /* cmp/eq #<imm>,R0 10001000i8*1.... */
  case 168:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        SET_SR_T (R0 == SEXT (i));
      }
      break;
    }
  /* jsr/n @@(<disp>,TBR) 10000011i8p4.... */
  case 169:
    {
      int i = (iword & 0xff) << 2;
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        PR = PH2T (PC + 2);
        if (~doprofile)
          gotcall (PR, i + TBR);
        SET_NIP (PT2H (i + TBR));
      }
      break;
    }
  /* ldre @(<disp>,PC) 10001110i8p1.... */
  case 170:
    {
      int i = (iword & 0xff);
      {
        RE = SEXT (i) * 2 + 4 + PH2T (PC);
      }
      break;
    }
  /* ldrs @(<disp>,PC) 10001100i8p1.... */
  case 171:
    {
      int i = (iword & 0xff);
      {
        RS = SEXT (i) * 2 + 4 + PH2T (PC);
      }
      break;
    }
  /* ldrc #<imm> 10001010i8*1.... */
  case 172:
    {
      int i = (iword & 0xff);
      {
        SET_RC (i);
        loop = get_loop_bounds_ext (RS, RE, memory, mem_end, maskw, endianw);
        CHECK_INSN_PTR (insn_ptr);
        RE |= 1;
      }
      break;
    }
  /* setrc #<imm> 10000010i8*1.... */
  case 173:
    {
      int i = (iword & 0xff);
      {
        if (BUSERROR (RS | RE, maskw))
          RAISE_EXCEPTION (SIGILL);
        else {
          SET_RC (i);
          loop = get_loop_bounds (RS, RE, memory, mem_end, maskw, endianw);
          CHECK_INSN_PTR (insn_ptr);
        }
      }
      break;
    }
  /* bld/st #<imm>, <REG_M> 10000111mmmmi4*1 */
  case 174:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf);
      TL (m);
      CREF (m);
      {
        /* MSB of 'i' is true for load, false for store.  */
        if (i <= 7)
          if (T)
            R[m] |= (1 << i);
          else
            R[m] &= ~(1 << i);
        else
          SET_SR_T ((R[m] & (1 << (i - 8))) != 0);
      }
      break;
    }
  /* bset/clr #<imm>, <REG_M> 10000110mmmmi4*1 */
  case 175:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf);
      TL (m);
      CREF (m);
      {
        /* MSB of 'i' is true for set, false for clear.  */
        if (i <= 7)
          R[m] &= ~(1 << i);
        else
          R[m] |= (1 << (i - 8));
      }
      CDEF (m);
      break;
    }
  /* mov.b @(<disp>,<REG_M>),R0 10000100mmmmi4*1 */
  case 176:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf);
      TL (m);
      CREF (m);
      {
        MA (1);
        R0 = RSBAT (i + R[m]);
        L (0);
      }
      CDEF (0);
      break;
    }
  /* mov.b R0,@(<disp>,<REG_M>) 10000000mmmmi4*1 */
  case 177:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf);
      TL (m);
      CREF (m);
      CREF (0);
      {
        MA (1);
        WBAT (i + R[m], R0);
      }
      break;
    }
  /* mov.w @(<disp>,<REG_M>),R0 10000101mmmmi4*2 */
  case 178:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf) << 1;
      TL (m);
      CREF (m);
      {
        MA (1);
        R0 = RSWAT (i + R[m]);
        L (0);
      }
      CDEF (0);
      break;
    }
  /* mov.w R0,@(<disp>,<REG_M>) 10000001mmmmi4*2 */
  case 179:
    {
      int m = (iword >> 4) & 0xf;
      int i = (iword & 0xf) << 1;
      TL (m);
      CREF (0);
      CREF (m);
      {
        MA (1);
        WWAT (i + R[m], R0);
      }
      break;
    }
  /* mov.w @(<disp>,PC),<REG_N> 1001nnnni8p2.... */
  case 180:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff) << 1;
      TL (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        MA (1);
        R[n] = RSWAT (PH2T (PC + 4 + i));
        L (n);
      }
      CDEF (n);
      break;
    }
  /* bra <bdisp12> 1010i12......... */
  case 181:
    {
      int i = (iword & 0xfff);
      i = (i ^ (1 << 11)) - (1 << 11);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        SET_NIP (PC + 4 + (SEXT12 (i) * 2));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* bsr <bdisp12> 1011i12......... */
  case 182:
    {
      int i = (iword & 0xfff);
      i = (i ^ (1 << 11)) - (1 << 11);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        PR = PH2T (PC + 4);
        SET_NIP (PC + 4 + (SEXT12 (i) * 2));
        cycles += 2;
        Delay_Slot (PC + 2);
      }
      break;
    }
  /* and #<imm>,R0 11001001i8*1.... */
  case 183:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        R0 &= i;
      }
      CDEF (0);
      break;
    }
  /* and.b #<imm>,@(R0,GBR) 11001101i8*1.... */
  case 184:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        MA (1);
        WBAT (GBR + R0, RBAT (GBR + R0) & i);
      }
      break;
    }
  /* mov.b @(<disp>,GBR),R0 11000100i8*1.... */
  case 185:
    {
      int i = (iword & 0xff);
      {
        MA (1);
        R0 = RSBAT (i + GBR);
        L (0);
      }
      CDEF (0);
      break;
    }
  /* mov.b R0,@(<disp>,GBR) 11000000i8*1.... */
  case 186:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        MA (1);
        WBAT (i + GBR, R0);
      }
      break;
    }
  /* mov.l @(<disp>,GBR),R0 11000110i8*4.... */
  case 187:
    {
      int i = (iword & 0xff) << 2;
      {
        MA (1);
        R0 = RLAT (i + GBR);
        L (0);
      }
      CDEF (0);
      break;
    }
  /* mov.l R0,@(<disp>,GBR) 11000010i8*4.... */
  case 188:
    {
      int i = (iword & 0xff) << 2;
      CREF (0);
      {
        MA (1);
        WLAT (i + GBR, R0);
      }
      break;
    }
  /* mov.w @(<disp>,GBR),R0 11000101i8*2.... */
  case 189:
    {
      int i = (iword & 0xff) << 1;
      {
        MA (1);
        R0 = RSWAT (i + GBR);
        L (0);
      }
      CDEF (0);
      break;
    }
  /* mov.w R0,@(<disp>,GBR) 11000001i8*2.... */
  case 190:
    {
      int i = (iword & 0xff) << 1;
      CREF (0);
      {
        MA (1);
        WWAT (i + GBR, R0);
      }
      break;
    }
  /* mova @(<disp>,PC),R0 11000111i8p4.... */
  case 191:
    {
      int i = (iword & 0xff) << 2;
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        R0 = ((i + 4 + PH2T (PC)) & ~0x3);
      }
      CDEF (0);
      break;
    }
  /* or #<imm>,R0 11001011i8*1.... */
  case 192:
    {
      int i = (iword & 0xff);
      {
        R0 |= i;
      }
      CDEF (0);
      break;
    }
  /* or.b #<imm>,@(R0,GBR) 11001111i8*1.... */
  case 193:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        MA (1);
        WBAT (R0 + GBR, (RBAT (R0 + GBR) | i));
      }
      break;
    }
  /* trapa #<imm> 11000011i8*1.... */
  case 194:
    {
      int i = (iword & 0xff);
      {
        long imm = 0xff & i;
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        if (i < 20 || i == 33 || i == 34 || i == 0xc3)
          nip += trap (sd, i, &R0, PC, memory, maskl, maskw, endianw);
        else if (!SR_BL) {
          SSR = GET_SR ();
          SPC = PH2T (PC + 2);
          SET_SR (GET_SR () | SR_MASK_MD | SR_MASK_BL | SR_MASK_RB);
          /* FIXME: EXPEVT = 0x00000160; */
          SET_NIP (PT2H (RLAT (VBR + (imm<<2))));
        }
      }
      CDEF (0);
      break;
    }
  /* tst #<imm>,R0 11001000i8*1.... */
  case 195:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        SET_SR_T ((R0 & i) == 0);
      }
      break;
    }
  /* tst.b #<imm>,@(R0,GBR) 11001100i8*1.... */
  case 196:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        MA (1);
        SET_SR_T ((RBAT (GBR+R0) & i) == 0);
      }
      break;
    }
  /* xor #<imm>,R0 11001010i8*1.... */
  case 197:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        R0 ^= i;
      }
      break;
    }
  /* xor.b #<imm>,@(R0,GBR) 11001110i8*1.... */
  case 198:
    {
      int i = (iword & 0xff);
      CREF (0);
      {
        MA (1);
        ult = RBAT (GBR+R0);
        ult ^= i;
        WBAT (GBR + R0, ult);
      }
      break;
    }
  /* mov.l @(<disp>,PC),<REG_N> 1101nnnni8p4.... */
  case 199:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff) << 2;
      TL (n);
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        MA (1);
        R[n] = RLAT ((PH2T (PC) & ~3) + 4 + i);
        L (n);
      }
      CDEF (n);
      break;
    }
  /* mov #<imm>,<REG_N> 1110nnnni8*1.... */
  case 200:
    {
      int n = (iword >> 8) & 0xf;
      int i = (iword & 0xff);
      TL (n);
      {
        R[n] = SEXT (i);
      }
      CDEF (n);
      break;
    }
  /* fadd <FREG_M>,<FREG_N> 1111nnnnmmmm0000 */
  case 201:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_OP (n, +, m);
      }
      break;
    }
  /* fsub <FREG_M>,<FREG_N> 1111nnnnmmmm0001 */
  case 202:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_OP (n, -, m);
      }
      break;
    }
  /* fmul <FREG_M>,<FREG_N> 1111nnnnmmmm0010 */
  case 203:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_OP (n, *, m);
      }
      break;
    }
  /* fdiv <FREG_M>,<FREG_N> 1111nnnnmmmm0011 */
  case 204:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_OP (n, /, m);
        /* FIXME: check for DP and (n & 1) == 0?  */
      }
      break;
    }
  /* fcmp/eq <FREG_M>,<FREG_N> 1111nnnnmmmm0100 */
  case 205:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_CMP (n, ==, m);
      }
      break;
    }
  /* fcmp/gt <FREG_M>,<FREG_N> 1111nnnnmmmm0101 */
  case 206:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        FP_CMP (n, >, m);
      }
      break;
    }
  /* fmov.s @(R0,<REG_M>),<FREG_N> 1111nnnnmmmm0110 */
  case 207:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (0);
      CREF (m);
      {
        if (FPSCR_SZ) {
          MA (2);
          RDAT (R[0]+R[m], n);
        }
        else
        {
          MA (1);
          SET_FI (n, RLAT (R[0] + R[m]));
        }
      }
      break;
    }
  /* fmov.s <FREG_M>,@(R0,<REG_N>) 1111nnnnmmmm0111 */
  case 208:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (0);
      CREF (n);
      {
        if (FPSCR_SZ) {
          MA (2);
          WDAT (R[0]+R[n], m);
        }
        else
        {
          MA (1);
          WLAT ((R[0]+R[n]), FI (m));
        }
      }
      break;
    }
  /* fmov.s @<REG_M>,<FREG_N> 1111nnnnmmmm1000 */
  case 209:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        if (FPSCR_SZ) {
          MA (2);
          RDAT (R[m], n);
        }
        else
        {
          MA (1);
          SET_FI (n, RLAT (R[m]));
        }
      }
      break;
    }
  /* fmov.s @<REG_M>+,<FREG_N> 1111nnnnmmmm1001 */
  case 210:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (m);
      {
        if (FPSCR_SZ) {
          MA (2);
          RDAT (R[m], n);
          R[m] += 8;
        }
        else
        {
          MA (1);
          SET_FI (n, RLAT (R[m]));
          R[m] += 4;
        }
      }
      CDEF (m);
      break;
    }
  /* fmov.s <FREG_M>,@<REG_N> 1111nnnnmmmm1010 */
  case 211:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        if (FPSCR_SZ) {
          MA (2);
          WDAT (R[n], m);
        }
        else
        {
          MA (1);
          WLAT (R[n], FI (m));
        }
      }
      break;
    }
  /* fmov.s <FREG_M>,@-<REG_N> 1111nnnnmmmm1011 */
  case 212:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      CREF (n);
      {
        if (FPSCR_SZ) {
          MA (2);
          R[n] -= 8;
          WDAT (R[n], m);
        }
        else
        {
          MA (1);
          R[n] -= 4;
          WLAT (R[n], FI (m));
        }
      }
      CDEF (n);
      break;
    }
  /* fmov <FREG_M>,<FREG_N> 1111nnnnmmmm1100 */
  case 213:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        if (FPSCR_SZ) {
          int ni = XD_TO_XF (n);
          int mi = XD_TO_XF (m);
          SET_XF (ni + 0, XF (mi + 0));
          SET_XF (ni + 1, XF (mi + 1));
        }
        else
        {
          SET_FR (n, FR (m));
        }
      }
      break;
    }
  /* fabs <FREG_N> 1111nnnn01011101 */
  case 214:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
          union
          {
            unsigned int i;
            float f;
          } u;
          u.f = FR (n);
          u.i &= 0x7fffffff;
          SET_FR (n, u.f);
      }
      break;
    }
  /* fcnvds <DR_N>,FPUL 1111nnnn10111101 */
  case 215:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (! FPSCR_PR || n & 1)
          RAISE_EXCEPTION (SIGILL);
        else
        {
          union
          {
            int i;
            float f;
          } u;
          u.f = DR (n);
          FPUL = u.i;
        }
      }
      break;
    }
  /* fcnvsd FPUL,<DR_N> 1111nnnn10101101 */
  case 216:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (! FPSCR_PR || n & 1)
          RAISE_EXCEPTION (SIGILL);
        else
        {
          union
          {
            int i;
            float f;
          } u;
          u.i = FPUL;
          SET_DR (n, u.f);
        }
      }
      break;
    }
  /* fipr <FV_M>,<FV_N> 1111vvVV11101101 */
  case 217:
    {
      int v1 = ((iword >> 10) & 3) * 4;
      int v2 = ((iword >> 8)  & 3) * 4;
      {
        if (FPSCR_PR)
          RAISE_EXCEPTION (SIGILL);
        else
        {
          double fsum = 0;
          if (saved_state.asregs.bfd_mach == bfd_mach_sh2a)
            RAISE_EXCEPTION (SIGILL);
          /* FIXME: check for nans and infinities.  */
          fsum += FR (v1+0) * FR (v2+0);
          fsum += FR (v1+1) * FR (v2+1);
          fsum += FR (v1+2) * FR (v2+2);
          fsum += FR (v1+3) * FR (v2+3);
          SET_FR (v1+3, fsum);
        }
      }
      break;
    }
  /* fldi0 <FREG_N> 1111nnnn10001101 */
  case 218:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        SET_FR (n, (float) 0.0);
        /* FIXME: check for DP and (n & 1) == 0?  */
      }
      break;
    }
  /* fldi1 <FREG_N> 1111nnnn10011101 */
  case 219:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        SET_FR (n, (float) 1.0);
        /* FIXME: check for DP and (n & 1) == 0?  */
      }
      break;
    }
  /* flds <FREG_N>,FPUL 1111nnnn00011101 */
  case 220:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
          union
          {
            int i;
            float f;
          } u;
          u.f = FR (n);
          FPUL = u.i;
      }
      break;
    }
  /* float FPUL,<FREG_N> 1111nnnn00101101 */
  case 221:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (FPSCR_PR)
          SET_DR (n, (double) FPUL);
        else
        {
          SET_FR (n, (float) FPUL);
        }
      }
      break;
    }
  /* fneg <FREG_N> 1111nnnn01001101 */
  case 222:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
          union
          {
            unsigned int i;
            float f;
          } u;
          u.f = FR (n);
          u.i ^= 0x80000000;
          SET_FR (n, u.f);
      }
      break;
    }
  /* fpchg 1111011111111101 */
  case 223:
    {
      {
        SET_FPSCR (GET_FPSCR () ^ FPSCR_MASK_PR);
      }
      break;
    }
  /* frchg 1111101111111101 */
  case 224:
    {
      {
        if (FPSCR_PR)
          RAISE_EXCEPTION (SIGILL);
        else if (saved_state.asregs.bfd_mach == bfd_mach_sh2a)
          RAISE_EXCEPTION (SIGILL);
        else
          SET_FPSCR (GET_FPSCR () ^ FPSCR_MASK_FR);
      }
      break;
    }
  /* fsca 1111eeee11111101 */
  case 225:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (FPSCR_PR)
          RAISE_EXCEPTION (SIGILL);
        else if (saved_state.asregs.bfd_mach == bfd_mach_sh2a)
          RAISE_EXCEPTION (SIGILL);
        else
          {
            SET_FR (n, fsca_s (FPUL, &sin));
            SET_FR (n+1, fsca_s (FPUL, &cos));
          }
      }
      break;
    }
  /* fschg 1111001111111101 */
  case 226:
    {
      {
        SET_FPSCR (GET_FPSCR () ^ FPSCR_MASK_SZ);
      }
      break;
    }
  /* fsqrt <FREG_N> 1111nnnn01101101 */
  case 227:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        FP_UNARY (n, sqrt);
      }
      break;
    }
  /* fsrra <FREG_N> 1111nnnn01111101 */
  case 228:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (FPSCR_PR)
          RAISE_EXCEPTION (SIGILL);
        else if (saved_state.asregs.bfd_mach == bfd_mach_sh2a)
          RAISE_EXCEPTION (SIGILL);
        else
          SET_FR (n, fsrra_s (FR (n)));
      }
      break;
    }
  /* ftrc <FREG_N>, FPUL 1111nnnn00111101 */
  case 229:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
        if (FPSCR_PR) {
          if (DR (n) != DR (n)) /* NaN */
            FPUL = 0x80000000;
          else
            FPUL =  (int) DR (n);
        }
        else
        if (FR (n) != FR (n)) /* NaN */
          FPUL = 0x80000000;
        else
          FPUL = (int) FR (n);
      }
      break;
    }
  /* ftrv <FV_N> 1111vv0111111101 */
  case 230:
    {
      int v1 = ((iword >> 10) & 3) * 4;
      {
        if (FPSCR_PR)
          RAISE_EXCEPTION (SIGILL);
        else
        {
          if (saved_state.asregs.bfd_mach == bfd_mach_sh2a)
            RAISE_EXCEPTION (SIGILL);
          /* FIXME not implemented.  */
          printf ("ftrv xmtrx, FV%d\n", v1);
        }
      }
      break;
    }
  /* fsts FPUL,<FREG_N> 1111nnnn00001101 */
  case 231:
    {
      int n = (iword >> 8) & 0xf;
      TL (n);
      {
          union
          {
            int i;
            float f;
          } u;
          u.i = FPUL;
          SET_FR (n, u.f);
      }
      break;
    }
  /* fmac <FREG_0>,<FREG_M>,<FREG_N> 1111nnnnmmmm1110 */
  case 232:
    {
      int n = (iword >> 8) & 0xf;
      int m = (iword >> 4) & 0xf;
      TB (m,n);
      {
        SET_FR (n, FR (m) * FR (0) + FR (n));
        /* FIXME: check for DP and (n & 1) == 0? */
      }
      break;
    }
  /* movs.w @-<REG_N>,<DSP_REG_M> 111101NNMMMM0000 */
  case 233:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 2;
        DSP_R (m) = RSWAT (R[n]) << 16;
        DSP_GRD (m) = SIGN32 (DSP_R (m));
      }
      CDEF (n);
      break;
    }
  /* movs.w @<REG_N>,<DSP_REG_M> 111101NNMMMM0100 */
  case 234:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]) << 16;
        DSP_GRD (m) = SIGN32 (DSP_R (m));
      }
      break;
    }
  /* movs.w @<REG_N>+,<DSP_REG_M> 111101NNMMMM1000 */
  case 235:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]) << 16;
        DSP_GRD (m) = SIGN32 (DSP_R (m));
        R[n] += 2;
      }
      CDEF (n);
      break;
    }
  /* movs.w @<REG_N>+REG_8,<DSP_REG_M> 111101NNMMMM1100 */
  case 236:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]) << 16;
        DSP_GRD (m) = SIGN32 (DSP_R (m));
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.w @-<REG_N>,<DSP_GRD_M> 111101NNGGGG0000 */
  case 237:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 2;
        DSP_R (m) = RSWAT (R[n]);
      }
      CDEF (n);
      break;
    }
  /* movs.w @<REG_N>,<DSP_GRD_M> 111101NNGGGG0100 */
  case 238:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]);
      }
      break;
    }
  /* movs.w @<REG_N>+,<DSP_GRD_M> 111101NNGGGG1000 */
  case 239:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]);
        R[n] += 2;
      }
      CDEF (n);
      break;
    }
  /* movs.w @<REG_N>+REG_8,<DSP_GRD_M> 111101NNGGGG1100 */
  case 240:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        DSP_R (m) = RSWAT (R[n]);
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_REG_M>,@-<REG_N> 111101NNMMMM0001 */
  case 241:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 2;
        WWAT (R[n], DSP_R (m) >> 16);
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_REG_M>,@<REG_N> 111101NNMMMM0101 */
  case 242:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WWAT (R[n], DSP_R (m) >> 16);
      }
      break;
    }
  /* movs.w <DSP_REG_M>,@<REG_N>+ 111101NNMMMM1001 */
  case 243:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += 2;
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_REG_M>,@<REG_N>+REG_8 111101NNMMMM1101 */
  case 244:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_GRD_M>,@-<REG_N> 111101NNGGGG0001 */
  case 245:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 2;
        WWAT (R[n], SEXT (DSP_R (m)));
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_GRD_M>,@<REG_N> 111101NNGGGG0101 */
  case 246:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WWAT (R[n], SEXT (DSP_R (m)));
      }
      break;
    }
  /* movs.w <DSP_GRD_M>,@<REG_N>+ 111101NNGGGG1001 */
  case 247:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WWAT (R[n], SEXT (DSP_R (m)));
        R[n] += 2;
      }
      CDEF (n);
      break;
    }
  /* movs.w <DSP_GRD_M>,@<REG_N>+REG_8 111101NNGGGG1101 */
  case 248:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        WWAT (R[n], SEXT (DSP_R (m)));
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.l @-<REG_N>,<DSP_REG_M> 111101NNMMMM0010 */
  case 249:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        DSP_R (m) = RLAT (R[n]);
        DSP_GRD (m) = SIGN32 (DSP_R (m));
      }
      CDEF (n);
      break;
    }
  /* movs.l @<REG_N>,<DSP_REG_M> 111101NNMMMM0110 */
  case 250:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RLAT (R[n]);
        DSP_GRD (m) = SIGN32 (DSP_R (m));
      }
      break;
    }
  /* movs.l @<REG_N>+,<DSP_REG_M> 111101NNMMMM1010 */
  case 251:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        DSP_R (m) = RLAT (R[n]);
        DSP_GRD (m) = SIGN32 (DSP_R (m));
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* movs.l @<REG_N>+REG_8,<DSP_REG_M> 111101NNMMMM1110 */
  case 252:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        DSP_R (m) = RLAT (R[n]);
        DSP_GRD (m) = SIGN32 (DSP_R (m));
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_REG_M>,@-<REG_N> 111101NNMMMM0011 */
  case 253:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        WLAT (R[n], DSP_R (m));
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_REG_M>,@<REG_N> 111101NNMMMM0111 */
  case 254:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WLAT (R[n], DSP_R (m));
      }
      break;
    }
  /* movs.l <DSP_REG_M>,@<REG_N>+ 111101NNMMMM1011 */
  case 255:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WLAT (R[n], DSP_R (m));
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_REG_M>,@<REG_N>+REG_8 111101NNMMMM1111 */
  case 256:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        WLAT (R[n], DSP_R (m));
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_GRD_M>,@-<REG_N> 111101NNGGGG0011 */
  case 257:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        R[n] -= 4;
        WLAT (R[n], SEXT (DSP_R (m)));
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_GRD_M>,@<REG_N> 111101NNGGGG0111 */
  case 258:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WLAT (R[n], SEXT (DSP_R (m)));
      }
      break;
    }
  /* movs.l <DSP_GRD_M>,@<REG_N>+ 111101NNGGGG1011 */
  case 259:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      {
        MA (1);
        WLAT (R[n], SEXT (DSP_R (m)));
        R[n] += 4;
      }
      CDEF (n);
      break;
    }
  /* movs.l <DSP_GRD_M>,@<REG_N>+REG_8 111101NNGGGG1111 */
  case 260:
    {
      int n = (((iword >> 8) - 2) & 0x3) + 2;
      int m = (iword >> 4) & 0xf;
      CREF (n);
      CREF (8);
      {
        MA (1);
        WLAT (R[n], SEXT (DSP_R (m)));
        R[n] += R[8];
      }
      CDEF (n);
      break;
    }
  /* movx.w @<REG_xy>,<DSP_XY> 111100xyXY0001?? */
  case 261:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      break;
    }
  /* movx.l @<REG_xy>,<DSP_XY> 111100xyXY010100 */
  case 262:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RLAT (R[n]);
      }
      break;
    }
  /* movx.w @<REG_xy>+,<DSP_XY> 111100xyXY0010?? */
  case 263:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : 2;
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      CDEF (n);
      break;
    }
  /* movx.l @<REG_xy>+,<DSP_XY> 111100xyXY011000 */
  case 264:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RLAT (R[n]);
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : 4;
      }
      CDEF (n);
      break;
    }
  /* movx.w @<REG_xy>+REG_8,<DSP_XY> 111100xyXY0011?? */
  case 265:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (8);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : R[8];
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      CDEF (n);
      break;
    }
  /* movx.l @<REG_xy>+REG_8,<DSP_XY> 111100xyXY011100 */
  case 266:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              ((iword >> 7) & 1) + 8 :
              DSP_xy ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (8);
      {
        DSP_R (m) = RLAT (R[n]);
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : R[8];
      }
      CDEF (n);
      break;
    }
  /* movx.w <DSP_Ax>,@<REG_xy> 111100xyax1001?? */
  case 267:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WWAT (R[n], DSP_R (m) >> 16);
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      break;
    }
  /* movx.l <DSP_Ax>,@<REG_xy> 111100xyax110100 */
  case 268:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WLAT (R[n], DSP_R (m));
      }
      break;
    }
  /* movx.w <DSP_Ax>,@<REG_xy>+ 111100xyax1010?? */
  case 269:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : 2;
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      CDEF (n);
      break;
    }
  /* movx.l <DSP_Ax>,@<REG_xy>+ 111100xyax111000 */
  case 270:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WLAT (R[n], DSP_R (m));
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : 4;
      }
      CDEF (n);
      break;
    }
  /* movx.w <DSP_Ax>,@<REG_xy>+REG_8 111100xyax1011?? */
  case 271:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (8);
      {
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : R[8];
        if (iword & 3)
          {
            iword &= 0xfd53; goto top;
          }
      }
      CDEF (n);
      break;
    }
  /* movx.l <DSP_Ax>,@<REG_xy>+REG_8 111100xyax111100 */
  case 272:
    {
      int n = (iword & 3) ?
              ((iword >> 9) & 1) + 4 :
              REG_xy ((iword >> 8) & 3);
      int m = (iword & 3) ?
              7 - ((iword >> 6) & 2) :
              DSP_ax ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (8);
      {
        WLAT (R[n], DSP_R (m));
        R[n] += ((R[n] & 0xffff) == MOD_ME) ? MOD_DELTA : R[8];
      }
      CDEF (n);
      break;
    }
  /* movy.w @<REG_yx>,<DSP_YX> 111100yxYX000001 */
  case 273:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
      }
      break;
    }
  /* movy.w @<REG_yx>+,<DSP_YX> 111100yxYX000010 */
  case 274:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : 2;
      }
      CDEF (n);
      break;
    }
  /* movy.w @<REG_yx>+REG_9,<DSP_YX> 111100yxYX000011 */
  case 275:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (9);
      {
        DSP_R (m) = RSWAT (R[n]) << 16;
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : R[9];
      }
      CDEF (n);
      break;
    }
  /* movy.w <DSP_Ay>,@<REG_yx> 111100yxAY010001 */
  case 276:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WWAT (R[n], DSP_R (m) >> 16);
      }
      break;
    }
  /* movy.w <DSP_Ay>,@<REG_yx>+ 111100yxAY010010 */
  case 277:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : 2;
      }
      CDEF (n);
      break;
    }
  /* movy.w <DSP_Ay>,@<REG_yx>+REG_9 111100yxAY010011 */
  case 278:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (9);
      {
        WWAT (R[n], DSP_R (m) >> 16);
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : R[9];
      }
      CDEF (n);
      break;
    }
  /* movy.l @<REG_yx>,<DSP_YX> 111100yxYX100001 */
  case 279:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RLAT (R[n]);
      }
      break;
    }
  /* movy.l @<REG_yx>+,<DSP_YX> 111100yxYX100010 */
  case 280:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        DSP_R (m) = RLAT (R[n]);
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : 4;
      }
      CDEF (n);
      break;
    }
  /* movy.l @<REG_yx>+REG_9,<DSP_YX> 111100yxYX100011 */
  case 281:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              ((iword >> 6) & 1) + 10 :
              DSP_yx ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (9);
      {
        DSP_R (m) = RLAT (R[n]);
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : R[9];
      }
      CDEF (n);
      break;
    }
  /* movy.l <DSP_Ay>,@<REG_yx> 111100yxAY110001 */
  case 282:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WLAT (R[n], DSP_R (m));
      }
      break;
    }
  /* movy.l <DSP_Ay>,@<REG_yx>+ 111100yxAY110010 */
  case 283:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      {
        WLAT (R[n], DSP_R (m));
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : 4;
      }
      CDEF (n);
      break;
    }
  /* movy.l <DSP_Ay>,@<REG_yx>+REG_9 111100yxAY110011 */
  case 284:
    {
      int n = (iword & 0xc) ?
              ((iword >> 8) & 1) + 6 :
              REG_yx ((iword >> 8) & 3);
      int m = (iword & 0xc) ?
              7 - ((iword >> 5) & 2) :
              DSP_ay ((iword >> 6) & 3);
      TL (n);
      CREF (n);
      CREF (9);
      {
        WLAT (R[n], DSP_R (m));
        R[n] += ((R[n] | ~0xffff) == MOD_ME) ? MOD_DELTA : R[9];
      }
      CDEF (n);
      break;
    }
  /* nopx nopy 1111000000000000 */
  case 285:
    {
      {
        /* nop */
      }
      break;
    }
  /* ppi 1111100000000000 */
  case 286:
    {
      {
        RAISE_EXCEPTION_IF_IN_DELAY_SLOT ();
        ppi_insn (RIAT (nip));
        SET_NIP (nip + 2);
        iword &= 0xf7ff; goto top;
      }
      break;
    }
  default:
    {
      RAISE_EXCEPTION (SIGILL);
    }
  }
}
