NAME = multidupehack
CXX = g++ -Ofast -flto -DBOOST_MATH_OVERFLOW_ERROR_POLICY=ignore_error -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers
# CXX = g++ -g -DBOOST_MATH_OVERFLOW_ERROR_POLICY=ignore_error -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers
# CXX = clang++ -O3 -flto -Wall -Weffc++ -std=c++11 -pedantic # the produced binary is about 18% slower than with g++
EXTRA_CXXFLAGS = -lboost_program_options
SRC = src/utilities src/measures src/core
DEPS = $(wildcard $(patsubst %,%/*.h,$(SRC))) Parameters.h makefile
CODE = $(wildcard $(patsubst %,%/*.cpp,$(SRC)))
OBJ = $(patsubst %.cpp,%.o,$(CODE))
ALL = $(DEPS) $(CODE) COPYING example INSTALL README

.PHONY: install clean dist-gzip dist-bzip2 dist-xz dist
.SILENT: $(NAME) install clean dist-gzip dist-bzip2 dist-xz dist

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $<

$(NAME): $(OBJ)
	$(CXX) -o $@ $^ $(EXTRA_CXXFLAGS)
	echo "$(NAME) built!"

install: $(NAME)
	mv $(NAME) /usr/bin
	echo "$(NAME) installed!"

clean:
	rm -f $(patsubst %,%/*.o,$(SRC)) $(patsubst %,%/*~,$(SRC)) *~

dist-gzip:
	tar --format=posix --transform 's,^,$(NAME)/,' -czf $(NAME).tar.gz $(ALL)

dist-bzip2:
	tar --format=posix --transform 's,^,$(NAME)/,' -cjf $(NAME).tar.bz2 $(ALL)

dist-xz:
	tar --format=posix --transform 's,^,$(NAME)/,' -cJf $(NAME).tar.xz $(ALL)

dist: dist-xz
