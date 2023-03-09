#include "animator.h"
#include <cmath>
#include <QThread>

#include <iostream>

Animator::Animator(QObject *parent)
    : QObject{parent}
{

}

void Animator::doWork()
{
    QThread::msleep(3000);
    int N = 1000;
    // main process:
    for(int i=0; i<N; i++)
    {
        emit updateSignal(i,N);
        QThread::msleep(80);
    }
    std::cout << "done" << std::endl;
}
