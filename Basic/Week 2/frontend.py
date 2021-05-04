from   pyfiglet    import Figlet
from   backend     import *
from   prettytable import PrettyTable
from   threading   import Thread
import os
import sys
import tty
import termios
import time
import os
import getpass
import html

# Global variables --------------------------------------------------------------------------------------
finishLoading  = False
finishFinshing = False

"""
    /function/ getch():
        "" Purpose: Wait for the user to press a key.
"""
def getch():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    tty.setraw(sys.stdin.fileno())
    ch = sys.stdin.read(1)
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch

"""
    /function/ printWaitingScreen():
        "" Purpose: Print out "/" and "\" sign switching around
                    after some time interval (default 0.5)
                              --------- terminal --------
                    (example) 0s:   this is some text. \
                                                       |
                                                       v             
                              0.5s: this is some text. /
                                                       |
                                                       v 
                              1s:   this is some text. \
                              ----------------------------
"""
def printWaitingScreen():
    global finishLoading
    global finishFinshing
    print("\\", end='')
    sys.stdout.flush()
    while not finishLoading:
        print("\b/", end='')
        sys.stdout.flush()
        time.sleep(0.5)
        print("\b\\", end='')
        sys.stdout.flush()
        time.sleep(0.5)
    print("\bdone!")
    sys.stdout.flush()
    finishFinshing = True

"""
    /function/ stopWaitingScreen():
        "" Purpose:
            Stop the printing out the dangling symbols.
"""
def stopWaitingScreen():
    global finishLoading
    finishLoading = True
    while not finishFinshing:
        pass

"""
    /function/ startWaitingScreen(text):
        "" Purpose:
            Create a seperate thread to display a text in which
            2 characters '\' and '/' switching back and forth
            are appended to the end of string. Terminated by
            stopWaitingScreen().
"""
def startWaitingScreen(text):
    global finishLoading
    global finishFinshing

    finishLoading  = False
    finishFinshing = False

    print(text, end='')
    sys.stdout.flush()
    printWaitScreenThread = Thread(target=printWaitingScreen)
    printWaitScreenThread.start()

"""
    /function/ getCoolshit(text):
        "" Purpose:
            Render cool text font to print to terminal.
"""
def getCoolshit(text):
    f = Figlet(font='slant')
    return f.renderText(text)

"""
    /function/ printCoolshit(text):
        "" Purpose:
            Print the cool text.
"""
def printCoolshit(text):
    print(getCoolshit(text))

"""
    /function/ printSlogan():
        "" Purpose:
            Print the banner of the program, in form of cool text.
            The text is the name of the program itself.
"""
def printSlogan():
    printCoolshit(sys.argv[0])

"""
    /function/ printDescriptionByID(id):
        "" Purpose:
            Print the description of an exploit by its ID.
"""
def printDescriptionByID(id):
    # Printing wait status... ---------------------------------------------------------------------------
    startWaitingScreen("[i] Fetching exploit by id \"{}\"...".format(id))

    # Get terminal size ---------------------------------------------------------------------------------
    cols, rows = os.get_terminal_size().columns, os.get_terminal_size().lines

    # Print header --------------------------------------------------------------------------------------
    output_string = ''
    coolshit = getCoolshit(id)
    output_string += coolshit + '\n'

    # Get description -----------------------------------------------------------------------------------
    description = getDescriptionByID(id)
    output_string += description + '\n'

    # Get display lines of descriptions -----------------------------------------------------------------
    output_lines = []
    for line in output_string.split('\n'):
        if line == '':
            output_lines.append('')
        while line != '':
            output_lines.append(line[:cols])
            line = line[cols:]

    # Stop waiting screen! ------------------------------------------------------------------------------
    stopWaitingScreen()

    # Output the terminal size rows first! --------------------------------------------------------------
    for i in range(min(rows-1, len(output_lines))):
        print(output_lines[i])

    # Press 'q' for quit, any for more... kind of display -----------------------------------------------
    for i in range(rows, len(output_lines)):
        if len("--- Press any key to continue, press q to exit ---") < cols:
            print("--- Press any key to continue, press q to exit ---", end='')
            sys.stdout.flush()
        ch = getch()
        if ch != 'q':
            if len("--- Press any key to continue, press q to exit ---") < cols:
                print('\b' * len("--- Press any key to continue, press q to exit ---"), end = '')
                print(' ' * len("--- Press any key to continue, press q to exit ---"), end = '')
                print('\b' * len("--- Press any key to continue, press q to exit ---"), end = '')
            print(output_lines[i])
        else:
            break

    # Just a final knockdown... --------------------------------------------------------------------------
    print()
    print()

"""
    /function/ printHelp():
        "" Purpose:
            Print the help menu during interactive console mode.
"""
def printHelp():
    print()
    print("Stuck in someshit??")
    print("========================")
    print()
    print("    Command                Description")
    print("    -------------------    -----------------------------------")
    print("    /help                  Display this help.")
    print("    /list                  Display the first page of exploits.")
    print("    /list  <num>           Display exploit at page <num>.")
    print("    /desc  <exploit-id>    Print description of an exploit with ID <exploit-id>.")
    print("    /myfav                 Display your first favorite page of exploits. (yey :D)")
    print("    /myfav <num>           Display your favorite exploit at page <num>. (still yey :D)")
    print("    /fav   <exploit-id>    Mark an exploit as favorite.")
    print("    /unfav <exploit-id>    Unmark an exploit as favorite.")
    print("    /quit                  Quit this motherfucking program.")
    print()

"""
    /function/ displayExploitTable(page, itemsPerPage=15):
        "" Purpose:
            Display the list of <itemsPerPage> exploits in table form.
"""
def displayExploitTable(page, itemsPerPage=15):
    if page < 0:
        print("[!] What the fuck page you gave me?\n")
        return

    # Get data from backend.py ------------------------------------------------------------------------
    startWaitingScreen("[i] Getting data from backend.py... ")
    data = loadExploitJSON(page, itemsPerPage)
    stopWaitingScreen()

    # Create table header -----------------------------------------------------------------------------
    table = PrettyTable(['EDB-ID', 'Product', 'Vulnerbility', 'CVE', 'Platform'])

    # Parse data from JSON ----------------------------------------------------------------------------
    for exploit in data["data"]:
        # Find CVE -------------------------------------------------------------------------------
        cve = ""
        for code in exploit["code"]:
            if code["code_type"] == "cve":
                cve = "CVE-" + code["code"]
                break
        
        # Add row --------------------------------------------------------------------------------
        table.add_row([
            html.unescape(exploit["id"].rstrip().lstrip()), 
            html.unescape(exploit["description"][1].split('-')[0].rstrip().lstrip()),
            html.unescape(exploit["description"][1].split('-')[1].rstrip().lstrip()),
            html.unescape(cve.rstrip().lstrip()),
            html.unescape(exploit["platform"]["platform"].rstrip().lstrip())
        ])

    # Print table -------------------------------------------------------------------------------------
    print(table)
    print("** You are viewing page {}/{}. **".format(page, math.ceil(data["recordsTotal"] / itemsPerPage)))

    print()

# Display the table of favorites
def displayFavoriteTable(page=1, itemsPerPage=15):
    if page < 0:
        print("[!] What the fuck page you gave me?")
        return
    
    # Start waiting screen ----------------------------------------------------------------------------
    startWaitingScreen("[i] Getting data from backend.py... ")

    # Create table header -----------------------------------------------------------------------------
    table = PrettyTable(['EDB-ID', 'Product', 'Vulnerbility', 'CVE', 'Platform'])

    # Aquire list of FAVORITE ids from backend --------------------------------------------------------
    ids, noFavoriteIDs = getFavoriteIDs((page-1)*itemsPerPage, page*itemsPerPage)

    # Fetch data about those IDs from the website -----------------------------------------------------
    for id in ids:
        data = loadOneExploitJSONByID(id)
        if len(data["data"]) == 0:
            continue
        exploit = data["data"][0]

        # Find CVE -------------------------------------------------------------------------------
        cve = ""
        for code in exploit["code"]:
            if code["code_type"] == "cve":
                cve = "CVE-" + code["code"]
                break
        
        # Add row --------------------------------------------------------------------------------
        table.add_row([
            html.unescape(exploit["id"].rstrip().lstrip()), 
            html.unescape(exploit["description"][1].split('-')[0].rstrip().lstrip()),
            html.unescape(exploit["description"][1].split('-')[1].rstrip().lstrip()),
            html.unescape(cve.rstrip().lstrip()),
            html.unescape(exploit["platform"]["platform"].rstrip().lstrip())
        ])

    # Stop waiting screen -----------------------------------------------------------------------------
    stopWaitingScreen()

    # Print table -------------------------------------------------------------------------------------
    print(table)
    print("** You are viewing page {}/{}. **".format(page, math.ceil(noFavoriteIDs / itemsPerPage)))

    print()