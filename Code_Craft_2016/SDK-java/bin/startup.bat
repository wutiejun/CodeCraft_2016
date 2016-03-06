@echo off

if "%OS%"=="Windows_NT" @setlocal

rem %~dp0 is expanded pathname of the current script under NT
set DEFAULT_PNS_HOME=%~dp0..
echo %DEFAULT_PNS_HOME%
if "%PNS_HOME%"=="" set PNS_HOME=%DEFAULT_PNS_HOME%
set DEFAULT_PNS_HOME=

:doneStart
goto checkJava

:checkJava
set _JAVACMD=%JAVACMD%

if "%JAVA_HOME%" == "" goto noJavaHome
if not exist "%JAVA_HOME%\bin\java.exe" goto noJavaHome
if "%_JAVACMD%" == "" set _JAVACMD=%JAVA_HOME%\bin\java.exe
goto runAnt

:noJavaHome
if "%_JAVACMD%" == "" set _JAVACMD=java.exe
echo.
echo Warning: JAVA_HOME environment variable is not set.
echo.

:runAnt

if "%PNS_BASE%" == "" set PNS_BASE=%PNS_HOME%

if "%PNS_ETC%" == "" set PNS_ETC=%PNS_HOME%\etc

if "%PNS_OPTS%" == "" set PNS_OPTS=-Xms64M -Xmx64M

rem set classpath
set MYCLASSPATH=.\*;
"%_JAVACMD%"  %PNS_OPTS% -cp "%MYCLASSPATH%" "com.filetool.main.Main" %1 %2 %3

goto end

:end
set _JAVACMD=
if "%OS%"=="Windows_NT" @endlocal

:mainEnd