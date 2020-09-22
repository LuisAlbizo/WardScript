"Vim syntax file
"Language: WardScript
"Maintainer: Luis Albizo
"Latest Revision: 20 - Sep - 2020

autocmd BufRead,BufNewFile *.ward set filetype=ward

syn keyword builtinFunctions print present import finish type input bool
syn keyword controlStats if then else end loop exit
syn keyword definitions func nonlocal
syn keyword variables NIL NODE BYTE FUNCTION
syn keyword notes TODO NOTES XXX BUG FIXME INFO
syn region comment start="?" end="?" contains=notes
syn region string start="\"" end="\"" contains=escapechar
syn match comment ";.\+$"
syn match byte "[^a-zA-Z0-9_'$]\zs\d\{1,3}\ze"
syn match byte "^\d\{1,3}"
syn match byte "\[^\d\{1,3}"
syn match invalidbyte "[^a-zA-Z_'$]\+\zs\d\{4,}\ze"
syn match invalidbyte "^\d\{4,}"
syn match escapechar "\\[ntv"]"
syn match escapechar "\\x[0-9a-fA-F]\{2}"
syn match functions "\zs[a-zA-Z0-9_'$]\+\ze\s*:=\s*func"
syn match functions "\zs[a-zA-Z0-9_'$]\+\ze("
syn match members "[a-zA-Z0-9_'$]\+\zs\(\.[a-zA-Z0-9_'$]\+\)\+\ze"
syn match brackets "[[\]{}]"

highlight builtinFunctions ctermfg=lightblue
highlight controlStats ctermfg=yellow
highlight definitions ctermfg=lightgreen

highlight variables ctermfg=magenta
highlight functions ctermfg=lightblue
highlight string ctermfg=magenta
highlight comment ctermfg=cyan
highlight byte ctermfg=magenta
highlight escapechar ctermfg=lightmagenta
highlight invalidbyte ctermbg=red
highlight notes ctermbg=yellow ctermfg=black
highlight members ctermfg=cyan
highlight brackets ctermfg=lightgreen

if exists("b:current_syntax")
	finish
endif

