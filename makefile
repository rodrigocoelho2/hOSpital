CC := gcc
CFLAGS := -Wall -Wextra -I include
LIBS = -lm

OBJ_dir := obj
BIN_dir := bin
TARGET = hOSpital
OBJECTS = doctor.o main.o memory.o patient.o process.o receptionist.o synchronization.o log.o stats.o configuration.o
doctor.o = doctor.h memory.h
main.o = main.h main-private.h memory.h process.h synchronization.h configuration.h log.h stats.h 
memory.o = memory.h
patient.o = patient.h memory.h synchronization.h
process.o = process.h doctor.h patient.h receptionist.h
receptionist.o = receptionist.h memory.h synchronization.h
synchronization.o = synchronization.h
log.o = log.h
stats.o = stats.h main.h
configuration.o = configuration.h memory.h main-private.h

hOSpital: $(OBJECTS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTS)) -o bin/$(TARGET) $(LIBS)

%.o: src/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<

clean:
	rm -f $(BIN_dir)/hOSpital $(OBJ_dir)/*.o log.txt stats.txt