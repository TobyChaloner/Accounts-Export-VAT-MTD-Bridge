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

  Does normal UK mainland VAT and FRV
  Limitation. Does not do EU imports and exports
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

  // cached lookups
  int offsetOfTitleType;
  int offsetOfTitleName;
  int offsetOfTitleGroupUnder;
  int offsetOfLines;
  int offsetOfFRV;
  int offsetOfDate;

public:
  ProcessSolarExport(shared_ptr<VatDate> _vatDate): incomeAmount(0.0), incomeVatAmount(0.0),
						    purchasesAmount(0.0),reclaimableAmount(0.0),
						    vatDate(_vatDate),
						    offsetOfTitleType(-1),
						    offsetOfTitleName(-1),
						    offsetOfTitleGroupUnder(-1),
						    offsetOfLines(-1),
						    offsetOfFRV(-1),
						    offsetOfDate(-1)
  {};
  ~ProcessSolarExport();


  // opens the file exported from Solar
  void open(const string &filename); // throws on issue

  // the CSV file defines the Title of each column,
  // these are not fixed in order or presence,
  // some are predictably present for particular record.
  void loadTitles();

  // Accounts live under top account Groups like Assets, Expenses etc
  // This loads the accounts and associates them with whether they are an Expense etc.
  void loadAccountsOnly();

  // Each Invoice and Expense 'Money Paid Out' has its contribution to the VAT100 figures
  // accumulated.
  void accumulateAllVatInformation(); // assumes titles and accounts loaded.  accumulates vat

  // extraction functions to get the figures for the VAT100
  float getVatDueOnSales() {return incomeVatAmount;};
  float getVatReclaimable() {return reclaimableAmount;};
  float getTotalSalesExVat() {return incomeAmount;};
  float getTotalPurchasesExVat() {return purchasesAmount;};

    // has a column with this title been read from the file?
  bool isTitleKnown(const string &title);

private:
  // given an account name, it provides the name of the top account group eg 'Assets'
  string findTopNode(const string &node);
  // loads one line, naming an account and its account group
  PostAction loadAccountsLine(vector<string> &vec);
  // traverses records in the CSV, applying the passed in function to the line
  void loadFromRecords(PostAction (ProcessSolarExport::*fn)(vector<string> &vec));
  // loads the header title record from the CSV file.
  // this line defines what each column contains
  // NB columns dont have to be present
  PostAction loadTitle(vector<string> &vec);
  // process a line which adds to the VAT100 accumulation
  PostAction accumulateAllVatInformationLine(vector<string> &vec);
  // process a line that describes an invoice
  PostAction processInvoiceVat(vector<string> &vec);
  // process a line that describes an expense
  PostAction processPurchaseVat(vector<string> &vec);
  // get column index/offset containing this title
  int getTitleOffset(const string title);
  // where an invoice or expense has multiple entries, this gets the amount for that entry
  float getAmount(vector<string> &vec, const string& prefix, int lineNo, const string& postfix);

public:
  // ****************
  // for testing only
  // ****************

  // Debug - print the VAT100  accumulation
  void showAccumulation();


  // is this account known
  bool isAccountKnown(const string &accountName);
  // what it the top account name (eg expenses) for this account
  string getAccountTopNode(const string &childAccountName);
};


#endif
