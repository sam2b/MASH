CC=gcc
CFLAGS=-fno-stack-protector
LDFLAGS=-lm

binaries=mash

all: $(binaries)

clean:
	$(RM) -f $(binaries) *.o *.out *.output 


 
