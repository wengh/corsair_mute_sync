; A system-wide mute toggle for Zoom Meetings.

action = %1%


; Zoom appears not to accept ControlSend when in the background, so
; we isolate the Zoom and current windows, switch over to Zoom, send
; its own mute-toggle hotkey, and then switch back.
;
; Get the current window
WinGet, active_window, ID, A
;
; First check if we're sharing our screen and capture the toolbar:
zoom_window := WinExist("ahk_class ZPFloatToolbarClass")
;
; If we aren't sharing our screen, pull the Zoom window:
if (zoom_window = "0x0") {
    zoom_window := WinExist("ahk_class ZPContentViewWndClass")
}
;
; Do we know we have a zoom_window? If not, bail.
if (zoom_window = "0x0") {
    ExitApp, 1
}
;
; Whichever we have, switch over to it:
WinActivate, ahk_id %zoom_window%
;
; Toggle Mute
Send %action%
;
; Go back
WinActivate ahk_id %active_window%