#ifndef HTML_H
#define HTML_H

#include <assert.h>

#include "lex.h"
#include "logger.h"

void RenderHtml(Lexer *lexer);
void RenderInlineHtml(Lexer *lexer, Token *token);

#endif  // HTML_H