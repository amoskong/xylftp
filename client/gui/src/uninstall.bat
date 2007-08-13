::Copyright @ zhaochongri@gmail.com
::Updated 2007.7.25
@echo off 

set sys=
set/p sys=请输入系统所在盘(如: C )：

if not exist %sys%:\"Program Files"\xylftp echo 此文件不存在！
del %sys%:\"Program Files"\xylftp
rmdir %sys%:\"Program Files"\xylftp

echo You have successfully uninstalled this software!

