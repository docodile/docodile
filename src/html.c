#include "html.h"

static void RenderHeading(int heading, Token *token) {
  assert(heading > 0);
  assert(heading <= 6);

  printf("<h%d>", heading);
  TokenPrint(token);
  printf("</h%d>", heading);
}

static void RenderParagraph(Lexer *lexer, Token *token) {
  printf("<p>");
  RenderInlineHtml(lexer, token);
  printf("</p>");
}

static void RenderQuote(Lexer *lexer, Token *token) {
  printf("<blockquote>");
  Lexer inline_lexer = LexerNew(lexer->input, token->start, token->end);
  RenderHtml(&inline_lexer);
  printf("</blockquote>");
}

static void RenderOrderedList(Lexer *lexer, Token *token) {
  printf("<ol>\n");
  RenderInlineHtml(lexer, token);
  printf("\n</ol>");
}

static void RenderUnorderedList(Lexer *lexer, Token *token) {
  printf("<ul>\n");
  RenderInlineHtml(lexer, token);
  printf("\n</ul>");
}

static void RenderEmphasis(Lexer *lexer, Token *token) {
  printf("<em>");
  RenderInlineHtml(lexer, token);
  printf("</em>");
}

static void RenderStrong(Lexer *lexer, Token *token) {
  printf("<strong>");
  RenderInlineHtml(lexer, token);
  printf("</strong>");
}

static void RenderBreak(Lexer *lexer, Token *token) { printf("<br>"); }

static void RenderListItem(Lexer *lexer, Token *token) {
  printf("<li>");
  RenderInlineHtml(lexer, token);
  printf("</li>");
}

// HACK This is a dirty sscanf implementation.
static void ParseLink(Token *token) {
  char buffer[2500];
  size_t c = 0;
  // TODO TokenPrint function to print to string?
  for (size_t i = token->start; i < token->end; i++) {
    buffer[c++] = token->input[i];
  }

  buffer[c] = '\0';

  char href[2500];
  char label[2500];
  sscanf(buffer, "[%2500[^]]](%2500[^)])", label, href);

  printf("<a href=\"%s\">%s</a>", href, label);
}

static void RenderInlineToken(Lexer *lexer, Token *token) {
  switch (token->type) {
    case TOKEN_LINK:
      ParseLink(token);
      break;
    case TOKEN_ITALICS:
      RenderEmphasis(lexer, token);
      break;
    case TOKEN_BOLD:
      RenderStrong(lexer, token);
      break;
    case TOKEN_BR:
      RenderBreak(lexer, token);
      break;
    case TOKEN_LISTITEM:
      RenderListItem(lexer, token);
      break;
    case TOKEN_UNORDEREDLIST:
      RenderUnorderedList(lexer, token);
      break;
    case TOKEN_ORDEREDLIST:
      RenderOrderedList(lexer, token);
      break;
    default:
      TokenPrint(token);
      break;
  }
}

void RenderHtml(Lexer *lexer) {
  Token token = NextToken(lexer);

  while (token.type != TOKEN_NULL) {
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
      case TOKEN_QUOTE:
        RenderQuote(lexer, &token);
        break;
      case TOKEN_ORDEREDLIST:
        RenderOrderedList(lexer, &token);
        break;
      case TOKEN_UNORDEREDLIST:
        RenderUnorderedList(lexer, &token);
        break;
      default:
        RenderParagraph(lexer, &token);
        break;
    }

    printf("\n");

    token = NextToken(lexer);
  }
}

void RenderInlineHtml(Lexer *lexer, Token *token) {
  Lexer inline_lexer = LexerNew(lexer->input, token->start, token->end);
  Token inline_token = NextInlineToken(&inline_lexer);

  while (inline_token.type != TOKEN_NULL) {
    RenderInlineToken(&inline_lexer, &inline_token);
    inline_token = NextInlineToken(&inline_lexer);
  }
}