/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "util.h"
#include "cmdParser.h"
#include <locale> 
#include <string>
#include <sstream>
#include <queue>
using namespace std;
#define depth 256

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();


//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool
CmdParser::openDofile(const string& dof)
{
   // TODO...
//   cout << "dof = "<< dof << endl;
   ifstream infile;
   char* c = new char[dof.size() + 1];
   copy(dof.begin(), dof.end(), c); 
   c[dof.size()] = '\0';
   infile.open(c);
   delete[] c;
   if (infile.fail())
   {
       //cout << "file opening failed" << endl;
       return false;
   } 
   else 
   {
        infile.close();
         _dofile = new ifstream(dof.c_str());
       if(_dofile->is_open()){  // basic would open success
            if(_dofileStack.size()<=depth)
            {
       //       cout << "_dofileStack.size() = " << _dofileStack.size() << endl;
              _dofileStack.push(_dofile);
              return true;
            }
            else 
            {
              cerr << "Error: dofile stack overflow (" <<  depth <<")" << endl;
              closeDofile();
              return false;
            }
             
       } else {
         //   cout << "my Open fail" << endl;
            return false;
       }     
       //   else {
       //   cout << "_dofile = " << _dofile  << endl;
       //   return false;
       // }
   }
   //cout << "_dofile = " << _dofile  << endl;
}

// Must make sure _dofile != 0
void
CmdParser::closeDofile()
{
  // cout << "close Dofile " << endl;
   assert(_dofile != 0);
   // TODO...
   delete _dofile;
    //cout << "1. _dofileStack.size() = " << _dofileStack.size() << endl;
    //cout << "1. _dofile =  " << _dofile << endl;
   _dofileStack.pop();
    //cout << "2. _dofileStack.size() = " << _dofileStack.size() << endl;
    //cout << "2. _dofile =  " << _dofile << endl;
   if(_dofileStack.size()>0)
   {
       //cout << "1. _dofile =  " << _dofile << endl;
       _dofile = _dofileStack.top();  // stack pop only decrese size() , pointer location not change
       //cout << "2. _dofile =  " << _dofile << endl;
   } 
   else 
   { 
       _dofile = 0;  // when _dofile used done , need free pointer
                    // execOneCmd would check have file or not 
   } 
   //cout << "delete" << endl;
}

// Return false if registration fails
bool
CmdParser::regCmd(const string& cmd, unsigned nCmp, CmdExec* e)
{
   // Make sure cmd hasn't been registered and won't cause ambiguity
   string str = cmd;
   unsigned s = str.size();
   if (s < nCmp) return false;
   while (true) {
      if (getCmd(str))
      {
        //cout << "test return false" << endl;  //Justin 
        return false;
      }
      if (s == nCmp) break;
      str.resize(--s);
   }
   
   // Change the first nCmp characters to upper case to facilitate
   //    case-insensitive comparison later.
   // The strings stored in _cmdMap are all upper case
   //
   assert(str.size() == nCmp);  // str is now mandCmd
   string& mandCmd = str;
   for (unsigned i = 0; i < nCmp; ++i)
      mandCmd[i] = toupper(mandCmd[i]);
   string optCmd = cmd.substr(nCmp);
   assert(e != 0);
   e->setOptCmd(optCmd);


   // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
   return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
   bool newCmd = false;
   if (_dofile != 0)
      newCmd = readCmd(*_dofile);
   else
      newCmd = readCmd(cin);
   
    //cout << "newCmd = " << newCmd << endl;
   // execute the command
   if (newCmd) {
      string option;
    //cout << "parseCmd = " << newCmd << endl;
      CmdExec* e = parseCmd(option);
      if (e != 0)
         return e->exec(option);
   }
   return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void
CmdParser::printHelps() const
{
   // TODO...

   for (CmdMap::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it ){ //const_iterator is needed
        it->second->help();
        
   }
   cout << endl;
    
}

void
CmdParser::printHistory(int nPrint) const
{
   assert(_tempCmdStored == false);
   if (_history.empty()) {
      cout << "Empty command history!!" << endl;
      return;
   }
   int s = _history.size();
   if ((nPrint < 0) || (nPrint > s))
      nPrint = s;
   for (int i = s - nPrint; i < s; ++i)
      cout << "   " << i << ": " << _history[i] << endl;
}


//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec*
CmdParser::parseCmd(string& option)
{
   assert(_tempCmdStored == false);
   assert(!_history.empty());
   string str = _history.back();

   // TODO...
   assert(str[0] != 0 && str[0] != ' ');

   //cout << "str = " << str << endl;
   CmdExec* e = 0;
   e = getCmd(str);
   
   
   if(!e) // Justin add
   {
        stringstream iss(str);
        string subStr;
        iss >> subStr;
        cerr << "Illegal command!! (" << subStr << ")" << endl;
   } else {
      stringstream iss(str);
      string subStr;
      iss >> subStr;
       
      getline(iss, option);   //streamstring remain part use geline can parse
    
      //cout << "option = " <<option << endl; //Justin
      
      return e;
        //e = getCmd(str);
   }
   //cout << " e " << e << endl;
   //cout << " e->getOptCmd() " << e->getOptCmd() << endl;
   return NULL;
}

// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be 
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. [Before] Null cmd
//    cmd> $
//    -----------
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    -----------
//    [Before] partially matched (multiple matches)
//    cmd> h$aaa                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$aaa                // and then re-print the partial command
//
// 3. [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $
//    -----------
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$hahah
//    [After Tab]
//    cmd> heLp $hahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//
// 4. [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. [Before] Already matched
//    cmd> help asd$fg
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$fg
//
// 6. [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location
//
void
CmdParser::listCmd(const string& str)
{
   // TODO...
   string mainCmd , subStr;
   locale loc;  

   mainCmd = _readBuf;
   //cout << _readBuf << endl;
   string upperStr = mainCmd;
   for (int i=0; i<mainCmd.length(); ++i){
        upperStr[i] = toupper(mainCmd[i],loc);
   } 
//debug use
   //cout << endl << "upperStr = " <<  upperStr << endl;
   //cout << endl << "_readBuf = " <<  _readBuf << endl;  
   size_t offset = _readBufPtr - _readBuf;
   //cout << "offset = " << offset << endl;
   char c = ' ';
   bool spaceFlag = false;
   for(int i=0; i<offset ;i++)
   {
    // subStr = upperStr.substr(offset);
      if(upperStr[i] ==' ' && spaceFlag == false){
           // do nothing
      }
      else //if (upperStr[i]!=' ')
      {
   //      cout << "upperStr[i] = " <<upperStr[i] << endl;
         c = upperStr[i];
         subStr.append(1,c);
         spaceFlag = true;
      } 

      
   }
   //cout << endl << "subStr = " << subStr << endl;
   reprintCmd();
//debug end   

    int count = 0 ;
    bool findFlag = false , speciFlag = false;
    queue<string> tmpQ;
    string qString;
    size_t offsetToEnd = _readBufEnd - _readBufPtr;
    if(!(upperStr.find_first_not_of(' ') != string::npos)) // list All option
    {
       cout << endl;
        for (CmdMap::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it ){ //const_iterator is needed
            cout << setw(12) << left << it->first + it->second->getOptCmd() ;
            ++count;
            
            if(count%5 == 0){
                cout << endl;
            }
            
        }
        reprintCmd();
    } else {   // parse string

        int subStrLen = subStr.length();
        for (CmdMap::const_iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it ){ //const_iterator is needed
           //TODO
           if( it->first.compare(0,subStrLen,subStr.substr(0,subStrLen)) == 0 ) //equal
           {
                tmpQ.push(it->first + it->second->getOptCmd()) ;
             /*   if(findFlag == false){
                    cout << endl;
                    findFlag = true;
                 }  
                 cout << setw(12) << left << it->first + it->second->getOptCmd() ;
                 //cout << "count = " << count << endl;
                 ++count;
                
                if(count%5 == 0){
                    cout << endl;
                }*/            
           }               
        }      
        
        if(tmpQ.size()==1){
            qString = tmpQ.front();
            cout << qString.substr(subStrLen);
            int  backtimes = 0;
            speciFlag = true;
            char *tmpPtr = _readBufEnd;
            /*
            //offsetEnd += subStrLen;   //total offset is include
            _readBufPtr += qString.substr(subStrLen).length(); //  replace respond char
            _readBufEnd += offsetToEnd; //  replace respond char
            while(_readBufPtr != _readBufEnd){
                *(_readBufEnd) = *(_readBufEnd-offsetToEnd); // replace respond char
                _readBufEnd--;
            }
            _readBufEnd = tmpPtr;      
            // *_readBufPtr = ch;
            //_readBufPtr++;
            tmpPtr = _readBufPtr;  // record now cursor location
            while(_readBufPtr != _readBufEnd){
                 cout << *_readBufPtr;
                _readBufPtr++;
                backtimes++;
            }   
            _readBufPtr = tmpPtr;
            for(int i = 0; i < backtimes ; i++){  // Don't forget back to origin position
                cout << "\b" ;
            }   
            */
 
        }
        
        while (!tmpQ.empty() && speciFlag == false)
        {
            if(findFlag == false){
                    cout << endl;
                    findFlag = true;
            }  

            cout << setw(12) << left << tmpQ.front() ;
            tmpQ.pop();
             ++count;
                
            if(count%5 == 0){
                cout << endl;
            }

        }
    
        if(findFlag == true){ 
           reprintCmd();
        }
    }
   //cout << "str = "   << str << endl; 
//   cout << setw(12) << left << endl;
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec*
CmdParser::getCmd(string cmd)
{
   CmdExec* e = 0;
   // TODO...
   locale loc;
   string upperStr = cmd;
   for (int i=0; i<cmd.length(); ++i){
        upperStr[i] = toupper(cmd[i],loc);
   }
   //cout << "cmd = " << cmd << endl;
   for (CmdMap::iterator it = _cmdMap.begin(); it != _cmdMap.end(); ++it ){
         //   cout << "it->first = " << it->first << endl;
        
        if(upperStr.find(it->first)== 0)  //if find , get position of the first character of the first match
        {                             //if not function returns string::npos
            e = it->second;  //if find the begin string , they use same CmdExec object
                             //origin expect e would be null
        }
       
        // cout << "upperStr.find(it->first) = " << upperStr.find(it->first) << endl; 
       // cout << "e = " << e << endl; 
        //cout << it->first << " => " << it->second->getOptCmd() << '\n';
   }
   return e;
}


//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool
CmdExec::lexSingleOption
(const string& option, string& token, bool optional) const
{
   size_t n = myStrGetTok(option, token);
   if (!optional) {
      if (token.size() == 0) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
   }
   if (n != string::npos) {
      errorOption(CMD_OPT_EXTRA, option.substr(n));
      return false;
   }
   return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool
CmdExec::lexOptions
(const string& option, vector<string>& tokens, size_t nOpts) const
{
   string token;
   size_t n = myStrGetTok(option, token);
   while (token.size()) {
      tokens.push_back(token);
      n = myStrGetTok(option, token, n);
   }
   if (nOpts != 0) {
      if (tokens.size() < nOpts) {
         errorOption(CMD_OPT_MISSING, "");
         return false;
      }
      if (tokens.size() > nOpts) {
         errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
         return false;
      }
   }
   return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string& opt) const
{
   switch (err) {
      case CMD_OPT_MISSING:
         cerr << "Missing option";
         if (opt.size()) cerr << " after (" << opt << ")";
         cerr << "!!" << endl;
      break;
      case CMD_OPT_EXTRA:
         cerr << "Extra option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_ILLEGAL:
         cerr << "Illegal option!! (" << opt << ")" << endl;
      break;
      case CMD_OPT_FOPEN_FAIL:
         cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
      break;
      default:
         cerr << "Unknown option error type!! (" << err << ")" << endl;
      exit(-1);
   }
   return CMD_EXEC_ERROR;
}

