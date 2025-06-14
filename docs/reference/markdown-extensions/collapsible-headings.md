# Collapsible headings

## Example { open }

    A simple addition to the basic Markdown syntax is the ability to indent content below a heading to make it a collapsible area. The heading will become an interactive collapse toggle and all indented content will be hidden away when the reader clicks it.

    !!! note "Note"

        You can use any Markdown inside these collapsed sections, including other Markdown extensions such as admonitions.

    ### Nested collapsible headings

        You can nest collapsing headings, but it is not recommended as it can become quite hard for the reader to understand the hierarchy of content. Try to stick to one level of nesting. If within a collapsing area you want to de-emphasise the importance of something, or indicate that it is an aside, use admonitions instead.

By default collapsible headings will be closed. But you can override this behaviour using inline attributes.

```markdown
# Collapsing heading { open }
```

## Mutually exclusive collapsing

Sometimes you might want to have a set of collapsing headings where only one can be open at a time. If you give multiple collapsing headings the same name attribute.

### Graduation Requirements { name="requirements" }

    Requires 40 credits, including a passing grade in health, geography, history, economics, and wood shop.

### System Requirements { name="requirements" }

    Requires a computer running an operating system. The computer must have some memory and ideally some kind of long-term storage. An input device as well as some form of output device is recommended.

### Job Requirements { name="requirements" }

    Requires knowledge of HTML, CSS, JavaScript, accessibility, web performance, privacy, security, and internationalization, as well as a dislike of broccoli.
