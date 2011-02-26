/*******************************************************************************

 File:    IdNumberFactory.cpp
 Project: OpenSonATA
 Authors: The OpenSonATA code is the result of many programmers
          over many years

 Copyright 2011 The SETI Institute

 OpenSonATA is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 OpenSonATA is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with OpenSonATA.  If not, see<http://www.gnu.org/licenses/>.
 
 Implementers of this code are requested to include the caption
 "Licensed through SETI" with a link to setiQuest.org.
 
 For alternate licensing arrangements, please contact
 The SETI Institute at www.seti.org or setiquest.org. 

*******************************************************************************/

/*
 * PURPOSE:  
 --*/

#include "IdNumberFactory.h"
#include "IdDb.h"

using namespace std;

// make the first activity number consistent with using the
// database
const int FirstActNumber = 1;
static ActivityId_t id = FirstActNumber;

auto_ptr<IdNumber> IdNumberFactory(const char *filename)
{
  auto_ptr<IdNumber> idNumber;

  try {
    idNumber.reset(new IdNumberAsciiFile(filename));
  }
  catch (IdCreationFailed & except){
    idNumber.reset(new IdNumberCount(id));
  }
  return idNumber;
}

auto_ptr<IdNumber> IdNumberFactory(const DbParameters& dbparam)
{
  auto_ptr<IdNumber> idNumber;

  try {
    if (! dbparam.useDb()){
      throw IdCreationFailed("DB turned off");
    }
    idNumber.reset(new IdNumberDb(dbparam));
  }
  catch (IdCreationFailed & except){
    idNumber.reset(new IdNumberCount(id));
  }
  return idNumber;
}

auto_ptr<IdNumber> IdNumberFactory(const DbParameters& dbparam,
				   const char *filename)
{
  auto_ptr<IdNumber> idNumber;

  if (dbparam.useDb()){
    try {
      idNumber.reset(new IdNumberDb(dbparam));
      return idNumber;
    }
    catch (IdCreationFailed & except){
      // just continue on
    }
  }
  
  try {
    idNumber.reset(new IdNumberAsciiFile(filename));
  }
  catch (IdCreationFailed & except){
    idNumber.reset(new IdNumberCount(id));
  }
  return idNumber;
}