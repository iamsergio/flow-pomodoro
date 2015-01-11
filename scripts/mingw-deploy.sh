QT=/data/extra2/installation/qt/x86_64-mingw-5.4/

rm -rf flow &> /dev/null
mkdir flow
cd flow

cp $QT/bin/Qt5Core.dll .
cp $QT/bin/Qt5Gui.dll .
cp $QT/bin/Qt5Qml.dll .
cp $QT/bin/Qt5Quick.dll .
cp $QT/bin/Qt5Widgets.dll .
cp $QT/bin/Qt5Network.dll .
cp $QT/bin/Qt5DBus.dll .

cp /usr/i686-w64-mingw32/bin/libgcc_s_sjlj-1.dll .
cp /usr/i686-w64-mingw32/bin/libstdc++-6.dll .
cp /usr/i686-w64-mingw32/bin/libwinpthread-1.dll .
cp /data/opt/mingw32/bin/opengl32.dll .


rsync -av $QT/plugins/platforms/ platforms/
rsync -av $QT/qml/QtQml/ QtQml/
rsync -av $QT/qml/QtQuick/ QtQuick/
rsync -av $QT/qml/QtQuick.2/ QtQuick.2/

cp ../flow.exe .
mkdir plugins
cp ../plugins/*.dll plugins

cd ..

zip -r flow.zip flow/
