// Copyright 2014 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef TUPLE_FILE_WRITER_H_
#define TUPLE_FILE_WRITER_H_

#include <string>
#include <vector>
#include <fstream>
#include <map>

class TupleFileWriter {
    public:
        TupleFileWriter(std::string fileName,
                        const std::vector<std::map<const unsigned int, const std::string>> &ids2Labels,
                        std::string dimensionSeparator);

        void write(const std::vector<unsigned int> &tuple);
        void write(const std::vector<std::vector<unsigned int>> &tuples);

        void close();

    private:
        std::ofstream file_;
        const std::vector<std::map<const unsigned int, const std::string>> &ids2Labels_;
        std::string dimensionSeparator_;
};

#endif
