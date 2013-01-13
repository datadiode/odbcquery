$(ConfigurationName): \
	$(ConfigurationName)Libs \
	$(SolutionDir)bin\$@\SciLexer.dll \
	$(SolutionDir)bin\$@\ODBCTrace.dll \
	$(SolutionDir)bin\$@\ODBCQuery.html \
	$(SolutionDir)bin\$@\ODBCQuery.ini \
	$(SolutionDir)int\include\midl\MSRDO20.lic

DebugLibs ReleaseLibs:

System32Libs: \
	$(SolutionDir)lib\MSVCRT.lib \
	$(SolutionDir)lib\MFC42.lib \
	$(SolutionDir)lib\EASTL.lib
	
$(SolutionDir)lib\MSVCRT.lib:
	-md $(SolutionDir)lib
	$(SolutionDir)bin\Common\makedef "%%SystemRoot%%\Microsoft.NET\Framework\v1.1.4322\MSVCR71.dll" > "$(SolutionDir)lib\MSVCRT.def"
	lib /MACHINE:X86 /DEF:"$(SolutionDir)lib/MSVCRT.def" /OUT:"$@"

$(SolutionDir)lib\MFC42.lib:
	-md $(SolutionDir)lib
	$(SolutionDir)bin\Common\makedef "%%SystemRoot%%\System32\MFC42.dll" "%%SystemRoot%%\symbols\dll\MFC42.pdb" "\dm\MFC\src\intel\MFC42.DEF" > "$(SolutionDir)lib\MFC42.def"
	lib /MACHINE:X86 /DEF:"$(SolutionDir)lib/MFC42.def" /OUT:"$@"

$(SolutionDir)lib\EASTL.lib: \
	$(SolutionDir)int\EASTL\allocator.obj \
	$(SolutionDir)int\EASTL\assert.obj \
	$(SolutionDir)int\EASTL\fixed_pool.obj \
	$(SolutionDir)int\EASTL\hashtable.obj \
	$(SolutionDir)int\EASTL\red_black_tree.obj \
	$(SolutionDir)int\EASTL\string.obj
	LIB /OUT:$@ $**

{$(SolutionDir)EASTL\src}.cpp{$(SolutionDir)int\EASTL}.obj::
	-md $(SolutionDir)int
	-md $(SolutionDir)int\EASTL
	cl -c -O1 -Oi -GS- -MD -DNDEBUG -Deastl=std -DEASTL_STD_ITERATOR_CATEGORY_ENABLED=0 -I $(SolutionDir)EASTL/include -Fo$(SolutionDir)int\EASTL\ $<

$(SolutionDir)lib\system32.obj: \
	$(SolutionDir)Common\system32.cpp
	cl -c -O1 -Oi -GS- -MD -DNDEBUG -Fo$(SolutionDir)lib\ $**

$(SolutionDir)lib\monikers.obj: \
	$(SolutionDir)Common\monikers.asm
	ml -c -Fo$(SolutionDir)lib\ $**

$(SolutionDir)bin\Debug\SciLexer.dll:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\scintilla
	-md $(SolutionDir)int\scintilla\$(ConfigurationName)
	cd $(SolutionDir)scintilla\win32
	nmake DEBUG=1 DIR_O="$(SolutionDir)int\scintilla\$(ConfigurationName)" DIR_BIN="$(@D)" -e -f scintilla.mak "$@"

$(SolutionDir)bin\Release\SciLexer.dll:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\scintilla
	-md $(SolutionDir)int\scintilla\$(ConfigurationName)
	cd $(SolutionDir)scintilla\win32
	nmake DIR_O="$(SolutionDir)int\scintilla\$(ConfigurationName)" DIR_BIN="$(@D)" -e -f scintilla.mak "$@"

$(SolutionDir)bin\System32\SciLexer.dll: \
	$(SolutionDir)lib\MSVCRT.lib \
	$(SolutionDir)lib\EASTL.lib \
	$(SolutionDir)lib\system32.obj \
	$(SolutionDir)lib\monikers.obj
	-md $(SolutionDir)int
	-md $(SolutionDir)int\scintilla
	-md $(SolutionDir)int\scintilla\$(ConfigurationName)
	cd $(SolutionDir)scintilla\win32
	nmake DIR_O="$(SolutionDir)int\scintilla\$(ConfigurationName)" \
		DIR_BIN="$(@D)" \
		LIBS="KERNEL32.lib USER32.lib GDI32.lib IMM32.lib OLE32.LIB UUID.lib $**" \
		CXXFLAGS="-Zi -Fd$$(DIR_O)/ -TP -W4 -EHsc -Zc:forScope -Zc:wchar_t -D_CRT_SECURE_NO_DEPRECATE=1 -Deastl=std -DEASTL_STD_ITERATOR_CATEGORY_ENABLED=0 -I../include -I../src -I../lexlib -I$(SolutionDir)EASTL/include -I$(SolutionDir)EASTLINC -O1 -Oi -MD -DNDEBUG -GL -GS-" \
		LDFLAGS="-OPT:REF -LTCG -SAFESEH:NO -NODEFAULTLIB -ENTRY:DllMain" \
		-e -f scintilla.mak "$@"

$(SolutionDir)bin\$(ConfigurationName)\ODBCTrace.dll: $(SolutionDir)bin\Common\ODBCTrace.dll
	copy "$**" "$@"

$(SolutionDir)int\include\midl\MSRDO20.lic:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\include
	-md $(SolutionDir)int\include\midl
	$(SolutionDir)bin\Common\makelic {9A8831F0-A263-11D1-8DCF-00A0C90FFFC2} MSRDO20.dll > "$@"

$(SolutionDir)bin\$(ConfigurationName)\ODBCQuery.html \
$(SolutionDir)bin\$(ConfigurationName)\ODBCQuery.ini: $(@F)
	-md $(SolutionDir)bin
	-md $(SolutionDir)bin\$(ConfigurationName)
	copy "$(@F)" "$@"
