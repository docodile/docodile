# Organisation

The structure of your `docs` folder dictates the structure of your site. Each top-level directory will appear on the header as a main section of your site. These sections should split your content at a very coarse-grain level. Further levels of nesting will appear in the sidebar navigation as drop downs. 

## Ordering

By default `Docodile` will order navigation alphabetically but with `index.md` files sorted to the top. Sometimes alphabetical isn't the appropriate sorting method as your content might have a strict order relationship. In these cases you can add a `_nav.md` file to the directory you want to sort. These hidden nav files just use a regular Markdown list to control the ordering of your  

```markdown
- getting-started
- setup
- reference
```