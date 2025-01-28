# Development

Building this software can be performed two ways: using the STM32CubeIDE or using command line tools.

## STM32CubeIDE

The easiest way to start working with the STM32CubeIDE is to create a new project for the STM32F103RCTx.
Once this is created, remove the auto-generated source code.
Next, drag the contents of the `source` folder into the project and choose to link to files.
You will need to update the build settings for include paths and point to the new `.ld` linker file.

## Command line tools and building a release

In the `source` folder there is a `Makefile` that can be used to build the repository using command line tools.
When running the `make` command, specify which model of the device and the language(s) you would like to use.

### Windows (MSYS)

1. Install MSYS from the [official website](https://msys2.org)
2. Set up prerequisites using
```
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc mingw-w64-x86_64-libwinpthread-git python3 python3-pip 
```
3. Download and unpack RiscV toolchain from [repository](https://github.com/xpack-dev-tools/riscv-none-elf-gcc-xpack/releases) into Mingw64 environment location (e.g `C:\msys64\mingw64`) without replacing files
3. If needed, install OpenOCD or stlink for easy flashing:
```
pacman -S mingw-w64-x86_64-openocd
```

```
pacman -S mingw-w64-x86_64-stlink
```

4. Follow steps 3-8 from [macOS section](#macos)

### macOS

Use the following steps to set up a build environment for IronOS on the command line (in Terminal).

1. [Follow steps 1 – 3 here to install the toolchain](https://github.com/glegrain/STM32-with-macOS#0---installing-the-toolchain) needed to compile for STM32 microcontrollers.
2. Install `python`:

```
brew install python
```

3. (Optional) Update `pip` so it doesn't warn you about being out-of-date:

```
python3 -m pip install --upgrade pip
```

4. Change to the `source` directory:

```
cd source
```

5. Create a Python virtual environment for IronOS named `ironos-venv` to keep your Python installation clean:

```
python3 -m venv ironos-venv
```

6. Activate the Python virtual environment:

```
source ironos-venv/bin/activate
```

7. Install the dependencies required to run `make-translation.py`:

```
pip install bdflib
```

8. All done! See some examples below for how you can build your own IronOS.

### Examples

To build a single language Simplified Chinese firmware for the TS80P with 8 simultaneous jobs:

```
make -j8 model=TS80P firmware-ZH_CN
```

To build a European multi-language firmware for the Pinecil with as many simultaneous jobs as there are logical processors on Linux:

```
make -j$(nproc) model=Pinecil firmware-multi_European
```

To build a Cyrillic compressed multi-language firmware for the Pinecil with as many simultaneous jobs as there are logical processors on macOS:

```
make -j$(sysctl -n hw.logicalcpu) model=Pinecil firmware-multi_compressed_Belorussian+Bulgarian+Russian+Serbian+Ukrainian
```

To build a custom multi-language firmware including English and Simplified Chinese for the TS80:

```
make -j8 model=TS80 custom_multi_langs="EN ZH_CN" firmware-multi_Custom
```

To build a custom compressed multi-language firmware including German, Spanish, and French for the TS100 (note if `model` is unspecified, it will default to `TS100`):

```
make -j8 custom_multi_langs="DE ES FR" firmware-multi_compressed_Custom
```

To build a release instead, run the `build.sh` script. This will update translations and also build every language for all device models. For macOS users, replace `make -j$(nproc)` in the script with `make -j$(sysctl -n hw.logicalcpu)` before running.

## Updating languages

To update the language translation files and their associated font maps, execute the `make_translation.py` code from the `Translations` directory.
If you edit the translation definitions or the English translation, please also run `gen_menu_docs.py` to update the settings menu documentation automatically.

## Building Pinecil V1

I highly recommend using the command line tools and using Docker to run the compiler.
It's a bit fussier on setup than the STM tooling, and this is by far the easiest way.
If you _need_ an IDE I have used [Nuclei's IDE](https://nucleisys.com/download.php).
Follow the same idea as the STM Cube IDE notes above.

## Building Pinecil V2

To build the Pinecil V2 firmware, you can use a Docker container that provides a consistent development environment across different operating systems, including Windows with WSL2. Here's how to do it:

### Prerequisites

Docker Desktop: Install the latest version of Docker Desktop for your operating system from the official website.

On Windows follow the instructions on the official documentation to install 'Windows Subsystem for Linux' (WSL2).

### Building Steps

1. Clone the repository, initialize and update submodules:

    ```sh
    git clone --recurse-submodules https://github.com/Ralim/IronOS.git
    ```

2. Start the Docker container with the development environment:

    ```sh
    cd IronOS
    ./scripts/deploy.sh
    ```

    This script will build a Docker image and run a container with the necessary tools to build the firmware.

3. Build the firmware for Pinecil V2:

    ```sh
    cd source/
    ./build.sh -l EN -m Pinecilv2
    ```

    This command will compile the firmware with English language support for Pinecil V2 board.

4. Find the firmware artifacts:
    After the build completes successfully, you can find the firmware artifacts in the `source/Hexfile` directory.
