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
LDADD = \
	/usr/local/lib/libPWMControl.a \
	/usr/local/lib/libINS.a

ROSV_Control_SOURCES = \
	CameraManager.cpp \
	CameraManager.h \
	CmdModule.h \
	ControlProtocol.cpp \
	ControlProtocol.h \
	DataSource.cpp \
	DataSource.h \
	LightManager.cpp \
	LightManager.h \
	parson.c \
	parson.h \
	PowerManager.cpp \
	PowerManager.h \
	SubControl.cpp \
	SubControl.h \
	SubProtocol.cpp \
	SubProtocol.h \
	TcpServer.cpp \
	TcpServer.h \
	main.cpp

