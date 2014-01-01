
all:
	(cd src && $(MAKE))
	mv src/xfps .

clean:
	(cd src && $(MAKE))
	rm -f xfps core

