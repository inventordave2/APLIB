# Very simple. Compile object file aplib.o, and link to app object file.
# This simple Makefile assumes a console .exe output.

CC =gcc
NOEXE =-c
OPTIM =-O0
DEBUG =-g -DDEBUG
COMPILE =$(CC) $(DEBUG) $(OPTIM)
CURL_CFLAGS ?=$(shell pkg-config --cflags libcurl 2>/dev/null)
CURL_LIBS ?=$(shell pkg-config --libs libcurl 2>/dev/null || printf '%s' '-lcurl')
EXPECT_STDLIB =-DEXPECT_STDLIB
NO_STDLIB =-DNO_STDLIB
appname=app

.PHONY: curl_module

all: aplib lean_string trig ieee754 native curl_module.o tests app_front

debug_it:
	gdb ./$(appname).exe

tests: ./tests.c ./tests.h
	$(COMPILE) $(NOEXE) $(EXPECT_STDLIB) -o ./tests.o ./tests.c

aplib: ./aplib.c ./aplib.h ./aplib_constants.h ./lean_string.h 
	$(COMPILE) $(NOEXE) $(EXPECT_STDLIB) -o ./aplib.o ./aplib.c

lean_string: ./lean_string.h
	$(COMPILE) $(NOEXE) -o ./lean_string.o ./lean_string.h

trig: ./trig_portable.c ./trig.h
	$(COMPILE) $(NOEXE) -o ./trig.o ./trig_portable.c

ieee754: ./ieee754.c ./ieee754.h
	$(COMPILE) $(NOEXE) -o ./ieee754.o ./ieee754.c

native: ./native.c ./native.h
	$(COMPILE) $(NOEXE) -o ./native.o ./native.c

curl_module.o: ./curl_module.c ./curl_module.h
	$(COMPILE) $(CURL_CFLAGS) $(NOEXE) -o ./curl_module.o ./curl_module.c

curl_module: curl_module.o

app_front: ./main.c ./lean_string.h ./curl_module.o
	$(COMPILE) $(EXPECT_STDLIB) -o ./$(appname).exe ./aplib.o ./trig.o ./ieee754.o ./native.o ./curl_module.o ./tests.o ./main.c -lm $(CURL_LIBS)

clean:
	rm *.o

wipe: clean
	rm *.exe

