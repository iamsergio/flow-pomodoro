use qt-mingw 5.6
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR/..
mkdir build-mingw
cd build-mingw
qmake .. && make -j10 && sh ../scripts/mingw-deploy.sh
