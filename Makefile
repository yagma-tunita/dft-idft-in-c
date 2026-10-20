CC      := gcc
TARGET  := run.exe
OBJS    := main.o fft.o complex.o signal.o view.o plot.o twiddle.o
CFLAGS  := -O2 -Wall -Wextra
LDFLAGS := -mwindows
LIBS    := -lgdi32 -luser32 -lm

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o:    main.c    fft.h signal.h plot.h view.h complex.h
fft.o:     fft.c     fft.h twiddle.h complex.h
complex.o: complex.c complex.h
signal.o:  signal.c  signal.h
view.o:    view.c    view.h plot.h
plot.o:    plot.c    plot.h
twiddle.o: twiddle.c twiddle.h complex.h

clean:
	del /Q /F $(OBJS) $(TARGET) 2>NUL

.PHONY: all clean