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

### Proxy issues

For network requests from this software itself, the original upstream behavior is very confusing:

Requests are forced proxied even if the user has stopped the core/kernel when settings *Use proxy* is on, thus errors: `Request with proxy but no profile started.`;
Requests are forced proxied even if settings *Use proxy* is off when the core/kernel is running,
thus causing failures if the requested resource is in LAN, e.g. if you have locally deployed subscription convert and merge services.

Fixed behavior for network requests from this software itself:

Force proxy sites used to resolve exit IP and location;
For other traffic, direct connect if settings *Use proxy* is off or the core/kernel is not running, else go through user-defined routing settings.
You can add your local IP/domain to the routing settings, so the software can direct connect to fetch resources in your LAN while keep using proxy to fetch other resources.
