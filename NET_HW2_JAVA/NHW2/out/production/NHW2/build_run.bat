rem set JAVA_HOME "..\jdk1.8.0_291\"
rem set classpath %JAVA_HOME%\lib\tools.jar;%JAVA_HOME%\lib\dt.jar;."

..\jdk1.8.0_291\bin\javac  -encoding "utf-8" -classpath ./jna-5.9.0.jar MyDecode.DecodeHelper.java
..\jdk1.8.0_291\bin\javac  -encoding "utf-8" -classpath ./ MyDecode.Decode.java

..\jdk1.8.0_291\bin\java -Djava.ext.dirs=./ -Djna.library.path=../ MyDecode.Decode