@echo off

echo Note: this will only work if Zoom mute/unmute hotkey is globally set to "Ctrl+Alt+Shift+Page Down"

corsair-mute-sync.exe --01 "autohotkey hotkey.ahk ^^^^!+{PgDn}" -c -p 20