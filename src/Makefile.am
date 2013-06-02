# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = --pedantic -Wall
AM_CFLAGS = --pedantic -Wall

#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
ROSV_Controldir = /usr/bin/
ROSV_Control_PROGRAMS = ROSV_Control

ROSV_Control_Filesdir= /etc
ROSV_Control_Files_DATA=ROSV_Motors.json
LDADD=/usr/local/lib/libPWMControl.a

ROSV_Control_SOURCES = \
	ControlProtocol.cpp \
	ControlProtocol.h \
	DataLogger.cpp \
	DataLogger.h \
	DataSource.h \
	LightManager.cpp \
	LightManager.h \
	parson.c \
	parson.h \
	TcpServer.cpp \
	TcpServer.h \
	main.cpp

