/* Statement Evaluator definition file: evaluator.h
 * Luis Albizo "07-27-18"
 */
#ifndef eval_h
#define eval_h

#include "ast.h"
#include "stack.h"
#include "scope.h"
#include "error.h"
#include "object.h"
#include <stdlib.h>

/*
 *
 *
 *
 */

B_Object *to_bool(B_Object *);

/* NAME */
st_st *eva_name(st_name *, Scope *);
/* BOP */
st_st *eva_bop(st_bop *);
/* UOP */
st_st *eva_uop(st_uop *);
/* ASSIGNMENT */
st_st *eva_assignment(st_assignment *, Scope *);
/* IF */
st_st *eva_if(st_if *, Scope *);
/* FOREVER */
st_st *eva_forever(st_forever *, Scope *);
/* CALL */
st_st *eva_call(st_call *, Scope *);
/* METHODCALL */
st_st *eva_methodcall(st_methodcall *, Scope *);
/* MEMBER */
st_st *eva_member(st_member *, Scope *);
/* MEMBER ASSIGN */
st_st *eva_member_assign(st_member_assign *, Scope *);
/* GENERAL EVALUATOR */
st_st *eva_(st_st *, Scope *);



#endif
