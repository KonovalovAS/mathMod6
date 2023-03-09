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
    int N = 3000;
    // main process:
    for(int i=0; i<N; i++)
    {
        emit updateSignal(i,N);
        QThread::msleep(5);
    }
    std::cout << "done" << std::endl;
}
