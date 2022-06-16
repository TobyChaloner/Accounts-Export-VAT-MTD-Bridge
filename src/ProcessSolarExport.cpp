//#include <csv.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/tokenizer.hpp>


using namespace std;

#include "ProcessSolarExport.h"


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



void ProcessSolarExport::loadTitles()
{
  loadFromRecords(&ProcessSolarExport::loadTitle); 
}



PostAction ProcessSolarExport::loadTitle(vector<string> &vec)
{
  for(const auto& title: vec) {
    titles.push_back(title);
    titleOffset[title] = titles.size() - 1;
  }
 
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
  if (vec[0] == accountGroupPrefix)
  {
    // its a group
    // its either a top node, or a sub node.  top nodes have 3 fields, subnodes have more
    if (vec.size() == 3)
    {
      string name = rtrim(vec[1]);
      // top node
      hierachy[name] = name;
    }
    else
    {
      string name = rtrim(vec[1]);
      string parent = rtrim(vec[3]);
	
      // size is 4
      // flatten hierachy, so all nodes point to their top node.  i.e. Expenses point to Expenses
      string topNode = findTopNode(parent);
      hierachy[name] = topNode; 
    }
  }

  // looking at an account
  else if (vec[0] == accountPrefix)
  {
    // an account needs to be identified as an Expenses or Income account
    string topNode = findTopNode(vec[3]);
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
  if (vec[0] == "Invoice")
  {
    processInvoiceVat(vec);
  }
  if (vec[0] == "Money Paid Out")
  {
    processPurchaseVat(vec);
  }
  return PostAction::not_finished;  
  
}


// lineNo eg Line 1 VAT Amount so 1
float ProcessSolarExport::getAmount(vector<string> &vec, const string& prefix, int lineNo, const string& postfix)
{
    stringstream ss;
    ss << prefix<<lineNo<<postfix;
#if 0
    cout << "Lookingup:"<<ss.str()<<"-"<<endl;
#endif    
    auto it = titleOffset.find(ss.str());
    if (it == titleOffset.end())
    {
      string msg = "titles map did not contain node " + ss.str();
      throw (runtime_error(msg));
    }

    int offset = it->second;
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
  int linesOffset = titleOffset["Lines"];
  string linesStr = vec[linesOffset];
  int lines = stoi(linesStr); // string to int C++11
  for (int i=1; i <= lines; ++i)
  {
    // if in interesting dates
    int dateOffset = titleOffset["Date"];
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
  int linesOffset = titleOffset["Lines"];
  string linesStr = vec[linesOffset];
  int lines = stoi(linesStr); // string to int C++11
  for (int i=1; i <= lines; ++i)
  {
    // if in interesting dates
    int dateOffset = titleOffset["Date"];
    string recDate = vec[dateOffset];
    if (vatDate->isInVatPeriod(recDate))
    {
      float amountIncVat = getAmount(vec, string("Line "), i, string(" Amount"));
#if 0
      cout << "purchase amount (inc VAT) "<< amountIncVat<<endl;
#endif
      // Only VAT Rate is stored, amount needs to be calculated
      
      float vatPercent = getAmount(vec, string("Line "), i, string(" VAT Rate"));
      float originalAmount(0.0);
      float vatAmount(0.0);
      if (vatPercent > 0.0)
      {
	// only interested in figures that have some VAT on them
	originalAmount = amountIncVat * 100 / (100+vatPercent); // *100 to avoid truncation of floating point
	vatAmount = amountIncVat - originalAmount;
#if 0
	cout << "VAT%  "<< vatPercent<<endl;
	cout << "VAT amount  "<< vatAmount<<endl;
#endif
	reclaimableAmount += vatAmount;
	purchasesAmount += originalAmount;
      }
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
