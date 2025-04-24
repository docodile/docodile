#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include <assert.h>

#include "lex.h"
#include "logger.h"

void RenderInlineHtml(TokenStream *stream);

void RenderHeading(int heading, Token *token) {
  assert(heading > 0);
  assert(heading <= 6);

  printf("<h%d>", heading);
  TokenPrint(token);
  printf("</h%d>", heading);
}

void RenderParagraph(Token *token) {
  printf("<p>");
  TokenStream *stream = LexInline(token);
  RenderInlineHtml(stream);
  printf("</p>");
}

void RenderEmphasis(Token *token) {
  printf("<em>");
  TokenStream *stream = LexInline(token);
  RenderInlineHtml(stream);
  printf("</em>");
}

void RenderStrong(Token *token) {
  printf("<strong>");
  TokenStream *stream = LexInline(token);
  RenderInlineHtml(stream);
  printf("</strong>");
}

void RenderHtml(TokenStream *stream) {
  for (size_t i = 0; i < stream->count; i++) {
    Token token = TokenStreamNext(stream);

    switch (token.type) {
      case TOKEN_H1:
        RenderHeading(1, &token);
        break;
      case TOKEN_H2:
        RenderHeading(2, &token);
        break;
      case TOKEN_H3:
        RenderHeading(3, &token);
        break;
      case TOKEN_H4:
        RenderHeading(4, &token);
        break;
      case TOKEN_H5:
        RenderHeading(5, &token);
        break;
      case TOKEN_H6:
        RenderHeading(6, &token);
        break;
      default:
        RenderParagraph(&token);
        break;
    }

    printf("\n");
  }
}

// HACK This is a dirty sscanf implementation.
void ParseLink(Token token) {
  char buffer[2500];
  size_t c = 0;
  // TODO TokenPrint function to print to string?
  for (size_t i = token.start; i < token.end; i++) {
    buffer[c++] = token.input[i];
  }

  buffer[c] = '\0';

  char href[2500];
  char label[2500];
  sscanf(buffer, "[%2500[^]]](%2500[^)])", label, href);

  printf("<a href=\"%s\">%s</a>", href, label);
}

void RenderInlineToken(Token token) {
  switch (token.type) {
    case TOKEN_LINK:
      ParseLink(token);
      break;
    case TOKEN_ITALICS:
      RenderEmphasis(&token);
      break;
    case TOKEN_BOLD:
      RenderStrong(&token);
      break;
    default:
      TokenPrint(&token);
  }
}

void RenderInlineHtml(TokenStream *stream) {
  for (size_t i = 0; i < stream->count; i++) {
    Token token = TokenStreamNext(stream);

    RenderInlineToken(token);
  }
}

#endif  // HTMLGENERATOR_H