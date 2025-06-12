# Markdown extensions

## Admonitions

!!! note "Note"

    This is a note admonition

!!! success "Success"

    This is a success admonition

!!! question "Question"

    This is a question admonition

!!! warning "Warning"

    This is a warning admonition

!!! critical "Critical"

    This is a critical admonition

## Tables

| Syntax    | Description |
| --------- | ----------- |
| Header    | Title       |
| Paragraph | Text        |

```markdown
| Syntax    | Description |
| --------- | ----------- |
| Header    | Title       |
| Paragraph | Text        |
```

## Collapsible headings

    A simple addition to the basic Markdown syntax is the ability to indent content below a heading to make it a collapsible area. The heading will become an interactive collapse toggle and all indented content will be hidden away when the reader clicks it.

    !!! note "Note"

        You can use any Markdown inside these collapsed sections, including other Markdown extensions such as admonitions.

    ### Nested collapsible headings

        You can nest collapsing headings, but it is not recommended as it can become quite hard for the reader to understand the hierarchy of content. Try to stick to one level of nesting. If within a collapsing area you want to de-emphasise the importance of something, or indicate that it is an aside, use admonitions instead.

    By default collapsible will be open.
