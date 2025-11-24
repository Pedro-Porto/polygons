all:
	cmake -S . -B build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
	cmake --build build -j

run: all
	./build/app

clean:
	rm -rf build