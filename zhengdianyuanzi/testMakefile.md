main:main.o input.o calcu.o
    gcc -o main main.o input.o calcu.o
main.o :main.c
    gcc -c main.c
input.o: input.c
    gcc -c input.c
calcu.o: calcu.c
    gcc -c calcu.c


clear:
    rm *.o
    rm main

objects = main.o input.o calcu.o
main: $(objects)
    gcc -o main $(objects)

objects = main.o input.o calcu.o
main: $(objects)
    gcc -o main $(objects)

%.o : %.c
    gcc -c $<

.PHONY clean #伪代码，即使文件夹中有clean文件，也会运行Makefile里面的clean

clean:
    rm *.o
    rm main