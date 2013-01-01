.386
.model flat
.code

; eh vector constructor iterator
public ??_L@YGXPAXIHP6EX0@Z1@Z
extern _eh_vec_ctor@20 : near
??_L@YGXPAXIHP6EX0@Z1@Z: jmp _eh_vec_ctor@20

; eh vector destructor iterator
public ??_M@YGXPAXIHP6EX0@Z@Z
extern _eh_vec_dtor@16 : near
??_M@YGXPAXIHP6EX0@Z@Z: jmp _eh_vec_dtor@16

; CString::FormatV() public vs. protected
public ?FormatV@CString@@QAEXPBDPAD@Z
extern ?FormatV@CString@@IAEXPBDPAD@Z : near
?FormatV@CString@@QAEXPBDPAD@Z: jmp ?FormatV@CString@@IAEXPBDPAD@Z

end
