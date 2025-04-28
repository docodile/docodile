#include "lex.h"

static void DebugInfo(Lexer *lexer, Token *token, bool is_inline) {
  fprintf(stderr, "=== DEBUG START ===\n");
  fprintf(stderr, "%s\n", is_inline ? "INLINE" : "BLOCK");
  switch (token->type) {
    case TOKEN_BOLD:
      fprintf(stderr, "token->type: TOKEN_BOLD\n");
      break;
    case TOKEN_BR:
      fprintf(stderr, "token->type: TOKEN_BR\n");
      break;
    case TOKEN_H1:
      fprintf(stderr, "token->type: TOKEN_H1\n");
      break;
    case TOKEN_H2:
      fprintf(stderr, "token->type: TOKEN_H2\n");
      break;
    case TOKEN_H3:
      fprintf(stderr, "token->type: TOKEN_H3\n");
      break;
    case TOKEN_H4:
      fprintf(stderr, "token->type: TOKEN_H4\n");
      break;
    case TOKEN_H5:
      fprintf(stderr, "token->type: TOKEN_H5\n");
      break;
    case TOKEN_H6:
      fprintf(stderr, "token->type: TOKEN_H6\n");
      break;
    case TOKEN_ITALICS:
      fprintf(stderr, "token->type: TOKEN_ITALICS\n");
      break;
    case TOKEN_LINK:
      fprintf(stderr, "token->type: TOKEN_LINK\n");
      break;
    case TOKEN_NULL:
      fprintf(stderr, "token->type: TOKEN_NULL\n");
      break;
    case TOKEN_P:
      fprintf(stderr, "token->type: TOKEN_P\n");
      break;
    case TOKEN_TEXT:
      fprintf(stderr, "token->type: TOKEN_TEXT\n");
      break;
    case TOKEN_UNKNOWN:
      fprintf(stderr, "token->type: TOKEN_UNKNOWN\n");
      break;
    case TOKEN_ORDEREDLIST:
      fprintf(stderr, "token->type: TOKEN_ORDERDEDLIST\n");
      break;
    case TOKEN_UNORDEREDLIST:
      fprintf(stderr, "token->type: TOKEN_UNORDEREDLIST\n");
      break;
    case TOKEN_LISTITEM:
      fprintf(stderr, "token->type: TOKEN_LISTITEM\n");
      break;
    default:
      fprintf(stderr, "token->type: [error]\n");
      break;
  }
  fprintf(stderr, "token->start: %zu\n", token->start);
  fprintf(stderr, "token->end: %zu\n", token->end);
  fprintf(stderr, "token->length: %zu\n", token->length);
  fprintf(stderr, "lexeme: \033[34m%.*s\033[0m\n", token->length,
          &token->input[token->start]);

  fprintf(stderr, "lexer->pos: %zu\n", lexer->pos);
  fprintf(stderr, "lexer->end: %zu\n", lexer->end);
  fprintf(stderr, "==== DEBUG END ====\n");
  fprintf(stderr, "\n");
}

static Token TokenNew(char *input, size_t pos) {
  return (Token){.input  = input,
                 .type   = TOKEN_UNKNOWN,
                 .length = 0,
                 .start  = pos,
                 .end    = pos,
                 .line   = 0};
}

static Token TokenNull() { return (Token){.input = NULL, .type = TOKEN_NULL}; }

static void Advance(Lexer *lexer) {
  assert(lexer->pos < lexer->end);
  lexer->pos++;
}

static char Peek(Lexer *lexer) {
  if (lexer->pos >= lexer->end) return '\0';
  return lexer->input[lexer->pos];
}

static void NewLine(Lexer *lexer) {
  if (Peek(lexer) == '\n') {
    Advance(lexer);
  }
}

static int Whitespace(Lexer *lexer) {
  int count = 0;
  while (Peek(lexer) == ' ' && ++count) Advance(lexer);
  return count;
}

static int Repeats(Lexer *lexer, char expected) {
  int count = 0;

  while (Peek(lexer) == expected) {
    Advance(lexer);
    count++;
  }

  return count;
}

static size_t ConsumeUntilAny(Lexer *lexer, const char *ends, bool inclusive) {
  char c = 0;
  while ((c = Peek(lexer)) != '\0' && strchr(ends, c) == NULL) {
    Advance(lexer);
  }
  if (inclusive) Advance(lexer);
  return lexer->pos;
}

static size_t ConsumeLine(Lexer *lexer) {
  return ConsumeUntilAny(lexer, "\n", false);
}

static Token LexHeading(Lexer *lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);

  int count = Repeats(lexer, '#');
  switch (count) {
    case 1:
      token.type = TOKEN_H1;
      break;
    case 2:
      token.type = TOKEN_H2;
      break;
    case 3:
      token.type = TOKEN_H3;
      break;
    case 4:
      token.type = TOKEN_H4;
      break;
    case 5:
      token.type = TOKEN_H5;
      break;
    default:
      token.type = TOKEN_H6;
      break;
  }

  Whitespace(lexer);
  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexQuote(Lexer *lexer) {
  int count = Repeats(lexer, '>');
  Whitespace(lexer);
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_QUOTE;
  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);

  NewLine(lexer);
  count = Repeats(lexer, '>');
  char c;
  while (count > 0 && (c = Peek(lexer)) != '\0' && c != '\n') {
    end = ConsumeLine(lexer);
    NewLine(lexer);
    count = Repeats(lexer, '>');
  }

  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexParagraph(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_P;
  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);

  NewLine(lexer);
  char c;
  while ((c = Peek(lexer)) != '\0' && c != '\n') {
    end = ConsumeLine(lexer);
    NewLine(lexer);
  }

  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexOrderedList(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_ORDEREDLIST;
  size_t start = lexer->pos;
  if (isdigit(Peek(lexer))) {
    Advance(lexer);
    if (Peek(lexer) == '.') Advance(lexer);
  }
  size_t end = ConsumeLine(lexer);
  NewLine(lexer);
  Whitespace(lexer);
  char c;
  while (isdigit(c = Peek(lexer)) && c != '\n') {
    end = ConsumeLine(lexer);
    NewLine(lexer);
    Whitespace(lexer);
  }

  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexUnorderedList(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_UNORDEREDLIST;
  size_t start = lexer->pos;
  int spaces   = 0;
  Repeats(lexer, '-');
  size_t end = ConsumeLine(lexer);
  NewLine(lexer);
  Whitespace(lexer);
  char c;
  while ((c = Peek(lexer)) == '-' && c != '\n') {
    end = ConsumeLine(lexer);
    NewLine(lexer);
    spaces = Whitespace(lexer);
  }

  token.start  = start;
  token.end    = end;
  token.length = end - start;

  return token;
}

static Token LexText(Lexer *lexer) {
  // HACK Remove quote.
  if (Repeats(lexer, '>') > 0) Whitespace(lexer);

  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_TEXT;
  size_t start = lexer->pos;
  size_t end   = ConsumeUntilAny(lexer, "[*_\n", false);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexLink(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_LINK;
  size_t start = lexer->pos;
  size_t end   = ConsumeUntilAny(lexer, ")", true);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexListItem(Lexer *lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_TEXT;

  char c    = Peek(lexer);
  int count = 0;
  if (c == '-') {
    Advance(lexer);
    token.type = TOKEN_LISTITEM;
  } else if (isdigit(c)) {
    Advance(lexer);
    count = Repeats(lexer, '.');
    if (count > 0) token.type = TOKEN_LISTITEM;
  }

  Whitespace(lexer);

  size_t start = lexer->pos;
  size_t end   = ConsumeLine(lexer);
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  NewLine(lexer);
  return token;
}

static Token LexBreak(Lexer *lexer) {
  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_BR;
  size_t start = lexer->pos;
  NewLine(lexer);
  size_t end   = lexer->pos;
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

#define MIN(a, b) ((a) > (b) ? (b) : (a))

static Token LexEmphasis(Lexer *lexer) {
  char format            = Peek(lexer);
  int opening            = Repeats(lexer, format);
  size_t start           = lexer->pos;
  const char *format_str = (char[]){format, '\0'};
  size_t end             = ConsumeUntilAny(lexer, format_str, false);
  int closing            = Repeats(lexer, format);
  int matching           = MIN(opening, closing);

  TokenType type;
  if (matching == 0) type = TOKEN_TEXT;
  if (matching == 1) type = TOKEN_ITALICS;
  if (matching >= 2) type = TOKEN_BOLD;

  Token token  = TokenNew(lexer->input, start);
  token.type   = type;
  token.end    = end;
  token.length = end - start;

  return token;
}

Lexer LexerNew(char *input, size_t start, size_t end) {
  return (Lexer){.input = input, .pos = start, .end = end, .current_line = 1};
}

Token NextToken(Lexer *lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  Token token;

  if (isdigit(c)) {
    token = LexOrderedList(lexer);
  } else {
    switch (c) {
      case '-':
        token = LexUnorderedList(lexer);
        break;
      case '#':
        token = LexHeading(lexer);
        break;
      case '>':
        token = LexQuote(lexer);
        break;
      default:
        token = LexParagraph(lexer);
        break;
    }
  }

  Whitespace(lexer);
  NewLine(lexer);

  DebugInfo(lexer, &token, false);

  return token;
}

Token NextInlineToken(Lexer *lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  Token token;

  if (isdigit(c) || c == '-') {
    token = LexListItem(lexer);
  } else {
    switch (c) {
      case '[':
        token = LexLink(lexer);
        break;
      case '*':
      case '_':
        token = LexEmphasis(lexer);
        break;
      case '\n':
        token = LexBreak(lexer);
        break;
      default:
        token = LexText(lexer);
        break;
    }
  }

  DebugInfo(lexer, &token, true);

  return token;
}

void TokenPrint(Token *token) {
  for (size_t i = token->start; i < token->end; i++) {
    putchar(token->input[i]);
  }
}