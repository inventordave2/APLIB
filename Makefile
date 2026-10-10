# Very simple. Compile object file aplib.o, and link to app object file. This simple Makefile assumes WIN32 console .exe output.

CC=gcc
NOEXE = -c 
OPTIM = O0 
DEBUG =-g -DDEBUG
COMPILE =$(CC) $(DEBUG) $(OPTIM)
EXPECT_STDLIB = -DEXPECT_STDLIB 
NO_STDLIB = -DNO_STDLIB 


all: aplib lean_string trig ieee754 native app_front

aplib: ./aplib.c ./aplib.h ./aplib_constants.h ./lean_string.h 
    $(COMPILE) $(NOEXE) $(EXPECT_STDLIB) -o ./aplib.o ./aplib.c

lean_string: ./lean_string.h 
    $(COMPILE) $(NOEXE) -o ./lean_string.o ./lean_string.h 

trig: ./trig.c ./trig.h
    $(COMPILE) $(NOEXE) -o ./trig.o ./trig.c 

ieee754: ./ieee754.c ./ieee754.h 
    $(COMPILE) $(NOEXE) -o ./ieee754.o ./ieee754.c 

native: ./native.c ./native.h
    $(COMPILE) $(NOEXE) -o ./native.o ./native.c

app_front: ./main.c ./lean_string.h
    $(COMPILE) $(EXPECT_STDLIB) -o ./app.exe ./aplib.o ./trig.o ./ieee754.o ./native.o ./main.c 
    


