# corsair_mute_sync

```
Run commands when Corsair device microphone is muted or unmuted.

Usage:
  corsair_mute_sync [arguments]

Arguments:

  -h OR --help
    show this help message

  -0 "<command>"
    run <command> when mic turns off; defaults to "echo muted"

  -1 "<command>"
    run <command> when mic turns on; defaults to "echo unmuted"

  --01 "<command>"
    set both -0 and -1 to <command>

  -l
    list supported devices

  -d <device ID>
    specify the device to listen; required when multiple supported devices exist

  -p <interval>
    refresh device state every <interval> ms; defaults to 100

  -c
    do not call command in the initial poll
)";
```
