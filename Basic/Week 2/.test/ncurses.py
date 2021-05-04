import curses
import signal
import time
import os

stdscr = None
def initScreen():
    stdscr = curses.initscr()
    curses.noecho()      # Don't echo the keys written to screen
    curses.cbreak()      # Program reacts to keystroke instantly
    stdscr.keypad(True)  # Don't display special characters as multi-byte characters

def termScreen():
    global stdscr
    curses.nocbreak()
    if stdscr:
        stdscr.keypad(False)
        stdscr = None
    curses.echo()
    curses.endwin()
    exit(0)

initScreen()

pad = curses.newpad(200, 200)
# These loops fill the pad with letters; addch() is
# explained in the next section
for y in range(0, 99):
    for x in range(0, 99):
        pad.addch(y,x, ord('a') + (x*x+y*y) % 26)

# Displays a section of the pad in the middle of the screen.
# (0,0) : coordinate of upper-left corner of pad area to display.
# (5,5) : coordinate of upper-left corner of window area to be filled
#         with pad content.
# (20,75) : coordinate of lower-right corner of window area to be
#         : filled with pad content.



print(terminal_size)
pad.refresh(0,0, 0,0, 40,170)

time.sleep(10)

termScreen()