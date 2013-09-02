input=main.c local.c net.c message.c list.c
output=flyfile

CFLAGS=-Wall -pthread

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
