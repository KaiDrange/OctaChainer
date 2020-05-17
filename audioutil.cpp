#include "audioutil.h"
#include <QFileInfo>

namespace AudioUtil {

QStringList audioExtensions = { "wav", "aif", "ogg", "flac", "iff", "svx" };

bool isAudioFileName(QString file)
{
    return audioExtensions.contains(QFileInfo(file).suffix().toLower());
}

QString audioFileFilter()
{
    QString f = "Audio Files (";
    for (int i = 0; i < audioExtensions.length(); i++)
    {
        if (i > 0) {
            f.append(" ");
        }
        f.append("*.");
        f.append(audioExtensions[i]);
    }
    f.append(")");
    return f;
}

}
