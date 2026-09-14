# ButtonCheck
A benchmark for testing Netcodes, primarily focussed on information accuracy for competitive multiplayer games.

You can find a repository with Makefiles to compile this project here:
- https://github.com/michiel9797/Michiels-Makefiles/tree/master

For a thorough explanation on the design of the program, see section 4.2 of the accompanying thesis linked here: 
- [ButtonCheck: a Framework to Benchmark the Performance of Netcodes for Fighting Games](https://theses.liacs.nl/3881)

A simple generic 2D fighting game made for the purposes of this benchmark can be found here:
- [Benchy-Fighters](https://github.com/michiel9797/Benchy-Fighters)

The following libraries are required to compile and run this program:
- [net-tools](https://sourceforge.net/projects/net-tools/#)
- [libboost-all-dev](https://www.boost.org)

**NOTE:** You will need to have Boost version 1.88 or greater installed to compile this program

## Building
Before running the program, compile it using c++20. For a pre-assembled makefile, see the link above.

## Running the program
If you want to run the program using its network simulation features, you will have to call it with administrator priviledges.

There are two ways to use the program. It can be called as follows:
```bash
./ButtonCheck
```
When calling as such, the programs menu will open allowing you to adjust various settings before running a test.

It can also be called this way:
```bash
./ButtonCheck -F
```
When calling as such, the program will immediately run a test with the settings as found in the setting file.
For more information on how to use -F, call the program with --help.

## Running with Benchy Fighters
In order to run tests using Benchy Fighters, some of the following options need to be set:
- For the client and server executable, insert the path to where the BenchyFighters executable is located
- For the client inputs, insert the path to the inputs generated with the [timestampedkeylogger](https://github.com/michiel9797/timestampedKeylogger)
- For the custom arguments of both the client and server (found in the extra options menu), choose either YOJIMBO, GNS or RAKNET. Both client and server need the same custom argument

## Netem files
An example netem file called "Netem_Example.json" can be found in the "Examples" folder. Use it to create your own netem json file and set the path to it as the netem file to use.

## Custom game support
ButtonCheck can support different games then the one provided, though there are a set of restrictions and requirements.


Firstly ButtonCheck can only run in Linux, as it makes use of Linux network simulation tools.


Secondly, ButtonCheck expects any underlaying programs to meet the following structural requirements:
- The program used to run an emulation and client instances for simulations must be the exact same, servers can be run from either the same or a different program.


### Emulation
- Emulation programs will be provided the following commandline inputs:
  1. Executable name, as defined in ButtonChecks menu
  2. EMULATE (to signify it must run in emulation mode)
  3. The first players input file, as defined in ButtonChecks menu
  4. The second players input file, as defined in ButtonChecks menu
- Emulation programs must start by printing a handshake to the standard commandline:
```bash
EMULATE START
```
- Afterwards, emulation programs must only print in-order gamestates one line at a time to the standard commandline.
- Emulation programs may print errors.
- Emulation programs must end by themselves.


### Simulation
- Simulation programs will be provided the following commandline inputs:
  1. Executable name, as defined in ButtonChecks menu
  2. SIMULATE (to signify it must run in simulation mode)
  3. The current players input file, as defined in ButtonChecks menu
  4. The number corresponding to which player is playing on this device, 1 or 2
  5. The IP the client must use, 10.0.0.2 or 10.0.0.3 respectively
  6. The IP and port the server must use, 10.0.0.1:40000
- Simulation programs must only print in-order gamestates one line at a time to the standard commandline.
- Simulation programs may print errors, only the errors of the instance to which network emulation is applied will be shown.
- Simulation programs must end by themselves.


### Server
- Server programs will be provided the following commandline inputs:
  1. Executable name, as defined in ButtonChecks menu
  2. SERVER (to singify it must run in server mode)
  3. The IP and port the server must use, 10.0.0.1:40000
- Server programs must start by printing a handshake to the standard commandline:
```bash
SERVER START
```
- Server programs may print errors.
- Server programs must end by themselves.
