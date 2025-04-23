#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include "lex.h"
#include "logger.h"

void RenderInlineHtml(TokenStream *stream);

void RenderHtml(TokenStream *stream) {
  for (size_t i = 0; i < stream->count; i++) {
    Token token = TokenStreamNext(stream);

    switch (token.type) {
      case TOKEN_H1:
        printf("<h1>%.*s</h1>\n", (int)token.length, token.lexeme);
        break;
      case TOKEN_H2:
        printf("<h2>%.*s</h2>\n", (int)token.length, token.lexeme);
        break;
      case TOKEN_H3:
        printf("<h3>%.*s</h3>\n", (int)token.length, token.lexeme);
        break;
      case TOKEN_H4:
        printf("<h4>%.*s</h4>\n", (int)token.length, token.lexeme);
        break;
      case TOKEN_H5:
        printf("<h5>%.*s</h5>\n", (int)token.length, token.lexeme);
        break;
      case TOKEN_H6:
        printf("<h6>%.*s</h6>\n", (int)token.length, token.lexeme);
        break;
      default:
        TokenStream *stream = LexInline(token);
        printf("<p>");
        RenderInlineHtml(stream);
        printf("</p>\n");
        break;
    }
  }
}

// HACK This is a dirty sscanf implementation.
void ParseLink(Token token) {
  char href[2500];
  char label[2500];

  sscanf(token.lexeme, "[%2500[^]]](%2500[^)])", label, href);

  printf("<a href=\"%s\">%s</a>", href, label);
}

void RenderInlineToken(Token token) {
  switch (token.type) {
    case TOKEN_LINK:
      ParseLink(token);
      break;
    case TOKEN_ITALICS: {
      printf("<em>");
      TokenStream *stream = LexInline(token);
      RenderInlineHtml(stream);
      printf("</em>");
    } break;
    case TOKEN_BOLD: {
      printf("<strong>");
      TokenStream *stream = LexInline(token);
      RenderInlineHtml(stream);
      printf("</strong>");
    } break;
    default:
      printf("%.*s", token.length, token.lexeme);
  }
}

void RenderInlineHtml(TokenStream *stream) {
  for (size_t i = 0; i < stream->count; i++) {
    Token token = TokenStreamNext(stream);

    RenderInlineToken(token);
  }
}

#endif  // HTMLGENERATOR_H