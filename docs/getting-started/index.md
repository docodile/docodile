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
