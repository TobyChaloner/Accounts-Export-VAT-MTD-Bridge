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

  // This can be 1 for january or 12 for december
  // required for other calcs
  void setVatYearEndMonth(int month);

  // looks at current date and sets the interesting VAT period end month,year
  void setInterestingVatPeriodAsOneBeforeCurrent();

  // This works out the VAT period that contains this month/year
  // based on the VatYearEndMonth
  void setInterestingVatPeriodContaining(int month, int year);
  bool isInVatPeriod(string& theDate);

  // used for testing

  // year is 2022 or 1978
  // month is 1-12
  // only months within last 3 are then deemed in vat period
  // NB this assumes the date given is know to be the end of the vat period.
  // no check against the yearEndMonth is made.
  // This is used for debugging
  void setInterestingVatPeriodEnd(int month, int year);
  int get_interestingVatPeriodEndMonth() {return interestingVatPeriodEndMonth;};
  int get_vatPeriodEndForMonth(int month) {return monthToVatEndMonth[month];};
};



#endif
