# CubeRoverWatchdog software

This branch contains code to run the MSP430FR5994 acting as a watchdog in the CubeRover.

For an up-to-date view of what features are implemented, please consult the Trello board.



## Testing

The goal is that every single line of code written in this repository has been throughoughly both on the Watchdog and
for general correctness (running on a regular computer) when possible. To that end, a `Makefile` is provided that will
allow for compiling and testing certain portions of code on a host computer, and additional code is provided in the
`Host/` subdirectory to support testing and debugging on-device.
