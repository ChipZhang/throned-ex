#!/bin/bash
set -e

VERSION="$1"
ARCH="$2"
SUFFIX=""
[[ $3 == "systemqt" ]] && SUFFIX="-system-qt"

PKG=$(mktemp -d)
trap 'rm -rf "$PKG"' EXIT
chmod 0755 "$PKG"
mkdir -p "$PKG/DEBIAN" "$PKG/opt"
cp -r "linux-$ARCH$SUFFIX" "$PKG/opt/Throned"
rm -f "$PKG/opt/Throned/Throned.debug"

# basic
cat >"$PKG/DEBIAN/control" <<-EOF
Package: throned
Version: $VERSION
Architecture: $ARCH
Maintainer: Throned contributors <troshkindm@users.noreply.github.com>
Depends: desktop-file-utils$([[ $3 == "systemqt" ]] && echo ", libqt6core6, libqt6gui6, libqt6network6, libqt6widgets6, qt6-qpa-plugins, qt6-wayland, qt6-gtk-platformtheme, qt6-xdgdesktopportal-platformtheme, libxcb-cursor0, fonts-noto-color-emoji")
Description: Qt based cross-platform GUI proxy configuration manager (backend: sing-box)
EOF

cat >"$PKG/DEBIAN/postinst" <<-EOF
cat >/usr/share/applications/Throned.desktop<<-END
[Desktop Entry]
Name=Throned
Comment=Qt based cross-platform GUI proxy configuration manager (backend: sing-box)
Exec=sh -c "PATH=/opt/Throned:\$PATH /opt/Throned/Throned -appdata"
Icon=/opt/Throned/Throned.png
Terminal=false
Type=Application
Categories=Network;Application;
END

update-desktop-database
EOF

chmod 0755 "$PKG/DEBIAN/postinst"

# desktop && PATH

dpkg-deb --root-owner-group --build "$PKG" Throned.deb
