# What is Plop?

Plop is a tool that allows developers to quickly and easily generate boilerplate code using a single shell command and a template file(s).

## Getting Started

## 1. Install node

(Recommended) Install `nvm` which will allow you to install and change between multiple versions of node.
For information about installing nvm, click [here](https://github.com/nvm-sh/nvm#installing-and-updating).

Using nvm, install the latest version of `node` and `npm` with the following command:

```bash
nvm install --lts
```

## 2. Install `plop`

With `node` and `npm` installed, `plop` can now be installed in one of two ways:

### Option 1. Add plop to your project

```bash
npm install --save-dev plop
```

### Option 2. Install globally (recommended)

```bash
npm install -g plop
```

## 3. Run `plop`

At this point plop should be fully operational.
Simply run the `plop` command from the root of the `sln_smart_tlhmi` repo to get started.

After running the command,
you will need to answer a series of prompts to specify the `device type`, `name`, and the desired `core` with which to use this new HAL device.

Ex.

```bash
❯ plop
? Device type input
? Device name Example
? Core cm4
✔  ++ /framework/hal/input/hal_input_example.c
✔  +- /cm4/source/sln_smart_tlhmi_cm4.cpp
✔  +- /cm4/source/sln_smart_tlhmi_cm4.cpp
✔  +- /cm4/board/board_define.h
```

## Going further

For more information about **Plop** and its usage, check out the [official Plop documentation](https://plopjs.com/).
