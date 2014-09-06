for i in qml/*.qml; do
    if ! grep $i resources.qrc &> /dev/null ; then
        echo $i not in resources.qrc
        exit 1
    fi
done

exit 0
