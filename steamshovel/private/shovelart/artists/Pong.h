#ifndef I3_SHOVEL_ARTIST_PONG_H
#define I3_SHOVEL_ARTIST_PONG_H

#include "shovelart/Artist.h"

#include <QWidget>
#include <QPoint>
#include <QPointF>
#include <QRect>
class QTimer;

class PongWidget: public QWidget {
    Q_OBJECT;

    QTimer* timer_;
    int mode_;
    int score_[2];
    QRect player_[2];
    qreal player_vel_[2];
    bool player_move_[2];
    struct Ball: public QRect {
        Ball();
        QPointF vel;
        void propagate( qreal dt ){
            translate( vel.x() * dt, vel.y() * dt );
        }
    } ball_;

protected:
    void keyPressEvent( QKeyEvent* ev );
    void keyReleaseEvent( QKeyEvent* ev );

    void updateState();
    void init();
    void newBall( int player );

    void paintEvent( QPaintEvent* ev );

    void timerStart();

public:
    qreal ball_speed_;
    qreal ball_speedup_;
    qreal player_speed_;
    qreal player_slowdown_;
    qreal player_push_;

    PongWidget();
};

class Pong: public WidgetArtist {
public:
    ARTIST_REQUIRED_TYPES();

    Pong();
    QWidget* create() const;
    void updateFrame( I3FramePtr );
};

REGISTER_ARTIST(Pong);

#endif
