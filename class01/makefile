all:
	as  --32 -o hello.o hello1.s
	gcc -m32 -s -o hello hello.o

hello64:
	as  -o hello.o hello64.s
	gcc -s -o hello hello.o	