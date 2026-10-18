CC      := gcc
TARGET  := run.exe
OBJS    := main.o dft.o complex.o signal.o view.o plot.o
CFLAGS  := -O2 -Wall -Wextra
LDFLAGS := -mwindows
LIBS    := -lgdi32 -luser32 -lm

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o:    main.c    dft.h signal.h plot.h view.h complex.h
dft.o:     dft.c     dft.h complex.h
complex.o: complex.c complex.h
signal.o:  signal.c  signal.h
view.o:    view.c    view.h plot.h
plot.o:    plot.c    plot.h

clean:
	del /Q /F $(OBJS) $(TARGET) 2>NUL

.PHONY: all clean