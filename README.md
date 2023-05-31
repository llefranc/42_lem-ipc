# lem-ipc (@42Paris)

> This project was coded for Linux. Here is the [subject][1].
>

![Alt text](https://github.com/llefranc/42_lem-ipc/blob/main/img/lem-ipc_example.png)

## About

lem-ipc is a little game to understand how mutliple processes can communicate between themselves **using the UNIX System V IPC**.

> System V IPC is the name given to three interprocess communication mechanisms that are widely available on UNIX systems: message queues, semaphore, and shared memory.

The game consist of a grid filled with players from different teams (maximum 7 different teams), with each team fighting each other and trying to be the last team standing.

## Building and running the project

1. Download/Clone this repo

        git clone https://github.com/llefranc/42_lem-ipc

2. `cd` into the root directory and run `make` or `make debug` for additional logs

        cd 42_lem-ipc
        make

3. Run several time `lemipc` in different terminals with a team number between 0 and 7

		# Two players in team 1 vs one player in team 2, and the graphic mode
		./lemipc 1
		./lemipc 1
		./lemipc 2
		./lemipc 0   # graphic mode



## Modes

		./lemipc [team-number]

`lemipc` can be launch in two different modes : **graphic mode or player mode**.   

The mode is determinated based on the team number provided as argument.

During the game, you will have:
- one `lemipc` instance for the graphic mode.
- and x `lemipc` instances for x players alive. 

### Graphic mode

		./lemipc 0
		
To launch the graphic mode, you need to run `lemipc` with **0 as team number**.   

The graphic mode display the grid with the position of each player, and refreshed the grid each time a player mooves.  

- :warning: Only one instance of the graphic mode can run at any time :warning:
- :warning: The graphic mode must run during the game, otherwise the game ends immediately :warning:  

_graphic mode example_

![Alt text](https://github.com/llefranc/42_lem-ipc/blob/main/img/lem-ipc_graphic_mode_example.png)

### Player mode

		./lemipc [1-7]

To launch the player mode, you need to run `lemipc` with **a team number between 1 and 7**.  

The player is automatically mooved on the grid when it's it turn based on a very simple IA.

_player mode example_

![Alt text](https://github.com/llefranc/42_lem-ipc/blob/main/img/lem-ipc_player_mode_example.png)

## Lobby

When launching the first instance of `lemipc`, this one will init the shared ressources and init a starting time.  

A timer will then be displayed, and other `lemipc` instances **can joined the game during this time**.  

The game is **launched when the timer reaches 0**. At this point, no other instances will be able to join the game.

> The waiting time is defined with the macroconstant `SEC_START_TIME` in `game_utils.h`. You can change this value to increase or decrease the lobby waiting time.

## Game rules

The game is designed based on the following rules:

- ➡️ Each player is represented by its team number on the grid.
- ➡️ Each second a player mooves.
- ➡️ A player can move only on an empty tile.
- ➡️ A player can move only in 4 directions (up, down, left and right).
- ➡️ A player can move several times in a row.
- ➡️ You cannot differentiate players from the same team on the grid.
- ➡️ All the players of a same team target all together the same ennemy player.
- ➡️ When two players from the same team surround an ennemy player, this one dies (diagonals works).
- ➡️ It's mandatory for a player to move when it's its turn, even if it's just next to its target.
- ➡️ The game stop when only one team is remaining.

## System V mechanics

The three interprocess communication mechanisms of System V are used the following way:

- `shared memory segment`: contain the map and game datas.
- `semaphore sets` : secure the access of the shared memory segment.
- `message queue` : allow the players of a same team to exchange the ennemy player ID to focus.

> **The first instance of `lempic` to be launch will init all the System V ressources; the last instance to exit will destroy all the System V ressources.**

[1]: https://github.com/llefranc/42_lem-ipc/blob/main/lem-ipc.en.subject.pdf
