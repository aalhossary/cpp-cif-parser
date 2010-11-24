//$$FILE$$
//$$VERSION$$
//$$DATE$$
//$$LICENSE$$


/*!
** \file CifScannerBase.C
**
** \brief Implementation file for CifScanner class.
*/


/* 
  PURPOSE:    A DDL 2.1 compliant CIF file parser.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CifScannerBase.h"
#include "CifScannerInt.h"
#include "CifParserBase.h"
#include "CifParser.h"

extern int cifparser_leng;
extern char* cifparser_text;
extern YYSTYPE cifparser_lval;
extern FILE* cifparser_in;

#define yyleng cifparser_leng
#define yytext cifparser_text
#define yylval cifparser_lval
#define yyin cifparser_in

extern "C" void cif_yy_less(int i);

extern CifParser* CifParserP;

using std::ios;
using std::endl;

CifScanner::CifScanner() {
  Clear();
  _verbose=false;
  // VLAD: WATCH HERE
  _tBuf = new string;
  _tBuf->clear();
 if (_verbose) log << "Default constructor called" << endl;
}

void CifScanner::OpenLog(const string& logName, bool verboseLevel)
{
  _verbose = verboseLevel;
  if (!logName.empty()) log.open(logName.c_str(),ios::out|ios::trunc);
}

/*
CifScanner::CifScanner(istream *in) {
  Clear();
  _verbose=false;
  _tBuf = new CifString(1025,512);
  _tBuf->Clear();
  yyin=in;
 if (_verbose) log << "CifScanner::CifScanner(istream *in, int verbose) constructor called" << endl;
}
*/

void CifScanner::Clear(void) {
  _tBuf=NULL;
  _isText = false;
  NDBlineNo=1;
}


void CifScanner::Reset(void) {
if (_tBuf) delete _tBuf;
  _tBuf=NULL;
  Clear();
}

int CifScanner::yylex()
{
   return(0);
}


int CifScanner::ProcessNone()
{

#if DEBUG
          log << "LS0: line "<<  NDBlineNo <<  " length " << yyleng << " yytext=" << yytext << endl;
#endif  
          NDBlineNo++;
          if (_isText == true) {          /* end of text value */
             for (_i=yyleng-1; _i >= 0; _i--) {
               if ( yytext[_i] == ' ' || yytext[_i] == '\t' ||  yytext[_i] == '\n' || yytext[_i] == '\r') {
                  yytext[_i]='\0';
               } else if ( yytext[_i] == ';') {
                    yytext[_i]='\0';
                    break;
               } else
                  break;
             }
             (*_tBuf)+=yytext;
          _tBuf->erase(strlen(_tBuf->c_str())-1,1);
             yylval.cBuf=(char*)_tBuf->c_str();
             _isText = false;
#if DEBUG
          log << "LS1: String[" <<  strlen(yylval.cBuf) << "] " << yylval.cBuf << endl;
#endif
             return(LSITEMVALUE_CIF);
          } else {  /* text value begins */
             _isText = true;

             for (_i=0; _i < yyleng; _i++) {
                 if (yytext[_i] == ';') {  break; }
             }
             _tBuf->clear();
             string tmpP;
             for (unsigned int tmpI = 0; tmpI < strlen(&yytext[_i+1]); tmpI++)
             {
                 if (yytext[_i+1+tmpI] != '\r')
                 {
                     tmpP.push_back(yytext[_i+1+tmpI]);
                 }
             }
       
             (*_tBuf) += tmpP;
          }
    return (0);
}


void CifScanner::ProcessWhiteSpace()
{
         for (_i=0; _i < yyleng; _i++)
           if (yytext[_i] == '\n') NDBlineNo++;
    if (_isText)
        (*_tBuf) += yytext;

}

int CifScanner::ProcessData()
{
      if (_isText)
        (*_tBuf) += yytext;
      else {
        yylval.cBuf=yytext;
        return (DATABLOCK_CIF);
      }

      return (0);
}

int CifScanner::ProcessLoopScanner()
{
      if (_isText)
        (*_tBuf) += yytext;
      else
        return (LOOP_CIF);

      return (0);
}

void CifScanner::ProcessStop()
{
      if (_isText)
        (*_tBuf) += yytext;
}

int CifScanner::ProcessDot()
{
      if (_isText)
        (*_tBuf) += yytext;
      else
        return (UNKNOWN_CIF);

      return (0);
}

int CifScanner::ProcessQuestion()
{
      if (_isText)
        (*_tBuf) += yytext;
      else
        return (MISSING_CIF);

      return (0);
}

void CifScanner::ProcessComment()
{
      if (_isText)
        (*_tBuf) += yytext;
}

int CifScanner::ProcessUnderscore()
{
      if (_isText) {
        (*_tBuf) += yytext;
      } else {
        /* If the beginning of text is in buffer yytext */
         for (_i=0; _i<yyleng; _i++)
           if (yytext[_i] == '_') break;
         yylval.cBuf=yytext;
         return(ITEMNAME_CIF);
      }

      return (0);
}

int CifScanner::ProcessBadStrings()
{
     if (!_isText) {
        /*
        ** The string is not part of a multiline CIF value, but a CIF value
        ** anywhere else, in a loop or out of the loop.
        */
        _j=0;
        yylval.cBuf=&yytext[_j];
#if DEBUG
        log << "UQ: String " << yylval.cBuf << endl;
#endif

        unsigned int cBufLen = strlen(yylval.cBuf);
        for (unsigned int i = 0; i < cBufLen; ++i)
        {
            if ((yylval.cBuf[i] == '\'') || (yylval.cBuf[i] == '\"'))
            {
                log << "ERROR - Invalid character at line " <<
                  String::IntToString(NDBlineNo) << " in CIF value " <<
                  yylval.cBuf << endl;
            }
        }

        return(ITEMVALUE_CIF);
     }
     else {
        /*
        ** The string is part of a multiline CIF value. It is processed as is.
        */
#if DEBUG
          log << "UQx: String " << yytext<< endl;
#endif
        (*_tBuf) += yytext;
     }

     return (0);
}

int CifScanner::ProcessSQuotedStrings()
{
char * p;
     if (!_isText) {
        p=yytext;
                  p++;
        while ((p=strchr(p,'\''))) {
          p++;
          if ( p[0] == ' ' || p[0] == '\t' || p[0] == '\n' || p[0] == '\r') {
             _i=yyleng-strlen(p);
             cif_yy_less(_i);
             p=&yytext[yyleng];
          }
        }
        yylval.cBuf=&yytext[1];
        yylval.cBuf[_i-2]='\0';
        return(ITEMVALUE_CIF);
     }
     else {
        if (yytext[yyleng-1] == '\n')
        {
            if (yytext[yyleng-2] == '\r')
            {
               cif_yy_less(yyleng-2);
            }
            else
            {
               cif_yy_less(yyleng-1);
            }
        }

        (*_tBuf) += yytext;
     }

     return (0);
}

int CifScanner::ProcessDQuotedStrings()
{
char * p;
     if (!_isText) {
        p=yytext;
                  p++;
        while ((p=strchr(p,'\"'))) {
          p++;
          if ( p[0] == ' ' || p[0] == '\t' || p[0] == '\n' || p[0] == '\r') {
             _i=yyleng-strlen(p);
             cif_yy_less(_i);
             p=&yytext[yyleng];
          }
        }
        yylval.cBuf=&yytext[1];
        yylval.cBuf[_i-2]='\0';
        return(ITEMVALUE_CIF);
     }
     else {
        if (yytext[yyleng-1] == '\n')
        {
            if (yytext[yyleng-2] == '\r')
            {
               cif_yy_less(yyleng-2);
            }
            else
            {
               cif_yy_less(yyleng-1);
            }
        }

        (*_tBuf) += yytext;
     }

     return (0);
}

int CifScanner::ProcessEof()
{
   if (_isText == true) {
      _isText=false;
           log<<"String is not not finish at line "<<NDBlineNo<< endl;
           return(1);
        }
        else
           return(0);
}

int ProcessNoneFromScanner()
{
    return (CifParserP->ProcessNone());
}

void ProcessWhiteSpaceFromScanner()
{
    CifParserP->ProcessWhiteSpace();
}

int ProcessDataFromScanner()
{
    return (CifParserP->ProcessData());
}

int ProcessLoopFromScanner()
{
    return (CifParserP->ProcessLoopScanner());
}

void ProcessStopFromScanner()
{
    CifParserP->ProcessStop();
}

int ProcessDotFromScanner()
{
    return (CifParserP->ProcessDot());
}


int ProcessQuestionFromScanner()
{
    return (CifParserP->ProcessQuestion());
}


void ProcessCommentFromScanner()
{
    CifParserP->ProcessComment();
}


int ProcessUnderscoreFromScanner()
{
    return (CifParserP->ProcessUnderscore());
}


int ProcessBadStringsFromScanner()
{
    return (CifParserP->ProcessBadStrings());
}


int ProcessSQuotedStringsFromScanner()
{
    return (CifParserP->ProcessSQuotedStrings());
}


int ProcessDQuotedStringsFromScanner()
{
    return (CifParserP->ProcessDQuotedStrings());
}


int ProcessEofFromScanner()
{
    return (CifParserP->ProcessEof());
}

