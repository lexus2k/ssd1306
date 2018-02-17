# Contributing

When contributing to this repository, please first discuss the change you wish to make via issue.
Please note we have a code of conduct, please follow it in all your interactions with the project.

## Pull Request Process

Prior to submitting pull request, create issue first.

  - Make sure that you have signed [GitHub CLA](https://cla.github.com/), when doing pull requests to the project.
  - Do not include issue numbers in the PR title.
  - Follow the code style used across the project.
  - Avoid platform-dependent code, where it can be done.
  - If your commit introduces a breaking change it should contain an explanation about the reason of the breaking
    change, which situation would trigger the breaking change and what is the exact change.
    - If you change behaviour of public API function, please describe new behavior in doxygen format very carefully.
      In most cases changing behaviour of public API function is subject to reject pull-request.
  - When adding new functionality, please provide example sketch in the examples/ folder, or modify existing example.
  - Trivial pull requests or pull requests that refactor the code with no clear benefits may be immediately closed
    by the maintainers to reduce unnecessary workload on reviewing.

**Pull Request Checklist**

1. Submit issue first and discuss the changes, you're going to make.
2. Check via [cppcheck tool](http://cppcheck.sourceforge.net/), that no warning messages are displayed.
3. Check that Arkanoid sketch can be compiled for Arduino Nano (Atmega328p), Attiny85 without any issues.
4. Check that ssd1306_demo sketch can be compiled and **run** for Arduino Nano (Atmega328p), Attiny85, ESP8266, ESP32 and
   without any issues. If you don't have a chance to run ssd1306_demo on the listed platforms, please, clearly inform about that
   in your pull request.

**Trivial Pull Request Checklist**

For trivial pull requests, just check compilation for different platforms: Arduino Nano, Attiny85, ESP8266.

If accepted, your contribution may be heavily modified as needed prior to merging. You will likely retain author
attribution for your Git commits granted that the bulk of your changes remain intact.

## Copyright

By contributing to this repository, you agree to license your work under the MIT license. Any work contributed
where you are not the original author must contain its license header with the original author(s) and source.
