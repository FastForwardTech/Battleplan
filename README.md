# Battleplan

Battleplan is a homebrew desktop application for virtually managing a D&D tabletop campaign. Its main purpose is to maintain a visual display of characters' positions and vital statistics, including HP (current/max) and any conditions.


# Building

## Dependencies

- Windows 7 or later
- Qt (Built with v5.14.2, but versions as early as v4 may work; however, they are not officially supported)
- C++11 compatible compiler. Built with MinGW-64, but MSVC may also work

## Steps

- Ensure make is in your $Path environment variable
- Open the terminal included with your Qt installation
- cd src/
- qmake Battleplan.pro
- cd release/
- make
### OR
- Open Battleplan.pro in Qt Creator
- Click `Configure Project`
- Click the `Build` button in the bottom left corner
