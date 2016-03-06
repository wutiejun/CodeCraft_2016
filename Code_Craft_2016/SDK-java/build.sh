#!/bin/bash
basepath=$(cd `dirname $0`; pwd)
APP_HOME=$basepath

#±àÒë
echo building...
MAKE_FILE=$APP_HOME/makelist.txt
cd "$APP_HOME/code/future_net/src"
javac -d $APP_HOME/code/future_net/bin -encoding UTF-8 @$MAKE_FILE

#´ò°ü
echo make jar...
cd "$APP_HOME/code/future_net/bin"
JAR_NAME=$APP_HOME/bin/future_net.jar
jar -cvf $JAR_NAME *

cd $APP_HOME

echo build jar success!
exit
