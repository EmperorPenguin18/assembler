build: assembler.h assembler.c
	gcc -D DEBUG assembler.c -o assembler

release: assembler.h assembler.c
	gcc assembler.c -o assembler

install: assembler.h assembler
	@mkdir -p /usr/include/
	@mkdir -p /usr/bin/
	@cp assembler.h /usr/include/assembler.h
	@mv assembler /usr/bin/assembler

clean:
	rm -f assembler
	rm -f *.txt
