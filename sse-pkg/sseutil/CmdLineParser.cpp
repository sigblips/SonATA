/*******************************************************************************

 File:    CmdLineParser.cpp
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



#include "CmdLineParser.h" 
#include "Assert.h"
#include "SseUtil.h"
#include <sstream>
#include <iostream>

using namespace std;

// -----
// helper classes 

// Base for different option types
class CmdLineOption
{
 public:
   CmdLineOption(const string &name);
   virtual ~CmdLineOption();

   virtual void setTakesArg(bool takesArg);
   
   virtual string getName();
   virtual bool takesArg();
   virtual bool wasRequested();
   virtual void setWasRequested();

   // subclasses should override these

   // This could throw an SseException on failure
   virtual void setValue(const string &value);

   virtual string getValue();

 private:
   string name_;
   bool takesArg_;
   bool wasRequested_;
};

class CmdLineStringOption : public CmdLineOption
{
 public:
    CmdLineStringOption(const string &name, const string &defaultValue);
    virtual ~CmdLineStringOption();

    virtual void setValue(const string &value);
    virtual string getValue();

 private:

    string defaultValue_;
    string value_;

    // Disable copy construction & assignment.
    // Don't define these.
    //CmdLineStringOption(const CmdLineStringOption& rhs);
    //CmdLineStringOption& operator=(const CmdLineStringOption& rhs);

};


class CmdLineDoubleOption : public CmdLineOption
{
 public:
    CmdLineDoubleOption(const string &name, double defaultValue);
    virtual ~CmdLineDoubleOption();

    virtual string getValue();
    virtual void setValue(const string &value);

    double getDoubleValue();

 private:

    double defaultValue_;
    double value_;

    // Disable copy construction & assignment.
    // Don't define these.
    //CmdLineDoubleOption(const CmdLineDoubleOption& rhs);
    //CmdLineDoubleOption& operator=(const CmdLineDoubleOption& rhs);

};

class CmdLineIntOption : public CmdLineOption
{
 public:
    CmdLineIntOption(const string &name, int defaultValue);
    virtual ~CmdLineIntOption();

    virtual string getValue();
    virtual void setValue(const string &value);

    int getIntValue();

 private:

    int defaultValue_;
    int value_;

    // Disable copy construction & assignment.
    // Don't define these.
    //CmdLineIntOption(const CmdLineIntOption& rhs);
    //CmdLineIntOption& operator=(const CmdLineIntOption& rhs);

};


// -----

CmdLineParser::CmdLineParser()
{
   usageHelpStrm_.precision(6);           // show N places after the decimal
   usageHelpStrm_.setf(std::ios::fixed);  // show all decimal places up to precision
}

CmdLineParser::~CmdLineParser()
{
   OptionList::iterator it;
   for (it = options_.begin(); it != options_.end();
        ++it)
   {
      CmdLineOption *opt(*it);
      delete(opt);
   }
}

string CmdLineParser::indent()
{
   return "  ";
}

bool CmdLineParser::parse(int argc, char *argv[])
{
   Assert(argc >= 1);

   progName_ = argv[0];

   try {

      int index = 1;
      while (index < argc)
      {
         string requestedOpt(argv[index++]);
         bool validArgName(false);

         // Look for the option by name
         OptionList::iterator it;
         for (it = options_.begin(); it != options_.end();
              ++it)
         {
            CmdLineOption * opt(*it);
            if (opt->getName() == requestedOpt)
            {
               validArgName = true;
               opt->setWasRequested();

               // does option take an argument?
               if (opt->takesArg())
               {
                  // get the next command line arg
                  if (index < argc)
                  {
                     const string &value(argv[index++]);
                     opt->setValue(value);
                  }
                  else {
                     errorStrm_ << "Error: no value supplied for " 
                                << opt->getName() << " option " << endl;
                     return false;
                  }
               }
            }
         }
         if (! validArgName) 
         {
            errorStrm_ << "Error: invalid option: " 
                       << requestedOpt;
            return false;
         }
      }
   }
   catch (SseException &except)
   {
      errorStrm_ << except.descrip();
      return false;
   }      
   return true;
}

void CmdLineParser::addFlagOption(const string &name,
                                  const string &helpText)
{
   string value("");
   CmdLineOption *flag = new CmdLineStringOption(name, value);
   flag->setTakesArg(false);
   options_.push_back(flag);

   commandSummaryStrm_ << name << " ";

   usageHelpStrm_ << indent() << name << ": "
                  << helpText << endl;
}

bool CmdLineParser::getFlagOption(const string &flagName)
{ 
   OptionList::iterator it;
   for (it = options_.begin(); it != options_.end();
        ++it)
   {
      CmdLineOption * opt(*it);
      if (opt->getName() == flagName)
      {
         return opt->wasRequested();
      }
   }

   AssertMsg(0, "requested flag option '" + flagName 
             + "' was not found");
}

void CmdLineParser::addStringOption(
   const string &name, const string &defaultValue,
   const string &helpText)
{
   options_.push_back(new CmdLineStringOption(
      name, defaultValue));

   commandSummaryStrm_ << name << " <value> ";

   usageHelpStrm_ << indent() << name << ": "
                  << helpText << " (default: " 
                  << defaultValue << ")" << endl;

}

string CmdLineParser::getStringOption(
   const string &name)
{
   OptionList::iterator it;
   for (it = options_.begin(); it != options_.end();
        ++it)
   {
      CmdLineOption * opt(*it);
      if (opt->getName() == name)
      {
         return opt->getValue();
      }
   }

   AssertMsg(0, "requested string option '" + name
             + "' was not found");
}


void CmdLineParser::addDoubleOption(
   const string &name, double defaultValue, const string &helpText)
{
   options_.push_back(new CmdLineDoubleOption(
      name, defaultValue));

   commandSummaryStrm_ << name << " <value> ";

   usageHelpStrm_ << indent() << name << ": "
                  << helpText << " (default: " 
                  << defaultValue << ")" << endl;

}

double CmdLineParser::getDoubleOption(const string &name)
{
   OptionList::iterator it;
   for (it = options_.begin(); it != options_.end();
        ++it)
   {
      CmdLineOption * opt(*it);
      if (opt->getName() == name)
      {
         CmdLineDoubleOption *doubleOpt = 
            dynamic_cast<CmdLineDoubleOption *>(opt);
         if (doubleOpt)
         {
            return doubleOpt->getDoubleValue();
         }
         else
         {
            AssertMsg(0, "requested option '" + name
                      + "' as a double, but it's the wrong type");
         }
      }
   }

   AssertMsg(0, "requested double option '" + name
             + "' was not found");

}

void CmdLineParser::addIntOption(
   const string &name, int defaultValue, const string &helpText)
{
   options_.push_back(new CmdLineIntOption(
      name, defaultValue));

   commandSummaryStrm_ << name << " <value> ";

   usageHelpStrm_ << indent() << name << ": " 
                  << helpText << " (default: " 
                  << defaultValue << ")" << endl;

}

int CmdLineParser::getIntOption(const string &name)
{
   OptionList::iterator it;
   for (it = options_.begin(); it != options_.end();
        ++it)
   {
      CmdLineOption * opt(*it);
      if (opt->getName() == name)
      {
         CmdLineIntOption *intOpt = 
            dynamic_cast<CmdLineIntOption *>(opt);
         if (intOpt)
         {
            return intOpt->getIntValue();
         }
         else
         {
            AssertMsg(0, "requested option '" + name
                      + "' as a int, but it's the wrong type");
         }
      }
   }

   AssertMsg(0, "requested int option '" + name
             + "' was not found");

}

string CmdLineParser::getErrorText()
{
   return errorStrm_.str();
}

string CmdLineParser::getUsage()
{
   stringstream strm;
    
   strm << "Usage: " << progName_ << " " 
        << commandSummaryStrm_.str() << endl
        << usageHelpStrm_.str();
   
   return strm.str();

}

// -----------------------------------------------
// helper classes

CmdLineOption::CmdLineOption(const string &name)
   :
   name_(name),
   takesArg_(true),
   wasRequested_(false)
{
}

CmdLineOption::~CmdLineOption()
{
}

void CmdLineOption::setTakesArg(bool takesArg)
{
   takesArg_ = takesArg;
}

string CmdLineOption::getName()
{
   return name_;
}

bool CmdLineOption::takesArg()
{
   return takesArg_;
}

bool CmdLineOption::wasRequested()
{
   return wasRequested_;
}

void CmdLineOption::setWasRequested()
{
   wasRequested_ = true;
}

void CmdLineOption::setValue(const string &value)
{
   // do nothing
}

string CmdLineOption::getValue()
{
   return "";
}

//--------------------------
CmdLineStringOption::CmdLineStringOption(const string &name,
                                         const string &defaultValue)
   : CmdLineOption(name),
     defaultValue_(defaultValue),
     value_(defaultValue)
{
}

CmdLineStringOption::~CmdLineStringOption()
{
}

void CmdLineStringOption::setValue(const string &value)
{
   value_ = value;
}

string CmdLineStringOption::getValue() 
{
   return value_;
}

// ------------

CmdLineDoubleOption::CmdLineDoubleOption(const string &name,
                                         double defaultValue)
   : CmdLineOption(name), 
     defaultValue_(defaultValue),
     value_(defaultValue)
{
}

CmdLineDoubleOption::~CmdLineDoubleOption()
{
}

double CmdLineDoubleOption::getDoubleValue()
{
   return value_;
}

string CmdLineDoubleOption::getValue() 
{
   // double to string
   stringstream strm;

   strm << value_;

   return strm.str(); 
}

void CmdLineDoubleOption::setValue(const string &value)
{
   try {
      value_ = SseUtil::strToDouble(value);
   }
   catch (SseException & except)
   {
      stringstream strm;
      strm << "Error: " << getName() << " option value '" << value 
           << "' is not a valid number.";

      throw SseException(strm.str());
   }

}


// ------------

CmdLineIntOption::CmdLineIntOption(const string &name,
                                   int defaultValue)
   : CmdLineOption(name), 
     defaultValue_(defaultValue),
     value_(defaultValue)
{
}

CmdLineIntOption::~CmdLineIntOption()
{
}

int CmdLineIntOption::getIntValue()
{
   return value_;
}

string CmdLineIntOption::getValue() 
{
   // int to string
   stringstream strm;

   strm << value_;

   return strm.str(); 
}

void CmdLineIntOption::setValue(const string &value)
{
   try {
      value_ = SseUtil::strToInt(value);
   }
   catch (SseException & except)
   {
      stringstream strm;
      strm << "Error: " << getName() << " option value '" << value 
           << "' is not a valid number.";

      throw SseException(strm.str());
   }

}