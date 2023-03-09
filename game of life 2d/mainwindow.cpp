#include "mainwindow.h"

#include <iostream>
#include <QMouseEvent>
#include <cmath>

#include "animator.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QGLWidget(parent)
{

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    this->resize(1000,800);

    int n(100),m(100);
    grid = Grid2D(n,m);
//    for(int i=0; i<n; i++)
//    {
//        for(int j=0; j<40; j++)
//        {
//            grid(i,j).alive = (rand()%(100)>50);
//        }

//        for(int j=60; j<m; j++)
//        {
//            grid(i,j).alive = (rand()%(100)>50);
//        }
//    }

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            grid(i,j).alive = (rand()%(n+m)>(i+j));
        }
    }

//    grid(n/2+1,m/2+0).alive = true;
//    grid(n/2+2,m/2+1).alive = true;
//    grid(n/2+0,m/2+2).alive = true;
//    grid(n/2+1,m/2+2).alive = true;
//    grid(n/2+2,m/2+2).alive = true;


//    std::cout << "Init: " << grid.H << " " << grid.W << std::endl;

}

MainWindow::~MainWindow()
{
}

void MainWindow::runAnimation()
{
    Animator * worker = new Animator();
    QThread * thread = new QThread();

    connect(worker, &Animator::updateSignal,
            this, &MainWindow::updateFrame);
    connect(thread, &QThread::finished,
            worker, &QObject::deleteLater);
    connect(thread, &QThread::started,
            worker, &Animator::doWork);

    worker->moveToThread(thread);
    thread->start();
}

void MainWindow::initializeGL()
{
    // Set up the rendering context, load shaders and other resources, etc.:

    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);  // устанавливает режим проверки глубины пикселей
    // устанавливаем режим, когда строятся только внешние поверхности
//    glShadeModel(GL_FLAT); // откл сглаживание цветов

    glClearColor(0.8,0.8,0.8,1);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);

//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_COLOR_MATERIAL);
//    glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
//    glEnable(GL_NORMALIZE);
}

void MainWindow::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat d = 3;
    GLfloat xa(-d), xb(d), ya(-d), yb(d), za(-d), zb(d);

    GLfloat ratio = (GLfloat)h/(GLfloat)w;
    if(w>=h)
        glOrtho(xa/ratio, xb/ratio, ya, yb, za, zb);
    else glOrtho(xa, xb, ya*ratio, yb*ratio, za, zb);

    glViewport(0, 0, (GLint)w, (GLint)h);

}

float x_crd(float p)
{
    return cos(p);
}

float y_crd(float p)
{
    return sin(p) * ( 1 + exp( -abs(p*(p-2*M_PI)/0.4) ) );
}

float z_crd(float p)
{
    return 0.4 * sin(15*p) * exp( -pow( (p-M_PI)/0.5 ,2) )
            - 0.5 * exp( -abs(p*(p-2*M_PI)) );
}

void MainWindow::paintGL()
{

    QVector<GLfloat> vert, col;
    QVector<GLuint> ind;
    int n = grid.H,
        m = grid.W;
    GLfloat step = 3.0/n;
    GLfloat X0 = -n*step/2,
            Y0 = -m*step/2;
    GLuint index = 0;
    for(int i=0; i<n; i++)
        for(int j=0; j<m; j++)
        {
            GLfloat curX(X0+i*step),
                    curY(Y0+j*step);

            auto cur_cell = grid(i,j);
//            GLfloat r( (GLfloat)cur_cell.r ),
//                    g( (GLfloat)cur_cell.g ),
//                    b( (GLfloat)cur_cell.b );
            GLfloat c = (cur_cell.alive) ? 1.0 : 0.0;
            GLfloat r(c),g(c),b(c);

            vert.append({curX,curY,0});
            vert.append({curX+step,curY,0});
            vert.append({curX+step,curY+step,0});
            vert.append({curX,curY+step,0});

            col.append({r,g,b,r,g,b,r,g,b,r,g,b});

            for(int k=0; k<4; k++)
            {
                ind.push_back(index);
                index += 1;
            }
        }

    //

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth( 5 );

//    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, vert.begin());
        glColorPointer(3, GL_FLOAT, 0, col.begin());

        glDrawElements(GL_QUADS,
                       ind.size(),
                       GL_UNSIGNED_INT,
                       ind.begin());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

//    glPointSize(5);
//    glBegin(GL_POINTS);
//        glColor3f(0,0,0);
//        glVertex3f(lightPos[0],lightPos[1],lightPos[2]);
//    glEnd();

    glPushMatrix();

}

// ANIMATION

void MainWindow::updateFrame(int iter, int steps)
{
//    float D = 2;
//    float p = 2 * M_PI * (float) iter / (float) (steps-1);

//    lightPos[0] = D * cos(10*p);
//    lightPos[1] = D * sin(10*p);
//    lightPos[2] = D * ( p / M_PI - 1 );

    // game of life
    QVector<int> di = {-1,  0,  1, -1, 1, -1, 0, 1};
    QVector<int> dj = {-1, -1, -1,  0, 0,  1, 1, 1};

    Grid2D updGrid(grid.H,grid.W);
//    std::cout << grid.H << " " << grid.W << std::endl;

    for(int i=0; i<grid.H; i++)
        for(int j=0; j<grid.W; j++)
        {
            int an = 0; // number of alive neighbours
            for(int k=0; k<8; k++)
            {
                if( grid(i+di[k],j+dj[k]).alive )
                    an++;
            }

            if( !grid(i,j).alive && (an==3) )
            {
                updGrid(i,j).alive = true;
            }
            else if(grid(i,j).alive && (an<2 || an>3)){
                updGrid(i,j).alive = false;
            }
            else if(grid(i,j).alive && (an>1 && an<4)){
                updGrid(i,j).alive = true;
            }
        }
    grid._cells = updGrid._cells;

    updateGL();
}


// INTERACTION

void MainWindow::mousePressEvent(QMouseEvent* pe){
    mPos = pe->pos();
}
void MainWindow::mouseMoveEvent(QMouseEvent* pe){
//    xRot += (0.23)*(pe->pos().y()-mPos.y());
//    int mult = (xRot>=0) ? 1 : -1;
//    yRot += (0.25)*mult*(pe->pos().x()-mPos.x());

    auto mMove = (pe->pos() - mPos);
    float angle = 0.3 * mMove.manhattanLength();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
        glRotatef(angle,(GLfloat)-mMove.y(),(GLfloat)mMove.x(),0);
    glPushMatrix();

    mPos = pe->pos();
    updateGL();
}
void MainWindow::mouseReleaseEvent(QMouseEvent *pe){
    mPos = pe->pos();
}

void MainWindow::wheelEvent(QWheelEvent *pe){
    QPoint numDegrees = pe->angleDelta();
//    if (numDegrees.y() < 0)  zoomScale = zoomScale/1.1;
//    if (numDegrees.y() > 0)  zoomScale = zoomScale*1.1;
    float sc = (numDegrees.y() < 0) ? 1/1.1 : 1.1;
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glScalef(sc,sc,sc);
    glPushMatrix();
//    std::cout << "wheelEvent" << std::endl;
    updateGL(); // call paintGL()
}
