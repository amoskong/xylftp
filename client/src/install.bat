::Copyright @ kongjianjun@gmail.com
::Updated 2007.7.2

@echo off

echo Welcome to use XylFTP! And you must have a JVM environment.
echo Your working directory is:
cd
set sys=
set/p sys=请输入系统所在盘(如: C )：

echo And this software will be installed in %sys%:\"Program Files"\xylftp\

javac XylFTPMain.java
jar cfm xylftp.jar manifest.mf *.class
if not exist %sys%:\"Program Files"\xylftp mkdir %sys%:\"Program Files"\xylftp  
move xylftp.jar %sys%:\"Program Files"\xylftp\xylftp.jar
cd ../doc/
copy User_Manual_Client.pdf %sys%:\"Program Files"\xylftp\User_Manual_Client.pdf
echo Creat link!

echo @echo off >%sys%:\WINDOWS\system32\xylftp.bat
echo java -jar "%sys%:\Program Files\xylftp\xylftp.jar" %%1 %%2 %%3 %%4 %%5 %%6 %%7 %%8 %%9 >>%sys%:\WINDOWS\system32\xylftp.bat


echo You have successfully installed this software!
