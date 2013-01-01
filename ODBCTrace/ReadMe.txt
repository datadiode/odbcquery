The purpose of ODBCTrace is to trace calls into the ODBC API on a Win32 machine,
while offering a reasonable user interface and more elaborated options than the
odbctrac.dll shipped by Microsoft.

For a very readable article on ODBC tracing, take a look at
http://www.codeproject.com/database/ODBCTracer.asp.

While Doga Arinir's article proved to be an excellent starting point, his code
somewhat lacks efficiency due to its heavily using STL strings and collections,
and a late shortcutting of calls for which tracing is turned off.

Features:
- Permits you to enable/disable tracing for every individual function so you can trade
  level of detail against execution time overhead, and reduce undesired output noise.
- Optionally dumps values of bound columns after fetch/update/refresh.
- Upon statement execution, dumps the entire statement including values of bound parameters.
- Is thread safe (I think).
- Tells you ID of calling thread.
- Tells you nesting levels of calls within calling thread (particularly helpful with cursor library).
- Tells you approximate execution time of each call.
- Sends output to either a file and/or a RichEdit based log window.

Tech tweaks:
- Uses C++ smart pointer for thread local storage access.
- Pops up Options dialog from DLL_PROCESS_ATTACH by creating a satellite process.
- Attaches a TabRuler to a RichEdit control without subclassing.
- Doesn't link to MSVCRT.
- Uses IAT/EAT patching to hook into process termination.
- Uses EAT/IAT patching to catch debug strings.

Todo:
- Improve "bulk awareness" (i.e. be aware of bulk inserts & updates).

[CHANGELOG]

120:

o Make it operable on Vista

110b:

o BUGFIX: While RUNDLL32.EXE is expected to expect a comma between DllName and
  FunctionName, ODBCTrace was erroneously passing a space (which unfortunately
  happened to work for me, but has been observed to fail on a Windows NT box).

o BUGFIX: No longer rely on Chinese (Simplified) locale for formatting dates,
  as this has been observed to yield odd results on a Windows NT box.

109b:

o BUGFIX: ODBCTraceHookIntoSystemTracer() missed to hook TraceSQLColAttributes()
  due to inappropriate use of lstrcmpA() in CModuleDirectory::FindEATEntry().

o BUGFIX: An attempt to use "Wait for close" or "Catch debug strings" option on
  a system lacking the Toolhelp32 API (i.e., Windows NT) would cause a crash.

o ODBCTraceHookIntoSystemTracer() will now return 0 in case Options dialog has
  been canceled. If that happens, the calling application should FreeLibrary()
  the ODBCTrace.dll.

108b:

o Applications can explicitly LoadLibrary("ODBCTrace.dll") from a location other
  than the windows system directory, and then call ODBCTraceHookIntoSystemTracer()
  to load and patch Microsoft's odbctrac.dll so calls into the latter will actually
  end up in ODBCTrace. This allows ODBCTrace to take effect without the need for
  changing the registry and copying ODBCTrace.dll to the windows system directory.

107b:

o LoaderLock issues again:
  - Revert from ForceForegroundWindow() to plain SetForegroundWindow().
  - Use SendMessageCallback() to pass debug strings to trace window asynchronously.

o Issue an extra LoadLibrary() to prevent ODBCTrace.dll from being unloaded.

106b:

o Fix potential deadlock due to LoaderLock when using 'Trace thread lifetime' option with file output turned on.
o Fix file output line endings - 105b writes plain LFs rather than CR/LF in some places.

105b:

o The trace window generates infotips on selected text:
  - Doubleclick a hexadecimal number in 0x notation to see its decimal representation.
  - Doubleclick a decimal number to see its hexadecimal representation.
  - Doubleclick a field name inside an INSERT statement to see the corresponding value.
  - Tripleclick a line to see its entire contents in a word wrapped infotip.

o The trace window can be edited while tracing is disabled. One possible application
  is to infotip an INSERT statement pasted in through the clipboard.

o Don't force caret into view after tab ruler adjustment.
o No horizontal scrolling while drag-selecting entire lines.
o The trace window provides a context menu with some usual edit commands.

104b:

o BUGFIX: 'Disable trace' now also disables OutputDebugString() tracing.
o 'Kill Process' command to kill the hosting process (invocable from Options dialog).
o 'Trace thread lifetime' option to trace start and termination of threads.

103b:

o 'Catch debug strings' option to catch OutputDebugString() calls.

102b:

o Fix SQL_VARCHAR parameter formatting (Patch #1601248).
o Limit log file size. If file grows beyond limit, rename to something like ODBCTrace--.log and start a new one.
o 'Wait for close' option to delay process termination until user closes trace window.

101b: Initial release.
