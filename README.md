ODBCQuery is an ODBC-based SQL database query tool I created quite some time ago. It is heavily inspired by http://www.codeproject.com/database/isqlmfc.asp.

Things are arranged to enable Visual C++ 2008 Express to compile the code with marginally modified MFC/ATL headers from http://ftp.digitalmars.com/Digital_Mars_C++/MS/4.21/. This involves the creation of an import library based on information gathered from %SystemRoot%\MFC42.dll and %SystemRoot%\symbols\dll\MFC42.pdb, which is then used to upgrade the MFC42.def found in the MFC4.21.zip as required. The most relevant point of failure in this experiment is how to catch exceptions thrown from inside MFC42.