#ifndef VatDate_H
#define  VatDate_H

#include<string>
#include<map>
#include<vector>

using std::string;
using std::map;
using std::vector;


/*
  Class to workout the VAT period of interest.
  Also identifies if a date is of interest (in one of the three months)
 */
class VatDate
{
  int vatYearPeriodEnd;
  int monthsInVatPeriod;
  int interestingVatPeriodEndMonth;
  int interestingVatPeriodEndYear;
  map<string,int> validMonths;
  vector<int> monthToVatEndMonth;
public:
  VatDate():  vatYearPeriodEnd(-1), monthsInVatPeriod(3), interestingVatPeriodEndMonth(-1), interestingVatPeriodEndYear(0)  {};

  /*
    Some companies have a VAT year.
    One of the VAT quarter ends is often considered the VAT year end.
    Identifying the month (1..12) will set the other VAT quarter ends.
    This is a prerequisite of other functions
  */
  void setVatYearEndMonth(int month);

  /*
    Called to set the interesting VAT period to before the current live one.
    This makes sense as the VAT return is not done until the period has finished and
    generally is done within the following period (should be).
  */
  void setInterestingVatPeriodAsOneBeforeCurrent();

  /*
    For historic and rectification and testing reasons, the 
    months in the VAT period need to be set from a known month (not necessarily the last in the period)
    
    This assumes that the End month (for one VAT period, not necessarily this) has been entered.

    This works out the VAT period that contains this month/year
  */
  void setInterestingVatPeriodContaining(int month, int year);

  /*   
       The question 'is this date of interest to the calculation' needs to be answered.
       This allows the object to be asked that question.

       Date example
       17/09/2021
  */
  bool isInVatPeriod(string& theDate);



  
  // used for testing


  /*
    The question 'is this date of interest to the calculation' needs to be answered.
    This allows the object to be able to answer that question.
    By knowing the last month in the VAT period, this can record the preceeding two months as well.
    So later on a different call to the object, the question can be answered.
  */
  // NOTE: This is used publically for debugging
  void setInterestingVatPeriodEnd(int month, int year);
  // testing only
  int get_interestingVatPeriodEndMonth() {return interestingVatPeriodEndMonth;};
  // testing only
  int get_vatPeriodEndForMonth(int month) {return monthToVatEndMonth[month];};
};



#endif
