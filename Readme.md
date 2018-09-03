File-Explorer
This is a Terminal based File-Explorer.

Getting Started
It works in two modes:

    1. Normal Mode: 
        In this mode you can view all the files in current Directory. You can navigate up and down using arrow keys.
        When Enter is pressed if the corresponding entry is file it will get open using the system's default application for that particular file extension.
        While If the corresponding entry is Folder You wil navigate into that Particular Folder
        You can use Backspace Key to go to parent of the current directory.
        You can use left and right arrow keys to move to the previously visited locations.

    2. Command Mode:
        A) copy it can copy multiple files and folder at asingle time.  
        B) move it can move multiple files and folder at a time.
        C) rename it will rename a file
        D) create_file create file at the path specified. Use '.' for current working directory.
        E) create_directory create directory at the path specified. Use '.' for current working directory.
        F) delete_file delete file at the path specified. Use '.' for current working directory.
        G) delete_directory delete directory at the path specified. Use '.' for current working directory.
        H) goto use to navigate anywhere within the system Use only Absolute path with respect to the root of your application.
        I) search Search for the given file/folder name within the current working directory
        J) Snapshot snapshot the folder into current file
        K) On pressing the ESC key you will go back to Normal Mode

    Formats of Path :
    ~/Path  :  Path relative to your root
    ./Path  :  Path relative to your current working directory
    Path    :  Path relative to your current working directory

    SPACES AND OTHER SPECIAL CHARACTERS:
    for space seperated file please use "'" single quotes for enclosing full file name
    for "/" in file name use "//" escaping slash with a slash
    for "'" in file name use "/'"escaping single quote with a slash.

    ERROR Handling:
    A short description of error will be given on the bottom line
    For more detailed error check error.txt

Application Open in Normal Mode by default
Press ':' to move to command mode
Press 'ESC' to move to normal mode

Prerequisites
Nothing just compile the files by typing make 

Give examples

Built With
Visual Studio Code - The web framework used

Authors
Aman Joshi

Acknowledgments
Hat tip to anyone whose code was used