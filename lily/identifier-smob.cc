/*
  identifier-smob.cc -- implement glue to pass Scheme expressions off as
  identifiers.

  source file of the GNU LilyPond music typesetter

  (c) 2002--2007 Han-Wen Nienhuys <hanwen@xs4all.nl>
*/

#include "identifier-smob.hh"

scm_t_bits package_tag;

static int
print_box (SCM b, SCM port, scm_print_state *)
{
  SCM value = SCM_CELL_OBJECT_1 (b);

  scm_puts ("#<packaged object ", port);
  scm_write (value, port);
  scm_puts (">", port);

  /* Non-zero means success.  */
  return 1;
}

/* This defines the primitve `make-box', which returns a new smob of
   type `box', initialized to `#f'.  */
LY_DEFINE (ly_export, "ly:export",
	   1, 0, 0, (SCM arg),
	   "Export a Scheme object to the parser"
	   " so it is treated as an identifier.")
{
  SCM_RETURN_NEWSMOB (package_tag, arg);
}

SCM
unpack_identifier (SCM box)
{
  if (SCM_IMP (box) || SCM_CELL_TYPE (box) != package_tag)
    return SCM_UNDEFINED;

  return SCM_CELL_OBJECT_1 (box);
}

static void
init_box_type (void)
{
  package_tag = scm_make_smob_type ("box", 0);
  scm_set_smob_mark (package_tag, scm_markcdr);
  scm_set_smob_print (package_tag, print_box);
}

ADD_SCM_INIT_FUNC (package, init_box_type);
