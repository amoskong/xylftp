::Copyright @ kongjianjun@gmail.com
::Updated 2007.7.2

@echo off 

set sys=
set/p sys=������ϵͳ������(��: C )��

del *.class
del %sys%:\WINDOWS\system32\xylftp.bat
del %sys%:\"Program Files"\xylftp\xylftp.jar
del %sys%:\"Program Files"\xylftp\User_Manual_Client.pdf
rmdir %sys%:\"Program Files"\xylftp

echo You have successfully uninstalled this software!
