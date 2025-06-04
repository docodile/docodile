# Customisation

## Appearance

In `gendoc.config` you have a few ways to quickly change the appearance of your docs site.

```ini
[theme]
font-family=IBM Plex Sans
color-scheme=dark
accent-color=violet
```

### Color schemes

`gendoc` currently offers two main color schemes, `dark` and `light`.

<div style="display: flex; gap: 10px; flex-wrap: wrap;">
    <button class="scheme-button" data-gd-color-scheme="dark" onclick="changeScheme('dark')">dark</button>
    <button class="scheme-button" data-gd-color-scheme="light" onclick="changeScheme('light')">light</button>
    <style>.scheme-button { border: none; color: var(--gd-fg-primary); font-weight: bold; padding: 0.25rem 1rem; background: var(--gd-bg-primary); border-radius: 999px; font-size: 0.875rem; }</style>
    <script>function changeScheme(color) { document.querySelector('body').setAttribute('data-gd-color-scheme', color); }</script>
</div>

### Accent color

You can also change the accent color of the site. The options currently available are:

<div style="display: flex; gap: 10px; flex-wrap: wrap;">
    <button class="color-button" data-gd-accent-color="red" onclick="changeColor('red')">red</button>
    <button class="color-button" data-gd-accent-color="fuchsia rose" onclick="changeColor('fuchsia rose')">fuchsia rose</button>
    <button class="color-button" data-gd-accent-color="marsala" onclick="changeColor('marsala')">marsala</button>
    <button class="color-button" data-gd-accent-color="honeysuckle" onclick="changeColor('honeysuckle')">honeysuckle</button>
    <button class="color-button" data-gd-accent-color="chili pepper" onclick="changeColor('chili pepper')">chili pepper</button>
    <button class="color-button" data-gd-accent-color="true red" onclick="changeColor('true red')">true red</button>
    <button class="color-button" data-gd-accent-color="viva magenta" onclick="changeColor('viva magenta')">viva magenta</button>
    <button class="color-button" data-gd-accent-color="mocha mousse" onclick="changeColor('mocha mousse')">mocha mousse</button>
    <button class="color-button" data-gd-accent-color="sand dollar" onclick="changeColor('sand dollar')">sand dollar</button>
    <button class="color-button" data-gd-accent-color="rose quartz" onclick="changeColor('rose quartz')">rose quartz</button>
    <button class="color-button" data-gd-accent-color="living coral" onclick="changeColor('living coral')">living coral</button>
    <button class="color-button" data-gd-accent-color="orange" onclick="changeColor('orange')">orange</button>
    <button class="color-button" data-gd-accent-color="tangerine tango" onclick="changeColor('tangerine tango')">tangerine tango</button>
    <button class="color-button" data-gd-accent-color="tigerlily" onclick="changeColor('tigerlily')">tigerlily</button>
    <button class="color-button" data-gd-accent-color="peach fuzz" onclick="changeColor('peach fuzz')">peach fuzz</button>
    <button class="color-button" data-gd-accent-color="mimosa" onclick="changeColor('mimosa')">mimosa</button>
    <button class="color-button" data-gd-accent-color="yellow" onclick="changeColor('yellow')">yellow</button>
    <button class="color-button" data-gd-accent-color="illuminating" onclick="changeColor('illuminating')">illuminating</button>
    <button class="color-button" data-gd-accent-color="greenery" onclick="changeColor('greenery')">greenery</button>
    <button class="color-button" data-gd-accent-color="green" onclick="changeColor('green')">green</button>
    <button class="color-button" data-gd-accent-color="emerald" onclick="changeColor('emerald')">emerald</button>
    <button class="color-button" data-gd-accent-color="turquoise" onclick="changeColor('turquoise')">turquoise</button>
    <button class="color-button" data-gd-accent-color="blue turquoise" onclick="changeColor('blue turquoise')">blue turquoise</button>
    <button class="color-button" data-gd-accent-color="aqua sky" onclick="changeColor('aqua sky')">aqua sky</button>
    <button class="color-button" data-gd-accent-color="blue" onclick="changeColor('blue')">blue</button>
    <button class="color-button" data-gd-accent-color="serenity" onclick="changeColor('serenity')">serenity</button>
    <button class="color-button" data-gd-accent-color="cerulean" onclick="changeColor('cerulean')">cerulean</button>
    <button class="color-button" data-gd-accent-color="classic blue" onclick="changeColor('classic blue')">classic blue</button>
    <button class="color-button" data-gd-accent-color="blue iris" onclick="changeColor('blue iris')">blue iris</button>
    <button class="color-button" data-gd-accent-color="very peri" onclick="changeColor('very peri')">very peri</button>
    <button class="color-button" data-gd-accent-color="indigo" onclick="changeColor('indigo')">indigo</button>
    <button class="color-button" data-gd-accent-color="ultra violet" onclick="changeColor('ultra violet')">ultra violet</button>
    <button class="color-button" data-gd-accent-color="violet" onclick="changeColor('violet')">violet</button>
    <button class="color-button" data-gd-accent-color="radiant orchid" onclick="changeColor('radiant orchid')">radiant orchid</button>
    <button class="color-button" data-gd-accent-color="ultimate gray" onclick="changeColor('ultimate gray')">ultimate gray</button>
    <button class="color-button" data-gd-accent-color="black" onclick="changeColor('black')">black</button>
    <button class="color-button" data-gd-accent-color="white" onclick="changeColor('white')">white</button>
    <style>.color-button { border: none; color: var(--gd-header-fg-color); font-weight: bold; padding: 0.25rem 1rem; background: var(--gd-accent-color); border-radius: 999px; font-size: 0.875rem; }</style>
    <script>function changeColor(color) { document.querySelector('body').setAttribute('data-gd-accent-color', color); }</script>
</div>
