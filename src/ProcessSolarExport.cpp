#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/tokenizer.hpp>
#include "ProcessSolarExport.h"

//#define DEBUG_PURCHASE 1

using namespace std;
using namespace boost;



// trim from right
// needed as exports from Windows have interesting line endings
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}


void ProcessSolarExport::open(const string &filename)
{
  fstr.open(filename);
  if (!fstr.is_open())
  {
    string msg = "Failed to open: " + filename;
    throw runtime_error(msg);
  }
}



ProcessSolarExport::~ProcessSolarExport()
{
  if (fstr.is_open())
  {
    fstr.close();
  }
}




void ProcessSolarExport::loadTitles()
{
  loadFromRecords(&ProcessSolarExport::loadTitle); 
}



PostAction ProcessSolarExport::loadTitle(vector<string> &vec)
{
  for(const auto& title: vec)
  {
    titles.push_back(title);
    titleOffset[title] = titles.size() - 1;
  }

  offsetOfTitleType = getTitleOffset("Type");
  offsetOfTitleName = getTitleOffset("Name");
  offsetOfTitleGroupUnder = getTitleOffset("Group Under");

  return PostAction::finish_done_record;  // only ever one record
}



string ProcessSolarExport::findTopNode(const string &node)
{
  auto it = hierachy.find(node);
  if (it == hierachy.end())
  {
    string msg = "map did not contain node " + node;
    throw (runtime_error(msg));
  }
  

  // simplest case node = node === top node
  if (it->second == node)
  {
    return node;
  }
  // recurse on node
  return findTopNode(it->second);


}



void ProcessSolarExport::loadAccountsOnly()
{
  loadFromRecords(&ProcessSolarExport::loadAccountsLine);
}


PostAction ProcessSolarExport::loadAccountsLine(vector<string> &vec)
{
  const string accountPrefix("Account");
  const int lenPrefix(accountPrefix.size());
  const string accountGroupPrefix("Account Group");
  const int lenGroupPrefix(accountGroupPrefix.size());
  
  if (vec[offsetOfTitleType] == accountGroupPrefix)
  {
    // its a group

    // there are two types of group record.  One that is a topnode (no or blank 'Group Under') and a sub node, which has a 'Group Under'
    // the Group Under need to recurse back to their topnode.

    
    
    // its either a top node, or a sub node.  top nodes have fewer fields than subnodes
    if (vec.size() < offsetOfTitleGroupUnder+1 || vec[offsetOfTitleGroupUnder] == string(""))
    {
      string name = rtrim(vec[offsetOfTitleName]);
      // top node
      hierachy[name] = name;
    }
    else
    {
      string name = rtrim(vec[offsetOfTitleName]);
      string parent = rtrim(vec[offsetOfTitleGroupUnder]);
	
      // flatten hierachy, so all nodes point to their top node.  i.e. Expenses point to Expenses
      string topNode = findTopNode(parent);
      hierachy[name] = topNode; 
    }
  }

  // looking at an account
  else if (vec[offsetOfTitleType] == accountPrefix)
  {
    // an account needs to be identified as an Expenses or Income account
    string topNode = findTopNode(vec[offsetOfTitleGroupUnder]);
    hierachy[vec[1]] = topNode; 
  }
  else
  {
    // finished looking at Accounts, reset file pointer to start of line and finish
    // return read position to start of the uninteresting record
    return PostAction::finish_reprocess_record; // finished
  }
  return PostAction::not_finished;  
}




void ProcessSolarExport::accumulateAllVatInformation()
{
  // assume have accounts loaded and titles
  loadFromRecords(&ProcessSolarExport::accumulateAllVatInformationLine);
}



PostAction ProcessSolarExport::accumulateAllVatInformationLine(vector<string> &vec)
{
  if (vec[offsetOfTitleType] == "Invoice")
  {
    processInvoiceVat(vec);
  }
  if (vec[offsetOfTitleType] == "Money Paid Out")
  {
    processPurchaseVat(vec);
  }
  return PostAction::not_finished;  
  
}


// get column index/offset containing this title
int ProcessSolarExport::getTitleOffset(const string title)
{
    auto it = titleOffset.find(title);
    if (it == titleOffset.end())
    {
      string msg = "titles map did not contain node " + title;
      throw (runtime_error(msg));
    }
    return it->second;  
}



// lineNo eg Line 1 VAT Amount so 1
float ProcessSolarExport::getAmount(vector<string> &vec, const string& prefix, int lineNo, const string& postfix)
{
    stringstream ss;
    ss << prefix<<lineNo<<postfix;
#if 0
    cout << "Lookingup:"<<ss.str()<<"-"<<endl;
#endif    
    int offset = getTitleOffset(ss.str());
    string AmountStr = vec[offset];

    const char poundSymbol = '\243';
    AmountStr.erase(remove(AmountStr.begin(), AmountStr.end(), poundSymbol), AmountStr.end()); //remove pound from string
    AmountStr.erase(remove(AmountStr.begin(), AmountStr.end(), ','), AmountStr.end()); //remove , from string
    AmountStr.erase(remove(AmountStr.begin(), AmountStr.end(), '%'), AmountStr.end()); //remove percent from string
#if 0
    cout << "getAmount:"<<AmountStr<<endl;
#endif
    return stof(AmountStr);
}


/*
  Oddly enough, the Solar Accounts csv contains no separated VAT for the expenses.
*/
PostAction ProcessSolarExport::processInvoiceVat(vector<string> &vec)
{
  int linesOffset = getTitleOffset("Lines");
  string linesStr = vec[linesOffset];
  int lines = stoi(linesStr); // string to int C++11
  for (int i=1; i <= lines; ++i)
  {
    // if in interesting dates
    int dateOffset = getTitleOffset("Date");
    string recDate = vec[dateOffset];
    if (vatDate->isInVatPeriod(recDate))
    {
      float amount = getAmount(vec, string("Line "), i, string(" Amount"));
#if 0
      cout << "amount "<< amount<<endl;
#endif
      incomeAmount += amount;
      float vatAmount = getAmount(vec, string("Line "), i, string(" VAT Amount"));
      incomeVatAmount += vatAmount;
    }
    

  }
  return PostAction::not_finished;  
}



PostAction ProcessSolarExport::processPurchaseVat(vector<string> &vec)
{
  int linesOffset = getTitleOffset("Lines");
  string linesStr = vec[linesOffset];
  int lines = stoi(linesStr); // string to int C++11
  for (int i=1; i <= lines; ++i)
  {
    // if in interesting dates
    int dateOffset = getTitleOffset("Date");
    string recDate = vec[dateOffset];
    if (vatDate->isInVatPeriod(recDate))
    {
      float amountIncVat = getAmount(vec, string("Line "), i, string(" Amount"));
      // Only VAT Rate is stored, amount needs to be calculated
      
      float vatPercent = getAmount(vec, string("Line "), i, string(" VAT Rate"));
      float expenseExVat(0.0);
      float vatAmount(0.0);
#if DEBUG_PURCHASE
      cout << "purchase amount (inc any VAT): "<< amountIncVat<<endl;
      cout << "VAT: "<< vatPercent<<"%"<<endl;
#endif
      expenseExVat = amountIncVat;  // assuming 0% VAT
      if (vatPercent > 0.0)
      {
	// only figures that have some VAT on them
	expenseExVat = amountIncVat * 100 / (100+vatPercent); // *100 to avoid truncation of floating point
#if DEBUG_PURCHASE
	cout << "original amount  "<< expenseExVat<< " (un rounded)"<<endl;
#endif
	// Round up
	expenseExVat *= 100.0; // to make a whole number
	expenseExVat = round(expenseExVat);
	expenseExVat /= 100.0; // move back to a pounds and pence number
	// End Rounding
	vatAmount = amountIncVat - expenseExVat;
#if DEBUG_PURCHASE
	cout << "VAT amount  "<< vatAmount<<endl;
#endif
	reclaimableAmount += vatAmount;
      }
      // figures with and without VAT are totalled up.
      purchasesAmount += expenseExVat;
    }
  }
  return PostAction::not_finished;  
}




// =============== GENERIC loader =====================

// expects to load until the line processor responds false,
// the fp will be set to beginning of records where false returned
//
void ProcessSolarExport::loadFromRecords(PostAction (ProcessSolarExport::*fn)(vector<string> &vec))
{
  string line;
  typedef tokenizer< escaped_list_separator<char> > Tokenizer;
  vector< string > vec;
  int getPosition = fstr.tellg();
  int lastQuote(0);
  bool insideQuotes(false);
  string buffer;
  while (getline(fstr, buffer))
  {
    // deal with multi line records
    lastQuote = buffer.find_first_of('"');
    while (lastQuote != string::npos)
    {
      insideQuotes = !insideQuotes;
      lastQuote = buffer.find_first_of('"', lastQuote + 1);
    }
    line.append(buffer);
    if (insideQuotes)
    {
      line.append("\n");
      continue;
    }

    // ONLY gets here if not in a continued record
#if 0
    cout << "line:" << rtrim(line)<<endl;
#endif
    
    // Now tokenise
    Tokenizer tok(line);
    vec.assign(tok.begin(),tok.end());
    // vec is now loaded
    PostAction action = (*this.*fn)(vec);
    if (action == PostAction::finish_reprocess_record)
    {
      fstr.seekg(getPosition);
      line.clear();
      break;
    }
    if (action == PostAction::finish_done_record)
    {
      line.clear();
      break;
    }
    line.clear();
  } // while getline
}


void ProcessSolarExport::showAccumulation()
{
  cout << "Vat due on Sales,"<< incomeVatAmount<< endl
       << "VAT Reclaimable,"<<reclaimableAmount<< endl
       << "Total Sales," << incomeAmount << endl
       << "Total Purchases," <<purchasesAmount << endl;
}





// ========================== TEST HARNESS TOOLS ===========================


// true if the title has been loaded
bool ProcessSolarExport::isTitleKnown(const string &title)
{
  return titleOffset.find(title) != titleOffset.end();
}


// is this account known
bool ProcessSolarExport::isAccountKnown(const string &accountName)
{
  return hierachy.find(accountName) != hierachy.end();
}

// returns topnode of account
// string "Account Not known" if not there
string ProcessSolarExport::getAccountTopNode(const string &childAccountName)
{
  string topNodeName("Account Not known");
  map<string,string>::iterator itr = hierachy.find(childAccountName);
  if (itr != hierachy.end())
  {
    topNodeName = itr->second;
  }
  return topNodeName;
}
