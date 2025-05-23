/* libthread_db helper functions for the remote server for GDB.
   Copyright (C) 2002-2025 Free Software Foundation, Inc.

   Contributed by MontaVista Software.

   This file is part of GDB.

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


/* This file is currently tied to GNU/Linux.  It should scale well to
   another libthread_db implementation, with the appropriate gdbserver
   hooks, but for now this means we can use GNU/Linux's target data.  */

#include "linux-low.h"

#include "gdb_proc_service.h"

typedef struct ps_prochandle *gdb_ps_prochandle_t;
typedef void *gdb_ps_read_buf_t;
typedef const void *gdb_ps_write_buf_t;
typedef size_t gdb_ps_size_t;

#ifdef HAVE_LINUX_REGSETS
#define HAVE_REGSETS
#endif

#ifdef HAVE_REGSETS
static struct regset_info *
gregset_info (void)
{
  int i = 0;
  const regs_info *regs_info = the_linux_target->get_regs_info ();
  struct regsets_info *regsets_info = regs_info->regsets_info;

  while (regsets_info->regsets[i].size != -1)
    {
      if (regsets_info->regsets[i].type == GENERAL_REGS)
	break;
      i++;
    }

  return &regsets_info->regsets[i];
}
#endif

/* Search for the symbol named NAME within the object named OBJ within
   the target process PH.  If the symbol is found the address of the
   symbol is stored in SYM_ADDR.  */

ps_err_e
ps_pglobal_lookup (gdb_ps_prochandle_t ph, const char *obj,
		   const char *name, psaddr_t *sym_addr)
{
  CORE_ADDR addr;

  if (thread_db_look_up_one_symbol (name, &addr) == 0)
    return PS_NOSYM;

  *sym_addr = (psaddr_t) (unsigned long) addr;
  return PS_OK;
}

/* Read SIZE bytes from the target process PH at address ADDR and copy
   them into BUF.  */

ps_err_e
ps_pdread (gdb_ps_prochandle_t ph, psaddr_t addr,
	   gdb_ps_read_buf_t buf, gdb_ps_size_t size)
{
  if (read_inferior_memory ((uintptr_t) addr, (gdb_byte *) buf, size) != 0)
    return PS_ERR;
  return PS_OK;
}

/* Write SIZE bytes from BUF into the target process PH at address ADDR.  */

ps_err_e
ps_pdwrite (gdb_ps_prochandle_t ph, psaddr_t addr,
	    gdb_ps_write_buf_t buf, gdb_ps_size_t size)
{
  if (target_write_memory ((uintptr_t) addr, (const gdb_byte *) buf, size)
      != 0)
    return PS_ERR;
  return PS_OK;
}

/* Get the general registers of LWP LWPID within the target process PH
   and store them in GREGSET.  */

ps_err_e
ps_lgetregs (gdb_ps_prochandle_t ph, lwpid_t lwpid, prgregset_t gregset)
{
#ifdef HAVE_REGSETS
  struct lwp_info *lwp;
  struct regcache *regcache;

  lwp = find_lwp_pid (ptid_t (lwpid));
  if (lwp == NULL)
    return PS_ERR;

  scoped_restore_current_thread restore_thread;
  switch_to_thread (lwp->thread);
  regcache = get_thread_regcache (current_thread);
  gregset_info ()->fill_function (regcache, gregset);

  return PS_OK;
#else
  return PS_ERR;
#endif
}

/* Set the general registers of LWP LWPID within the target process PH
   from GREGSET.  */

ps_err_e
ps_lsetregs (gdb_ps_prochandle_t ph, lwpid_t lwpid, const prgregset_t gregset)
{
  /* Unneeded.  */
  return PS_ERR;
}

/* Get the floating-point registers of LWP LWPID within the target
   process PH and store them in FPREGSET.  */

ps_err_e
ps_lgetfpregs (gdb_ps_prochandle_t ph, lwpid_t lwpid, prfpregset_t *fpregset)
{
  /* Unneeded.  */
  return PS_ERR;
}

/* Set the floating-point registers of LWP LWPID within the target
   process PH from FPREGSET.  */

ps_err_e
ps_lsetfpregs (gdb_ps_prochandle_t ph, lwpid_t lwpid, const prfpregset_t *fpregset)
{
  /* Unneeded.  */
  return PS_ERR;
}

/* Return overall process id of the target PH.  Special for GNU/Linux
   -- not used on Solaris.  */

pid_t
ps_getpid (gdb_ps_prochandle_t ph)
{
  return current_process ()->pid;
}
