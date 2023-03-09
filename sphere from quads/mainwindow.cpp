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

    this->resize(800,800);

    getSphere(vert,col,norm,ind);
    std::cout << ind.size() << " " << col.size() << std::endl;
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
    glShadeModel(GL_FLAT);

    glClearColor(0.8,0.8,0.8,1);

//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_ALPHA);

    // LIGHT:
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_COLOR_MATERIAL);
//    glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
//    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
//    glEnable(GL_NORMALIZE);

}

void MainWindow::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat d = 7;
    GLfloat xa(-d), xb(d), ya(-d), yb(d), za(-d), zb(d);

    GLfloat ratio = (GLfloat)h/(GLfloat)w;
    if(w>=h)
        glOrtho(xa/ratio, xb/ratio, ya, yb, za, zb);
    else glOrtho(xa, xb, ya*ratio, yb*ratio, za, zb);

    glViewport(0, 0, (GLint)w, (GLint)h);

}

float gauss(float x, float m, float s)
{
    return exp(-pow((x-m)/(s),2));
}

void getSphere(QVector<GLfloat> &vert,
               QVector<GLfloat> &col,
               QVector<GLfloat> &norm,
               QVector<GLint> &ind)
{
    int d = 5; // dividing cube's edge into d parts

    GLfloat A = 2; // cube's edge length
    vert.clear();
    col.clear();
    norm.clear();
    ind.clear();

    // setting the sphere points
    {
        GLint index_shift = 0; //(GLint) vert.size()/3;

        float angle_step = M_PI / (2*d);
        QVector<GLfloat> step = {0};
        for(int i=1; i<d+1; i++)
        {
            float angle_lower = -M_PI/4 + (i-1)*angle_step,
                    angle_upper = angle_lower + angle_step;
//            float add = (A/d);
            float add = (A/2)*(tan(angle_upper) - tan(angle_lower));

            step.push_back( step[i-1] + add );
        }

        // filling middle part
        {
            QVector<GLfloat> central_ring(0);
            { // filling central_ring
                GLfloat x0 = -A/2,
                        y0 = -A/2;
                for(int i=0; i<d; i++)
                {
                    central_ring.push_back(x0+step[i]); // x
                    central_ring.push_back(y0); // y
                    //            central_ring.push_back(z0); // z
                }
                x0 = A/2;
                for(int i=0; i<d; i++)
                {
                    central_ring.push_back(x0); // x
                    central_ring.push_back(y0+step[i]); // y
                    //            central_ring.push_back(z0); // z
                }
                y0 = A/2;
                for(int i=0; i<d; i++)
                {
                    central_ring.push_back(x0-step[i]); // x
                    central_ring.push_back(y0); // y
                    //            central_ring.push_back(z0); // z
                }
                x0 = -A/2;
                for(int i=0; i<d; i++)
                {
                    central_ring.push_back(x0); // x
                    central_ring.push_back(y0-step[i]); // y
                }
            }

            for(int i=0; i<d; i++) // i -- the number of tail
            {
                GLfloat upper_z = A/2 - step[i];
                int shift = i * (4*d) + index_shift;
                for(int j=0; j<4*d; j++)
                {
                    vert.append( { central_ring[2*j+0],
                                   central_ring[2*j+1],
                                   upper_z } );

                    // filling indecies
                    {
                        ind.push_back(shift + j);
                        ind.push_back(shift + 4*d + j);
                        ind.push_back(shift + 4*d + (j+1)%(4*d));
                        ind.push_back(shift + (j+1)%(4*d));
                    }
                }
            }
            for(int i=0; i<4*d; i++) // i -- the number of tail
            {
                GLfloat lower_z = -A/2;
                vert.append( { central_ring[2*i+0],
                               central_ring[2*i+1],
                               lower_z } );
            }
        }

        // filling upper and lower surfaces
        {

            QVector<GLfloat> hor_surf(0); // not shifted along Z square surface
            QVector<GLint> hor_surf_ind(0); // indices without shift

            // initializing a non-shifted surface
            {
                GLfloat x0 = -A/2,
                        y0 = -A/2;
                for(int i=0; i<d+1; i++) // ~row
                {
                    for(int j=0; j<d+1; j++) // ~column
                    {
                        hor_surf.push_back( x0 + step[i] ); // x
                        hor_surf.push_back( y0 + step[j] ); // y
                    }
                }
                for(int i=0; i<d; i++) // ~row
                {
                    for(int j=0; j<d; j++) // ~column
                    {
                        GLint index = i*(d+1) + j;
                        hor_surf_ind.append({ index,
                                              index+(d+1),
                                              index+(d+1)+1,
                                              index+1 });
                    }
                }
            }

            int S = hor_surf.size()/2; // points on the hor_surf = (d+1)^2

            // upper surface
            index_shift = (GLint) vert.size()/3;
            for(int i=0; i<S; i++)
            {
                vert.append({hor_surf[2*i+0],
                             hor_surf[2*i+1],
                             A/2}); // A/2 = z_upper
            }
            for(int i=0; i<hor_surf_ind.size(); i++)
                ind.push_back( index_shift + hor_surf_ind[i] );

            // lower surface
            index_shift = (GLint) vert.size()/3;
            for(int i=0; i<S; i++)
            {
                vert.append({hor_surf[2*i+0],
                             hor_surf[2*i+1],
                             -A/2}); // -A/2 = z_lower
            }
            for(int i=0; i<hor_surf_ind.size(); i++)
                ind.push_back( index_shift + hor_surf_ind[i] );
        }
    }

    //

    // Running through all vertices and rescaling their positions

    GLfloat R = 2;
    int N = vert.size()/3; // num of vertices
    auto length = [](GLfloat a, GLfloat b, GLfloat c)
    {
       return sqrt(a*a + b*b + c*c);
    };

    for(int i=0; i<N; i++)
    {
        auto l = length(vert[3*i],vert[3*i+1],vert[3*i+2]);
        vert[3*i+0] *= R/l;
        vert[3*i+1] *= R/l;
        vert[3*i+2] *= R/l;

        norm.append({ vert[3*i+0]/R,
                      vert[3*i+1]/R,
                      vert[3*i+2]/R } );

//        float phi = atan(vert[3*i+1]/vert[3*i+0]);
//        if( vert[3*i+0]<0 )
//            phi = M_PI + phi;
//        float theta = acos(vert[3*i+2]/R);

        GLfloat p = (GLfloat) i / (GLfloat) N;
        GLfloat r = (GLfloat) abs(sin(23*p)),
                g = (GLfloat) 1-abs(sin(47*p)),
                b = (GLfloat) abs(cos(51*p)); //abs(sin(17*theta));
        col.append({ r, g, b });
    }

    /*
    int quadsNum = ind.size()/4;
    for(int i=0; i<quadsNum; i++)
    {
//        float p = i/(quadsNum-1);
//        GLfloat r = (GLfloat) p, //abs(sin(2*M_PI*p)),
//                g = (GLfloat) p,
//                b = (GLfloat) p;
//        r = 1; g = 0; b = 0.5;
        col.append({ 0,0,0,
                     1,0,0,
                     0,1,0,
                     0,0,1 });
    }
    */
}

void MainWindow::paintGL()
{
    //

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth( 5 );

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, vert.begin());
//        glNormalPointer(GL_FLOAT, 0, norm.begin());
        glColorPointer(3, GL_FLOAT, 0, col.begin());

        glDrawElements(GL_QUADS,
                       ind.size(),
                       GL_UNSIGNED_INT,
                       ind.begin());

    glDisableClientState(GL_VERTEX_ARRAY);
//    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

//    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
//    glPointSize(8);
//    glBegin(GL_POINTS);
//        glColor3f(1,0,0);
//        glVertex3f(lightPos[0],lightPos[1],lightPos[2]);
//    glEnd();

    glPushMatrix();

}

// ANIMATION

void MainWindow::updateFrame(int iter, int steps)
{
    float D = 5;
    float p = 2 * M_PI * (float) iter / (float) (steps-1);

    lightPos[0] = D * cos(10*p);
    lightPos[1] = D * sin(10*p);
    lightPos[2] = D * ( p / M_PI - 1 );

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
