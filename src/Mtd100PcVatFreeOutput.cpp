#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

#include "Mtd100PcVatFreeOutput.h"


// writes format expected by Mtd100PcVatFree
//
// Unix line endings seem to be ok
void Mtd100PcVatFreeOutput::writeFile(string filename, float vatOnSales,
		 float vatDueFromEU, float vatReclaimable,
		 int totalSales, int totalPurchases,
		 int goodsToEu, int goodsFromEu)
{
  ofstream ostr;
  ostr.open(filename);
  if (!ostr.is_open())
  {
    string msg = "Failed to open output file " + filename + " for writing"; 
    throw runtime_error(msg);
  }
  ostr << fixed << std::setprecision(2);
  ostr << "Vat due on Sales," << vatOnSales  << endl;
  ostr << "Vat due from EU,"  << vatDueFromEU << endl;
  ostr << "VAT Reclaimable,"  << vatReclaimable << endl;
  //
  // Integers
  //
  ostr << "Total Sales,"     << totalSales << endl;
  ostr << "Total Purchases," << totalPurchases << endl;
  ostr << "Goods to EU,"     << goodsToEu << endl;
  ostr << "Goods from EU,"   << goodsFromEu << endl;
  ostr.close();
}
