#!/usr/bin/env python
import curses

def main(stdscr):

    # see 2/howto/curses.html for info.
    # look into curses.textpad, which turns a window into a textbox with bindings
    # look into curses.ascii for easier char handing

    # the curses magical setup
    stdscr = curses.initscr() # setup intial window
    curses.noecho() # dont echo keystrokes
    curses.cbreak() # dont wait for enter, handle keys immediately
    stdscr.keypad(1) # use aliases for special keys
    curses.start_color()
    #curses.curs_set(0) # supress the blinking cursor

    # ----------------------
    # useful functions
    # ----------------------
    # window.move(y,x)
    # window.addstr(...)
    # window.addch(str/int)
    # window.refresh()
    # getch([y,x]) - blocks for input, returns keypress as int, with cursor at (y,x)
    # getstr([y,x], [n]) - get a string of len(n) if specified, with cursor at (y,x)
    # nodelay(bool) - makes getch() non blocking (ch ? int(ch) : curses.ERR(int(-1)))

    # notes
    # ------------
    # getstr() only captures ascii printable chars.
    # 

    # ----------------------
    # example input loop
    # ----------------------
    # if python supported switch-case
    # this would be that! but, elif chain!
    #
    # while True:
    #     c = stdscr.getch()
    #     if c == ord('p'): PrintDocument()
    #     elif c == ord('q'): break  # Exit the while()
    #     elif c == curses.KEY_HOME: x = y = 0
    #	  elif c > 255: pass # special key. nonprintable char. handle accordingly.

    # -----------------------
    # the forms of addstr
    # no more mvwaddch(wtf, ...)!!!
    #------------------------
    #addstr(str) # Display str at current location
    #addstr(str, attr) # Display str at current location with attribute (attr) set
    #addstr(y, x, str) # Display str at location (y,x)
    #addstr(y, x, str, attr) # Display str at location (y,x) with attribute (attr) set

    # -----------------------
    #  color
    # -----------------------
    # set color pair 1 to be red text, white bg
    # Note: anything currently printed with that pair will change color as well!
    # curses.init_pair(1, curses.COLOR_RED, curses.COLOR_WHITE)

    # print something using color pair 1
    # stdscr.addstr(0,0, "RED ALERT!", curses.color_pair(1))

    # ----------------------
    # attributes
    # ----------------------
    # curses.A_BLINK
    # curses.A_BOLD
    # curses.A_DIM 
    # curses.A_REVERSE
    # curses.A_STANDOUT
    # curses.A_UNDERLINE
    # curses.color_pair(int)

    # atrributes can be OR'd
    # ------------------------
    # statusbar = (curses.A_REVERSE | curses.color_pair(2))
    # window.addstr("woot!", statusbar)
    # window.refresh()

    pad = curses.newpad(100, 100)

    pad.addstr('A'*10000)

    pad.refresh( 0,0, 5,5, 20,75)

    x = y = 0
    while True:
        c = stdscr.getch(y, x)
        if c == ord('q'): break  # Exit the while()
        elif c == curses.KEY_DOWN:         y += 1
        elif c == curses.KEY_UP and y > 0: y -= 1
        elif c == curses.KEY_HOME:         x = y = 0
        elif c > 255: pass # special key. nonprintable char. handle accordingly.

    # undo special stuff and close curses
    curses.nocbreak(); 
    stdscr.keypad(0); 
    curses.echo()
    curses.endwin()

curses.wrapper(main)