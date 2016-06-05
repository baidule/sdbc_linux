/*---------------------------------------------------------------------------
 * sqlo_execute1
 *--------------------------------------------------------------------------*/
int 
DEFUN(sqlo_execute1, (sth, begin,iterations), 
      sqlo_stmt_handle_t     sth          AND 
	  unsigned int			begin	AND
      unsigned int           iterations)
{
  register sqlo_stmt_struct_ptr_t  stp;
  sqlo_db_struct_ptr_t dbp;

  CHECK_STHANDLE(stp, sth, "sqlo_execute", SQLO_INVALID_STMT_HANDLE);
  assert( stp->dbp != NULL);
  dbp = stp->dbp;

  TRACE(3, fprintf(_get_trace_fp(dbp), 
		   "sqlo_execute [%2d]: iter=%u, stmt=%.80s\n", 
                   sth, iterations, _get_stmt_string(stp)););

  /* For REF CURSORS and NESTED TABLES, we determine the statement type and
   * define the output here
   */
  if (DEFAULT != stp->cursor_type && 0 == stp->num_executions ) {
    /* REF CURSOR or NESTED TABLE */
    dbp->status = OCIAttrGet( (dvoid*) stp->stmthp, 
			      (ub4) OCI_HTYPE_STMT, 
			      (dvoid*) &(stp->stype),
			      (ub4 *) 0, 
			      (ub4) OCI_ATTR_STMT_TYPE, 
			      (OCIError *) dbp->errhp 
			      );

    CHECK_OCI_STATUS_RETURN(dbp, dbp->status, "sqlo_execute", "GetStmtType");
    dbp->status = _define_output(stp);
    CHECK_OCI_STATUS_RETURN(dbp, dbp->status, "sqlo_execute", "_define_output");

  } else if ( _is_prepared(stp) ) {

    dbp->status = OCIStmtExecute( dbp->svchp, 
				  stp->stmthp, 
				  dbp->errhp, 
				  (ub4) iterations, 
				  (ub4) begin,
				  (OCISnapshot *) 0, 
				  (OCISnapshot *) 0,
				  dbp->exec_flags
				  );
      
    if (OCI_SUCCESS != dbp->status && OCI_NO_DATA != dbp->status) {
      if (OCI_STILL_EXECUTING == dbp->status) {
        stp->still_executing = TRUE;
        stp->opened          = TRUE;
      } else {

	_save_oci_status(dbp, "sqlo_execute", 
			 _get_stmt_string(stp), __LINE__);
      }
    } else {
      stp->opened          = TRUE;
      stp->still_executing = FALSE;

      _bindpv_reset(stp);       /* reset the number of elements in the bindpv.
				 * The next sqlo_bind_by_name will not have to
				 * to allocate again memory
				 */
    } /* end if OCI_SUCCESS != status  */
  } else {
    dbp->status = SQLO_STMT_NOT_PARSED;
    CHECK_OCI_STATUS_RETURN(dbp, dbp->status, "sqlo_execute", "");
  }

  TRACE(3, fprintf(_get_trace_fp(dbp), 
		   "sqlo_execute returns %d\n", dbp->status););

  return (dbp->status);
}

/*---------------------------------------------------------------------------
 *	 close all open cursors  on this database connection
 *-------------------------------------------------------------------------*/
int 
DEFUN(sqlo_close_all_db_cursors, (dbh),  sqlo_db_handle_t dbh)
{
  sqlo_db_struct_ptr_t  dbp;
  
  if ( !VALID_DBH_RANGE(dbh) || !_dbv[ dbh ]->used || 
      !_dbv[ dbh ]->session_created ) {
    TRACE(1, fprintf(_trace_fp, "Invalid Database handle %d in sqlo_session_end\n",
                     dbh);); 
    return SQLO_INVALID_DB_HANDLE;
  }

  
  dbp = _dbv[ dbh ];             /* setup the pointer to the db structure */
  assert( dbp != NULL );

  TRACE(2, fprintf(_get_trace_fp(dbp), "sqlo_session_end[%d] starts\n", dbh); );


  /* close all open cursors  on this database connection */
  _close_all_db_cursors( dbp );
 return SQLO_SUCCESS;
}
