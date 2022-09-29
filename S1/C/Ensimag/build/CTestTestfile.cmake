# CMake generated Testfile for 
# Source directory: /home/c/choulesl/S1/C/ensimag-rappeldec
# Build directory: /home/c/choulesl/S1/C/ensimag-rappeldec/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[ListeChainee]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/listechainee")
set_tests_properties([=[ListeChainee]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;57;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[valgrindListeChainee]=] "valgrind" "/home/c/choulesl/S1/C/ensimag-rappeldec/build/listechainee")
set_tests_properties([=[valgrindListeChainee]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;58;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[GC]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/ensitestgc" "--all")
set_tests_properties([=[GC]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;60;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[ValgrindGC]=] "valgrind" "/home/c/choulesl/S1/C/ensimag-rappeldec/build/ensitestgc" "--all")
set_tests_properties([=[ValgrindGC]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;61;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[Binaire]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/../tests/binairestests.rb")
set_tests_properties([=[Binaire]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;63;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[Hello]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/../tests/hellotests.rb")
set_tests_properties([=[Hello]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;65;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[Flottants]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/../tests/flottantstests.rb")
set_tests_properties([=[Flottants]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;67;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[TriComplexe]=] "/home/c/choulesl/S1/C/ensimag-rappeldec/build/fqsort")
set_tests_properties([=[TriComplexe]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;69;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
add_test([=[valgrindTriComplexe]=] "valgrind" "/home/c/choulesl/S1/C/ensimag-rappeldec/build/fqsort")
set_tests_properties([=[valgrindTriComplexe]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;70;add_test;/home/c/choulesl/S1/C/ensimag-rappeldec/CMakeLists.txt;0;")
