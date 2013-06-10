/*-------------------------------------------------------------------------
 *
 * copy.h
 *	  Definitions for using the POSTGRES copy command.
 *
 *
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * $PostgreSQL: pgsql/src/include/commands/copy.h,v 1.32 2009/01/01 17:23:58 momjian Exp $
 *
 *-------------------------------------------------------------------------
 */
#ifndef COPY_H
#define COPY_H

#include "nodes/parsenodes.h"
#include "tcop/dest.h"


extern uint64 DoCopy(const CopyStmt *stmt, const char *queryString);

extern DestReceiver *CreateCopyDestReceiver(void);

#endif   /* COPY_H */
