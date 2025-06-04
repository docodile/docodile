# Customisation

## Appearance

In `gendoc.config` you have a few ways to quickly change the appearance of your docs site.

```ini
font-family=IBM Plex Sans

[theme]
color-scheme=dark
accent-color=violet
```

### Color schemes

`gendoc` currently offers two main color schemes, `dark` and `light`.

### Accent color

You can also change the accent color of the site. The options currently available are:

<div style="display: flex; gap: 10px;">
    <button class="color-button" style="background-color: var(--gd-red)" onclick="changeColor('red')">red</button>
    <button class="color-button" style="background-color: var(--gd-orange)" onclick="changeColor('orange')">orange</button>
    <button class="color-button" style="background-color: var(--gd-yellow)" onclick="changeColor('yellow')">yellow</button>
    <button class="color-button" style="background-color: var(--gd-green)" onclick="changeColor('green')">green</button>
    <button class="color-button" style="background-color: var(--gd-blue)" onclick="changeColor('blue')">blue</button>
    <button class="color-button" style="background-color: var(--gd-indigo)" onclick="changeColor('indigo')">indigo</button>
    <button class="color-button" style="background-color: var(--gd-violet)" onclick="changeColor('violet')">violet</button>
    <style>.color-button { border: none; color: black; font-weight: bold; padding: 0 1rem; }</style>
    <script>function changeColor(color) { document.querySelector('body').setAttribute('data-gd-accent-color', color); }</script>
</div>
