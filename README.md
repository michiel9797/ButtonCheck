# ButtonCheck
A benchmark for testing Netcodes, primarily focussed on information accuracy for competitive multiplayer games.

A simple generic 2D fighting game made for the purposes of this benchmark can be found here:
- (Benchy-Fighters)[https://github.com/michiel9797/Benchy-Fighters]

The following libraries are required to compile and run this program:
- (net-tools)[https://sourceforge.net/projects/net-tools/#]
- (libboost-all-dev)[https://www.boost.org]

## Custom game support
ButtonCheck can support different games then the one provided, though there are a set of restrictions and requirements.


Firstly ButtonCheck can only run in Linux, as it makes use of Linux network simulation tools.


Secondly, ButtonCheck expects any underlaying programs to meet the following structural requirements:
- The program used to run an emulation and client instances for simulations must be the exact same, servers can be run from either the same or a different program.


### Emulation
- Emulation programs will be provided the following commandline inputs:
  1. Executable name, as defined in ButtonChecks menu
  2. EMULATE (to signify it must run in emulation mode)
  3. The first players input, as defined in ButtonChecks menu
  4. The second players input, as defined in ButtonChecks menu
- Emulation programs must start by printing a handshake to the standard commandline:
```bash
EMULATE START
```
- Afterwards, emulation programs must only print in-order gamestates one line at a time to the standard commandline.
- For the timer displayed by ButtonCheck during emulation, 60fps is expected. Other framerates will still work, but this timer may be unreliable in such cases.
- Emulation programs must end by themselves.


### Simulation
- Simulation programs will be provided the following commandline inputs:
  1. Executable name, as defined in ButtonChecks menu
  2. SIMULATE (to signify it must run in simulation mode)
  3. The current players input, as defined in ButtonChecks menu
  4. The number corresponding to which player is playing on this device, 1 or 2
  5. The IP the client must use, 10.0.0.2 or 10.0.0.3 respectively
  6. The IP and port the server must use, 10.0.0.1:40000
- Simulation programs must only print in-order gamestates one line at a time to the standard commandline.
- For the timer displayed by ButtonCheck during simulation, 60fps is expected. Other framerates will still work, but this timer may be unreliable in such cases.
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
- Server programs must end by themselves.
