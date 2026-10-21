#pragma once
#include <QImage>
#include <QWidget>
class QVariantAnimation;
// A skin on the player model, in 3D (drag to turn it, double-click to face it again) or as the flat front or back.
// Drawn with QPainter alone, so it works without OpenGL, in virtual machines and over remote desktops.
class SkinViewer:public QWidget {
public:
    explicit SkinViewer(QWidget *parent=nullptr);
    void setSkin(const QImage &skin,bool slim);
    // No skin: the text in the middle instead.
    void setMessage(const QString &text);
    void setThreeD(bool on);
    bool threeD() const {return m_threeD;}
    // 3D: half a turn, animated. 2D: the front or the back.
    void turnAround();
    // Angles in degrees: yaw 0 faces the player, 90 shows their right side; pitch above 0 looks from above.
    void setView(double yaw,double pitch);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    void paintModel(QPainter &p);
    QImage m_skin,m_base;QString m_message;bool m_slim=false,m_threeD=true,m_back=false,m_dragging=false;
    double m_yaw=-28,m_pitch=12;QPoint m_last;QVariantAnimation *m_turn;
};
