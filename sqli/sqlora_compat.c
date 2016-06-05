/* $Id: sqlora_compat.c 180 2002-04-22 06:04:45Z kpoitschke $ */
/**
 * @file sqlora_compat.c
 * Compatibility functions for backward compatibility to version 1.
 * These functions basically work without the dbh parameter. This means
 * you can only use 1 database connection.
 *
 * @author Kai Poitschke
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WANT_SQLORA1_COMPATIBILITY

#include "sqlora.h"


CONST char * sqlora_compatID="$Id: sqlora_compat.c 180 2002-04-22 06:04:45Z kpoitschke $";

#if defined(WIN32)
#define inline __inline
#endif

#if defined (__STDC__) || defined (_AIX) || \
            (defined (__mips) && defined (_SYSTYPE_SVR4)) ||\
             defined(WIN32) || defined(__cplusplus)\
            
#  define AND ,
#  define DEFUN(name, arglist, args)      name(args)
#  define DEFUN_VOID(name)                name(void) 
/*
 * Macro to use instead of "void" for arguments that must have
 * type "void *" in ANSI C;  maps them to type "char *" in
 * non-ANSI systems.
 */
#  define VOID void
#else
#  define AND ;
#  define DEFUN(name, arglist, args)      name arglist args;
#  define DEFUN_VOID(name)                name()
#  define VOID char
#endif

#ifndef NULL
#   define NULL 0
#endif

static int _dbh = -1;

/* ------------------------------------------------------------------------- */

/*
 * sql_init
 */
int DEFUN_VOID(sql_init)
{
  return(sqlo_init(0, 1, 256));         /* no threaded mode, 1K for stmt pointers */
}

/*
 * sql_trace
 */
int DEFUN(sql_trace,(on), int on) 
{
  return(sqlo_trace(_dbh,on));
}

/*
 * sql_geterror
 */
CONST char * DEFUN_VOID(sql_geterror) {
  return(sqlo_geterror(_dbh));
}

/*
 * sql_geterrcode
 */
int DEFUN_VOID(sql_geterrcode) 
{
  return(sqlo_geterrcode(_dbh));
}

/*
 * sql_exists
 */
int DEFUN(sql_exists, (table, field, value, where),
          CONST char * table AND
          CONST char * field AND
          CONST char * value AND
          CONST char * where) 
{
  return(sqlo_exists(_dbh,table,field,value,where));
}

/*
 * sql_count
 */
int DEFUN(sql_count, (table, field, value, where),
          CONST char * table AND
          CONST char * field AND
          CONST char * value AND
          CONST char * where)
{
  return(sqlo_count(_dbh,table,field,value,where));
}

/*
 * sql_run
 */
int DEFUN(sql_run,(stmt, argc, argv),
           char CONST * stmt AND
           int argc AND
           char CONST ** argv)
{
  return(sqlo_run(_dbh, stmt, argc, argv));
}

/*
 * sql_open
 */
int DEFUN(sql_open,(stmt, argc, argv),
          CONST char * stmt AND
          int argc AND
          CONST char ** argv)
{
  return(sqlo_open(_dbh, stmt, argc, argv));
}

/*
 * sql_reopen
 */
int DEFUN(sql_reopen,(sth, argc, argv),
          int sth AND
          int argc AND
          CONST char ** argv)
{
  return(sqlo_reopen(sth, argc, argv));
}

/*
 * sql_fetch
 */
int DEFUN(sql_fetch, (sth), int sth)
{
  return(sqlo_fetch(sth, 1));
}

char CONST ** DEFUN(sql_values,(sth, num, do_strip),
                    int sth AND
                    int * num AND
                    int do_strip)
{
  return(sqlo_values(sth, num, do_strip));
}

/*
 * sql_command
 */
char CONST * DEFUN(sql_command,(sth), int sth)
{
  return(sqlo_command(sth));
}

/*
 * sql_close
 */
int DEFUN(sql_close,(sth), int sth)
{
  return(sqlo_close(sth));
}

/*
 * sql_print
 */
int DEFUN(sql_print, (sth), int sth)
{
  return(sqlo_print(sth));
}

/*
 * sql_finish
 */
int DEFUN_VOID(sql_finish)
{
  return(sqlo_finish(_dbh));
}

/*
 * sql_getdatabase
 */
char CONST * DEFUN_VOID(sql_getdatabase)
{
  return(sqlo_getdatabase(_dbh));
}

/*
 * sql_prows
 */
int DEFUN(sql_prows,(sth), int sth)
{
  return(sqlo_prows(sth));
}

/*
 * sql_connect
 */
int DEFUN(sql_connect,(connect_str), CONST char * connect_str)
{
  return(sqlo_connect(&_dbh, connect_str));
}

int DEFUN_VOID(sql_commit)
{
  return(sqlo_commit(_dbh));
}

/*
 * sql_rollback
 */
int DEFUN_VOID(sql_rollback)
{
  return(sqlo_rollback(_dbh));
}

/*
 * sql_exec
 */
int DEFUN(sql_exec,(stmt), CONST char * stmt)
{
  return(sqlo_exec(_dbh, stmt));
}

/*
 * sql_isopen
 */
int DEFUN(sql_isopen,(sth), int sth)
{
  return(sqlo_isopen(sth));
}

/*
 * sql_prepare
 */
int DEFUN(sql_prepare, (stmt), CONST char * stmt)
{
  return(sqlo_prepare(_dbh, stmt));
}

/*
 * sql_bind_by_name
 */
int DEFUN(sql_bind_by_name,(sth, name, param_type, param_addr, param_size, 
                            ind_addr, is_array),
          int sth AND
          CONST char * name AND
          int param_type AND
          CONST void * param_addr AND
          unsigned int param_size AND
          short * ind_addr AND
          int is_array)
{
  return(sqlo_bind_by_name(sth, name, param_type, param_addr, param_size,
                           ind_addr, is_array));
}

/*
 * sql_bind_by_pos
 */
int DEFUN(sql_bind_by_pos,(sth, position, param_type, param_addr, param_size,
                           ind_addr, is_array),
          int sth AND
          int position AND
          int param_type AND
          CONST void * param_addr AND
          unsigned int param_size AND
          short * ind_addr AND
          int is_array)
{
  return(sqlo_bind_by_pos(sth, position, param_type, param_addr, param_size,
                          ind_addr, is_array));
}

/*
 * sql_define_by_pos
 */
int DEFUN(sql_define_by_pos,(sth, value_pos, value_type, value_addr, 
                             value_size, ind_addr, rlen_addr, 
                             is_array),
          int sth AND
          int value_pos AND
          int value_type AND
          CONST void * value_addr AND
          unsigned int value_size AND
          short * ind_addr AND
          short * rlen_addr AND
          int is_array)
{
  return(sqlo_define_by_pos(sth, value_pos, value_type, value_addr, value_size,
                          ind_addr, rlen_addr, is_array));
}

/*
 * sql_execute
 */
int DEFUN(sql_execute, (sth, iterations), 
          int sth AND
          int iterations)
{
  return(sqlo_execute(sth, iterations));
}

/*
 * sql_ocol_names
 */
char CONST ** DEFUN(sql_ocol_names, (sth, num), int sth AND int * num)
{
  return(sqlo_ocol_names(sth, num));
}

/*
 * sql_ocol_name_lens
 */
int CONST * DEFUN(sql_ocol_name_lens, (sth, num), int sth AND int * num)
{
  return(sqlo_ocol_name_lens(sth, num));
}

/*
 * sql_value_lens
 */
unsigned short CONST * DEFUN(sql_value_lens, (sth, num), int sth AND int * num)
{
  return(sqlo_value_lens(sth, num));
}

/*
 * sql_ncols
 */
int DEFUN(sql_ncols, (sth, in), int sth AND int in)
{
  return(sqlo_ncols(sth, in));
}

int DEFUN_VOID(sql_getdbh)
{
  return _dbh;
}
#endif /* WANT_SQLORA1_COMPATIBILITY */

/* $Id: sqlora_compat.c 180 2002-04-22 06:04:45Z kpoitschke $ */
