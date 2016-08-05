#include "version.h"

QString Version::ver("1.0");//Change this line everytime you update

QString Version::GetVersion() {
    return ver;
}

bool Version::CheckVersion(const QString &version) {
    return version==ver;
}
