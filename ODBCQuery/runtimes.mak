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
	$(SolutionDir)lib\MFC42.lib
	
$(SolutionDir)lib\MSVCRT.lib:
	-md $(SolutionDir)lib
	$(SolutionDir)bin\Common\makedef "%%SystemRoot%%\Microsoft.NET\Framework\v1.1.4322\MSVCR71.dll" > "$(SolutionDir)lib\MSVCRT.def"
	lib /MACHINE:X86 /DEF:"$(SolutionDir)lib/MSVCRT.def" /OUT:"$@"

$(SolutionDir)lib\MFC42.lib:
	-md $(SolutionDir)lib
	$(SolutionDir)bin\Common\makedef "%%SystemRoot%%\System32\MFC42.dll" "%%SystemRoot%%\symbols\dll\MFC42.pdb" "\dm\MFC4.21\src\intel\MFC42.DEF" > "$(SolutionDir)lib\MFC42.def"
	lib /MACHINE:X86 /DEF:"$(SolutionDir)lib/MFC42.def" /OUT:"$@"

$(SolutionDir)bin\Debug\SciLexer.dll:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\scintilla
	-md $(SolutionDir)int\scintilla\$(ConfigurationName)
	cd $(SolutionDir)scintilla\win32
	nmake DEBUG=1 DIR_O="$(SolutionDir)int\scintilla\$(ConfigurationName)" DIR_BIN="$(@D)" -e -f scintilla.mak "$@"

$(SolutionDir)bin\Release\SciLexer.dll \
$(SolutionDir)bin\System32\SciLexer.dll:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\scintilla
	-md $(SolutionDir)int\scintilla\$(ConfigurationName)
	cd $(SolutionDir)scintilla\win32
	nmake DIR_O="$(SolutionDir)int\scintilla\$(ConfigurationName)" DIR_BIN="$(@D)" -e -f scintilla.mak "$@"

$(SolutionDir)bin\$(ConfigurationName)\ODBCTrace.dll: $(SolutionDir)bin\Common\ODBCTrace.dll
	copy "$**" "$@"

$(SolutionDir)int\include\midl\MSRDO20.lic:
	-md $(SolutionDir)int
	-md $(SolutionDir)int\include
	-md $(SolutionDir)int\include\midl
	$(SolutionDir)bin\Common\makelic {9A8831F0-A263-11D1-8DCF-00A0C90FFFC2} MSRDO20.dll > "$@"

$(SolutionDir)bin\$(ConfigurationName)\ODBCQuery.html \
$(SolutionDir)bin\$(ConfigurationName)\ODBCQuery.ini:
	-md $(SolutionDir)bin
	-md $(SolutionDir)bin\$(ConfigurationName)
	copy "%%MakeDir%%\$(@F)" "$@"
