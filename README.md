# Gomoku

[![CodeFactor](https://www.codefactor.io/repository/github/shenmian/gomoku/badge)](https://www.codefactor.io/repository/github/shenmian/gomoku)

A simple gomoku, using [SFML] framework, supports mouse/keyboard and controller input, and offers both offline and LAN multiplayer modes.

<p align="center"><img src="docs/screenshot.png" width=50% height=50%></p>

## Rules

The rules are **free-style** and there are no forbidden moves. Players can decide the order of play in each round.  

## Modes

- **Online**: One player serves as the host while the other joins as a client, allowing games to be played over a local network.
- **Offline**: Two players take turns playing the game locally.

## Keymap

### Keyboard

| Key                                        | Action          |
| ------------------------------------------ | --------------- |
| `Space`                                    | Place piece     |
| `Backspace`                                | Undo            |
| `W`/`A`/`S`/`D`/`Up`/`Down`/`Left`/`Right` | Move the cursor |

### Controller

| Key          | Action          |
| ------------ | --------------- |
| `A`/`Cross`  | Place piece     |
| `B`/`Circle` | Undo            |
| D-pad        | Move the cursor |

[SFML]: https://github.com/SFML/SFML
