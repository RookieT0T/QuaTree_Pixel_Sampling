clean:
	rm *.txt

run: quaTree.cpp
	clang++ -Wall -std=c++11 quaTree.cpp -o quaTree
	./quaTree >> out.txt