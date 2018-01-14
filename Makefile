IncludeDir = /usr/include



poppler: | lib/poppler/build
	cd lib/poppler/build/ && cmake .. -DBUILD_GTK_TESTS=ON -DBUILD_QT5_TESTS=OFF -DBUILD_CPP_TESTS=ON -DENABLE_SPLASH=OFF -DENABLE_UTILS=OFF -DENABLE_CPP=ON -DENABLE_GLIB=ON -DENABLE_GTK_DOC=ON -DENABLE_QT5=OFF -DENABLE_LIBCURL=OFF -DENABLE_CMS=none -DENABLE_LIBOPENJPEG=none -DCMAKE_BUILD_TYPE=release
	cd lib/poppler/build/ && make

install-poppler:
	cd lib/poppler/build/ && make install


lib/poppler/build:
	mkdir lib/poppler/build

#Cleans the library directory
clean_libs:
	cd lib/poppler/build/ && make clean

./build:
	mkdir build


pdfcommentextractor.cpp:
	g++ -c -ggdb -std=c++14 -I$(IncludeDir)/poppler/glib -I$(IncludeDir)/glib-2.0/ -I/usr/lib/glib-2.0/include -I$(IncludeDir)/cairo/ src/pdfcommentextractor.cpp -o build/pdfcommentextractor.o

all: |./build pdfcommentextractor.cpp
	g++ -ggdb -lpoppler -lpoppler-glib -lglib-2.0 build/pdfcommentextractor.o -opdfcommentextractor

