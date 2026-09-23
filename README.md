<p align="center">
  <img src="res/public/Throned.png" width="96" alt="Throned">
</p>

<h1 align="center">Throned-Ex</h1>

<p align="center">
A Qt desktop proxy client powered by sing-box and Xray.<br>
Fork of <a href="https://github.com/troshkindm/throned">upstream</a> with bug fixes and improvements.
</p>

---

# Inherited features from the upstream project

Check [this file](README.upstream.md).

# Changes in this fork

## Program name and versioning

- Changed display name in window title bar and version number to v99.x.x, to be distinguished from the original program.
- Keep filename and path of program files the same as original, for compatibility.
- Changed upgrade checking URL accordingly.

## Fixed upstream bugs

- To clear profiles selections in main window, use shortcut key `Shift+ESC` instead of `ESC` since the latter does not work in all systems.
- If the user changes to a different routing profile via the *Routing* drop down menu, the bottom right stats/info widget in main window will display the new routing profile name correctly.
- For context menu of profile table, disable items Share/Delete/Clone if there is no selection.

### Fixed proxy bugs

For network requests from this software itself, the original upstream behavior is very confusing:

Requests are forced proxied even if the user has stopped the core/kernel when settings *Use proxy* is on, thus errors: `Request with proxy but no profile started.`;
Requests are forced proxied even if settings *Use proxy* is off when the core/kernel is running,
thus causing failures if the requested resource is in LAN, e.g. if you have locally deployed subscription convert and merge services.

Fixed behavior for network requests from this software itself:

Force proxy sites used to resolve exit IP and location;
For other traffic, direct connect if settings *Use proxy* is off or the core/kernel is not running, else go through user-defined routing settings.
You can add your local IP/domain to the routing settings, so the software can direct connect to fetch resources in your LAN while keep using proxy to fetch other resources.

### TODO Unfixed upstream bugs

- Restart core/kernel does not work.

## New features

### Keyboard shortcuts for main window

Changed `Delete` / `Backspace` key behavior in *Hotkey Settings*:

Allow single `Delete` key or combination of modifiers + `Delete` / `Backspace` key to be set. Use single `Backspace` key to unset.

Removed keyboard shortcuts:

- *Diagnostics*: Removed `Ctrl+Shift+D` due to conflict with default keyboard shortcuts for *Remove Duplicates*.

Added/changed *customizable* keyboard shortcuts and their default values:

- Exit program: `Ctrl+Q`.
- Restart program: `Ctrl+Shift+Q` (Removed default value for Scan QR Code).
- Open basic settings: `F12`.
- Open hotkey settings: `Ctrl+F12`.
- Open routing settings: `Ctrl+Shift+F12`.
- New group: `Ctrl+Shift+N`.
- Delete current group: `Ctrl+Shift+Del`.
- Edit current group: `Ctrl+F5`.
- Manage groups: `Ctrl+Shift+F5`.
- Delete selected profile: `Delete`.

Added/changed *non-customizable* keyboard shortcuts:

- Start core/kernel: `Return` / `Enter` (Not changed).
- Stop core/kernel: `Backspace` (Changed from `Ctrl+S`).
