from   backend  import *
from   frontend import *
import signal
    
"""
    /function/ handleArgs():
        "" Purpose:
            Handle the arguments.
"""
def handleArgs(args):
    if args.exploit_id:
        printDescriptionByID(args.exploit_id)
    
    if args.page_num:
        displayExploitTable(page=int(args.page_num))

    if args.favorite:
        displayFavoriteTable(page=int(args.favorite))

"""
    /function/ ctrl_c_catcher():

    "" Purpose:
        For SIGINT signal handler. Whether CTRL-C
        hits we execute this piece of code.
"""
def ctrl_c_catcher(sig, frame):
    # Printing exit...
    print("\n[i] Exiting...")
    cleanBackend()

"""
    /@main()/

    "" Purpose:
        Run the client interface.
"""

# Print slogan -------------------------------------------------------------------------------------------------------
printSlogan()

# Get arguments ------------------------------------------------------------------------------------------------------
args = getArgs()

# Initiate backend ---------------------------------------------------------------------------------------------------
startWaitingScreen("[i] Loading screen... ")
initBackend()
stopWaitingScreen()
print("[i] Press /help for a list of helps!\n")

# Handle request from arguments --------------------------------------------------------------------------------------
handleArgs(args)

# Catch CTRL-C -------------------------------------------------------------------------------------------------------
signal.signal(signal.SIGINT, ctrl_c_catcher)

# Get input ----------------------------------------------------------------------------------------------------------
while True:
    command = input(">>  ")
    try:
        if command.find('/') != -1:
            if command.find('/') == command.find('/list') and command[command.find('/list')+len('/list'):] == '':
                displayExploitTable(page=1)
            elif command.find('/') == command.find('/list '):
                displayExploitTable(page=int(command[command.find('/list ')+len('/list '):]))
            elif command.find('/') == command.find('/quit'):
                cleanBackend()
            elif command.find('/') == command.find('/help'):
                printHelp()
            elif command.find('/') == command.find('/desc '):
                printDescriptionByID(command[command.find('/desc ')+len('/desc '):])
            elif command.find('/') == command.find('/myfav') and command[command.find('/myfav')+len('/myfav'):] == '':
                displayFavoriteTable(page=1)
            elif command.find('/') == command.find('/myfav '):
                displayFavoriteTable(page=int(command[command.find('/myfav ')+len('/myfav '):]))
            elif command.find('/') == command.find('/fav '):
                addExploitToFavoriteByID(command[command.find('/fav ')+len('/fav '):])
            elif command.find('/') == command.find('/unfav '):
                removeExploitFromFavoriteByID(command[command.find('/unfav ')+len('/unfav '):])
            else:
                printHelp()
        else:
            printHelp()
    except Exception as e:
        print(e)
        printHelp()

