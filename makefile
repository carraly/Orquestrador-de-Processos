CFLAGS = -Wall -Wextra -g

processflow: main.o sequencial.o paralelo.o pipe.o files.o
	gcc $(CFLAGS) -o processflow $^

%.o: %.c header.h
	gcc $(CFLAGS) -c $<

run: processflow
	./processflow

clean:
	rm -f *.o processflow

.PHONY: run clean	