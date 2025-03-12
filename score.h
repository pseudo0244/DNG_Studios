#ifndef SCORE_H
#define SCORE_H

#include <QString>

class Score
{
public:
    Score() {}

    Score(const QString &t, const QString &c, const QString &le)
        : title(t), composer(c), lastEdited(le) {}

    QString title;
    QString composer;
    QString lastEdited;
};

#endif // SCORE_H
