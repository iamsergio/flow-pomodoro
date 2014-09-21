cd $1

for i in qml/*.qml; do
    if ! grep $i resources.qrc &> /dev/null ; then
        echo "error:" $i not in resources.qrc
        exit 1
    fi
done

for i in qml/*.qml; do
    if ! grep $i src.pro &> /dev/null ; then
        echo "error:" $i not in src.pro
        exit 1
    fi
done

exit 0
