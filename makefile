OBJS = cda.o queue.o integer.o string.o scanner.o tnode.o bst.o gst.o rbt.o trees.o
OOPTS = -Wall -Wextra -g -c -std=gnu99
LOPTS = -Wall -Wextra -g -std=gnu99

trees : $(OBJS)
	gcc $(LOPTS) -o trees $(OBJS)

trees.o : trees.c rbt.h gst.h string.h
	gcc $(OOPTS) trees.c

rbt.o : rbt.c rbt.h gst.h tnode.h
	gcc $(OOPTS) rbt.c

gst.o : gst.c gst.h bst.h
	gcc $(OOPTS) gst.c

bst.o : bst.c bst.h tnode.h
	gcc $(OOPTS) bst.c

tnode.o : tnode.c tnode.h
	gcc $(OOPTS) tnode.c

cda.o : cda.c cda.h
	gcc $(OOPTS) cda.c

queue.o : queue.c queue.h cda.h
	gcc $(OOPTS) queue.c

integer.o : integer.c integer.h
	gcc $(OOPTS) integer.c

string.o : string.c string.h
	gcc $(OOPTS) string.c

scanner.o : scanner.c scanner.h
	gcc $(OOPTS) scanner.c

clean :
	rm -f $(OBJS) trees

test :	trees
	./trees -r test.corpus test.commands

valgrind : trees
	valgrind ./trees -r shakespeare.data shakespeare.more
