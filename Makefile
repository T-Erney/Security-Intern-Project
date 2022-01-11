


default:
	gcc -g ~/School/Security-Intern-Project/implementations/* $(FILE) -o test -lssl -lcrypto

clean:
	rm test
