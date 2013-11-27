TEMPLATE = subdirs

contains(QT_CONFIG, dbus) {
    SUBDIRS += kmail
    # SUBDIRS += pidgin
}
