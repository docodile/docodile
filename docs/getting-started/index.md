# Getting started

## Installation

### Docker / Podman

The easiest way to get up and running on any machine is to pull [the Docker image](https://hub.docker.com/r/jakeecarrington/docodile "jakeecarrington/docodile") and mount your directories to the appropriate places as shown below.

```shell
podman run -d -v ./docs:/docodile/docs -v ./templates:/docodile/templates -v ./docodile.config:/docodile/docodile.config -p 6006:6006 docker.io/jakeecarrington/docodile:v0.1.3
```

### From source

To install from source you can clone this repo and run `make install`. This will only work on Linux, maybe MacOS.

!!! question "Want to help?"

    If you have knowledge of programming for Windows in C your pull requests would be greatly appreciated. See [Contributing](/getting-started/contributing.md) for more information.

## Initial setup

Once you have a way to run Docodile create a folder where you want to build your site. In this folder place a `docodile.config` (example included in this repo) and run `docodile serve` to see a hotloaded preview of your site.

From here it works much like any other static site generator. Add some Markdown files to your folder and see your website come to life in your browser. Want to publish your site? Go here to find out more (TODO).
