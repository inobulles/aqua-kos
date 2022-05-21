# aqua-kos

This repository contains the source code for the main AQUA KOS (the one that contains all of the code for every Unix-based platform).

## Building

`aqua-kos` is automatically built, along with all its dependencies, by [`aqua-unix`](https://github.com/inobulles/aqua-unix). The KOS is installed as an executable called `aqua` somewhere in your `$PATH`.

## Command-line arguments

Here is a list of all command-line arguments that can be passed to the KOS executable:

### --root [root directory path]

Specify where the root directory is.
If `NO_ROOT` is passed as the root directory path, the KOS will boot without a root directory.
The default value for the root path is defined by `KOS_DEFAULT_ROOT_PATH`, and is automatically set by `aqua-unix`.

### --boot [boot package path]

Specify where the boot package is.
Note that this is relative to the working directory, not the root directory.
The default value for the boot path is defined by `KOS_DEFAULT_BOOT_PATH`, and is automatically set by `aqua-unix` (generally to be the file called `boot.zpk` file in the root directory).

### --devices [device binaries directory path]

Specify where the device binaries directory is.
The default value for the device binaries directory path is defined by `KOS_DEFAULT_DEVICES_PATH`, and is automatically set by `aqua-unix`.