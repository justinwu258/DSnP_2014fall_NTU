/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirPIGate;
class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

class CirGate
{
public:
   //CirGate(){}
   CirGate(int l, int I): _lineNo(l), _ID(I){};
   virtual ~CirGate() {}
   //virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { 
        if(_type == 1)
            return "input"; 
        return ""; 
   }
   unsigned getLineNo() const { return _lineNo; }
   unsigned getID() const { return _ID; }

   // Printing functions
   //virtual void printGate() const = 0;
   void printGate() const;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   //friend class CirPIGate;
   
private:
    int _lineNo;
    int _type;
    
protected:
    int _flag;
    int _column;
    int _ID;
    string _name; 
    vector<CirGate*> _faninList;
    vector<CirGate*> _fanoutList;

};

class CirPIGate: public CirGate{
    public:
        //friend class CirGate;
        //CirPIGate(){}
        CirPIGate(int l, int I): CirGate(l, I){}
        ~CirPIGate() {}
        //void printGate() const;
        //void setLineNo(int number){ _lineNo = number; }
   // friend int _lineNo;
   // friend int _type;
    //void printGate();
};
class CirPOGate: public CirGate{
        CirPOGate(int l, int I): CirGate(l, I){};
        ~CirPOGate() {}
        //void printGate() const;
    //void printGate();
    
};

#endif // CIR_GATE_H
