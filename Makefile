


default:
	gcc -g ~/School/FALL_2021/Security-Intern-Project/implementations/* $(FILE) -o test -lssl -lcrypto

clean:
	rm test
