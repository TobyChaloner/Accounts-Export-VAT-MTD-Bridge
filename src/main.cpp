//#include "csv.h"
#include <string>
#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <iomanip>
#include <map>
//#include <boost/tokenizer.hpp>





#include "ProcessSolarExport.h"

#include "Mtd100PcVatFreeOutput.h"

void usage();

string inFilename;
string outFilename;
int vatYearEndMonth;
void processArgs(int argc, char *argv[])
{
  for (auto i=0; i<argc; ++i)
  {
    // safe to i++ here
    string arg(argv[i]);
    if (0 == arg.compare("-i")) // is it -i
    {
      ++i;
      if (i>=argc)
      {
	throw runtime_error("Expected input filename after argument -i.  So -i <filename>");
      }
      inFilename = string(argv[i]);
    }
    if (0 == arg.compare("-o")) // is it -i
    {
      ++i;
      if (i>=argc)
      {
	throw runtime_error("Expected output filename after argument -o.  So -o <filename>");
      }
      outFilename = string(argv[i]);
    }
    if (0 == arg.compare("-e")) // is it -e = vat year end month
    { 
      ++i;
      if (i>=argc)
      {
	throw runtime_error("Expected vat year end month after argument -e.  So -e <num> eg -e 8");
      }
      vatYearEndMonth = stoi(argv[i]);
    }
    if ((0 == arg.compare("-h")) || (0 == arg.compare("-?")) || (0 == arg.compare("--help"))) //
    {
      usage();
      exit(0);
    }
  }
}




void usage()
{
  cout << "-e <num: VAT month at end of year (any end vat month) > \n -o <out filename> \n-i <in filename (csv with full Solar export) >" << endl;
}

int main(int argc, char *argv[])
{
  float vatOnSales(0.0);
  float vatReclaimable(0.0);
  float totalSales(0.0);
  float totalPurchases(0.0);

  // lazy defaults
  vatYearEndMonth = 8;
  inFilename = "data/Exported Data - Chaloner Engineering Ltd.csv";
  outFilename = "output/mtd100PcVatFreeInput.csv";

  
  
  Mtd100PcVatFreeOutput outputter;
  try
  {
    processArgs(argc, argv);
    shared_ptr<VatDate> vatDate = std::make_shared<VatDate>();
    vatDate->setVatYearEndMonth(vatYearEndMonth);
    //vatDate->setInterestingVatPeriodEnd(05,2022); // TODO
    vatDate->setInterestingVatPeriodAsOneBeforeCurrent();

    ProcessSolarExport processor(vatDate);
    processor.open(inFilename);
    processor.loadTitles();
    processor.loadAccountsOnly();
    processor.accumulateAllVatInformation();
    processor.showAccumulation();

    
    outputter.writeFile(outFilename,
			vatOnSales,
			0,
			vatReclaimable,
			totalSales,
			totalPurchases,
			0,
			0);
  }
  catch (const std::exception( &e))
  {
    cout << "Caught Exception: " << e.what() << endl;
  }
  
}
