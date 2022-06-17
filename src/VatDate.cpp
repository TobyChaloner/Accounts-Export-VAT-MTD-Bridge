#include "VatDate.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

void VatDate::setVatYearEndMonth(int endMonth)
{
  vatYearPeriodEnd = endMonth;
  
  monthToVatEndMonth.reserve(12+1); // need 12 elements to populate, but indexing 1..12 (not 0..11)
  int periodEnd=vatYearPeriodEnd;
  int count=0;
  int mth = vatYearPeriodEnd; // start at period end, not 0 or 12
  int relativeMthToPeriodEnd = mth; // moves with periodEnd
  while (count++ < 12)
  {
    if (relativeMthToPeriodEnd < periodEnd - 2) // 3 mths in period, so outof range
    {
      periodEnd -= 3; // drop to next period
      if (periodEnd < 1)
      {
	periodEnd += 12; // move to high end of 1..12 range
	relativeMthToPeriodEnd += 12;
      }
    }
    monthToVatEndMonth[mth]=periodEnd;
#if 0
    cout << "monthToVatEndMonth["<<mth<<"]=periodEnd:"<<periodEnd<<endl;
#endif
    mth--;
    relativeMthToPeriodEnd--;
    if (mth < 1)
    {
      mth += 12;
    }
  }
}



  // year is 2022 or 1978
  // month is 1-12
  // only months within last 3 are then deemed in vat period
void VatDate::setInterestingVatPeriodEnd(int month, int year)
{
  if (vatYearPeriodEnd < 0) { throw runtime_error("VatDate::setInterestingVatPeriodAsLast, vatYearPeriodEnd never set - code error");}

  interestingVatPeriodEndMonth = month;
  interestingVatPeriodEndYear = year;

  if (month <1 || month > 12)
  {
    stringstream msg;
    msg << "Invalid month not 1-12:"<<month;
    throw runtime_error(msg.str());
  }
  if (year <1970 || year > 9999)
  {
    stringstream msg;
    msg << "Invalid year not 1970-9999:"<<year;
    throw runtime_error(msg.str());
  }
  
  validMonths.clear(); // during testing, this gets multi run
  stringstream adate;
  adate << std::setw(2) << std::setfill('0')
	<< month << "/" << year;
  validMonths[adate.str()] = 1;
#if 0
  cout << adate.str()<<endl;
#endif

  month--;
  if (month < 1)
  {
    year--;
    month=12;
  }
  adate.str("");
  adate << std::setw(2) << std::setfill('0')
	<< month << "/" << year;
  validMonths[adate.str()] = 1;
#if 0
  cout << adate.str()<<endl;
#endif
  
  month--;
  if (month < 1)
  {
    year--;
    month=12;
  }
  adate.str("");
  adate << std::setw(2) << std::setfill('0')
	<< month << "/" << year;
  validMonths[adate.str()] = 1;
#if 0
  cout << adate.str()<<endl;
#endif
  
}



// looks at current date and sets the interesting VAT period end month,year
// sets to one before current
void VatDate::setInterestingVatPeriodAsOneBeforeCurrent()
{
  if (vatYearPeriodEnd < 0) { throw runtime_error("VatDate::setInterestingVatPeriodAsLast, vatYearPeriodEnd never set - code error");}
  // using current date time, get m,yy and pass to setInterestingVatPeriodContaining
  std::time_t t = std::time(nullptr);
  std::tm *const timeInfo = std::localtime(&t);
  int year = 1900 + timeInfo->tm_year; // year without the century
  int month = 1 + timeInfo->tm_mon; // 0..11 corrected to 1..12
  // by moving month back 3, this will be in previous period
  month -= 3;
  if (month < 1)
  {
    --year;
    month += 12;
  }
  setInterestingVatPeriodContaining(month, year);
}


// This works out the VAT period that contains this month/year
void VatDate::setInterestingVatPeriodContaining(int month, int year)
{
  if (vatYearPeriodEnd < 0) { throw runtime_error("VatDate::setInterestingVatPeriodAsLast, vatYearPeriodEnd never set - code error");}

  int endMonth = get_vatPeriodEndForMonth(month);
  if (endMonth < month) // so moving from 12/2021 to 1/2022
  {
    // year roll
    ++year;
  }
  setInterestingVatPeriodEnd(endMonth, year);
}



/* Date example
17/09/2021
 */
bool VatDate::isInVatPeriod(string& theDate)
{
  int mthPos = theDate.find("/") + 1;
  int yrPos = theDate.rfind("/") + 1;
  string yr = theDate.substr(yrPos);
  string mth = theDate.substr(mthPos, yrPos-mthPos-1);
#if 0
  cout << "mth:"<<mth<<",yr:"<<yr<<endl;
#endif
  string monthDate = mth+"/"+yr;

  // going to have three strings.  They match or they dont.
  bool valid = validMonths.find(monthDate) != validMonths.end();
    
    
  // 1,2,3  interestingVatPeriodEndMonth
  // this or last    interestingVatPeriodEndYear
  return valid;
}
