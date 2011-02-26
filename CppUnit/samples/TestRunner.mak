# Microsoft Developer Studio Generated NMAKE File, Based on TestRunner.dsp
!IF "$(CFG)" == ""
CFG=TestRunner - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TestRunner - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TestRunner - Win32 Release" && "$(CFG)" !=\
 "TestRunner - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TestRunner.mak" CFG="TestRunner - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TestRunner - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "TestRunner - Win32 Debug" (based on\
 "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TestRunner - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestRunner.exe"

!ELSE 

ALL : "$(OUTDIR)\TestRunner.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AssertionFailedError.obj"
	-@erase "$(INTDIR)\ExampleTestCase.obj"
	-@erase "$(INTDIR)\Multicaster.obj"
	-@erase "$(INTDIR)\MulticasterTest.obj"
	-@erase "$(INTDIR)\TestCase.obj"
	-@erase "$(INTDIR)\TestFailure.obj"
	-@erase "$(INTDIR)\TestResult.obj"
	-@erase "$(INTDIR)\TestRunner.obj"
	-@erase "$(INTDIR)\TestSuite.obj"
	-@erase "$(INTDIR)\TextTestResult.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\TestRunner.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D\
 "_MBCS" /Fp"$(INTDIR)\TestRunner.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestRunner.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\TestRunner.pdb" /machine:I386 /out:"$(OUTDIR)\TestRunner.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AssertionFailedError.obj" \
	"$(INTDIR)\ExampleTestCase.obj" \
	"$(INTDIR)\Multicaster.obj" \
	"$(INTDIR)\MulticasterTest.obj" \
	"$(INTDIR)\TestCase.obj" \
	"$(INTDIR)\TestFailure.obj" \
	"$(INTDIR)\TestResult.obj" \
	"$(INTDIR)\TestRunner.obj" \
	"$(INTDIR)\TestSuite.obj" \
	"$(INTDIR)\TextTestResult.obj"

"$(OUTDIR)\TestRunner.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\TestRunner.exe"

!ELSE 

ALL : "$(OUTDIR)\TestRunner.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\AssertionFailedError.obj"
	-@erase "$(INTDIR)\ExampleTestCase.obj"
	-@erase "$(INTDIR)\Multicaster.obj"
	-@erase "$(INTDIR)\MulticasterTest.obj"
	-@erase "$(INTDIR)\TestCase.obj"
	-@erase "$(INTDIR)\TestFailure.obj"
	-@erase "$(INTDIR)\TestResult.obj"
	-@erase "$(INTDIR)\TestRunner.obj"
	-@erase "$(INTDIR)\TestSuite.obj"
	-@erase "$(INTDIR)\TextTestResult.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\TestRunner.exe"
	-@erase "$(OUTDIR)\TestRunner.ilk"
	-@erase "$(OUTDIR)\TestRunner.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GR /GX /Zi /Od /I "..\framework" /I\
 "..\..\samples" /I "..\..\samples\Multicaster" /I "..\framework\extensions" /D\
 "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\TestRunner.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\TestRunner.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\TestRunner.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\TestRunner.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AssertionFailedError.obj" \
	"$(INTDIR)\ExampleTestCase.obj" \
	"$(INTDIR)\Multicaster.obj" \
	"$(INTDIR)\MulticasterTest.obj" \
	"$(INTDIR)\TestCase.obj" \
	"$(INTDIR)\TestFailure.obj" \
	"$(INTDIR)\TestResult.obj" \
	"$(INTDIR)\TestRunner.obj" \
	"$(INTDIR)\TestSuite.obj" \
	"$(INTDIR)\TextTestResult.obj"

"$(OUTDIR)\TestRunner.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "TestRunner - Win32 Release" || "$(CFG)" ==\
 "TestRunner - Win32 Debug"
SOURCE=..\framework\AssertionFailedError.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_ASSER=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	

"$(INTDIR)\AssertionFailedError.obj" : $(SOURCE) $(DEP_CPP_ASSER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_ASSER=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	

"$(INTDIR)\AssertionFailedError.obj" : $(SOURCE) $(DEP_CPP_ASSER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\samples\ExampleTestCase.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_EXAMP=\
	"..\..\samples\ExampleTestCase.h"\
	
NODEP_CPP_EXAMP=\
	"..\..\samples\TestCase.h"\
	"..\..\samples\TestSuite.h"\
	

"$(INTDIR)\ExampleTestCase.obj" : $(SOURCE) $(DEP_CPP_EXAMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_EXAMP=\
	"..\..\samples\ExampleTestCase.h"\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestCase.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	"..\framework\TestSuite.h"\
	

"$(INTDIR)\ExampleTestCase.obj" : $(SOURCE) $(DEP_CPP_EXAMP) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\samples\Multicaster\Multicaster.cpp
DEP_CPP_MULTI=\
	"..\..\samples\Multicaster\Multicaster.h"\
	

"$(INTDIR)\Multicaster.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\samples\Multicaster\MulticasterTest.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_MULTIC=\
	"..\..\samples\Multicaster\Multicaster.h"\
	"..\..\samples\Multicaster\MulticasterTest.h"\
	
NODEP_CPP_MULTIC=\
	"..\..\samples\Multicaster\TestCase.h"\
	"..\..\samples\Multicaster\TestSuite.h"\
	

"$(INTDIR)\MulticasterTest.obj" : $(SOURCE) $(DEP_CPP_MULTIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_MULTIC=\
	"..\..\samples\Multicaster\Multicaster.h"\
	"..\..\samples\Multicaster\MulticasterTest.h"\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestCase.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	"..\framework\TestSuite.h"\
	

"$(INTDIR)\MulticasterTest.obj" : $(SOURCE) $(DEP_CPP_MULTIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\framework\TestCase.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_TESTC=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestCase.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestCase.obj" : $(SOURCE) $(DEP_CPP_TESTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TESTC=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestCase.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestCase.obj" : $(SOURCE) $(DEP_CPP_TESTC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\framework\TestFailure.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_TESTF=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestFailure.obj" : $(SOURCE) $(DEP_CPP_TESTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TESTF=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestFailure.obj" : $(SOURCE) $(DEP_CPP_TESTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\framework\TestResult.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_TESTR=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestResult.obj" : $(SOURCE) $(DEP_CPP_TESTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TESTR=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	

"$(INTDIR)\TestResult.obj" : $(SOURCE) $(DEP_CPP_TESTR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\TestRunner.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_TESTRU=\
	".\TextTestResult.h"\
	
NODEP_CPP_TESTRU=\
	".\ExampleTestCase.h"\
	".\MulticasterTest.h"\
	".\TestResult.h"\
	".\TestSuite.h"\
	".\TestTest.h"\
	

"$(INTDIR)\TestRunner.obj" : $(SOURCE) $(DEP_CPP_TESTRU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TESTRU=\
	"..\..\samples\ExampleTestCase.h"\
	"..\..\samples\Multicaster\Multicaster.h"\
	"..\..\samples\Multicaster\MulticasterTest.h"\
	"..\..\samples\TestTest.h"\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestCase.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	"..\framework\TestSuite.h"\
	".\TextTestResult.h"\
	

"$(INTDIR)\TestRunner.obj" : $(SOURCE) $(DEP_CPP_TESTRU) "$(INTDIR)"


!ENDIF 

SOURCE=..\framework\TestSuite.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"


"$(INTDIR)\TestSuite.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TESTS=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	"..\framework\TestSuite.h"\
	

"$(INTDIR)\TestSuite.obj" : $(SOURCE) $(DEP_CPP_TESTS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\TextTestResult.cpp

!IF  "$(CFG)" == "TestRunner - Win32 Release"

DEP_CPP_TEXTT=\
	".\TextTestResult.h"\
	
NODEP_CPP_TEXTT=\
	".\Test.h"\
	".\TestResult.h"\
	

"$(INTDIR)\TextTestResult.obj" : $(SOURCE) $(DEP_CPP_TEXTT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "TestRunner - Win32 Debug"

DEP_CPP_TEXTT=\
	"..\framework\AssertionFailedError.h"\
	"..\framework\CppUnit.h"\
	"..\framework\CppUnitException.h"\
	"..\framework\Test.h"\
	"..\framework\TestFailure.h"\
	"..\framework\TestResult.h"\
	".\TextTestResult.h"\
	

"$(INTDIR)\TextTestResult.obj" : $(SOURCE) $(DEP_CPP_TEXTT) "$(INTDIR)"


!ENDIF 


!ENDIF 

