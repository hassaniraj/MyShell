


build:
	gcc *.c -o myshell


run:
	gcc *.c -o myshell; ./myshell

debug:
	gcc *.c -o myshell -g; gdb -q myshell
      

clean:
	rm myshell





b: build
c: clean
r: run 
d: debug 
