// Copyright 2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA


#include "TupleFileWriter.h"

using namespace std;

TupleFileWriter::TupleFileWriter(string fileName,
                                 const vector< map< const unsigned int, const string > >& ids2Labels,
                                 string dimensionSeparator)
    : file_(fileName),
      ids2Labels_(ids2Labels),
      dimensionSeparator_(dimensionSeparator)
{
}

void TupleFileWriter::write(const vector< unsigned int >& tuple)
{
    for (unsigned i = 0; i < tuple.size(); i++) {
        if (i)
            file_ << dimensionSeparator_;

        file_ << ids2Labels_[i].find(tuple[i])->second;
    }

    file_ << '\n';
}

void TupleFileWriter::write(const vector< vector< unsigned int > >& tuples)
{
    for (const auto &tuple : tuples)
        write(tuple);
}

void TupleFileWriter::close()
{
    file_.close();
}
