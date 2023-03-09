#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QOpenGLFunctions>

void getSphere(QVector<GLfloat> &vert,
               QVector<GLfloat> &col,
               QVector<GLfloat> &norm,
               QVector<GLint> &ind);

class MainWindow : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void runAnimation();

//    void initializeData();

protected:

    QPoint mPos;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    virtual void mousePressEvent(QMouseEvent* pe) override;   // методы обработки события мыши при нажатии клавиши мыши
    virtual void mouseMoveEvent(QMouseEvent* pe) override;    // методы обработки события мыши при перемещении мыши
    virtual void mouseReleaseEvent(QMouseEvent* pe) override; // методы обработки событий мыши при отжатии клавиши мыши
    virtual void wheelEvent(QWheelEvent* pe) override;

    GLfloat lightPos[4] = {0,0,5,0};

    QVector<GLfloat> vert, col, norm;
    QVector<GLint> ind;

public slots:
    void updateFrame(int iter, int steps);

};
#endif // MAINWINDOW_H
