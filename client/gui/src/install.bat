::Copyright @ zhaochongri@gmail.com
::Updated 2007.7.25

@echo off

echo Welcome to use XylFTP! You must have a JVM environment and your JDK version should be 1.6 or over.
echo Your working directory is:
cd
set sys=
set/p sys=请输入系统所在盘(如: C )：

echo And this software will be installed in %sys%:\"Program Files"\xylftp\

javac XylFTPGUI.java
jar cfm XylFTPGUI.jar manifest.mf *.class Icon
if not exist %sys%:\"Program Files"\xylftp mkdir %sys%:\"Program Files"\xylftp  
move XylFTPGUI.jar %sys%:\"Program Files"\xylftp\XylFTPGUI.jar
copy XylFTPGUI.vbs %sys%:\"Program Files"\xylftp\XylFTPGUI.vbs
copy uninstall.bat %sys%:\"Program Files"\xylftp\uninstall.bat
copy README.txt %sys%:\"Program Files"\xylftp\README.txt
del *.class

echo @echo off >%sys%:\"Program Files"\xylftp\xylftpgui.bat
echo java -jar "%sys%:\Program Files\xylftp\XylFTPGUI.jar" >>%sys%:\"Program Files"\xylftp\xylftpgui.bat


echo You have successfully installed this software!
