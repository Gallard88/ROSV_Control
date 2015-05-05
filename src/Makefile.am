# what flags you want to pass to the C compiler & linker
AM_CXXFLAGS = --pedantic -Wall -std=gnu++0x -pthread
AM_CFLAGS = --pedantic -Wall

#LDFLAGS =
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
ROSV_Controldir = /usr/bin/
ROSV_Control_PROGRAMS = ROSV_Control

ROSV_Control_Filesdir= /etc/ROSV_Control
ROSV_Control_Files_DATA= \
	camera.json \
	lighting.json \
	motors.json \
	power.json

LDADD = \
	/usr/local/lib/libPWMControl.a \
        /usr/local/lib/libPowerMonitor.a \
	/usr/local/lib/libparson.a \
	/usr/local/lib/libRealTime.a

ROSV_Control_SOURCES = \
	Alarm.cpp \
	Alarm.h \
	AlarmManager.cpp \
	AlarmManager.h \
	CameraManager.cpp \
	CameraManager.h \
	ClientSocket.cpp \
	ClientSocket.h \
	CmdModule.h \
	EventMessages.cpp \
	EventMessages.h \
	LightManager.cpp \
	LightManager.h \
	MsgQueue.cpp \
	MsgQueue.h \
	Motor.cpp \
	Motor.h \
	Permissions.cpp \
	Permissions.h \
	PermissionManager.cpp \
	PermissionManager.h \
	PowerManager.cpp \
	PowerManager.h \
	Navigation.cpp \
	Navigation.h \
	SubControl.cpp \
	SubControl.h \
	SubProtocol.cpp \
	SubProtocol.h \
	TcpServer.cpp \
	TcpServer.h \
	VideoStream.cpp \
	VideoStream.h \
	Variable.cpp \
	Variable.h \
	main.cpp

