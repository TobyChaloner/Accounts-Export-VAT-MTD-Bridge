#ifndef ProcessSolarExport_H
#define ProcessSolarExport_H

#include <map>
#include <vector>
#include <memory>
#include <fstream>

#include "VatDate.h"

using namespace std;

enum class PostAction { not_finished, finish_reprocess_record, finish_done_record};
/*
  Class to interpret Solar Accounts export and to 
  recreate the VAT figures for the interesting VAT period


  I want to identify account names that are INPUT and OUTPUT
  These are the only ones that contain VAT.

  INPUT and output are hierachically organised (top of tree is Income or Expenses, but once this has identified top of tree, 
  the account is in one or the other
*/
class ProcessSolarExport
{
  float incomeAmount;
  float incomeVatAmount;
  float purchasesAmount;
  float reclaimableAmount;
  std::shared_ptr<VatDate> vatDate;

  map<string,string> hierachy; // ends in one of two nodes - input or expense.  Top node is == self
  ifstream fstr;
  vector<string> titles; // titles of CSV columns
  map<string,int> titleOffset; // titles of CSV columns -> index

  int offsetOfTitleType;
  int offsetOfTitleName;
  int offsetOfTitleGroupUnder;

public:
  ProcessSolarExport(shared_ptr<VatDate> _vatDate): incomeAmount(0.0), incomeVatAmount(0.0),
						    purchasesAmount(0.0),reclaimableAmount(0.0),
						    vatDate(_vatDate),
						    offsetOfTitleType(-1),
						    offsetOfTitleName(-1),
						    offsetOfTitleGroupUnder(-1)
  {
  };
  ~ProcessSolarExport();


  void open(const string &filename); // throws on issue
  void loadTitles();
  void loadAccountsOnly();
  void accumulateAllVatInformation(); // assumes titles and accounts loaded.  accumulates vat
  void showAccumulation();

  float getVatDueOnSales() {return incomeVatAmount;};
  float getVatReclaimable() {return reclaimableAmount;};
  float getTotalSalesExVat() {return incomeAmount;};
  float getTotalPurchasesExVat() {return purchasesAmount;};
  
private:
  string findTopNode(const string &node);
  PostAction loadAccountsLine(vector<string> &vec);
  void loadFromRecords(PostAction (ProcessSolarExport::*fn)(vector<string> &vec));
  PostAction loadTitle(vector<string> &vec);
  PostAction accumulateAllVatInformationLine(vector<string> &vec);
  PostAction processInvoiceVat(vector<string> &vec);
  PostAction processPurchaseVat(vector<string> &vec);
  // get column index/offset containing this title
  int getTitleOffset(const string title);
  float getAmount(vector<string> &vec, const string& prefix, int lineNo, const string& postfix);

public:
  // for testing only

  // has a column with this title been read from the file?
  bool isTitleKnown(const string &title);

  // is this account known
  bool isAccountKnown(const string &accountName);
  // what it the top account name (eg expenses) for this account
  string getAccountTopNode(const string &childAccountName);
};


#endif
