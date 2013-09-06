input=main.c local.c net.c message.c list.c interface.c cmd.c
output=flyfile

##CFLAGS=-std=gnu99 -Wall -pthread
CFLAGS=-Wall -pthread -lncurses -lpanel

all:$(output)

$(output):$(input:.c=.o)
	gcc  $(CFLAGS) $^ -o $@

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	-rm *.o
cleanall:
	-rm $(output) *.o

.PHONY:all clean cleanall
