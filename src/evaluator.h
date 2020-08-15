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

W_Object *to_bool(W_Object *);

/* NAME */
st_st *eva_name(st_name *, Scope *);
/* BOP */
W_Object *bop_BYTE(byte_t, W_Byte *, W_Object *); /* Binary OP for left-operand of type BYTE */
W_Object *bop_NODE(byte_t, W_Node *, W_Object *); /* Binary OP for left-operand of type NODE */
W_Object *bop_NIL(byte_t, W_Nil *, W_Object *); /* Binary OP for left-operand of type NIL */
W_Object *bop_FUNCTION(byte_t, W_Function *, W_Object *); /* Binary OP for left-operand of type FUNCTION */

st_st *eva_bop(st_bop *, Scope *);
/* UOP */
st_st *eva_uop(st_uop *, Scope *);
/* ASSIGNMENT */
st_st *eva_assignment(st_assignment *, Scope *);
/* NONLOCAL ASSIGNMENT */
st_st *eva_nonlocal_assignment(st_assignment *, Scope *);
/* IF */
st_st *eva_if(st_if *, Scope *);
/* FOREVER */
st_st *eva_forever(st_forever *, Scope *);
/* CALL */
st_st *eva_call(st_call *, Scope *);
/* METHODCALL */
st_st *eva_methodcall(st_methodcall *, Scope *);
/* NODE CONSTRUCT */
st_st *eva_node_construct(st_node_construct *, Scope *);
/* LIST CONSTRUCT */
st_st *eva_list_construct(st_list_construct *, Scope *);
/* FUNCTION CONSTRUCT */
st_st *eva_function_construct(st_function_construct *, Scope *);
/* MEMBER */
st_st *eva_member(st_member *, Scope *);
/* MEMBER ASSIGN */
st_st *eva_member_assign(st_member_assign *, Scope *);
/* GENERAL EVALUATOR */
st_st *eva_(st_st *, Scope *);



#endif
