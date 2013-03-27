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

jar:	xylftp.jar
xylftp.jar:	all
	jar cfm $@ manifest.mf *.class Icon/*
	echo -n "" > ./xylftp
	echo "#!/bin/sh" >> ./xylftp
	echo "# Copyright (C) Xi You Linux Group." >> ./xylftp
	echo "# GPLv2 applies." >> ./xylftp
	echo "java -jar ./xylftp.jar $$""@" >> ./xylftp
	echo "" >> ./xylftp
	chmod +x xylftp

install: xylftp.jar
	mkdir -p $(INSTALLDIR)
	mv ./xylftp.jar $(INSTALLDIR)
	cp ../../doc/User_Manual_Client.pdf $(INSTALLDIR)/
	echo "#!/bin/sh" >> ./tmp.sh
	echo "# Copyright (C) Xi You Linux Group." >> ./tmp.sh
	echo "# GPLv2 applies." >> ./tmp.sh
	echo "java -jar "$(INSTALLDIR)"/xylftp.jar $$""@" >> ./tmp.sh
	echo "" >> ./tmp.sh
	chmod +x ./tmp.sh
	mv ./tmp.sh $(BINDIR)/xylftp

uninstall: clean
	rm -rf $(INSTALLDIR)
	rm -f $(BINDIR)/xylftp

