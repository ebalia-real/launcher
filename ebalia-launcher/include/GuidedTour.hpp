#pragma once
#include <QWidget>
#include <functional>
class QLabel;
class QPushButton;
class QProgressBar;
class QFrame;
class QTimer;
class QToolButton;
class TourMascot;
class GuidedTour:public QWidget {
public:
 GuidedTour(QWidget *parent,std::function<void(int)> navigate);
 ~GuidedTour() override;
protected:
 bool eventFilter(QObject *watched,QEvent *event) override;
 void paintEvent(QPaintEvent *event) override;
private:
 void display();
 void place();
 void finish();
 // The step's text appears letter by letter while Silence talks, as in a game dialogue.
 void type();
 void showTyped();
 std::function<void(int)> m_navigate;
 int m_step=0,m_lastStep=-1,m_typed=0,m_pause=0,m_letters=0;
 QString m_text;
 QTimer *m_typing=nullptr;
 QToolButton *m_sound=nullptr;
 bool m_finished=false;
 QFrame *m_card=nullptr;
 TourMascot *m_mascot=nullptr;
 QLabel *m_title,*m_body,*m_count;
 QPushButton *m_previous,*m_next;
 QProgressBar *m_progress;
 QRect m_highlight;
};
