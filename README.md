# xexec - XBE x86 loader & emulator
`xexec`:
* is an Xbox executable loader & emulator for x86/x64 Linux;
* handles direct execution of x86 code, executing Xbox game code directly on the CPU, in userspace Linux;
* may be used in a `binfmt_misc` configuration (not at this time).

All Windows kernel calls from Xbox game code are translated into POSIX syscalls, with no dependency on Wine.

This project stems from an idea that _blueshogun_ had written on his blog, in January 2017:
* _**The macOS Experiment**_: [Part 1](http://shogun3d-cxbx.blogspot.com/2017/01/the-macos-experiment-part-1.html), [Part 2](http://shogun3d-cxbx.blogspot.com/2017/01/around-beginning-of-new-year-i.html)

[`loader.c`](https://github.com/haxar/cxbx-shogun/blob/wine/build/wine/loader.c) is the original code _haxar_ wrote in 2012, before Cxbx-R surfaced. It was an attempt to run Cxbx in Wine, which is currently not necessary.

The current state of `xexec` is best run through Turok, at this time of writing. The game was used to initially bootstrap & write POSIX translation code for its Windows kernel calls. It was chosen as one of the first games playable on Cxbx.

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
23:11:37.472792: enter: [0xfc146] --> xboxkrnl_NtReadFile()
23:11:37.472799:     var:   in: *FileHandle = 0x8009cf0 (134257904)
23:11:37.472807:     var:  str: *FileHandle->path = 'D/Data/Sound/dsstdfx.bin' (24) [0x8009cf0]
23:11:37.472815:     var:   in: *FileHandle->fd = 0x5 (5)
23:11:37.472822:     var:   in: *FileHandle->dir = 0x0 (0)
23:11:37.472830:     var:   in: *Event = 0x0 (0)
23:11:37.472838:     var:   in: *ApcRoutine = 0x0 (0)
23:11:37.472845:     var:   in: *ApcContext = 0x0 (0)
23:11:37.472853:     var:  out: *IoStatusBlock = 0xec0fedb4 (3960466868)
23:11:37.472860:     var:  out: *IoStatusBlock->Status = 0x1faa88 (2075272)
23:11:37.472868:     var:  out: *IoStatusBlock->Information = 0xffffffff (4294967295)
23:11:37.472876:     var:  out: *Buffer = 0xd88060 (14188640)
23:11:37.472884:     var:   in: *Length = 0xdf4 (3572)
23:11:37.472891:     var:   in: *ByteOffset = 0x0 (0)
23:11:37.472900:     var: dump: *lseek(FileHandle->fd, 0, 1) = 0x0 (0)
23:11:37.472911:     var: dump: *IoStatusBlock->Status = 0x0 (0)
23:11:37.472918:     var: dump: *IoStatusBlock->Information = 0xdf4 (3572)
23:11:37.472925:     var: dump: *ret = 0x0 (0)
23:11:37.472939: leave: [0xfc14c] <-- xboxkrnl_NtReadFile()
23:11:37.472952: enter: [0x1ae5f7] --> xboxkrnl_RtlEnterCriticalSection()
23:11:37.472959:     var:   in: *CriticalSection = 0x1cb6a8 (1881768)
23:11:37.472967:     var:   in: **CriticalSection = 0x40001 (262145)
23:11:37.472980: leave: [0x1ae5fd] <-- xboxkrnl_RtlEnterCriticalSection()
23:11:37.472993: enter: [0x1b2024] --> xboxkrnl_KfRaiseIrql()
23:11:37.473000:     var: stub: *NewIrql = 0x2 (2)
23:11:37.473014: leave: [0x1b202a] <-- xboxkrnl_KfRaiseIrql()
23:11:37.473026: enter: [0x1b203c] --> xboxkrnl_KfLowerIrql()
23:11:37.473033:     var: stub: *NewIrql = 0x0 (0)
23:11:37.473046: leave: [0x1b2042] <-- xboxkrnl_KfLowerIrql()
```
You can save a full dump, as:
* `./xexec D/default.xbe 2>&1 | tee "$(date +%Y%m%d-%H%M%S)"-xexec.log`

No framebuffer to see yet. There is no user support at this time.
