#include "Pong.h"
#include "moc_Pong.cpp"

#include <QFont>
#include <QColor>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QString>
#include <QRect>
#include <QSize>
#include <QPalette>

#include <cstdlib>
#include <cmath>
#include <cassert>

static const int kBarPad = 10;
static const int kDt = 10;

namespace {
    bool yintersect( const QRect& a, int b ){
        return a.top() <= b && b <= a.bottom();
    }

    bool yintersect( const QRect& a, const QRect& b ){
        return yintersect(a, b.top()) || yintersect(a, b.bottom());
    }
}

PongWidget::Ball::Ball():
    QRect( 0, 0, 20, 20 )
{}

void
PongWidget::keyPressEvent( QKeyEvent* ev ){
    if( ev->modifiers() )
        return QWidget::keyPressEvent( ev );

    switch( ev->key() ){
    case Qt::Key_Escape:
        mode_ = 0;
        timerStart();
    break;
    case Qt::Key_Space:
        if( mode_ < 2 )
            mode_ = 2;
        else
            if( timer_->isActive() )
                timer_->stop();
            else
                timerStart();
    break;
    case Qt::Key_W:
        player_vel_[0] = -player_speed_;
        player_move_[0] = true;
    break;
    case Qt::Key_S:
        player_vel_[0] = player_speed_;
        player_move_[0] = true;
    break;
    case Qt::Key_Up:
        player_vel_[1] = -player_speed_;
        player_move_[1] = true;
    break;
    case Qt::Key_Down:
        player_vel_[1] = player_speed_;
        player_move_[1] = true;
    break;
    default:
        QWidget::keyPressEvent( ev );
    };
}

void
PongWidget::keyReleaseEvent( QKeyEvent* ev ){
    if( ev->modifiers() )
        return QWidget::keyReleaseEvent( ev );

    switch( ev->key() ){
    case Qt::Key_W:
    case Qt::Key_S:
        player_move_[0] = false;
    break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        player_move_[1] = false;
    break;
    default:
        QWidget::keyReleaseEvent( ev );
    };
}

void
PongWidget::updateState(){
    if( mode_ == 0){
        init();
        mode_ = 1;
    }

    if( mode_ == 2 ){
        score_[0] = score_[1] = 0;
        newBall(-1);
        mode_ = 3;
    }

    // compute next player positions
    for( int i = 0; i < 2; ++i ){
        player_[i].translate( 0, player_vel_[i] * kDt );
        if( !player_move_[i] )
            player_vel_[i] *= player_slowdown_;
        if( fabs(player_vel_[i]) < 1e-6 )
            player_vel_[i] = 0.0;
        if( player_[i].top() < 0 ){
            player_[i].moveTop( 0 );
            player_vel_[i] = 0;
        }else
        if( player_[i].bottom() > height() ){
            player_[i].moveBottom( height() );
            player_vel_[i] = 0;
        }
    }

    // compute next ball position
    ball_.propagate( kDt );

    // wall reflection
    if( ball_.top() < 0 ){
        ball_.moveTop( -ball_.top() );
        ball_.vel.ry() *= -1;
    }else
    if( ball_.bottom() > height() ){
        ball_.moveBottom( 2 * height() - ball_.bottom() );
        ball_.vel.ry() *= -1;
    }

    if( ball_.vel.x() < 0 ){
        // check for collision with player 0
        if( ball_.left() < player_[0].right() ){
            qreal dt = (ball_.left() - player_[0].right()) / ball_.vel.x();
            if( dt < kDt ){
                ball_.propagate( -dt );
                if( yintersect( player_[0], ball_ ) ){
                    ball_.vel.rx() *= -1;
                    ball_.vel.ry() += player_vel_[0] * player_push_;
                }
                ball_.propagate( kDt - dt );
            }
        }        
        // check loss
        if( ball_.right() < 0 ){
            newBall( 0 );
            score_[1] += 1;
        }
    }else{
        // check for collision with player 1
        if( ball_.right() > player_[1].left() ){
            qreal dt = (ball_.right() - player_[1].left()) / ball_.vel.x();
            if( dt < kDt ){
                ball_.propagate( -dt );
                if( yintersect( player_[1], ball_ ) ){
                    ball_.vel.rx() *= -1;
                    ball_.vel.ry() += player_vel_[1] * player_push_;
                }
                ball_.propagate( kDt - dt );
            }
        }        
        // check loss
        if( ball_.left() > width() ){
            newBall( 1 );
            score_[0] += 1;
        }
    }

    ball_.vel.rx() *= ball_speedup_;
}

void
PongWidget::newBall( int player ){
    // random angle
    qreal angle = 0.4 * (2.0 * (qreal(rand()) / RAND_MAX) - 1.0);

    switch( player ){
    case -1:
        ball_.moveCenter( QPoint( width() / 2, height() / 2 ) );
        ball_.vel.setX( ball_speed_ * cos( angle ) * (2 * (rand() % 2) - 1) );
        ball_.vel.setY( ball_speed_ * sin( angle ) );
    break;
    case 0:
        ball_.moveTo( player_[0].right(),
                      player_[0].center().y() );
        ball_.vel.setX( ball_speed_ * cos( angle ) );
        ball_.vel.setY( ball_speed_ * sin( angle ) + player_vel_[0] );
    break;
    case 1:
        ball_.moveTo( player_[1].left() - ball_.width(),
                      player_[1].center().y() );
        ball_.vel.setX( -ball_speed_ * cos( angle ) );
        ball_.vel.setY( ball_speed_ * sin( angle ) + player_vel_[1] );
    break;
    default:
        assert(!"should never happen");
    }
}

void
PongWidget::init(){
    for( int i = 0; i < 2; ++i ){
        player_[i].moveTo( 0, (height() - player_[i].height()) / 2 );
        player_vel_[i] = 0;
    }
    ball_.vel *= 0.0;
    ball_.moveLeft( -ball_.width() );
}

void
PongWidget::paintEvent( QPaintEvent* ev ){
    updateState();

    QPainter p(this);
    QColor fg( 250, 250, 250 );
    QPen pen( fg );
    pen.setStyle( Qt::DashLine );
    pen.setWidth( 6 );
    p.setPen( pen );

    if( mode_ == 1 ){
        QFont f;
        f.setPointSize( 16 );
        p.setFont( f );
        p.drawText( QRectF( width() / 2 + kBarPad,
                            0, 300, height() ),        
                    Qt::AlignVCenter,
                    "Reset: Escape\n"
                    "Start/Pause: Space\n"
                    "Left player: W/S\n"
                    "Right player: Up/Down" );
    }

    // middle line
    p.drawLine( width() / 2, 0, width() / 2, height() );

    // score
    QFont f;
    f.setPointSize( 50 );
    p.setFont( f );
    p.drawText( 0, 0, width() / 2 - kBarPad, 100, Qt::AlignRight,
                QString::number(score_[0]) );
    p.drawText( width() / 2 + kBarPad, 0, width() / 2 - kBarPad, 100, Qt::AlignLeft,
                QString::number(score_[1]) );

    // player bars
    p.setPen( Qt::NoPen );
    p.setBrush( fg );
    player_[0].moveLeft( kBarPad );
    player_[1].moveRight( width() - kBarPad );
    p.drawRect( player_[0] );
    p.drawRect( player_[1] );

    // ball
    p.drawRect( ball_ );
}

void
PongWidget::timerStart(){
    timer_->start( kDt );    
}

PongWidget::PongWidget():
    timer_( new QTimer(this) ),
    mode_(0){
    for( int i = 0; i < 2; ++i ){
        score_[i] = 0;
        player_[i].setSize( QSize(24, 100) );
        player_vel_[i] = 0;
        player_move_[i] = false;
    }

    setPalette( QPalette( Qt::black ) );
    setAutoFillBackground( true );

    connect( timer_, SIGNAL(timeout()), SLOT(update()) );
    timerStart();
}

Pong::Pong(){
    addSetting( "Ball speed", RangeSetting(0.1, 1.0, 90, 0.3) );
    addSetting( "Ball speedup", RangeSetting(1.0, 1.01, 100, 1.0005) );
    addSetting( "Player speed", RangeSetting(0.1, 1.0, 90, 0.6) );    
    addSetting( "Player slowdown", RangeSetting(0.0, 0.9, 90, 0.75) );
    addSetting( "Player push", RangeSetting(0.0, 1.0, 10, 0.3) );
}

QWidget* Pong::create() const{
    return new PongWidget();
}

void Pong::updateFrame( I3FramePtr ){
    PongWidget& pw = widget<PongWidget>();
    pw.ball_speed_ = setting<RangeSetting>( "Ball speed" );
    pw.ball_speedup_ = setting<RangeSetting>( "Ball speedup" );
    pw.player_speed_ = setting<RangeSetting>( "Player speed" );
    pw.player_slowdown_ = setting<RangeSetting>( "Player slowdown" );
    pw.player_push_ = setting<RangeSetting>( "Player push" );
}
