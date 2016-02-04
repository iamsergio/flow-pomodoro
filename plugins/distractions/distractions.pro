TEMPLATE = subdirs

!android:!ios {
    contains(QT_CONFIG, dbus) {
        SUBDIRS += kmail
        SUBDIRS += pidgin
    }

    SUBDIRS += shellscript hosts
}
