#!/bin/bash
set -e

# رفتن به پوشه اصلی پروژه
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
cd "$PROJECT_ROOT"

echo "=========================================="
echo " Building LuminaClient AppImage for Linux "
echo "=========================================="

# ۱. پاکسازی کامل فایل‌های قبلی
rm -rf installer/packaging_dir
mkdir -p installer/packaging_dir

# ۲. مطمئن شدن از نصب بودن imagemagick برای آیکون
if ! command -v convert &> /dev/null; then
    echo "Installing imagemagick for icon resizing..."
    sudo apt update && sudo apt install -y imagemagick
fi

# ۳. ساخت پوشه Release و کامپایل پروژه
mkdir -p build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
cd "$PROJECT_ROOT"

# ۴. کپی فایل اجرایی و آماده‌سازی آیکون و دسکتاپ
cp build-release/Client/LuminaClient installer/packaging_dir/LuminaClient

echo "Resizing icon..."
convert Client/resources/Lumina_Logo.png -resize 512x512 installer/packaging_dir/Lumina_Logo.png

cat <<EOF > installer/packaging_dir/luminaclient.desktop
[Desktop Entry]
Type=Application
Name=Lumina EBook Client
Exec=LuminaClient
Icon=Lumina_Logo
Categories=Office;Qt;
Terminal=false
EOF

# ۵. دانلود ابزارهای بسته‌بندی در صورت عدم وجود
mkdir -p installer/tools
if [ ! -f "installer/tools/linuxdeploy-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy..."
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -O installer/tools/linuxdeploy-x86_64.AppImage
    chmod +x installer/tools/linuxdeploy-x86_64.AppImage
fi

if [ ! -f "installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage" ]; then
    echo "Downloading linuxdeploy Qt plugin..."
    wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -O installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage
    chmod +x installer/tools/linuxdeploy-plugin-qt-x86_64.AppImage
fi

# ۶. تنظیمات Qt6 و آماده‌سازی AppDir
export QMAKE=${QMAKE:-/usr/bin/qmake6}
export EXTRA_QT_PLUGINS="multimedia,mediaservice,playlistformats"

cd installer/packaging_dir

# مرحله اول: ساخت ساختار اولیه AppDir توسط linuxdeploy
../tools/linuxdeploy-x86_64.AppImage \
  --appdir AppDir \
  -e LuminaClient \
  -i Lumina_Logo.png \
  -d luminaclient.desktop \
  --plugin qt

# ۷. تزریق پلاگین‌های GStreamer به درون AppDir
GST_PLUGIN_PATH_SYS=$(pkg-config --variable=pluginsdir gstreamer-1.0 2>/dev/null || echo "/usr/lib/x86_64-linux-gnu/gstreamer-1.0")
if [ -d "$GST_PLUGIN_PATH_SYS" ]; then
    echo "Bundling GStreamer plugins from $GST_PLUGIN_PATH_SYS..."
    mkdir -p AppDir/usr/lib/gstreamer-1.0
    cp -r $GST_PLUGIN_PATH_SYS/* AppDir/usr/lib/gstreamer-1.0/ 2>/dev/null || true
fi

# ۸. تنظیم اسکریپت AppRun با ساخت پوشه apprun-hooks
mkdir -p AppDir/apprun-hooks
cat <<'EOF' > AppDir/apprun-hooks/gstreamer.sh
export GST_PLUGIN_SYSTEM_PATH_1_0="${APPDIR}/usr/lib/gstreamer-1.0"
export GST_PLUGIN_SCANNER="${APPDIR}/usr/lib/gstreamer-1.0/gst-plugin-scanner"
EOF

# بازسازی مجدد AppRun استاندارد با در نظر گرفتن هوک جدید
rm -f AppDir/AppRun
../tools/linuxdeploy-x86_64.AppImage \
  --appdir AppDir \
  --output appimage

echo "=========================================="
echo " SUCCESS! AppImage created in installer/packaging_dir/ "
echo "=========================================="
