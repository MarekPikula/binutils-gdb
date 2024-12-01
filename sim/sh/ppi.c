#define DSR_MASK_G 0x80
#define DSR_MASK_Z 0x40
#define DSR_MASK_N 0x20
#define DSR_MASK_V 0x10

#define COMPUTE_OVERFLOW do {\
  overflow = res_grd != SIGN32 (res) ? DSR_MASK_V : 0;\
  if (overflow && S) \
    { \
      if (res_grd & 0x80) \
        { \
          res = 0x80000000;\
          res_grd |=  0xff;\
        } \
      else \
        { \
          res = 0x7fffffff;\
          res_grd &= ~0xff;\
        } \
      overflow = 0;\
    } \
} while (0)

#define ADD_SUB_GE \
  (greater_equal = ~(overflow << 3 & res_grd) & DSR_MASK_G)

static void
ppi_insn (int iword)
{
  /* 'ee' = [x0, x1, y0, a1] */
  static char const e_tab[] = { 8,  9, 10,  5};
  /* 'ff' = [y0, y1, x0, a1] */
  static char const f_tab[] = {10, 11,  8,  5};
  /* 'xx' = [x0, x1, a0, a1]  */
  static char const x_tab[] = { 8,  9,  7,  5};
  /* 'yy' = [y0, y1, m0, m1]  */
  static char const y_tab[] = {10, 11, 12, 14};
  /* 'gg' = [m0, m1, a0, a1]  */
  static char const g_tab[] = {12, 14,  7,  5};
  /* 'uu' = [x0, y0, a0, a1]  */
  static char const u_tab[] = { 8, 10,  7,  5};

  int z;
  int res, res_grd;
  int carry, overflow, greater_equal;

  switch (ppi_table[iword >> 4]) {
  /* pshl #<imm>,dz 00000iiim16.zzzz */
  case 1:
    {
      int i = (iword >> 4) & 0x7f;
      z = iword & 0xf;
      {
        int Sz = DSP_R (z) & 0xffff0000;

        if (i <= 16)
          res = Sz << i;
        else if (i >= 128 - 16)
          res = (unsigned) Sz >> (128 - i);	/* no sign extension */
        else
          {
            RAISE_EXCEPTION (SIGILL);
            return;
          }
        res &= 0xffff0000;
        res_grd = 0;
        goto logical;
      }
      break;
    }
  /* psha #<imm>,dz 00010iiim32.zzzz */
  case 2:
    {
      int i = (iword >> 4) & 0x7f;
      z = iword & 0xf;
      {
        int Sz = DSP_R (z);
        int Sz_grd = GET_DSP_GRD (z);

        if (i <= 32)
          {
            if (i == 32)
              {
                res = 0;
                res_grd = Sz;
              }
            else
              {
                res = Sz << i;
                res_grd = Sz_grd << i | (unsigned) Sz >> (32 - i);
              }
            res_grd = SEXT (res_grd);
            carry = res_grd & 1;
          }
        else if (i >= 96)
          {
            i = 128 - i;
            if (i == 32)
              {
                res_grd = SIGN32 (Sz_grd);
                res = Sz_grd;
              }
            else
              {
                res = Sz >> i | Sz_grd << (32 - i);
                res_grd = Sz_grd >> i;
              }
            carry = Sz >> (i - 1) & 1;
          }
        else
          {
            RAISE_EXCEPTION (SIGILL);
            return;
          }
        COMPUTE_OVERFLOW;
        greater_equal = 0;
      }
      break;
    }
  /* pmuls Se,Sf,Dg 0100eeff0000gg00 */
  case 3:
    {
      int e = e_tab[(iword >> 10) & 3];
      int f = f_tab[(iword >> 8) & 3];
      int g = g_tab[(iword >> 2) & 3];

      res = (DSP_R (e) >> 16) * (DSP_R (f) >> 16) * 2;
      if (res == 0x80000000)
        res = 0x7fffffff;
      DSP_R (g) = res;
      DSP_GRD (g) = SIGN32 (res);
      return;
      break;
    }
  /* psub Sx,Sy,Du pmuls Se,Sf,Dg 0110eeffxxyygguu */
  case 4:
    {
      int e = e_tab[(iword >> 10) & 3];
      int f = f_tab[(iword >> 8) & 3];
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];
      int g = g_tab[(iword >> 2) & 3];
      int u = u_tab[(iword >> 0) & 3];

      int Sx = DSP_R (x);
      int Sx_grd = GET_DSP_GRD (x);
      int Sy = DSP_R (y);
      int Sy_grd = SIGN32 (Sy);

      res = (DSP_R (e) >> 16) * (DSP_R (f) >> 16) * 2;
      if (res == 0x80000000)
        res = 0x7fffffff;
      DSP_R (g) = res;
      DSP_GRD (g) = SIGN32 (res);

      z = u;
      res = Sx - Sy;
      carry = (unsigned) res > (unsigned) Sx;
      res_grd = Sx_grd - Sy_grd - carry;
      COMPUTE_OVERFLOW;
      ADD_SUB_GE;
      break;
    }
  /* padd Sx,Sy,Du pmuls Se,Sf,Dg 0111eeffxxyygguu */
  case 5:
    {
      int e = e_tab[(iword >> 10) & 3];
      int f = f_tab[(iword >> 8) & 3];
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];
      int g = g_tab[(iword >> 2) & 3];
      int u = u_tab[(iword >> 0) & 3];

      int Sx = DSP_R (x);
      int Sx_grd = GET_DSP_GRD (x);
      int Sy = DSP_R (y);
      int Sy_grd = SIGN32 (Sy);

      res = (DSP_R (e) >> 16) * (DSP_R (f) >> 16) * 2;
      if (res == 0x80000000)
        res = 0x7fffffff;
      DSP_R (g) = res;
      DSP_GRD (g) = SIGN32 (res);

      z = u;
      res = Sx + Sy;
      carry = (unsigned) res < (unsigned) Sx;
      res_grd = Sx_grd + Sy_grd + carry;
      COMPUTE_OVERFLOW;
      break;
    }
  /* psubc Sx,Sy,Dz 10100000xxyyzzzz */
  case 6:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sx - Sy - (DSR & 1);
        carry = (unsigned) res > (unsigned) Sx || (res == Sx && Sy);
        res_grd = Sx_grd + Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        DSR &= ~0xf1;

        if (res || res_grd)

          DSR |= greater_equal | (res_grd >> 2 & DSR_MASK_N) | overflow;

        else

          DSR |= DSR_MASK_Z | overflow;

        DSR |= carry;

        goto assign_z;

      }
      break;
    }
  /* paddc Sx,Sy,Dz 10110000xxyyzzzz */
  case 7:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sx + Sy + (DSR & 1);
        carry = (unsigned) res < (unsigned) Sx || (res == Sx && Sy);
        res_grd = Sx_grd + Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        DSR &= ~0xf1;

        if (res || res_grd)

          DSR |= greater_equal | (res_grd >> 2 & DSR_MASK_N) | overflow;

        else

          DSR |= DSR_MASK_Z | overflow;

        DSR |= carry;

        goto assign_z;

      }
      break;
    }
  /* pcmp Sx,Sy 10000100xxyyzzzz */
  case 8:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        z = 17; /* Ignore result.  */
        res = Sx - Sy;
        carry = (unsigned) res > (unsigned) Sx;
        res_grd = Sx_grd - Sy_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      break;
    }
  /* pwsb Sx,Sy,Dz 10100100....zzzz */
  case 9:
    {
      z = iword & 0xf;
      {
        return;
      }
      break;
    }
  /* pwad Sx,Sy,Dz 10110100....zzzz */
  case 10:
    {
      z = iword & 0xf;
      {
        return;
      }
      break;
    }
  /* (if cc) pabs Sx,Dz 100010ccxx01zzzz */
  case 11:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 12:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        /* FIXME: duplicate code pabs.  */
        res = DSP_R (x);
        res_grd = GET_DSP_GRD (x);
        if (res >= 0)
          carry = 0;
        else
          {
            res = -res;
            carry = (res != 0); /* The manual has a bug here.  */
            res_grd = -res_grd - carry;
          }
        COMPUTE_OVERFLOW;
        /* ??? The re-computing of overflow after
           saturation processing is specific to pabs.  */
        overflow = res_grd != SIGN32 (res) ? DSR_MASK_V : 0;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* pabs Sx,Dz 10001000xx..zzzz */
  case 13:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (x);
        res_grd = GET_DSP_GRD (x);
        if (res >= 0)
          carry = 0;
        else
          {
            res = -res;
            carry = (res != 0); /* The manual has a bug here.  */
            res_grd = -res_grd - carry;
          }
        COMPUTE_OVERFLOW;
        /* ??? The re-computing of overflow after
           saturation processing is specific to pabs.  */
        overflow = res_grd != SIGN32 (res) ? DSR_MASK_V : 0;
        ADD_SUB_GE;
      }
      break;
    }
  /* (if cc) prnd Sx,Dz 100110ccxx01zzzz */
  case 14:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 15:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        /* FIXME: duplicate code prnd.  */
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);

        res = (Sx + 0x8000) & 0xffff0000;
        carry = (unsigned) res < (unsigned) Sx;
        res_grd = Sx_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* prnd Sx,Dz 10011000xx..zzzz */
  case 16:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);

        res = (Sx + 0x8000) & 0xffff0000;
        carry = (unsigned) res < (unsigned) Sx;
        res_grd = Sx_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      break;
    }
  /* (if cc) pabs Sy,Dz 101010cc01yyzzzz */
  case 17:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 18:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        /* FIXME: duplicate code pabs.  */
        res = DSP_R (y);
        res_grd = 0;
        overflow = 0;
        greater_equal = DSR_MASK_G;
        if (res >= 0)
          carry = 0;
        else
          {
            res = -res;
            carry = 1;
            if (res < 0)
              {
                if (S)
                  res = 0x7fffffff;
                else
                  {
                    overflow = DSR_MASK_V;
                    greater_equal = 0;
                  }
              }
          }
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* pabs Sy,Dz 10101000..yyzzzz */
  case 19:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (y);
        res_grd = 0;
        overflow = 0;
        greater_equal = DSR_MASK_G;
        if (res >= 0)
          carry = 0;
        else
          {
            res = -res;
            carry = 1;
            if (res < 0)
              {
                if (S)
                  res = 0x7fffffff;
                else
                  {
                    overflow = DSR_MASK_V;
                    greater_equal = 0;
                  }
              }
          }
      }
      break;
    }
  /* (if cc) prnd Sy,Dz 101110cc01yyzzzz */
  case 20:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 21:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        /* FIXME: duplicate code prnd.  */
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = (Sy + 0x8000) & 0xffff0000;
        carry = (unsigned) res < (unsigned) Sy;
        res_grd = Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* prnd Sy,Dz 10111000..yyzzzz */
  case 22:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = (Sy + 0x8000) & 0xffff0000;
        carry = (unsigned) res < (unsigned) Sy;
        res_grd = Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      break;
    }
  /* (if cc) pshl Sx,Sy,Dz 100000ccxxyyzzzz */
  case 23:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 24:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x) & 0xffff0000;
        int Sy = DSP_R (y) >> 16 & 0x7f;

        if (Sy <= 16)
          res = Sx << Sy;
        else if (Sy >= 128 - 16)
          res = (unsigned) Sx >> (128 - Sy);	/* no sign extension */
        else
          {
            RAISE_EXCEPTION (SIGILL);
            return;
          }
        goto cond_logical;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) psha Sx,Sy,Dz 100100ccxxyyzzzz */
  case 25:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 26:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y) >> 16 & 0x7f;

        if (Sy <= 32)
          {
            if (Sy == 32)
              {
                res = 0;
                res_grd = Sx;
              }
            else
              {
                res = Sx << Sy;
                res_grd = Sx_grd << Sy | (unsigned) Sx >> (32 - Sy);
              }
            res_grd = SEXT (res_grd);
            carry = res_grd & 1;
          }
        else if (Sy >= 96)
          {
            Sy = 128 - Sy;
            if (Sy == 32)
              {
                res_grd = SIGN32 (Sx_grd);
                res = Sx_grd;
              }
            else
              {
                res = Sx >> Sy | Sx_grd << (32 - Sy);
                res_grd = Sx_grd >> Sy;
              }
            carry = Sx >> (Sy - 1) & 1;
          }
        else
          {
            RAISE_EXCEPTION (SIGILL);
            return;
          }
        COMPUTE_OVERFLOW;
        greater_equal = 0;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) psub Sx,Sy,Dz 101000ccxxyyzzzz */
  case 27:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 28:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sx - Sy;
        carry = (unsigned) res > (unsigned) Sx;
        res_grd = Sx_grd - Sy_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) psub Sy,Sx,Dz 100001ccxxyyzzzz */
  case 29:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 30:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sy - Sx;
        carry = (unsigned) res > (unsigned) Sy;
        res_grd = Sy_grd - Sx_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) padd Sx,Sy,Dz 101100ccxxyyzzzz */
  case 31:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 32:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sx + Sy;
        carry = (unsigned) res < (unsigned) Sx;
        res_grd = Sx_grd + Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pand Sx,Sy,Dz 100101ccxxyyzzzz */
  case 33:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 34:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (x) & DSP_R (y);
        cond_logical:
        res &= 0xffff0000;
        res_grd = 0;
        if (iword & 0x200)

          goto assign_z;

        logical:
        carry = 0;
        overflow = 0;
        greater_equal = 0;
        DSR &= ~0xf1;

        if (res)

          DSR |= res >> 26 & DSR_MASK_N;

        else

          DSR |= DSR_MASK_Z;

        goto assign_dc;

      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pxor Sx,Sy,Dz 101001ccxxyyzzzz */
  case 35:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 36:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (x) ^ DSP_R (y);
        goto cond_logical;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) por Sx,Sy,Dz 101101ccxxyyzzzz */
  case 37:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 38:
    {
      int x = x_tab[(iword >> 6) & 3];
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (x) | DSP_R (y);
        goto cond_logical;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pdec Sx,Dz 100010ccxx..zzzz */
  case 39:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 40:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);

        res = Sx - 0x10000;
        carry = Sx < (INT_MIN + 0x10000);
        res_grd = Sx_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        res &= 0xffff0000;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pinc Sx,Dz 100110ccxx..zzzz */
  case 41:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 42:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);

        res = Sx + 0x10000;
        carry = Sx > (INT_MAX - 0x10000);
        res_grd = Sx_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        res &= 0xffff0000;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pdec Sy,Dz 101010cc..yyzzzz */
  case 43:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 44:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sy - 0x10000;
        carry = Sy < (INT_MIN + 0x10000);
        res_grd = Sy_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        res &= 0xffff0000;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pinc Sy,Dz 101110cc..yyzzzz */
  case 45:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 46:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = Sy + 0x10000;
        carry = Sy > (INT_MAX - 0x10000);
        res_grd = Sy_grd + carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
        res &= 0xffff0000;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pclr Dz 100011cc....zzzz */
  case 47:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 48:
    {
      z = iword & 0xf;
      {
        res = 0;
        res_grd = 0;
        carry = 0;
        overflow = 0;
        greater_equal = 1;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* pclr Du pmuls Se,Sf,Dg 0100eeff0001gguu */
  case 49:
    {
      int e = e_tab[(iword >> 10) & 3];
      int f = f_tab[(iword >> 8) & 3];
      int g = g_tab[(iword >> 2) & 3];
      int u = u_tab[(iword >> 0) & 3];

      /* Do multiply.  */
      res = (DSP_R (e) >> 16) * (DSP_R (f) >> 16) * 2;
      if (res == 0x80000000)
        res = 0x7fffffff;
      DSP_R (g) = res;
      DSP_GRD (g) = SIGN32 (res);
      /* FIXME: update DSR based on results of multiply!  */

      /* Do clr.  */
      z = u;
      res = 0;
      res_grd = 0;
      goto assign_z;
      break;
    }
  /* (if cc) pdmsb Sx,Dz 100111ccxx..zzzz */
  case 50:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 51:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        unsigned Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);
        int i = 16;

        if (Sx_grd < 0)
          {
            Sx_grd = ~Sx_grd;
            Sx = ~Sx;
          }
        if (Sx_grd)
          {
            Sx = Sx_grd;
            res = -2;
          }
        else if (Sx)
          res = 30;
        else
          res = 31;
        do
          {
            if (Sx & ((unsigned)~0 << i))
              {
                res -= i;
                Sx >>= i;
              }
          }
        while (i >>= 1);
        res <<= 16;
        res_grd = SIGN32 (res);
        carry = 0;
        overflow = 0;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pdmsb Sy,Dz 101111cc..yyzzzz */
  case 52:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 53:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        unsigned Sy = DSP_R (y);
        int i = 16;

        if (Sy < 0)
          Sy = ~Sy;
        Sy <<= 1;
        res = 31;
        do
          {
            if (Sy & ((unsigned)~0 << i))
              {
                res -= i;
                Sy >>= i;
              }
          }
        while (i >>= 1);
        res <<= 16;
        res_grd = SIGN32 (res);
        carry = 0;
        overflow = 0;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pneg Sx,Dz 110010ccxx..zzzz */
  case 54:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 55:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);
        int Sx_grd = GET_DSP_GRD (x);

        res = 0 - Sx;
        carry = res != 0;
        res_grd = 0 - Sx_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pcopy Sx,Dz 110110ccxx..zzzz */
  case 56:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 57:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (x);
        res_grd = GET_DSP_GRD (x);
        carry = 0;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pneg Sy,Dz 111010cc..yyzzzz */
  case 58:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 59:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sy = DSP_R (y);
        int Sy_grd = SIGN32 (Sy);

        res = 0 - Sy;
        carry = res != 0;
        res_grd = 0 - Sy_grd - carry;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pcopy Sy,Dz 111110cc..yyzzzz */
  case 60:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 61:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        res = DSP_R (y);
        res_grd = SIGN32 (res);
        carry = 0;
        COMPUTE_OVERFLOW;
        ADD_SUB_GE;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) psts MACH,Dz 110011cc....zzzz */
  case 62:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 63:
    {
      z = iword & 0xf;
      {
        res = MACH;
        res_grd = SIGN32 (res);
        goto assign_z;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) psts MACL,Dz 110111cc....zzzz */
  case 64:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 65:
    {
      z = iword & 0xf;
      {
        res = MACL;
        res_grd = SIGN32 (res);
        goto assign_z;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) plds Dz,MACH 111011cc....zzzz */
  case 66:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 67:
    {
      z = iword & 0xf;
      {
        if (0xa05f >> z & 1)
          RAISE_EXCEPTION (SIGILL);
        else
          MACH = DSP_R (z);
        return;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) plds Dz,MACL 111111cc....zzzz */
  case 68:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 69:
    {
      z = iword & 0xf;
      {
        if (0xa05f >> z & 1)
          RAISE_EXCEPTION (SIGILL);
        else
          MACL = DSP_R (z);
        return;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pswap Sx,Dz 100111ccxx01zzzz */
  case 70:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 71:
    {
      int x = x_tab[(iword >> 6) & 3];

      z = iword & 0xf;
      {
        int Sx = DSP_R (x);

        res = ((Sx & 0xffff) * 65536) + ((Sx >> 16) & 0xffff);
        res_grd = GET_DSP_GRD (x);
        carry = 0;
        overflow = 0;
        greater_equal = res & 0x80000000 ? 0 : DSR_MASK_G;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  /* (if cc) pswap Sy,Dz 101111cc01yyzzzz */
  case 72:
    {
      if ((((iword >> 8) ^ DSR) & 1) == 0)
	return;
      ATTRIBUTE_FALLTHROUGH;
    }
  case 73:
    {
      int y = y_tab[(iword >> 4) & 3];

      z = iword & 0xf;
      {
        int Sy = DSP_R (y);

        res = ((Sy & 0xffff) * 65536) + ((Sy >> 16) & 0xffff);
        res_grd = SIGN32 (Sy);
        carry = 0;
        overflow = 0;
        greater_equal = res & 0x80000000 ? 0 : DSR_MASK_G;
      }
      if (iword & 0x200)
        goto assign_z;
      break;
    }
  default:
    {
      RAISE_EXCEPTION (SIGILL);
      return;
    }
  }
  DSR &= ~0xf1;
  if (res || res_grd)
    DSR |= greater_equal | (res_grd >> 2 & DSR_MASK_N) | overflow;
  else
    DSR |= DSR_MASK_Z | overflow;
 assign_dc:
  switch (DSR >> 1 & 7)
    {
    case 0: /* Carry Mode */
      DSR |= carry;
      break;
    case 1: /* Negative Value Mode */
      DSR |= res_grd >> 7 & 1;
      break;
    case 2: /* Zero Value Mode */
      DSR |= DSR >> 6 & 1;
      break;
    case 3: /* Overflow mode */
      DSR |= overflow >> 4;
      break;
    case 4: /* Signed Greater Than Mode */
      DSR |= DSR >> 7 & 1;
      break;
    case 5: /* Signed Greater Than Or Equal Mode */
      DSR |= greater_equal >> 7;
      break;
    }
 assign_z:
  if (0xa05f >> z & 1)
    {
      RAISE_EXCEPTION (SIGILL);
      return;
    }
  DSP_R (z) = res;
  DSP_GRD (z) = res_grd;
}
