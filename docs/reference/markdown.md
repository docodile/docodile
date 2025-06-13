# Markdown

`gendoc` supports all basic Markdown syntax.

## Headings

```markdown
# Heading level 1

## Heading level 2

### Heading level 3

#### Heading level 4

##### Heading level 5

###### Heading level 6
```

---

## Paragraphs

This is a paragraph.

This is a separate paragraph.

---

## Line breaks

This is a line
break within a paragraph.

---

## Emphasis

You can use _italics_ and **bold** to emphasise parts of text. You can combine **_bold and italics_**.

```markdown
You can use _italics_ and **bold** to emphasise parts of text. You can combine **_bold and italics_**.
```

---

## Blockquotes

> This is a block quote

```markdown
> This is a block quote.
```

---

## Lists

### Ordered Lists

1. First item
2. Second item
3. Third item
4. Fourth item

```markdown
1. First item
2. Second item
3. Third item
4. Fourth item
```

1. First item
2. Second item
3. Third item
   1. Indented item
   2. Indented item
4. Fourth item

```markdown
1. First item
2. Second item
3. Third item
   1. Indented item
   2. Indented item
4. Fourth item
```

### Unordered lists

- First item
- Second item
- Third item
- Fourth item

```markdown
- First item
- Second item
- Third item
- Fourth item
```

- First item
- Second item
- Third item
  - Indented item
  - Indented item
- Fourth item

```markdown
- First item
- Second item
- Third item
  - Indented item
  - Indented item
- Fourth item
```

---

## Code

### Inline code

You can inline `code` snippets.

```markdown
You can inline `code` snippets.
```

### Code blocks

```c
#include <stdio.h>

int main(int argc, char **argv) {
    printf("Hello, World!\n");
}
```

---

## Horizontal rules

```markdown
---
```

---

## Links

My favourite search engine is [Duck Duck Go](https://duckduckgo.com "yeah").
