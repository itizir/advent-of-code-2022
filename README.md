# Advent of Code 2022 x Flipper Zero

## What is this?

Great question! This is a program meant to run on the awesome [Flipper Zero](https://flipperzero.one/) that will calculate the answers to the just as awesome [Advent of Code](https://adventofcode.com/2022) puzzles (2022 edition) graciously offered each year by [Eric Wastl](http://was.tl/).

## Can I run this without a Flipper Zero?

Nope. At least not directly: you would first need to port all the Flipper-specific bits to your environment.

## What language is this in?

C. Github is not lying. I did take advantage of [mlib](https://github.com/P-p-H-d/mlib) to ease the pain, as it is already used in the Flipper Zero firmware.

## What hardware does the Flipper Zero have?

It is based on a [STM32WB55RG](https://www.st.com/en/microcontrollers-microprocessors/stm32wb55rg.html) chip: 32-bit 64 MHz CPU, 256 KB RAM.

However the aim here was to make a program that can be dynamically loaded by a running Flipper (i.e. a [FAP](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md)), and so the actual available memory is closer to 100 KB... Which was the main challenging constraint here (besides working with plain C).

## How fast is it?

Most days are solved fairly quickly (fraction of a second). A few however take a while. Day 23 — part 2, for instance, takes about 20 seconds. Again, the aim here wasn't speed, but simply trying to fit the problem onto the limited space of a Flipper Zero.

## I have a Flipper Zero! How do I run this?

The easiest is to first clone/check out the [official firmware](https://github.com/flipperdevices/flipperzero-firmware) (version [0.75.0](https://github.com/flipperdevices/flipperzero-firmware/releases/tag/0.75.0) would be the one this application was tested with), and then add this here repository as a submodule.

Inside the `flipperzero-firmware` top-level directory:

```
$ git submodule add -f https://github.com/itizir/advent-of-code-2022.git applications_user/advent_of_code_2022
```

Follow the instructions on how to build and run [FAPs](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md). For instance

```
$ ./fbt COMPACT=1 DEBUG=0 launch_app APPSRC=applications_users/advent_of_code_2022
```

I recommend compiling with those flags indeed (see point about bugs below).

The program will be installed in the `Games` Applications folder.

Then after obtaining puzzle inputs from the [AOC website](https://adventofcode.com/2022), save them as `.txt` files on the Flipper's SD card under `/advent/2022`, naming them after the corresponding day number. E.g. `/advent/2022/13.txt`.

## Is it safe?

The risk of your Flipper [catching fire](https://en.wikipedia.org/wiki/Halt_and_Catch_Fire_(computing)) because of it is... very low. And it won't steal your grandma's date of birth either.

Still, **use it at your own risk**!

The chances of your Flipper crashing or hanging because of it are high! Especially since most of the solvers simply _assume_ correctly formed input data, and appropriately sized problems, without checking.

## Are there known bugs and issues?

Yes. Just as I was preparing this release, I realised day 14 does not safely run if the release `COMPACT=1` and `DEBUG=0` flags are not used, as the recursion then seems to lead to a stack overflow. I may or may not get round to fixing this. :shrug:

It is also not currently possible to interrupt long calculations (e.g. pressing the 'back' button), which would be a nice addition.

And then obviously there are always _unknown_ bugs and issues...

Please do let me know if you find some, in particular if wrong answers are produced: the puzzle inputs are not all identical for everybody, so there may well be bugs I didn't notice because I was lucky with my input.
