IncludeDir = /usr/include
./build:
	mkdir build

pdfcommentextractor.cpp:
	g++ -c -ggdb -std=c++14 -I$(IncludeDir)/poppler/glib -I$(IncludeDir)/glib-2.0/ -I/usr/lib/glib-2.0/include -I$(IncludeDir)/cairo/ src/pdfcommentextractor.cpp -o build/pdfcommentextractor.o

all: |./build pdfcommentextractor.cpp
	g++ -ggdb -lpoppler -lpoppler-glib -lglib-2.0 build/pdfcommentextractor.o -opdfcommentextractor

