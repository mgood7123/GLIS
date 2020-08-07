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

debug_asan_build_dir = debug_asan_BUILD
debug_asan_build_dir_target = $(debug_asan_build_dir)-$(wildcard $(debug_asan_build_dir))
debug_asan_build_dir_present = $(debug_asan_build_dir)-$(debug_asan_build_dir)
debug_asan_build_dir_absent = $(debug_asan_build_dir)-
debug_asan_executable_dir = debug_asan_EXECUTABLE
debug_asan_executable_dir_target = $(debug_asan_executable_dir)-$(wildcard $(debug_asan_executable_dir))
debug_asan_executable_dir_present = $(debug_asan_executable_dir)-$(debug_asan_executable_dir)
debug_asan_executable_dir_absent = $(debug_asan_executable_dir)-
$(debug_asan_executable_dir_present):
$(debug_asan_build_dir_present):
$(debug_asan_build_dir_absent):
	mkdir $(debug_asan_build_dir)
$(debug_asan_executable_dir_absent):
	mkdir $(debug_asan_executable_dir)

all: debug debug_asan

debug: build_debug

release: build_release

debug_asan: build_debug_asan

debug_directories: | $(debug_build_dir_target) $(debug_executable_dir_target)

release_directories: | $(release_build_dir_target) $(release_executable_dir_target)

debug_asan_directories: | $(debug_asan_build_dir_target) $(debug_asan_executable_dir_target)

build_debug: debug_directories
	cd ${debug_build_dir} ; mkdir EXECUTABLES; cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG="-g3 -O0" .. ; make && if test -e EXECUTABLES ; then cd EXECUTABLES; for file in * ; do mv -v $$file ../../$(debug_executable_dir)/$$FILE ; done ; cd ..; rmdir EXECUTABLES; fi

build_release: release_directories
	cd ${release_build_dir} ; mkdir EXECUTABLES; cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-g0 -O3" .. ; make && if test -e EXECUTABLES ; then cd EXECUTABLES; for file in * ; do mv -v $$file ../../$(release_executable_dir)/$$FILE ; done ; cd ..; rmdir EXECUTABLES; fi

build_debug_asan: debug_asan_directories
	cd ${debug_asan_build_dir} ; mkdir EXECUTABLES; cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG="-g3 -O0 -fno-omit-frame-pointer -fsanitize=address -fsanitize-address-use-after-scope" .. ; make && if test -e EXECUTABLES ; then cd EXECUTABLES; for file in * ; do mv -v $$file ../../$(debug_asan_executable_dir)/$$FILE ; done ; cd ..; rmdir EXECUTABLES; fi

.PHONY: all

clean: clean_debug clean_release clean_debug_asan

clean_debug:
	rm -rf $(debug_build_dir) $(debug_executable_dir)

clean_release:
	rm -rf $(release_build_dir) $(release_executable_dir)

clean_debug_asan:
	rm -rf $(debug_asan_build_dir) $(debug_asan_executable_dir)

rebuild: rebuild_debug rebuild_release rebuild_debug_asan

rebuild_debug:
	make clean_debug
	make debug

rebuild_release:
	make clean_release
	make release

rebuild_debug_asan:
	make clean_debug_asan
	make debug_asan

test: test_debug test_debug_asan test_release

test_debug: debug
	for file in $(debug_executable_dir)/* ; do echo "testing $$file..." ; $$file ; echo "$$file returned with code $$?" ; done

test_release: release
	for file in $(release_executable_dir)/* ; do echo "testing $$file..." ; $$file ; echo "$$file returned with code $$?" ; done

asan_flags = asan_OPTIONS=detect_stack_use_after_return=1

test_debug_asan: debug_asan
	for file in $(debug_asan_executable_dir)/* ; do echo "testing $$file..." ; ${asan_flags} $$file ; echo "$$file returned with code $$?" ; done

valgrind_flags = --leak-check=full --show-leak-kinds=all -s --track-origins=yes

test_valgrind: test_debug_valgrind test_release_valgrind

test_debug_valgrind: debug
	for file in $(debug_executable_dir)/* ; do echo "testing $$file..." ; valgrind ${valgrind_flags} $$file ; echo "$$file returned with code $$?" ; done

test_release_valgrind: release
	for file in $(release_executable_dir)/* ; do echo "testing $$file..." ; valgrind ${valgrind_flags} $$file ; echo "$$file returned with code $$?" ; done

gdb_flags = -ex='set confirm on' -ex=run -ex=quit --quiet

test_gdb: test_debug_gdb test_debug_asan_gdb test_release_gdb

test_debug_gdb: debug
	for file in $(debug_executable_dir)/* ; do echo "testing $$file..." ; gdb ${gdb_flags} $$file ; echo "$$file returned with code $$?" ; done

test_release_gdb: release
	for file in $(release_executable_dir)/* ; do echo "testing $$file..." ; gdb ${gdb_flags} $$file ; echo "$$file returned with code $$?" ; done

test_debug_asan_gdb: debug_asan
	for file in $(debug_asan_executable_dir)/* ; do echo "testing $$file..." ; ${asan_flags} gdb ${gdb_flags} $$file ; echo "$$file returned with code $$?" ; done
