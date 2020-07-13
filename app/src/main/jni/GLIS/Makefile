debug_dir = debug
debug_dir_target = $(debug_dir)-$(wildcard $(debug_dir))
debug_dir_present = $(debug_dir)-$(debug_dir)
debug_dir_absent = $(debug_dir)-
release_dir = release
release_dir_target = $(release_dir)-$(wildcard $(release_dir))
release_dir_present = $(release_dir)-$(release_dir)
release_dir_absent = $(release_dir)-

all: debug release

debug: | $(debug_dir_target) build_debug

release: | $(release_dir_target) build_release

$(release_dir_present):

$(release_dir_absent):
	mkdir $(release_dir)

$(debug_dir_present):

$(debug_dir_absent):
	mkdir $(debug_dir)

build_debug: debug
	cd ${debug_dir} ; cmake -DCMAKE_BUILD_TYPE=Debug .. ; make || exit
build_release:
	cd ${release_dir} ; cmake -DCMAKE_BUILD_TYPE=Release .. ; make || exit
testDebug: build_debug
	cd ${debug_dir}/
testDebugGDB: build_debug
	cd ${debug_dir}/

.PHONY: all

clean:
	rm -rf $(debug_dir)
	rm -rf $(release_dir)
