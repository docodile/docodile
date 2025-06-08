# Customisation

## Appearance

In `gendoc.config` you have a few ways to quickly change the appearance of your docs site.

```ini
[theme]
font-family=IBM Plex Sans
color-scheme=light
primary-color=emerald
decorations=glass
```

### Color schemes

`gendoc` currently offers two main color schemes, `dark` and `light`.

<div style="display: flex; gap: 10px; flex-wrap: wrap; margin-top: 1rem;">
    <button class="scheme-button" data-gd-color-scheme="dark" onclick="changeScheme('dark')">dark</button>
    <button class="scheme-button" data-gd-color-scheme="light" onclick="changeScheme('light')">light</button>
    <style>.scheme-button { border: none; color: var(--gd-fg-primary); font-weight: bold; padding: 0.25rem 1rem; background: var(--gd-bg-primary); border-radius: 999px; font-size: 0.875rem; }</style>
    <script>function changeScheme(color) { document.querySelector('body').setAttribute('data-gd-color-scheme', color); }</script>
</div>

### Decorations

Decorations are toggles that can change the appearance of your site in interesting ways. Decorations can usually be combined.

<div style="display: flex; gap: 10px; flex-wrap: wrap; margin-top: 1rem;">
    <button class="decoration-button" onclick="toggleDecoration('glass')">glass</button>
    <button class="decoration-button" onclick="toggleDecoration('blend')">blend</button>
    <style>
        .decoration-button {
            border: none;
            color: var(--gd-header-fg-color);
            font-weight: bold;
            padding: 0.25rem 1rem;
            background: var(--gd-header-bg-color);
            border-radius: 999px;
            font-size: 0.875rem;
        }
    </style>
    <script>
        function toggleDecoration(decoration) {
            const body = document.querySelector("body");
            let decorations = body.getAttribute("data-gd-decorations");
            decorations = decorations.includes(decoration)
                ? decorations.replace(decoration, "")
                : decorations.concat(" " + decoration);
            body.setAttribute("data-gd-decorations", decorations.trim());
        }
    </script>
</div>

#### Glass

The `glass` decoration makes the header translucent and applies a backdrop blur to give it a glass-like appearance.

#### Blend

The `blend` decoration gives the header a gradient between the primary and secondary accent colors.

### Accent colors

You can also change the primary and secondary accent colors of the site. The options currently available are:

#### Primary

<div style="display: flex; gap: 10px; flex-wrap: wrap; margin-top: 1rem;">
    <button class="color-button" data-gd-primary-color="red" onclick="changeColor('red')">red</button>
    <button class="color-button" data-gd-primary-color="fuchsia rose" onclick="changeColor('fuchsia rose')">fuchsia rose</button>
    <button class="color-button" data-gd-primary-color="marsala" onclick="changeColor('marsala')">marsala</button>
    <button class="color-button" data-gd-primary-color="honeysuckle" onclick="changeColor('honeysuckle')">honeysuckle</button>
    <button class="color-button" data-gd-primary-color="chili pepper" onclick="changeColor('chili pepper')">chili pepper</button>
    <button class="color-button" data-gd-primary-color="true red" onclick="changeColor('true red')">true red</button>
    <button class="color-button" data-gd-primary-color="viva magenta" onclick="changeColor('viva magenta')">viva magenta</button>
    <button class="color-button" data-gd-primary-color="mocha mousse" onclick="changeColor('mocha mousse')">mocha mousse</button>
    <button class="color-button" data-gd-primary-color="sand dollar" onclick="changeColor('sand dollar')">sand dollar</button>
    <button class="color-button" data-gd-primary-color="rose quartz" onclick="changeColor('rose quartz')">rose quartz</button>
    <button class="color-button" data-gd-primary-color="living coral" onclick="changeColor('living coral')">living coral</button>
    <button class="color-button" data-gd-primary-color="orange" onclick="changeColor('orange')">orange</button>
    <button class="color-button" data-gd-primary-color="tangerine tango" onclick="changeColor('tangerine tango')">tangerine tango</button>
    <button class="color-button" data-gd-primary-color="tigerlily" onclick="changeColor('tigerlily')">tigerlily</button>
    <button class="color-button" data-gd-primary-color="peach fuzz" onclick="changeColor('peach fuzz')">peach fuzz</button>
    <button class="color-button" data-gd-primary-color="mimosa" onclick="changeColor('mimosa')">mimosa</button>
    <button class="color-button" data-gd-primary-color="yellow" onclick="changeColor('yellow')">yellow</button>
    <button class="color-button" data-gd-primary-color="illuminating" onclick="changeColor('illuminating')">illuminating</button>
    <button class="color-button" data-gd-primary-color="greenery" onclick="changeColor('greenery')">greenery</button>
    <button class="color-button" data-gd-primary-color="green" onclick="changeColor('green')">green</button>
    <button class="color-button" data-gd-primary-color="emerald" onclick="changeColor('emerald')">emerald</button>
    <button class="color-button" data-gd-primary-color="turquoise" onclick="changeColor('turquoise')">turquoise</button>
    <button class="color-button" data-gd-primary-color="blue turquoise" onclick="changeColor('blue turquoise')">blue turquoise</button>
    <button class="color-button" data-gd-primary-color="aqua sky" onclick="changeColor('aqua sky')">aqua sky</button>
    <button class="color-button" data-gd-primary-color="blue" onclick="changeColor('blue')">blue</button>
    <button class="color-button" data-gd-primary-color="serenity" onclick="changeColor('serenity')">serenity</button>
    <button class="color-button" data-gd-primary-color="cerulean" onclick="changeColor('cerulean')">cerulean</button>
    <button class="color-button" data-gd-primary-color="classic blue" onclick="changeColor('classic blue')">classic blue</button>
    <button class="color-button" data-gd-primary-color="blue iris" onclick="changeColor('blue iris')">blue iris</button>
    <button class="color-button" data-gd-primary-color="very peri" onclick="changeColor('very peri')">very peri</button>
    <button class="color-button" data-gd-primary-color="indigo" onclick="changeColor('indigo')">indigo</button>
    <button class="color-button" data-gd-primary-color="ultra violet" onclick="changeColor('ultra violet')">ultra violet</button>
    <button class="color-button" data-gd-primary-color="violet" onclick="changeColor('violet')">violet</button>
    <button class="color-button" data-gd-primary-color="radiant orchid" onclick="changeColor('radiant orchid')">radiant orchid</button>
    <button class="color-button" data-gd-primary-color="ultimate gray" onclick="changeColor('ultimate gray')">ultimate gray</button>
    <button class="color-button" data-gd-primary-color="jet black" onclick="changeColor('jet black')">jet black</button>
    <button class="color-button" data-gd-primary-color="midnight" onclick="changeColor('midnight')">midnight</button>
    <button class="color-button" data-gd-primary-color="black" onclick="changeColor('black')">black</button>
    <button class="color-button" data-gd-primary-color="white" onclick="changeColor('white')">white</button>
    <style>
        .color-button {
            border: none;
            color: var(--gd-header-fg-color);
            font-weight: bold;
            padding: 0.25rem 1rem;
            background: var(--gd-primary-color);
            border-radius: 999px;
            font-size: 0.875rem;
        }
    </style>
    <script>
        function changeColor(color) {
            document.querySelector("body").setAttribute("data-gd-primary-color", color);
        }
    </script>
</div>

#### Secondary

<div style="display: flex; gap: 10px; flex-wrap: wrap; margin-top: 1rem;">
    <button class="secondary-color-button" data-gd-secondary-color="red" onclick="changeSecondaryColor('red')">red</button>
    <button class="secondary-color-button" data-gd-secondary-color="fuchsia rose" onclick="changeSecondaryColor('fuchsia rose')">fuchsia rose</button>
    <button class="secondary-color-button" data-gd-secondary-color="marsala" onclick="changeSecondaryColor('marsala')">marsala</button>
    <button class="secondary-color-button" data-gd-secondary-color="honeysuckle" onclick="changeSecondaryColor('honeysuckle')">honeysuckle</button>
    <button class="secondary-color-button" data-gd-secondary-color="chili pepper" onclick="changeSecondaryColor('chili pepper')">chili pepper</button>
    <button class="secondary-color-button" data-gd-secondary-color="true red" onclick="changeSecondaryColor('true red')">true red</button>
    <button class="secondary-color-button" data-gd-secondary-color="viva magenta" onclick="changeSecondaryColor('viva magenta')">viva magenta</button>
    <button class="secondary-color-button" data-gd-secondary-color="mocha mousse" onclick="changeSecondaryColor('mocha mousse')">mocha mousse</button>
    <button class="secondary-color-button" data-gd-secondary-color="sand dollar" onclick="changeSecondaryColor('sand dollar')">sand dollar</button>
    <button class="secondary-color-button" data-gd-secondary-color="rose quartz" onclick="changeSecondaryColor('rose quartz')">rose quartz</button>
    <button class="secondary-color-button" data-gd-secondary-color="living coral" onclick="changeSecondaryColor('living coral')">living coral</button>
    <button class="secondary-color-button" data-gd-secondary-color="orange" onclick="changeSecondaryColor('orange')">orange</button>
    <button class="secondary-color-button" data-gd-secondary-color="tangerine tango" onclick="changeSecondaryColor('tangerine tango')">tangerine tango</button>
    <button class="secondary-color-button" data-gd-secondary-color="tigerlily" onclick="changeSecondaryColor('tigerlily')">tigerlily</button>
    <button class="secondary-color-button" data-gd-secondary-color="peach fuzz" onclick="changeSecondaryColor('peach fuzz')">peach fuzz</button>
    <button class="secondary-color-button" data-gd-secondary-color="mimosa" onclick="changeSecondaryColor('mimosa')">mimosa</button>
    <button class="secondary-color-button" data-gd-secondary-color="yellow" onclick="changeSecondaryColor('yellow')">yellow</button>
    <button class="secondary-color-button" data-gd-secondary-color="illuminating" onclick="changeSecondaryColor('illuminating')">illuminating</button>
    <button class="secondary-color-button" data-gd-secondary-color="greenery" onclick="changeSecondaryColor('greenery')">greenery</button>
    <button class="secondary-color-button" data-gd-secondary-color="green" onclick="changeSecondaryColor('green')">green</button>
    <button class="secondary-color-button" data-gd-secondary-color="emerald" onclick="changeSecondaryColor('emerald')">emerald</button>
    <button class="secondary-color-button" data-gd-secondary-color="turquoise" onclick="changeSecondaryColor('turquoise')">turquoise</button>
    <button class="secondary-color-button" data-gd-secondary-color="blue turquoise" onclick="changeSecondaryColor('blue turquoise')">blue turquoise</button>
    <button class="secondary-color-button" data-gd-secondary-color="aqua sky" onclick="changeSecondaryColor('aqua sky')">aqua sky</button>
    <button class="secondary-color-button" data-gd-secondary-color="blue" onclick="changeSecondaryColor('blue')">blue</button>
    <button class="secondary-color-button" data-gd-secondary-color="serenity" onclick="changeSecondaryColor('serenity')">serenity</button>
    <button class="secondary-color-button" data-gd-secondary-color="cerulean" onclick="changeSecondaryColor('cerulean')">cerulean</button>
    <button class="secondary-color-button" data-gd-secondary-color="classic blue" onclick="changeSecondaryColor('classic blue')">classic blue</button>
    <button class="secondary-color-button" data-gd-secondary-color="blue iris" onclick="changeSecondaryColor('blue iris')">blue iris</button>
    <button class="secondary-color-button" data-gd-secondary-color="very peri" onclick="changeSecondaryColor('very peri')">very peri</button>
    <button class="secondary-color-button" data-gd-secondary-color="indigo" onclick="changeSecondaryColor('indigo')">indigo</button>
    <button class="secondary-color-button" data-gd-secondary-color="ultra violet" onclick="changeSecondaryColor('ultra violet')">ultra violet</button>
    <button class="secondary-color-button" data-gd-secondary-color="violet" onclick="changeSecondaryColor('violet')">violet</button>
    <button class="secondary-color-button" data-gd-secondary-color="radiant orchid" onclick="changeSecondaryColor('radiant orchid')">radiant orchid</button>
    <button class="secondary-color-button" data-gd-secondary-color="ultimate gray" onclick="changeSecondaryColor('ultimate gray')">ultimate gray</button>
    <button class="secondary-color-button" data-gd-secondary-color="black" onclick="changeSecondaryColor('black')">black</button>
    <button class="secondary-color-button" data-gd-secondary-color="white" onclick="changeSecondaryColor('white')">white</button>
    <button class="color-button" data-gd-primary-color onclick="changeSecondaryColor('primary')">primary</button>
    <style>
        .secondary-color-button {
            border: none;
            color: var(--gd-header-fg-color);
            font-weight: bold;
            padding: 0.25rem 1rem;
            background: var(--gd-secondary-color);
            border-radius: 999px;
            font-size: 0.875rem;
        }
    </style>
    <script>
        function changeSecondaryColor(color) {
            document
            .querySelector("body")
            .setAttribute("data-gd-secondary-color", color);
        }
    </script>
</div>
