all:
	g++ main.cpp OSDPlib.c OSDPlib.h log.h log.c -lssl -lcrypto -g
