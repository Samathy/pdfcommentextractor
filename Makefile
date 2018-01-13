poppler: | lib/poppler/build
	cd lib/poppler/build/ && cmake .. -DBUILD_GTK_TESTS=OFF -DBUILD_QT5_TESTS=OFF -DBUILD_CPP_TESTS=ON -DENABLE_SPLASH=OFF -DENABLE_UTILS=OFF -DENABLE_CPP=ON -DENABLE_GLIB=OFF -DENABLE_GTK_DOC=OFF -DENABLE_QT5=OFF -DENABLE_LIBCURL=OFF -DENABLE_CMS=none -DENABLE_LIBOPENJPEG=none -DCMAKE_BUILD_TYPE=release
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
	g++ -c -I/usr/include/poppler/cpp src/pdfcommentextractor.cpp -o build/pdfcommentextractor.o

all: |./build pdfcommentextractor.cpp
	g++ -lpoppler -lpoppler-cpp build/pdfcommentextractor.o

