all:
	cmake -S . -B build
	cmake --build build -j

run: all
	./build/app

clean:
	rm -rf build