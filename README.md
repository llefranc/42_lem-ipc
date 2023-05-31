# lem-ipc (@42Paris)

> This project was coded for Linux. Here is the [subject][1].
>

![Alt text](https://github.com/llefranc/42_lem-ipc/blob/main/lem-ipc_example2.png)

## About

lem-ipc is a little game to understand how mutliple processes can communicate between themselves **using the UNIX System V IPC**.

> System V IPC is the name given to three interprocess communication mechanisms that are widely available on UNIX systems: message queues, semaphore, and shared memory.

The game consist of a grid filled with players from different teams (maximum 7 different teams), with each team fighting each other and trying to be the last team standing.

## Modes

		./lemipc [team-number]

`lemipc` can be launch in two different modes : **graphic mode or player mode**.   

The mode is determinated based on the team number provided as argument.

### Graphic mode

		./lemipc 0
		
To launch the graphic mode, you need to run `lemipc` with **0 as team number**.   

The graphic mode display the grid with the position of each player, and refreshed the grid each time a player mooves.  

- :warning: Only one instance of the graphic mode can run at any time :warning:
- :warning: The graphic mode must run during the game, otherwise the game end immediately :warning:  

### Player mode

		./lemipc [1-7]




- Each instance of a process is a player.
- 
- Each player is represented by its team number on the grid.
- Each player 
- When two players from the same team surround an ennemy player, this one dies.
- The game stop when only one team is remaining.

lem-ipc can be launch either in graphic mode, either in the player mode.

## Lobby

./lem-ipc 0

## Game

- player moove 1 time every second

- `graphic mode`: 

ft_ping uses a raw socket to send each second an ICMP Echo Request packet with a timestamp in its body.
It will also display a line of information for each received ICMP packet.

It accepts a simple IPv4 address or hostname as a parameter, and supports both numerical IP addresses and hostnames.

> The program should be run with appropriate permissions, as sending ICMP Echo Request packets may require administrative privileges.

When receiving an ICMP packet, ft_ping will check:
- If the packet was correctly addressed to this process by checking the PID stored in the ID field of ICMP Echo Request.
- If it's an error packet or not.

ft_ping will ping the targeted host until `Ctrl+C` is pressed. It will then display statistics about the received ICMP Echo Response packets.

ft_ping supports also the following options :
- `-h`: provides help information about the usage and command-line options of ft_ping.
- `-q`: enables quiet output, ft_ping will only display the end statistics.
- `-v`: enables verbose output and allows viewing the results in case of problems or errors related to the packets.

*Example of error packet (packet filtered) with verbose output*

![Alt text](https://github.com/llefranc/42_ft_ping/blob/main/ft_ping_example2.png)

## System V IPC

## Building and running the project

1. Download/Clone this repo

        git clone https://github.com/llefranc/42_lem-ipc

2. `cd` into the root directory and run `make`

        cd 42_lem-ipc
        make

3. Run several time `lemipc` in different terminals with a team number between 0 and 7.

		# Two players in team 1 vs one player in team 2, and the graphic mode
		./lemipc 1
		./lemipc 1
		./lemipc 2
		./lemipc 0   # graphic mode

[1]: https://github.com/llefranc/42_lem-ipc/blob/main/lem-ipc.en.subject.pdf
