@echo off
set APP_HOME=%~dp0

rem ±àÒë
echo building...
set MAKE_FILE=%APP_HOME%/makelist.txt
cd /d "%APP_HOME%/code/future_net/src"
javac -d ../bin -encoding UTF-8 @%MAKE_FILE%

rem ´ò°ü
echo make jar...
cd /d "%APP_HOME%/code/future_net/bin"
set JAR_NAME=%APP_HOME%/bin/future_net.jar
jar -cvf %JAR_NAME% *

cd /d %APP_HOME%
pause

echo build jar success!
exit
