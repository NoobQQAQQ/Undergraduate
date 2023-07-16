turing:
	g++ tm.h tm.cpp main.cpp -o ../turing

.PHONY: clean test
test:
	cd ..; ./turing case1
	cd ..; ./turing case2
clean:
	rm -rf ../turing ../case1/console.txt ../case1/result.txt ../case2/console.txt ../case2/result.txt
