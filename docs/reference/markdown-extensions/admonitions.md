# Admonitions

Admonitions are useful when you want to draw attention to a short snippet of content, or if you want to indicate that it's complimentary, but not necessarily essential, to the main content. For example you might use a warning admonition to alert the reader of something important. In other instances you might use a note admonition to give tangential context to a piece of content.

## Supported types

### Note

!!! note "Note"

    Notes can add context or peripheral information without breaking the flow of the main content.

### Success

!!! success "Success"

    A success admonition can be used to indicate that something good has happened.

### Question

!!! question "Question"

    Question admonitions can be used to engage the reader by posing a question to them. It can also be used to convey an unknown.

### Warning

!!! warning "Warning"

    Warnings should be used when the reader needs to pay careful attention to the content.

### Critical

!!! critical "Critical"

    A critical admonition is the most severe tone and should be reserved for very important messages.

## Custom types

Adding custom admonitions is super easy. Pick a name for the type and start using it. `Docodile` will automatically assign whatever you call it as a class on the admonition. You just need to edit `main.css` in your styles folder to change the color and the icon:

```css
.admonition {
  &.custom {
    --color: var(--gd-color-very-peri);
    --icon: "\F328";
  }
}
```

To find the code point for the icon just go to [Bootstrap Icons](https://icons.getbootstrap.com/).

!!! custom "Custom"

    This is a custom admonition.
