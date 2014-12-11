source /data/qt5-x86_64.source 5.3 static
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR/..
git clean -fdx
mkdir build-osx
cd build-osx
qmake .. && make -j10 && sh ../scripts/osx-deploy.sh
