source /data/qt5-x86_64-mingw.source 5.3
git clean -fdx
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR/..
mkdir build
cd build
qmake .. && make -j10 || exit -1
sh ../scripts/mingw-deploy.sh || exit -1
