FLOW_SRC_DIR=/data/extra/sources/flow-pomodoro/
FLOW_BUILD_DIR=/data/extra/sources/flow-pomodoro/build/
FLOW_BUILD_LOG=$FLOW_SRC_DIR/build.log
MAKECOMMAND="make -j10"

echo "This is a private script, don't use it\n"

set -e # abort if any command fails
#-------------------------------------------------------------------------------
echo "1. Building g++ in source"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
qmake &> $FLOW_BUILD_LOG
$MAKECOMMAND &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------
echo "2. Building g++ shadow build"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
mkdir $FLOW_BUILD_DIR &> $FLOW_BUILD_LOG
cd $FLOW_BUILD_DIR &> $FLOW_BUILD_LOG
qmake $FLOW_SRC_DIR &> $FLOW_BUILD_LOG
$MAKECOMMAND &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------
echo "3. Building Clang in source"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
qmake -spec linux-clang &> $FLOW_BUILD_LOG
$MAKECOMMAND &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------
echo "4. Building Blackberry in source"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
sh build-blackberry.sh &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------
echo "5. Building Android in source"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
sh build-android.sh &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------
# 6. MinGW in source
echo "6. Building MinGW in source"
cd $FLOW_SRC_DIR
git clean -fdx &> $FLOW_BUILD_LOG
sh scripts/build-mingw.sh &> $FLOW_BUILD_LOG
#-------------------------------------------------------------------------------

rm $FLOW_BUILD_LOG
cd $FLOW_SRC_DIR
git clean -fdx
echo "Success!"
