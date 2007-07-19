# This is the Makefile for xylftp.
# Copyright (C) WANG Cong, Apr. 30, 2007.
# GPLv2 applies.

.PHONY: all clean debug check

INSTALLDIR=/usr/local/share/xylftp
BINDIR=/usr/bin
JAVAC=javac

all:	XylFTPGUI.java XylFTPConnection.java XylFTPInterface.java XylFTPException.java XylTelnetConnection.java
	$(JAVAC) XylFTPGUI.java

check:
ifdef JAVA_HOME
	echo "$$JAVA_HOME seems OK."
else
	echo "Can't find java run time environment."
	exit
endif
ifdef CLASSPATH
	echo "$$CLASSPATH seems OK."
	echo "Check successfully. You can try \"make install\" now."
else
	echo "Can't find java run time environment."
	exit
endif

debug:  XylFTPGUI.java XylFTPConnection.java XylFTPInterface.java XylFTPException.java XylTelnetConnection.java
	$(JAVAC) -g:lines,vars,source XylFTPGUI.java

clean:
	rm -f ./*.class ./*.jar
	rm -f ./xylftp ./tmp.sh
