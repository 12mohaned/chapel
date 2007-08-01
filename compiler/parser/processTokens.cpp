#include <string.h>
#include "countTokens.h"
#include "misc.h"
#include "processTokens.h"
#include "yy.h"
#ifndef MODULE_FINDER
#include "chapel.tab.h"
#else
#include "modulefinder.tab.h"
#define countNewline()
#define countSingleLineComment(x)
#define countMultiLineComment(x)
#define countCommentLine()
#endif

extern YYLTYPE yylloc;

static int stringBuffLen = 0;
static int stringLen = 0;
static char* stringBuffer = NULL;


static void newString(void) {
  stringLen = 0;
  if (stringBuffLen) {
    stringBuffer[stringLen] = '\0';
  }
}


static void addChar(char c) {
  if (stringLen+2 > stringBuffLen) {
    stringBuffLen = 2*(stringBuffLen + 1);
    stringBuffer = (char*)realloc(stringBuffer, stringBuffLen*sizeof(char));
  }
  stringBuffer[stringLen] = c;
  stringLen++;
  stringBuffer[stringLen] = '\0';
}


void processNewline(void) {
  chplLineno++;
  yylloc.first_column = yylloc.last_column = 0;
  yylloc.first_line = yylloc.last_line = chplLineno;
  countNewline();
}


char* eatStringLiteral(const char* startChar) {
  register int c;
  
  newString();
  while (1) {
    while ((c = getNextYYChar()) != *startChar && c != EOF) {
      if (*startChar == '\'' && c == '\"') {
        addChar('\\');
      }
    FORCE_NEXT:
      addChar(c);
      if (c == '\\') {
        c = getNextYYChar();
        if (c != EOF) {
          goto FORCE_NEXT;
        }
      }
    } /* eat up string */
    if (c == EOF) {
      yyerror("EOF in string");
    }
    break;
  }
  return stringBuffer;
}


void processSingleLineComment(void) {
  register int c;

  newString();
  countCommentLine();
  while (1) {
    while ( (c = getNextYYChar()) != '\n' && c != EOF ) {
      addChar(c);
    }    /* eat up text of comment */
    countSingleLineComment(stringBuffer);
    if (c != EOF) {
      processNewline();
    }
    break;
  }
}


void processMultiLineComment(void) {
  register int c;
          
  newString();
  countCommentLine();
  while (1) {
    while ((c = getNextYYChar()) != '*' && c != EOF ) {
      if (c == '\n') {
        countMultiLineComment(stringBuffer);
        processNewline();
        newString();
        countCommentLine();
      } else {
        addChar(c);
      }
    }    /* eat up text of comment */
    
    if ( c == '*' ) {
      while ( (c = getNextYYChar()) == '*' ) {
        addChar(c);
      }
      if ( c == '/' ) {
        countMultiLineComment(stringBuffer);
        newString();
        break;    /* found the end */
      } else if (c == '\n') {
        countMultiLineComment(stringBuffer);
        processNewline();
        newString();
        countCommentLine();
      }
    } else {      // c == EOF
      yyerror( "EOF in comment" );
      break;
    }
  }
}


void processWhitespace(const char* tabOrSpace) {
  // might eventually want to keep track of column numbers and do
  // something here
}


void processInvalidToken() {
  yyerror("Invalid token");
}
