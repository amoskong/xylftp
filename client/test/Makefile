# Makefile for test cases.
# Copyright (C) WANG Cong, May 5, 2007

.PHONY: all clean
all: testXylTelnet.java testXylFTP.java
	(cd ../src; make all)
	mv ../src/*.class ./
	javac testXylTelnet.java
	javac testXylFTP.java
	javac testXylFTPCLI.java
clean:
	rm -f ./*.class
