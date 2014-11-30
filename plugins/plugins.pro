TEMPLATE = subdirs

!android:!blackberry:!ios {
    contains(QT_CONFIG, dbus) {
        SUBDIRS += kmail
        SUBDIRS += pidgin
    }

    SUBDIRS += shellscript
}
