#include "../lex.h"

static Token LexParagraph(Lexer* lexer);

static void Advance(Lexer* lexer) {
  assert(lexer->pos < lexer->end);
  lexer->pos++;
}

static char Peek(Lexer* lexer) {
  if (lexer->pos >= lexer->end) return '\0';
  return lexer->input[lexer->pos];
}

static void NewLine(Lexer* lexer) {
  if (Peek(lexer) == '\n') {
    Advance(lexer);
  }
}

static int Whitespace(Lexer* lexer) {
  int count = 0;
  while (Peek(lexer) == ' ') {
    Advance(lexer);
    count++;
  }
  return count;
}

static int Repeats(Lexer* lexer, char expected) {
  int count = 0;

  while (Peek(lexer) == expected) {
    Advance(lexer);
    count++;
  }

  return count;
}

static size_t ConsumeUntilAny(Lexer* lexer, const char* ends, bool inclusive) {
  char c = 0;
  while ((c = Peek(lexer)) != '\0' && strchr(ends, c) == NULL) {
    Advance(lexer);
  }
  if (inclusive) Advance(lexer);
  return lexer->pos;
}

static size_t ConsumeLine(Lexer* lexer) {
  return ConsumeUntilAny(lexer, "\n", false);
}

static Token LexHeading(Lexer* lexer, bool collapsing) {
  Token token = TokenNew(lexer->input, lexer->pos);

  int count;
  if (collapsing) count = Repeats(lexer, '?');
  if (!collapsing) count = Repeats(lexer, '#');
  switch (count) {
    case 1:
      token.type = collapsing ? TOKEN_H1_COLLAPSING : TOKEN_H1;
      break;
    case 2:
      token.type = collapsing ? TOKEN_H2_COLLAPSING : TOKEN_H2;
      break;
    case 3:
      token.type = collapsing ? TOKEN_H3_COLLAPSING : TOKEN_H3;
      break;
    case 4:
      token.type = collapsing ? TOKEN_H4_COLLAPSING : TOKEN_H4;
      break;
    case 5:
      token.type = collapsing ? TOKEN_H5_COLLAPSING : TOKEN_H5;
      break;
    default:
      token.type = collapsing ? TOKEN_H6_COLLAPSING : TOKEN_H6;
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

static Token LexCodeBlock(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  Repeats(lexer, '`');
  token.type  = TOKEN_CODEBLOCK;
  token.start = lexer->pos;

  char c = Peek(lexer);
  while (c != '`' && c != '\0') {
    token.end = ConsumeLine(lexer);
    NewLine(lexer);
    c = Peek(lexer);
  }

  Repeats(lexer, '`');  // TODO Check balanced backticks?

  token.length = token.end - token.start;
  NewLine(lexer);
  return token;
}

static Token LexCodeInline(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_CODEBLOCKINLINE;
  Repeats(lexer, '`');
  token.start = lexer->pos;
  token.end   = ConsumeUntilAny(lexer, "`", false);
  Repeats(lexer, '`');
  token.length = token.end - token.start;
  return token;
}

static Token LexCode(Lexer* lexer) {
  size_t saved_pos = lexer->pos;
  int count        = Repeats(lexer, '`');
  lexer->pos       = saved_pos;

  if (count >= 3) return LexCodeBlock(lexer);

  return LexParagraph(lexer);
}

static Token LexQuote(Lexer* lexer) {
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

static Token LexAdmonition(Lexer* lexer) {
  size_t pos  = lexer->pos;
  Token token = TokenNew(lexer->input, pos);
  token.type  = TOKEN_ADMONITION;
  int count   = Repeats(lexer, '!');
  if (count != 3) {
    token.type = TOKEN_P;
    return token;
  }

  token.start = lexer->pos;
  token.end   = ConsumeLine(lexer);
  NewLine(lexer);
  return token;
}

static Token LexTableRow(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_TABLEROW;
  size_t pos  = lexer->pos;
  Advance(lexer);
  Whitespace(lexer);
  char c = Peek(lexer);
  if (c == '-') {
    token.type = TOKEN_TABLESEPARATOR;
  }
  lexer->pos = pos;
  token.end  = ConsumeLine(lexer);
  NewLine(lexer);
  token.end = lexer->pos;
  return token;
}

static Token LexParagraph(Lexer* lexer) {
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

static Token LexEmptyLine(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_EMPTYLINE;

  NewLine(lexer);

  token.end    = lexer->pos;
  token.length = token.end - token.start;
  return token;
}

static Token LexText(Lexer* lexer) {
  // HACK Remove quote.
  if (Repeats(lexer, '>') > 0) Whitespace(lexer);

  Token token  = TokenNew(lexer->input, lexer->pos);
  token.type   = TOKEN_TEXT;
  token.start  = lexer->pos;
  token.end    = ConsumeUntilAny(lexer, "[*_`{\n", false);
  token.length = token.end - token.start;
  return token;
}

static Token LexLinkLabel(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_LINKLABEL;
  Repeats(lexer, '[');
  size_t start = lexer->pos;
  size_t end   = ConsumeUntilAny(lexer, "]", false);
  Repeats(lexer, ']');
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexLinkHref(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_LINKHREF;
  Repeats(lexer, '(');
  size_t start = lexer->pos;
  size_t end   = ConsumeUntilAny(lexer, ")", false);
  Repeats(lexer, ')');
  token.start  = start;
  token.end    = end;
  token.length = end - start;
  return token;
}

static Token LexListItem(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_TEXT;

  char c    = Peek(lexer);
  int count = 0;
  if (c == '-') {
    Advance(lexer);
    token.type = TOKEN_LISTITEMUNORDERED;
  } else if (isdigit(c)) {
    Advance(lexer);
    count = Repeats(lexer, '.');
    if (count > 0) token.type = TOKEN_LISTITEMORDERED;
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

static Token LexBreak(Lexer* lexer) {
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

static Token LexTableCell(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_TABLECELL;
  Advance(lexer);
  Whitespace(lexer);
  token.start = lexer->pos;
  token.end   = ConsumeUntilAny(lexer, "|\n", false);
  if (Peek(lexer) == '\n') {
    token.type = TOKEN_UNKNOWN;
    NewLine(lexer);
  }
  size_t len   = token.end - token.start;
  token.length = len;
  return token;
}

static Token LexAttributes(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_ATTRIBUTES;
  Advance(lexer);
  Whitespace(lexer);
  token.start = lexer->pos;
  token.end   = ConsumeUntilAny(lexer, "}\n", false);
  if (Peek(lexer) == '}') Advance(lexer);
  NewLine(lexer);
  token.length = token.end - token.start;
  return token;
}

#define MIN(a, b) ((a) > (b) ? (b) : (a))

static Token LexEmphasis(Lexer* lexer) {
  char format            = Peek(lexer);
  int opening            = Repeats(lexer, format);
  size_t start           = lexer->pos;
  const char* format_str = (char[]){format, '\0'};
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

static Token LexHtml(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_HTML;
  token.end   = ConsumeLine(lexer);
  NewLine(lexer);
  token.length = token.end - token.start;
  return token;
}

static Token LexHorizontalRule(Lexer* lexer) {
  Token token = TokenNew(lexer->input, lexer->pos);
  token.type  = TOKEN_HR;
  token.end   = ConsumeLine(lexer);
  NewLine(lexer);
  return token;
}

Lexer LexerNew(char* input, size_t start, size_t end) {
  return (Lexer){.input = input, .pos = start, .end = end, .current_line = 1};
}

Token PeekToken(Lexer* lexer) {
  size_t saved_pos = lexer->pos;
  Token token      = NextToken(lexer);
  lexer->pos       = saved_pos;
  return token;
}

Token NextToken(Lexer* lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  Token token;

  int spaces       = Whitespace(lexer);
  int indent_level = spaces / 2;  // TODO Handle tabs.
  c                = Peek(lexer);
  if (isdigit(c) || c == '-') {
    size_t temp = lexer->pos;
    if (Repeats(lexer, '-') > 2) {
      token = LexHorizontalRule(lexer);
    } else {
      lexer->pos = temp;
      token      = LexListItem(lexer);
    }
  } else {
    switch (c) {
      case '`':
        token = LexCode(lexer);
        break;
      case '\n':
        token = LexEmptyLine(lexer);
        break;
      case '#':
        token = LexHeading(lexer, false);
        break;
      case '?':
        token = LexHeading(lexer, true);
        break;
      case '>':
        token = LexQuote(lexer);
        break;
      case '<':
        token = LexHtml(lexer);
        break;
      case '!':
        token = LexAdmonition(lexer);
        break;
      case '|':
        token = LexTableRow(lexer);
        break;
      default:
        token = LexParagraph(lexer);
        break;
    }
  }

  token.indent_level = indent_level;

  return token;
}

Token NextInlineToken(Lexer* lexer) {
  char c = Peek(lexer);
  if (c == '\0') return TokenNull();

  Token token;
  switch (c) {
    case '`':
      token = LexCodeInline(lexer);
      break;
    case '[':
      token = LexLinkLabel(lexer);
      break;
    case '(':
      token = LexLinkHref(lexer);
      break;
    case '*':
    case '_':
      token = LexEmphasis(lexer);
      break;
    case '\n':
      token = LexBreak(lexer);
      break;
    case '|':
      token = LexTableCell(lexer);
      break;
    case '{':
      token = LexAttributes(lexer);
      break;
    default:
      token = LexText(lexer);
      break;
  }

  return token;
}
