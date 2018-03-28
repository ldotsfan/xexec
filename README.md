# xexec - XBE x86 loader & emulator
`xexec`:
* is an Xbox executable loader & emulator for x86/x64 Linux;
* handles direct execution of x86 code, executing Xbox game code directly on the CPU, in userspace Linux;
* may be used in a `binfmt_misc` configuration (not at this time).

All Windows kernel calls from Xbox game code are translated into POSIX syscalls, with no dependency on Wine.

This project stems from an idea that _blueshogun_ had written on his blog, in January 2017:
* _**The macOS Experiment**_: [Part 1](http://shogun3d-cxbx.blogspot.com/2017/01/the-macos-experiment-part-1.html), [Part 2](http://shogun3d-cxbx.blogspot.com/2017/01/around-beginning-of-new-year-i.html)

[`loader.c`](https://github.com/haxar/cxbx-shogun/blob/wine/build/wine/loader.c) is the original code _haxar_ wrote in 2012, before Cxbx-R. It was an attempt to run Cxbx in Wine, which is currently not necessary.

The current state of `xexec` is best run through Turok, at this time of writing. The game was used to initially build the POSIX translation layer around its Windows kernel calls. It was chosen as one of the first games playable on Cxbx.

## Dependencies
Host i686 Debian/Ubuntu:
* `$ sudo apt install build-essential mesa-common-dev libglew-dev libx11-dev`

Host amd64 Debian/Ubuntu:
* `$ sudo apt install build-essential gcc-multilib mesa-common-dev:i386 libglew-dev:i386 libx11-dev:i386`

## Building
* `$ cd xexec/`
* `$ make`

## Execution
Debugging w/ `gdb`:
* `$ xbe=D/default.xbe make gdb`

Debugging w/ `gdb` before jumping into the entry point:
* `$ xbe=D/default.xbe make wait`

Standalone:
* `$ ./xexec D/default.xbe`

Make `D` a symbolic link to your Turok directory, in this source directory, via:
* `$ ln -s /path/to/turok/directory D`
* `lrwxrwxrwx  1 1000 1000        14 Jan  6 14:03 D -> ../turok/`
* When done: `$ rm D`

`xexec` will translate partition paths into local paths, as directory `D/`, `E/`, et al.

Upon execution in the terminal, you will see the very helpful stack trace. Turok should deterministically execute & wait at the end here, in which you'll have to `Ctrl-C`, and find a solution to an APU register:
```
09:05:51.163811: enter: [0xfc146] --> xboxkrnl_NtReadFile()
09:05:51.163818:     var:   in: *FileHandle = 0xeba030b0 (3953143984)
09:05:51.163826:     var:  str: *FileHandle->path = 'D/Data/Sound/dsstdfx.bin' (24) [0xeba030b0]
09:05:51.163833:     var:   in: *Event = 0x0 (0)
09:05:51.163839:     var:   in: *ApcRoutine = 0x0 (0)
09:05:51.163845:     var:   in: *ApcContext = 0x0 (0)
09:05:51.163852:     var:  out: *IoStatusBlock = 0xec3fedb4 (3963612596)
09:05:51.163866:     var:  out: *IoStatusBlock->Status = 0x1faa88 (2075272)
09:05:51.163872:     var:  out: *IoStatusBlock->Information = 0xffffffff (4294967295)
09:05:51.163878:     var:  out: *Buffer = 0xeb600060 (3948937312)
09:05:51.163884:     var:   in: *Length = 0xdf4 (3572)
09:05:51.163890:     var:   in: *ByteOffset = 0x0 (0)
09:05:51.163899:     var: dump: *IoStatusBlock->Status = 0x0 (0)
09:05:51.163906:     var: dump: *IoStatusBlock->Information = 0xdf4 (3572)
09:05:51.163912:     var: dump: *ret = 0x0 (0)
09:05:51.163926: leave: [0xfc14c] <-- xboxkrnl_NtReadFile()
09:05:51.163941: enter: [0x1ae5f7] --> xboxkrnl_RtlEnterCriticalSection()
09:05:51.163949:     var:   in: *CriticalSection = 0x1cb6a8 (1881768)
09:05:51.163962:     var:   in: **CriticalSection = 0x40001 (262145)
09:05:51.163978: leave: [0x1ae5fd] <-- xboxkrnl_RtlEnterCriticalSection()
09:05:51.163994: enter: [0x1b2024] --> xboxkrnl_KfRaiseIrql()
09:05:51.164002:     var: stub: *NewIrql = 0x2 (2)
09:05:51.164017: leave: [0x1b202a] <-- xboxkrnl_KfRaiseIrql()
09:05:51.164034: enter: [0x1b203c] --> xboxkrnl_KfLowerIrql()
09:05:51.164042:     var: stub: *NewIrql = 0x0 (0)
09:05:51.164056: leave: [0x1b2042] <-- xboxkrnl_KfLowerIrql()
```
You can save a full dump, as:
* `./xexec D/default.xbe 2>&1 | tee "$(date +%Y%m%d-%H%M%S)"-xexec.log`

