// Copyright 2007-2013 Loïc Cerf (magicbanana@gmail.com)

// This file is part of d-peeler.

// d-peeler is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License version 3 as published by the Free Software Foundation

// d-peeler is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with d-peeler; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

#include "DataFormatException.h"

DataFormatException::DataFormatException(): completeMessage("")
{
}

DataFormatException::DataFormatException(const char* message): completeMessage(string(message))
{
}

DataFormatException::DataFormatException(const char* fileName, const unsigned int lineNb, const char* message): completeMessage("")
{
  completeMessage = (string(fileName) + ':' + boost::lexical_cast<string>(lineNb) + ": " + string(message)).c_str();
}

DataFormatException::~DataFormatException() throw()
{
}

const char* DataFormatException::what() const throw()
{
  return completeMessage.c_str();
}
