#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QObject>
//#include <QGLWidget>
#include <QVector>

class Animator : public QObject
{
    Q_OBJECT
public:
    explicit Animator(QObject *parent = nullptr);
    void doWork();

signals:
    void updateSignal(int iter, int steps);

};

#endif // ANIMATOR_H
