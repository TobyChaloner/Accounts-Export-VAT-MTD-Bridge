/*

needs to be run in build directory 
i.e.
proj
  src/tests/test_ProcessSolarExport.cpp
  build --- in here

 */

#include <gtest/gtest.h>
//#include <ctime>


#include "ProcessSolarExport.h"


TEST(ProcessSolarExportTest, open)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();
  {
    ProcessSolarExport pse(vatDate);
    pse.open("../src/data/Exported Data - MTD Test Set 1.csv");
    // should not throw
  }
  {
    EXPECT_THROW(
		{
		  ProcessSolarExport pse(vatDate);
		  
		  pse.open("../src/data/FILE_NO_EXIST.csv");
		},
		std::runtime_error);
    
    // should not throw
  }
}




TEST(ProcessSolarExportTest, loadTitles)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  string title = "Date";
  EXPECT_TRUE(pse.isTitleKnown(title));
  title = "Line 12 VAT Rate";
  EXPECT_TRUE(pse.isTitleKnown(title));
}

TEST(ProcessSolarExportTest, loadAccountsOnly)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();
  vatDate->setVatYearEndMonth(8); // year end month to sept == 8
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  string title = "Money To Be Banked";
  EXPECT_TRUE(pse.isAccountKnown(title));
  title = "Money To Be Banked";
  EXPECT_EQ(pse.getAccountTopNode(title), string("Assets"));
}



TEST(ProcessSolarExportTest, accumulateAllVatInformation)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();
  vatDate->setVatYearEndMonth(8); // year end month to sept == 8
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();
}

//  EXPECT_EQ(vd.get_vatPeriodEndForMonth(8), 8);
//  EXPECT_FALSE(vd.isInVatPeriod(d));
