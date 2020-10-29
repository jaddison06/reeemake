main: build/objects/reeemake.o build/objects/logging.o build/objects/args.o
		g++-8 build/objects/reeemake.o build/objects/logging.o build/objects/args.o -o ./reeemake

clean:
		rm -r ./build/objects
		mkdir ./build/objects
		rm ./reeemake

makefile:
		python3 ./build/generate_makefile.py

build/objects/reeemake.o: ./reeemake.cpp ./reeemake.h
		g++-8 -c ./reeemake.cpp -I . -Wall -Wno-sign-compare -std=c++17 -o ./build/objects/reeemake.o

build/objects/logging.o: ./logging.cpp ./logging.h
		g++-8 -c ./logging.cpp -I . -Wall -Wno-sign-compare -std=c++17 -o ./build/objects/logging.o

build/objects/args.o: ./args.cpp ./args.h
		g++-8 -c ./args.cpp -I . -Wall -Wno-sign-compare -std=c++17 -o ./build/objects/args.o

