// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef DATAFORMATEXCEPTION_H_
#define DATAFORMATEXCEPTION_H_

#include <exception>
#include <string>
#include <boost/lexical_cast.hpp>

using namespace std;

class DataFormatException : public std::exception
{
protected:
  string completeMessage;

public:
  DataFormatException();
  DataFormatException(const char* message);
  DataFormatException(const char* fileName, const unsigned int lineNb, const char* message);
  ~DataFormatException() throw();
  const char* what() const throw();
};

#endif /*DATAFORMATEXCEPTION_H_*/
