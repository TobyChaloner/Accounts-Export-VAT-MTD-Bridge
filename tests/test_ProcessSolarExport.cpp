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

#include <iostream>
using std::cout;


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



TEST(ProcessSolarExportTest, accumulateAllVatInformation1i2due1none)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(2,2010);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 210.0);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 1250.0);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 0.0);
}



TEST(ProcessSolarExportTest, accumulateAllVatInformation1e2due1none)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(6,2010);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 0.0);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 100.0);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 510.0);
}



// 1 invoice 6 lines with vat, 6 non vat lines
TEST(ProcessSolarExportTest, accumulateAllVatInformation1iOver10lines6v6n)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(9,2010);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 300.0);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 3450.0);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 0.0);
}


// 1 expense 6 lines with vat, 6 non vat lines
TEST(ProcessSolarExportTest, accumulateAllVatInformation1eOver10lines6v6n)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(12,2010);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 0.0);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 374.84);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 1961.16);
}






// ================== FRV ===================

TEST(ProcessSolarExportTest, accumulateAllVatInformation_FRV_1i1e)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(3,2020);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 189.80);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 1460.00);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 0.0);
}



TEST(ProcessSolarExportTest, accumulateAllVatInformation_FRV_2i1e)
{
  shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();

  vatDate->setVatYearEndMonth(3); // year end month (Jan:1, Dec:12)
  vatDate->setInterestingVatPeriodEnd(6,2020);
  
  ProcessSolarExport pse(vatDate);
  pse.open("../src/data/Exported Data - MTD Test Set 1.csv");

  pse.loadTitles();
  pse.loadAccountsOnly();
  pse.accumulateAllVatInformation();

  EXPECT_FLOAT_EQ(pse.getVatDueOnSales(), 194.78);
  EXPECT_FLOAT_EQ(pse.getVatReclaimable(), 0.0);
  EXPECT_FLOAT_EQ(pse.getTotalSalesExVat(), 1498.27);
  EXPECT_FLOAT_EQ(pse.getTotalPurchasesExVat(), 0.0);
}

