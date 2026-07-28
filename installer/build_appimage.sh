#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
cd "$PROJECT_ROOT"

echo "=========================================="
echo " Building LuminaClient AppImage for Linux "
echo "=========================================="

# ۱. پاکسازی فایل‌های قبلی
rm -rf installer/packaging_dir
mkdir -p installer/packaging_dir


# ۲. نصب imagemagick در صورت نیاز
if ! command -v convert &> /dev/null; then
    echo "Installing imagemagick..."
    sudo apt update && sudo apt install -y imagemagick
fi


# ۳. Build Release
mkdir -p build-release
cd build-release

cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

cd "$PROJECT_ROOT"


# ۴. آماده سازی فایل‌ها
cp build-release/Client/LuminaClient installer/packaging_dir/LuminaClient


echo "Resizing icon..."
convert Client/resources/Lumina_Logo.png \
    -resize 512x512 \
    installer/packaging_dir/Lumina_Logo.png


cat <<EOF > installer/packaging_dir/luminaclient.desktop
[Desktop Entry]
Type=Application
Name=Lumina EBook Client
Exec=LuminaClient
Icon=Lumina_Logo
Categories=Office;Qt;
Terminal=false
EOF



# ۵. دانلود ابزارها
mkdir -p installer/tools


if [ ! -f "installer/tools/linuxdeploy-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy..."

    wget -q \
    https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage \
    -O installer/tools/linuxdeploy-x86_64.AppImage

    chmod +x installer/tools/linuxdeploy-x86_64.AppImage
fi



if [ ! -f "installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    echo "Downloading Qt plugin..."

    wget -q \
    https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage \
    -O installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage

    chmod +x installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage
fi



# ۶. تنظیمات Qt
export QMAKE=${QMAKE:-/usr/bin/qmake6}

export EXTRA_QT_PLUGINS="multimedia,mediaservice,playlistformats"

export QML_SOURCES_PATHS="$PROJECT_ROOT/Client"

export QML2_IMPORT_PATH="/usr/lib/x86_64-linux-gnu/qt6/qml"



cd installer/packaging_dir



# ساخت AppDir اولیه
../tools/linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    -e LuminaClient \
    -i Lumina_Logo.png \
    -d luminaclient.desktop \
    --plugin qt



# ۷. اضافه کردن QML Modules
echo "Bundling Qt6 QML modules..."

QT_QML_PATH="/usr/lib/x86_64-linux-gnu/qt6/qml"

mkdir -p AppDir/usr/qml


if [ -d "$QT_QML_PATH/QtQml" ]; then
    cp -r "$QT_QML_PATH/QtQml" AppDir/usr/qml/
fi


if [ -d "$QT_QML_PATH/QtQuick" ]; then
    cp -r "$QT_QML_PATH/QtQuick" AppDir/usr/qml/
fi



# ۸. GStreamer plugins
GST_PLUGIN_PATH_SYS=$(pkg-config --variable=pluginsdir gstreamer-1.0 2>/dev/null || echo "/usr/lib/x86_64-linux-gnu/gstreamer-1.0")


if [ -d "$GST_PLUGIN_PATH_SYS" ]; then

    echo "Bundling GStreamer plugins..."

    mkdir -p AppDir/usr/lib/gstreamer-1.0

    cp -r "$GST_PLUGIN_PATH_SYS"/* \
    AppDir/usr/lib/gstreamer-1.0/ \
    2>/dev/null || true

fi



# ۹. AppRun hooks
mkdir -p AppDir/apprun-hooks


cat <<'EOF' > AppDir/apprun-hooks/gstreamer.sh
export GST_PLUGIN_SYSTEM_PATH_1_0="${APPDIR}/usr/lib/gstreamer-1.0"
export GST_PLUGIN_SCANNER="${APPDIR}/usr/lib/gstreamer-1.0/gst-plugin-scanner"
EOF



# ۱۰. ساخت نهایی AppImage
rm -f AppDir/AppRun


../tools/linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --output appimage



echo "=========================================="
echo " SUCCESS! AppImage created "
echo " Location: installer/packaging_dir/"
echo "=========================================="
