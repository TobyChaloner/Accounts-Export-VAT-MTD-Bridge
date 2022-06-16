#include <gtest/gtest.h>
#include "VatDate.h"
#include <ctime>


TEST(VatDateTest, isInVatPeriod)
{
  VatDate vd;
  vd.setVatYearEndMonth(12); // uninteresting
  vd.setInterestingVatPeriodEnd(2,2022);
  string d("20/02/2022");
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/03/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
  d ="20/01/2022";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/12/2021";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/12/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
}




TEST(VatDateTest, setVatYearEndMonth7)
{
  VatDate vd;
  vd.setVatYearEndMonth(7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(7), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(6), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(5), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(4), 4);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(1), 1);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(12), 1);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(11), 1);
}

TEST(VatDateTest, setVatYearEndMonth8)
{
  VatDate vd;
  vd.setVatYearEndMonth(8);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(8), 8);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(7), 8);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(6), 8);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(5), 5);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(4), 5);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(1), 2);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(12), 2);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(11), 11);
}


TEST(VatDateTest, setVatYearEndMonth9)
{
  VatDate vd;
  vd.setVatYearEndMonth(9);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(8), 9);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(7), 9);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(6), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(5), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(4), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(1), 3);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(12), 12);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(11), 12);
}

TEST(VatDateTest, setVatYearEndMonth1)
{
  VatDate vd;
  vd.setVatYearEndMonth(1);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(8), 10);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(7), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(6), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(5), 7);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(4), 4);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(1), 1);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(12), 1);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(11), 1);
}


TEST(VatDateTest, setVatYearEndMonth12)
{
  VatDate vd;
  vd.setVatYearEndMonth(12);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(8), 9);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(7), 9);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(6), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(5), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(4), 6);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(1), 3);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(12), 12);
  EXPECT_EQ(vd.get_vatPeriodEndForMonth(11), 12);
}



TEST(VatDateTest, setInterestingVatPeriodContaining12)
{
  VatDate vd;
  vd.setVatYearEndMonth(12);
  vd.setInterestingVatPeriodContaining(1,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 3);
  vd.setInterestingVatPeriodContaining(2,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 3);
  vd.setInterestingVatPeriodContaining(3,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 3);
  vd.setInterestingVatPeriodContaining(4,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 6);

  string d("20/04/2022");
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/05/2022";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/06/2022";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/04/2021";
  EXPECT_FALSE(vd.isInVatPeriod(d));
  d ="20/03/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
  d ="20/07/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
}


TEST(VatDateTest, setInterestingVatPeriodContaining1)
{
  VatDate vd;
  vd.setVatYearEndMonth(1);
  vd.setInterestingVatPeriodContaining(1,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 1);
  vd.setInterestingVatPeriodContaining(2,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 4);
  vd.setInterestingVatPeriodContaining(3,2022);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 4);
  vd.setInterestingVatPeriodContaining(12,2021);
  EXPECT_EQ(vd.get_interestingVatPeriodEndMonth(), 1);

  string d("20/02/2022");
  EXPECT_FALSE(vd.isInVatPeriod(d));
  d ="20/03/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
  d ="20/01/2022";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/11/2021";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/12/2021";
  EXPECT_TRUE(vd.isInVatPeriod(d));
  d ="20/12/2022";
  EXPECT_FALSE(vd.isInVatPeriod(d));
}


TEST(VatDateTest, setInterestingVatPeriodAsOneBeforeCurrent)
{
  VatDate vd;
  vd.setVatYearEndMonth(8);
  vd.setInterestingVatPeriodAsOneBeforeCurrent();
  
  std::time_t t = std::time(nullptr);
  std::tm *const timeInfo = std::localtime(&t);
  int current_year = 1900 + timeInfo->tm_year; // year without the century
  int current_month = 1 + timeInfo->tm_mon; // 0..11 corrected to 1..12

  // this month should not be end of the period
  EXPECT_NE(vd.get_interestingVatPeriodEndMonth(), current_month);
  // it could be within (current -1) to (current -1 -2 -3)
  int current_monthBackSix = current_month - 6;
  if (current_monthBackSix < 1)
  {
    current_monthBackSix += 12;
  }
  EXPECT_NE(vd.get_interestingVatPeriodEndMonth(), current_monthBackSix);

}
