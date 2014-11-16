/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(sizeof(MemTestObj))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   int numObjects, arraySize;
   if (!CmdExec::lexOptions(option, options, 0))  // put option to options[i]
      return CMD_EXEC_ERROR;

   //cout << "options.size =  "  <<  options.size() << endl;
   if(options.size() == 1){
       if(!myStr2Int(options[0],numObjects)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
       if(numObjects <= 0)
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
       mtest.newObjs(numObjects);
   } else if(options.size() == 3) {
       if(myStrNCmp("-Array",options[0],2) == 0) {  // cmp "-a/-A"
         if(!myStr2Int(options[1],arraySize)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);  
         if(!myStr2Int(options[2],numObjects)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         if(numObjects <= 0)
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         if(arraySize <= 0)
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
         mtest.newArrs(numObjects,arraySize);
       } else if(myStrNCmp("-Array",options[1],2) == 0) {
         if(!myStr2Int(options[2],arraySize))
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         if(!myStr2Int(options[0],numObjects))
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
         if(arraySize <= 0)
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         if(numObjects <= 0)
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
         mtest.newArrs(numObjects,arraySize);
       } else { CmdExec::errorOption(CMD_OPT_ILLEGAL, option); } 
   } else if(options.size() == 0){
       //cout << "CMD_EXEC_ERROR" << endl;
       return CmdExec::errorOption(CMD_OPT_MISSING, "");
   } else {
       return CmdExec::errorOption(CMD_OPT_EXTRA, options[options.size()-1]);
   }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string> options;
   int objIdx, arraySize;
   if (!CmdExec::lexOptions(option, options, 0))  // put option to options[i]
      return CMD_EXEC_ERROR;

   if(options.size() == 2){
       if(myStrNCmp("-Index",options[0],2) == 0) {  // cmp "-i/-I"
         if(!myStr2Int(options[1],objIdx)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);  
         mtest.deleteObj(objIdx);
       } else if(myStrNCmp("-Random",options[0],2) == 0) { // cmp "-r/-R"
         RandomNumGen(); 
       }
       //if(!myStr2Int(options[0],numObjects)) 
       //    return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
       //mtest.newObjs(numObjects);
   } else if(options.size() == 3) {
       /*if(myStrNCmp("-Array",options[0],2) == 0) {  // cmp "-a/-A"
         if(!myStr2Int(options[1],arraySize)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);  
         if(!myStr2Int(options[2],numObjects)) 
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         mtest.newArrs(numObjects,arraySize);
       } else if(myStrNCmp("-Array",options[1],2) == 0) {
         if(!myStr2Int(options[2],arraySize))
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
         if(!myStr2Int(options[0],numObjects))
           return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
         mtest.newArrs(numObjects,arraySize);
       } else { CmdExec::errorOption(CMD_OPT_ILLEGAL, option); } */
   } else if(options.size() == 0){
       //cout << "CMD_EXEC_ERROR" << endl;
       return CmdExec::errorOption(CMD_OPT_MISSING, "");
   } else {
       return CmdExec::errorOption(CMD_OPT_EXTRA, options[options.size()-1]);
   }

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


