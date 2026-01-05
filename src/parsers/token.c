#include "token.h"

const char* TokenName(Token* token) {
  switch (token->type) {
    case TOKEN_BOLD:
      return "BOLD";
    case TOKEN_BR:
      return "BREAK";
    case TOKEN_EMPTYLINE:
      return "EMPTYLINE";
    case TOKEN_H1:
      return "H1";
    case TOKEN_H2:
      return "H2";
    case TOKEN_H3:
      return "H3";
    case TOKEN_H4:
      return "H4";
    case TOKEN_H5:
      return "H5";
    case TOKEN_H6:
      return "H6";
    case TOKEN_ITALICS:
      return "ITALICS";
    case TOKEN_LINK:
      return "LINK";
    case TOKEN_LINKHREF:
      return "LINKHREF";
    case TOKEN_LINKLABEL:
      return "LINKLABEL";
    case TOKEN_LISTITEMORDERED:
      return "OL";
    case TOKEN_LISTITEMUNORDERED:
      return "UL";
    case TOKEN_P:
      return "PARAGRAPH";
    case TOKEN_QUOTE:
      return "QUOTE";
    case TOKEN_TEXT:
      return "TEXT";
    case TOKEN_CODEBLOCK:
    case TOKEN_CODEBLOCKINLINE:
      return "CODE";
    case TOKEN_UNKNOWN:
    default:
      return "UNKNOWN";
  }
}

Token TokenNew(char* input, size_t pos) {
  return (Token){.input        = input,
                 .type         = TOKEN_UNKNOWN,
                 .length       = 0,
                 .start        = pos,
                 .end          = pos,
                 .line         = 0,
                 .indent_level = 0};
}

Token TokenNull() { return (Token){.input = NULL, .type = TOKEN_NULL}; }

void TokenPrint(Token* token) {
  for (size_t i = token->start; i < token->end; i++) {
    putchar(token->input[i]);
  }
}
