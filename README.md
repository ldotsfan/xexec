# xexec - XBE x86 loader & emulator
[`xexec`](https://github.com/haxar/xexec):
* is an Xbox executable loader & emulator for x86/x64 Linux;
* handles direct execution of x86 code, executing Xbox game code directly on the CPU, in userspace Linux;
* may be used in a `binfmt_misc` configuration (not at this time).

All Windows kernel calls from Xbox game code are translated into POSIX syscalls, with no dependency on Wine & ReactOS.

This project stems from an idea that _blueshogun_ had written on his blog, in January 2017:
* _**The macOS Experiment**_: [Part 1](http://shogun3d-cxbx.blogspot.com/2017/01/the-macos-experiment-part-1.html), [Part 2](http://shogun3d-cxbx.blogspot.com/2017/01/around-beginning-of-new-year-i.html)

[`loader.c`](https://github.com/haxar/cxbx-shogun/blob/wine/build/wine/loader.c) is the original code _haxar_ wrote in 2012, before Cxbx-R surfaced. It was an attempt to run Cxbx in Wine, which is currently not necessary.

The current state of `xexec` is best run through Turok, at this time of writing. This particular game was used to initially bootstrap & write POSIX translation code around its Windows kernel calls. As one of the first games playable on Cxbx, it has been chosen for bootstrapping.

## Dependencies
Host i686 Debian/Ubuntu:
* `$ sudo apt install build-essential mesa-common-dev libglew-dev libx11-dev`

Host amd64 Debian/Ubuntu:
* `$ sudo apt install build-essential gcc-multilib mesa-common-dev:i386 libglew-dev:i386 libx11-dev:i386`

## Building
* `$ cd xexec/`
* `$ make`

## Execution
Standalone:
* `$ ./xexec D/default.xbe`

Debugging:
* `$ ./xexec -dd D/default.xbe`

Debugging w/ `gdb`:
* `$ xbe=D/default.xbe make gdb`

Debugging w/ `gdb` before jumping into the entry point:
* `$ xbe=D/default.xbe make wait`

Make `D` a symbolic link to your Turok directory, in this source directory, via:
* `$ ln -s /path/to/turok/directory D`
* `lrwxrwxrwx  1 1000 1000        14 Jan  6 14:03 D -> ../turok/`
* When done: `$ rm D`

`xexec` will translate partition paths into local paths, as directory `D/`, `E/`, et al.

Upon execution in the terminal with debugging enabled, you will see the very helpful stack trace. Turok should deterministically execute & wait at the end here, in which you'll have to `Ctrl-C`, and find a solution to an APU register:
```
16:23:44.248001: enter: [0x000fc146] --> xboxkrnl_NtReadFile()
16:23:44.248006:     var:   in: *FileHandle = 0x08009cf0 (134257904)
16:23:44.248011:     var:  str: *FileHandle->path = 'D/Data/Sound/dsstdfx.bin' (24) [0x08009cf0]
16:23:44.248016:     var:   in: *FileHandle->fd = 0x00000005 (5)
16:23:44.248020:     var:   in: *FileHandle->dir = 0x00000000 (0)
16:23:44.248025:     var:   in: *Event = 0x00000000 (0)
16:23:44.248029:     var:   in: *ApcRoutine = 0x00000000 (0)
16:23:44.248034:     var:   in: *ApcContext = 0x00000000 (0)
16:23:44.248039:     var:  out: *IoStatusBlock = 0xe9ffeda4 (3925863844)
16:23:44.248043:     var:  out: *IoStatusBlock->Status = 0x001faa88 (2075272)
16:23:44.248048:     var:  out: *IoStatusBlock->Information = 0xffffffff (4294967295)
16:23:44.248053:     var:  out: *Buffer = 0x00d88060 (14188640)
16:23:44.248057:     var:   in: *Length = 0x00000df4 (3572)
16:23:44.248062:     var:   in: *ByteOffset = 0x00000000 (0)
16:23:44.248068:     var: dump: *lseek(FileHandle->fd, 0, 1) = 0x00000000 (0)
16:23:44.248076:     var: dump: *IoStatusBlock->Status = 0x00000000 (0)
16:23:44.248081:     var: dump: *IoStatusBlock->Information = 0x00000df4 (3572)
16:23:44.248085:     var: dump: *ret = 0x00000000 (0)
16:23:44.248095: leave: [0x000fc14c] <-- xboxkrnl_NtReadFile()
16:23:44.248105: enter: [0x001ae5f7] --> xboxkrnl_RtlEnterCriticalSection()
16:23:44.248110:     var:   in: *CriticalSection = 0x001cb6a8 (1881768)
16:23:44.248117:     var:   in: **CriticalSection = 0x00040001 (262145)
16:23:44.248133: leave: [0x001ae5fd] <-- xboxkrnl_RtlEnterCriticalSection()
16:23:44.248143: enter: [0x001b2024] --> xboxkrnl_KfRaiseIrql()
16:23:44.248148:     var: stub: *NewIrql = 0x00000002 (2)
16:23:44.248157: leave: [0x001b202a] <-- xboxkrnl_KfRaiseIrql()
16:23:44.248167: enter: [0x001b203c] --> xboxkrnl_KfLowerIrql()
16:23:44.248172:     var: stub: *NewIrql = 0x00000000 (0)
16:23:44.248180: leave: [0x001b2042] <-- xboxkrnl_KfLowerIrql()
```
You can save a full non-DMA dump, via:
* `./xexec -dd D/default.xbe 2>&1 | tee "$(date +%Y%m%d-%H%M%S)"-xexec.log`

Or, as a XZ compressed dump (preferred), via:
* `./xexec -dd D/default.xbe 2>&1 | tee /proc/self/fd/2 | xz -c - > "$(date +%Y%m%d-%H%M%S)"-xexec.log.xz`
* Use `xzcat` to decompress the dump.

No framebuffer to see yet. There is no user support at this time.
