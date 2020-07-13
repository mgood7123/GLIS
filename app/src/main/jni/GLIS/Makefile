debug_build_dir = debug_BUILD
debug_build_dir_target = $(debug_build_dir)-$(wildcard $(debug_build_dir))
debug_build_dir_present = $(debug_build_dir)-$(debug_build_dir)
debug_build_dir_absent = $(debug_build_dir)-
release_build_dir = release_BUILD
release_build_dir_target = $(release_build_dir)-$(wildcard $(release_build_dir))
release_build_dir_present = $(release_build_dir)-$(release_build_dir)
release_build_dir_absent = $(release_build_dir)-

debug_executable_dir = debug_EXECUTABLE
debug_executable_dir_target = $(debug_executable_dir)-$(wildcard $(debug_executable_dir))
debug_executable_dir_present = $(debug_executable_dir)-$(debug_executable_dir)
debug_executable_dir_absent = $(debug_executable_dir)-
release_executable_dir = release_EXECUTABLE
release_executable_dir_target = $(release_executable_dir)-$(wildcard $(release_executable_dir))
release_executable_dir_present = $(release_executable_dir)-$(release_executable_dir)
release_executable_dir_absent = $(release_executable_dir)-

all: debug

debug: | $(debug_build_dir_target) $(debug_executable_dir_target) build_debug

release: | $(release_build_dir_target) $(release_executable_dir_target) build_release

build_debug:
	cd ${debug_build_dir} ; mkdir EXECUTABLES; cmake -DCMAKE_BUILD_TYPE=Debug .. ; make && (mv -v EXECUTABLES/* ../$(debug_executable_dir) && rmdir EXECUTABLES; true) || (rm -rf EXECUTABLES; false)
build_release:
	cd ${release_build_dir} ; mkdir EXECUTABLES; cmake -DCMAKE_BUILD_TYPE=Release .. ; make && (mv -v EXECUTABLES/* ../$(release_executable_dir) ; rmdir EXECUTABLES; true) || (rm -rf EXECUTABLES; false)

.PHONY: all

clean: clean_debug clean_release

clean_debug:
	rm -rf $(debug_build_dir) $(debug_executable_dir)

clean_release:
	rm -rf $(release_build_dir) $(release_executable_dir)

rebuild: rebuild_debug rebuild_release

rebuild_debug:
	make clean_debug
	make debug

rebuild_release:
	make clean_release
	make release

test:
	$(debug_executable_dir)/test

$(release_executable_dir_present):
$(release_build_dir_present):

$(release_build_dir_absent):
	mkdir $(release_build_dir)
$(release_executable_dir_absent):
	mkdir $(release_executable_dir)

$(debug_executable_dir_present):
$(debug_build_dir_present):

$(debug_build_dir_absent):
	mkdir $(debug_build_dir)
$(debug_executable_dir_absent):
	mkdir $(debug_executable_dir)
