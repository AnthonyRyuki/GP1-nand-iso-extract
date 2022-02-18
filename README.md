# Note
This is a fork of FIX94's triforce-nand-iso-extract that extracts an ISO out of MAME's Triforce NAND files.

# Usage
Grab the [TriNandIsoExtract.exe](bin/TriNandIsoExtract.exe?raw=true) and place it into a new folder.  
Copy the .bin files of your romset into that folder as well.  
Run TriNandIsoExtract, choose your game and wait for it to create a OUT.BIN.  
Rename that OUT.BIN to for example game.iso, it is now ready to play.  

NOTE: MAME currently has bad dumps for Mario Kart Arcade GP. The original version of this app requires an ISO of the American version of Mario Kart Arcade GP 2 for this reason.
Also, trying to extract an F-Zero AX Monster Ride ISO will result in a bad file for some reason. This app fixes the fst.bin location, but there's something else wrong with it that I can't quite figure out.
