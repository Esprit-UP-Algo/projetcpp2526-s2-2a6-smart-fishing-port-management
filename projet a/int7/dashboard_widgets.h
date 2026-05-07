#ifndef DASHBOARD_WIDGETS_H
#define DASHBOARD_WIDGETS_H

#include <QWidget>
#include <QTimerEvent>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
// WaveWidget
// ─────────────────────────────────────────────────────────────────────────────
class WaveWidget : public QWidget {
public:
    explicit WaveWidget(QColor color, QWidget *parent = nullptr)
        : QWidget(parent), m_color(color) { setFixedHeight(28); startTimer(40); }
protected:
    void timerEvent(QTimerEvent *) override { m_offset = (m_offset+1)%100; update(); }
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        int W=width(), H=height();
        QPainterPath path, line;
        path.moveTo(0,H);
        for (int x=0;x<=W;++x) {
            double y=H*0.5+H*0.35*std::sin((x+m_offset*3)*0.06);
            if(x==0){path.moveTo(x,y);line.moveTo(x,y);}
            else{path.lineTo(x,y);line.lineTo(x,y);}
        }
        path.lineTo(W,H); path.lineTo(0,H); path.closeSubpath();
        QColor fill=m_color; fill.setAlpha(40);
        p.fillPath(path,fill);
        p.setPen(QPen(m_color,1.5)); p.setBrush(Qt::NoBrush);
        p.drawPath(line);
    }
private:
    QColor m_color;
    int    m_offset = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// DonutWidget
// ─────────────────────────────────────────────────────────────────────────────
class DonutWidget : public QWidget {
public:
    struct Slice { QString label; int value; QColor color; };
    explicit DonutWidget(QWidget *parent=nullptr):QWidget(parent){setFixedSize(110,110);}
    void setSlices(const QList<Slice>&s){m_slices=s;update();}
    void setTotal(int t){m_total=t;update();}
protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        int W=width(),H=height();
        QRectF rect(8,8,W-16,H-16);
        int total=0; for(auto&s:m_slices)total+=s.value;
        if(total==0){
            p.setPen(QPen(QColor("#e2e8f0"),12)); p.setBrush(Qt::NoBrush);
            p.drawEllipse(rect);
        } else {
            int start=90*16;
            for(auto&s:m_slices){
                int span=(int)((double)s.value/total*360*16);
                p.setPen(QPen(s.color,12)); p.setBrush(Qt::NoBrush);
                p.drawArc(rect,start,-span); start-=span;
            }
        }
        p.setPen(QColor("#0f172a")); p.setFont(QFont("Arial",14,QFont::Bold));
        p.drawText(rect,Qt::AlignCenter,QString::number(m_total));
        p.setFont(QFont("Arial",7)); p.setPen(QColor("#94a3b8"));
        p.drawText(QRectF(8,H/2+8,W-16,16),Qt::AlignCenter,"Total");
    }
private:
    QList<Slice> m_slices;
    int m_total=0;
};

// ─────────────────────────────────────────────────────────────────────────────
// SparklineWidget
// ─────────────────────────────────────────────────────────────────────────────
class SparklineWidget : public QWidget {
public:
    explicit SparklineWidget(QWidget *parent=nullptr):QWidget(parent){setMinimumHeight(120);}
    void setData(const QList<int>&data,const QStringList&labels){m_data=data;m_labels=labels;update();}
protected:
    void paintEvent(QPaintEvent *) override {
        if(m_data.size()<2)return;
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        int W=width(),H=height(),mg=28,bot=20,cH=H-mg-bot,cW=W-2*mg;
        int maxV=*std::max_element(m_data.begin(),m_data.end()); if(maxV==0)maxV=1;
        // grille
        p.setPen(QPen(QColor("#e2e8f0"),1,Qt::DashLine));
        for(int i=0;i<=4;++i){
            int y=mg+cH-(i*cH/4);
            p.drawLine(mg,y,mg+cW,y);
            p.setPen(QColor("#94a3b8")); p.setFont(QFont("Arial",7));
            p.drawText(0,y-6,mg-2,14,Qt::AlignRight|Qt::AlignVCenter,QString::number(i*maxV/4));
            p.setPen(QPen(QColor("#e2e8f0"),1,Qt::DashLine));
        }
        int n=m_data.size(); double step=(double)cW/(n-1);
        // aire
        QPainterPath area; area.moveTo(mg,mg+cH);
        for(int i=0;i<n;++i){double x=mg+i*step,y=mg+cH-(double)m_data[i]/maxV*cH;
            if(i==0)area.lineTo(x,y);else area.lineTo(x,y);}
        area.lineTo(mg+cW,mg+cH); area.closeSubpath();
        QLinearGradient grad(0,mg,0,mg+cH);
        grad.setColorAt(0,QColor(37,99,235,80)); grad.setColorAt(1,QColor(37,99,235,5));
        p.fillPath(area,grad);
        // courbe
        QPainterPath line;
        for(int i=0;i<n;++i){double x=mg+i*step,y=mg+cH-(double)m_data[i]/maxV*cH;
            if(i==0)line.moveTo(x,y);else line.lineTo(x,y);}
        p.setPen(QPen(QColor("#2563eb"),2)); p.setBrush(Qt::NoBrush); p.drawPath(line);
        // points
        for(int i=0;i<n;++i){
            double x=mg+i*step,y=mg+cH-(double)m_data[i]/maxV*cH;
            p.setBrush(QColor("#2563eb")); p.setPen(QPen(Qt::white,1.5));
            p.drawEllipse(QPointF(x,y),4,4);
            if(i<m_labels.size()){p.setPen(QColor("#94a3b8"));p.setFont(QFont("Arial",7));
                p.drawText((int)(x-18),H-bot,36,bot,Qt::AlignCenter,m_labels[i]);}
        }
    }
private:
    QList<int>  m_data;
    QStringList m_labels;
};

#endif // DASHBOARD_WIDGETS_H
