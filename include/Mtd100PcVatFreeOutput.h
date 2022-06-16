#ifndef Mtd100PcVatFreeOutput_H
#define Mtd100PcVatFreeOutput_H

/*
  class for writing a CSV file as 'MTD 100% Free' expects it
 */
class Mtd100PcVatFreeOutput
{
public:
  void writeFile(string filename, float vatOnSales,
		 float vatDueFromEU, float vatReclaimable,
		 int totalSales, int totalPurchases,
		 int goodsToEu, int goodsFromEu);
};


#endif
