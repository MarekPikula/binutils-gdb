/* BFD support for the Intel MCU architecture.
   Copyright (C) 2015-2025 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

extern void *bfd_arch_i386_short_nop_fill (bfd_size_type, bool, bool);

#define N(number, name, print, next)			     \
  { 32, 32, 8, bfd_arch_iamcu, number, name, print, 3, true, \
    bfd_default_compatible, bfd_default_scan,		     \
    bfd_arch_i386_short_nop_fill, next, 0 }

static const bfd_arch_info_type bfd_iamcu_arch_intel_syntax =
  N (bfd_mach_i386_iamcu_intel_syntax, "iamcu:intel", "iamcu:intel", NULL);

const bfd_arch_info_type bfd_iamcu_arch =
  N (bfd_mach_i386_iamcu, "iamcu", "iamcu", &bfd_iamcu_arch_intel_syntax);

