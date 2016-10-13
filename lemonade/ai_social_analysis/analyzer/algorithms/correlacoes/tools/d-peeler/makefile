NAME = d-peeler
CXX = g++ -O3 -flto -Wall -Wextra -Weffc++ -std=c++11 -pedantic -Wno-unused-parameter -Wno-ignored-qualifiers
EXTRA_CXXFLAGS = -lboost_program_options -flto
SRC = src
DEPS = Parameters.h $(SRC)/UsageException.h $(SRC)/DataFormatException.h $(SRC)/NoFileException.h $(SRC)/AbstractHyperBitset.h $(SRC)/Itemset.h $(SRC)/HyperBitset.h $(SRC)/Groups.h $(SRC)/NRectangle.h $(SRC)/Tree.h $(SRC)/TupleFileReader.h $(SRC)/ExAnte.h
OBJ = $(SRC)/UsageException.o $(SRC)/DataFormatException.o $(SRC)/NoFileException.o $(SRC)/AbstractHyperBitset.o $(SRC)/Itemset.o $(SRC)/HyperBitset.o $(SRC)/Groups.o $(SRC)/NRectangle.o $(SRC)/Tree.o $(SRC)/TupleFileReader.o $(SRC)/d-peeler.o $(SRC)/ExAnte.o $(SRC)/TupleFileWriter.o $(SRC)/EmptyItemset.o $(SRC)/SparseItemset.o
DATA = COPYING example INSTALL makefile Parameters.h README

.PHONY: install clean dist-gzip dist-bzip2 dist
.SILENT: $(NAME) install clean dist-gzip dist-bzip2 dist

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $<

$(NAME): $(OBJ)
	$(CXX) -o $@ $^ $(EXTRA_CXXFLAGS)
	echo "$(NAME) built!"

install: $(NAME)
	mv $(NAME) /usr/bin
	echo "$(NAME) installed!"

clean:
	rm -f $(SRC)/*.o *~ $(SRC)/*~

dist-gzip: clean
	tar --format=posix --transform 's,^,$(NAME)/,' -czf $(NAME).tar.gz $(SRC) $(DATA)

dist-bzip2: clean
	tar --format=posix --transform 's,^,$(NAME)/,' -cjf $(NAME).tar.bz2 $(SRC) $(DATA)

dist-xz: clean
	tar --format=posix --transform 's,^,$(NAME)/,' -cJf $(NAME).tar.xz $(SRC) $(DATA)

dist: dist-bzip2
