CC = gcc
CFLAGS = -Wall -g

FILES =\
	msdos

CFILES = $(addsuffix .c, $(FILES))
OFILES = $(addsuffix .o, $(FILES))

MAIN = msdos

.PHONY: $(MAIN)

$(MAIN): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

.PHONY: clean

clean:
	rm -f *.o *~ .#* msdos




