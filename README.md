# aqua-kos
This repository contains the source code for the main AQUA KOS (the one that contains all of the code for every Unix-based platform).

## Command-line arguments
Here is a list of all command-line arguments that can be passed to the KOS executable.

### --help
Print out information for each argument.

### --root [root directory path]
Specify where the root directory is.

### --boot [boot package path]
Specifiy where the boot package is.
Note that this is relative to the working directory, not the root directory.

### --devices [device binaries directory path]
Specifiy where the device binaries directory is.

### --width [width in pixels]
Specify the prefered screen width in pixels when starting up.
Depending on the platform and application settings, this flag may not be respected.

### --height [height in pixels]
Specify the prefered screen height in pixels when starting up.
Depending on the platform and application settings, this flag may not be respected.

### --msaa [sample count to be used]
Tell the KOS how many MSAA samples you want to use. Passing "0" will disable MSAA.
This flag may not be implemented on all platforms.

### --no-vsync
Tell the KOS not to enable VSync.