/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar checkChar(char, istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      char ch = mygetc(istr);
      ParseChar pch = checkChar(ch, istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : cout << " ";/* TODO */ break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: moveBufPtr(_readBufPtr+1); /* TODO */ break;
         case ARROW_LEFT_KEY : moveBufPtr(_readBufPtr-1); /* TODO */ break;  // cout << '\b';   ok,too
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO */ break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY:   mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO...
   //cout << ptr << endl; // print point's address
   // cout << "_readBuf" << _readBuf << endl;
   //  cout << "_readBufEnd" << _readBufEnd << endl;
   if ((ptr > _readBufEnd) || (ptr < _readBuf) ){
        mybeep();
        return false;
   }
   
   while(_readBufPtr > ptr){
        cout << "\b" ;
        _readBufPtr--;
   }
    
   while(_readBufPtr < ptr){
        cout << *_readBufPtr ;
        _readBufPtr++;
   }
   //cout << "Home test" << endl;
   //cout.flush();
   return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO...
   return true;
}

// 1. Insert character 'ch' at _readBufPtr
// 2. Move the remaining string right for one character
// 3. The cursor should move right for one position afterwards
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k') ---
//
// cmd> This is kthe command
//               ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   cout << ch ;
   _readBufPtr++; 

  cout << "before plus _readBufPtr" << _readBufPtr << endl;
   int pos_start , pos_end ;
   _readBufEnd++;
   _readBufPtr++;

   pos_start=(int)(_readBufPtr-_readBuf);
   pos_end=(int)(_readBufEnd-_readBuf);
   cout << "pos_start " << pos_start << endl;
   cout << "pos_end " << pos_end << endl;
  cout << "After plus _readBufPtr" << _readBufPtr << endl;
  //char *ptr = "ken";
  int value = 10;
  char *ptr = new (char);
  ptr = "abc";
  printf("ptr = %d  (\% d)\n", ptr);
  printf("ptr = %s  (\% s)\n", ptr);
  printf("ptr = %c  (\% c)\n", ptr);
  cout << " ptr+1 = "  << ptr+1 << endl; 
  cout << " *ptr+1 = "  << *ptr+1 << endl; 
  cout << " &ptr = "  << &ptr << endl; 
  cout << " &(*ptr) = "  << &(*ptr) << endl; 
  ptr++;
  cout << " &(ptr+1) = "  << &(ptr) << endl; 
  cout << " (ptr+1) = "  << ptr << endl; 
  cout << " *(ptr+1) = "  << *ptr << endl; 
  ptr++;
  cout << " &(ptr+2) = "  << &(ptr) << endl; 
  cout << " (ptr+2) = "  << ptr << endl; 
  cout << " *(ptr+2) = "  << *ptr << endl; 
  /*
  cout << "before plus _readBufPtr" << _readBufPtr << endl;
  // cout << "before plus _readBuf" << &_readBuf << endl;
  // cout << "before plus _readBuf space" << _readBuf << endl;
   char *hello;
   hello = new (char);
   
    int var = 10; 
    int *myptr = &var ; 
   cout << "var = " << var 
         << endl; 
    cout << "*myptr = " << myptr 
         << endl; 

    *myptr = 20; 

    cout << "var = " << var 
         << endl;
    cout << "*myptr = " << *myptr 
         << endl;
   cout << "hello = " << hello << endl;
   char a = 'k';
   *_readBufPtr = "pppgggccc";
   // *_readBufPtr = 10;
   //_readBufPtr++;
   cout << "After plus _readBufPtr" <<  *_readBufPtr << endl;
   _readBufPtr = &a;
   cout << "After plus _readBufPtr2" <<  _readBufPtr << endl;
   
 
  // cout << "before plus _readBuf" << &_readBuf << endl;
  // cout << "before plus _readBuf space" << _readBuf << endl;
   //cout << ch ;
   */
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
// 6. Reset _readBufPtr and _readBufEnd to _readBuf
// 7. Make sure *_readBufEnd = 0 ==> _readBuf becomes null string
//
void
CmdParser::addHistory()
{
   // TODO...
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
