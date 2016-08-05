#ifndef VERSION_H
#define VERSION_H

#include <QString>

class Version {
private:
    static QString ver;
public:
    static QString GetVersion();
    static bool CheckVersion(const QString &version);
};

#endif // VERSION_H
