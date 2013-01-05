.386
.model flat
.code

; eh vector constructor iterator
extern _eh_vec_ctor@20 : near
public ??_L@YGXPAXIHP6EX0@Z1@Z
??_L@YGXPAXIHP6EX0@Z1@Z: jmp _eh_vec_ctor@20

; eh vector destructor iterator
extern _eh_vec_dtor@16 : near
public ??_M@YGXPAXIHP6EX0@Z@Z
??_M@YGXPAXIHP6EX0@Z@Z: jmp _eh_vec_dtor@16

; type_info destructor - never invoked but linker wants it
extern _abort : near
public ??_7type_info@@6B@
??_7type_info@@6B@: jmp _abort

; doule-to-long conversion
extern __ftol : near
public __ftol2, __ftol2_sse
__ftol2:
__ftol2_sse: jmp __ftol

end
