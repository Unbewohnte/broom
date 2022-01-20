# Broom
## incurable hoarder`s helpful friend

```
         _
        //
       // 
      //
     // 
  /####/
 //////
///////
```

### Broom is a command line utility to locate and manage duplicate and empty files

---

## Installation

### "Compile it yourself" way

- clone this repository 

`git clone https://github.com/Unbewohnte/broom`

- proceed to the directory

- compile manually or with cmake (in `build/` run:)

`cmake .`

`cmake --build .`

compiled binary for your platform will be in the newly created `bin` directory, you are free to put it 
wherever you desire

if you're on GNU/Linux - you can run `install.sh` for broom to become system-wide accessible 


### "The lazy" way

- proceed [to the releases page](https://github.com/Unbewohnte/broom/releases) and get yourself a pre-compiled binary


---

## Usage

broom [FLAGS..] [COMMAND] [DIRECTORY]

[FLAGS]

- `-v` or `--version` -> print version information and exit
- `-h` or `--help` -> print this message and exit
- `-od` or `--output-directory` -> path to the directory to save results file in

[COMMANDS]

- `sweep` -> scan for duplicate files, REMOVE empty files and REPLACE other duplicates with symlinks
- `scan` -> scan and save results in a file without removing anything [DEFAULT]


[DIRECTORY] is the path to the directory that will be searched for duplicate files

### Examples

- `broom scan -od . ~/homework`
- `broom sweep ~/homework`

after the scan the results file will be saved in your current working directory, unless you specified it to be somewhere else. Scan results file contains
a list of duplicate files that are grouped together so you can see EXACTLY WHERE each duplicate is in the filesystem.

---

## License
GPLv3

---

## TODO
- Make it go  `P` A `R` A `L` L `E` L
- ~~Output approximate size that could be freed~~
- ~~Remove duplicates and create symlinks~~