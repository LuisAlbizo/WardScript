"Vim syntax file
"Language: WardScript
"Maintainer: Luis Albizo
"Latest Revision: 13 - Aug - 10

syn keyword builtinFunctions print present import
syn keyword controlStats if then else end loop exit
syn keyword definitions func
syn match byte '\d\+'


if exists("b:current_syntax")
	finish
endif

