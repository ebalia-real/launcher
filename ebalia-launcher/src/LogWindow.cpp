#include "LogWindow.hpp"
#include "Language.hpp"
#include "Ui.hpp"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSyntaxHighlighter>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
namespace {
constexpr int maxLines=20000;
constexpr qint64 firstRead=1024*1024; // the end of a long log, not all of it
QString t(const char *s){return Language::key(QString::fromUtf8(s));}
// Errors (and the stack traces under them) in red, warnings in yellow.
class LogColours:public QSyntaxHighlighter {
public:
    using QSyntaxHighlighter::QSyntaxHighlighter;
protected:
    void highlightBlock(const QString &line) override {
        if(line.contains("ERROR")||line.contains("FATAL")||line.contains("Exception")||line.startsWith("\tat ")||line.startsWith("Caused by:"))setFormat(0,int(line.size()),QColor(255,135,145));
        else if(line.contains("WARN"))setFormat(0,int(line.size()),QColor(238,198,104));
    }
};
QToolButton *tool(const QString &icon,const QString &tip,const char *name){
    auto b=new QToolButton;b->setObjectName(name);b->setIcon(Ui::icon(icon));b->setIconSize(QSize(18,18));b->setToolTip(tip);b->setAccessibleName(tip);b->setCursor(Qt::PointingHandCursor);return b;
}
}
LogView::LogView(QWidget *parent):QWidget(parent){
    setObjectName("logPanel");
    auto layout=new QVBoxLayout(this);layout->setContentsMargins(0,0,0,0);layout->setSpacing(10);
    m_bar=new QHBoxLayout;m_bar->setSpacing(8);layout->addLayout(m_bar);
    // In narrow windows the state gives way first (its full text stays in the tooltip), then the filter.
    m_state=new QLabel;m_state->setObjectName("logState");m_state->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);m_state->setMinimumWidth(40);m_bar->addWidget(m_state,1);
    m_filter=new QLineEdit;m_filter->setObjectName("logFilter");m_filter->setPlaceholderText(t("Filter lines…"));m_filter->setClearButtonEnabled(true);m_filter->setMaximumWidth(280);m_filter->setMinimumWidth(90);
    m_filter->addAction(Ui::icon("search",QColor(180,180,188)),QLineEdit::LeadingPosition);m_bar->addWidget(m_filter,1);
    auto copy=tool("copy",t("Copy"),"logCopy");m_bar->addWidget(copy);
    auto open=tool("folder",t("Open folder"),"logOpenFolder");m_bar->addWidget(open);
    m_view=new QPlainTextEdit;m_view->setObjectName("logView");m_view->setReadOnly(true);m_view->setLineWrapMode(QPlainTextEdit::NoWrap);m_view->setMaximumBlockCount(maxLines);
    m_view->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));new LogColours(m_view->document());layout->addWidget(m_view,1);
    connect(copy,&QToolButton::clicked,this,[this]{QApplication::clipboard()->setText(m_view->toPlainText());});
    connect(open,&QToolButton::clicked,this,[this]{if(m_folder.isEmpty())return;const auto logs=QDir(m_folder+"/logs").exists()?m_folder+"/logs":m_folder;QDesktopServices::openUrl(QUrl::fromLocalFile(logs));});
    connect(m_filter,&QLineEdit::textChanged,this,[this]{refilter();});
    // The game writes in bursts; reading the new end of the file a few times a second is enough and works on every system.
    m_timer=new QTimer(this);m_timer->setInterval(400);connect(m_timer,&QTimer::timeout,this,[this]{poll();});
}
void LogView::setFolder(const QString &folder){
    if(folder==m_folder)return;
    m_folder=folder;m_position=-1;m_partial.clear();m_lines.clear();m_view->clear();
    m_file=QFile::exists(folder+"/launcher.log")||!QFile::exists(folder+"/logs/latest.log")?folder+"/launcher.log":folder+"/logs/latest.log";
    if(isVisible())poll();
}
void LogView::showEvent(QShowEvent *event){QWidget::showEvent(event);poll();m_timer->start();}
void LogView::hideEvent(QHideEvent *event){m_timer->stop();QWidget::hideEvent(event);}
void LogView::poll(){
    if(m_folder.isEmpty())return;
    const bool live=running&&running();
    m_state->setText(live?"●  "+t("Live"):t("The game is not running"));m_state->setToolTip(m_state->text());m_state->setStyleSheet(live?"color:#6fd15b;font-weight:800;":"color:#9a9aa3;");
    QFile file(m_file);
    if(!file.open(QIODevice::ReadOnly)){if(m_lines.isEmpty())m_view->setPlainText(t("No log yet."));return;}
    const auto size=file.size();
    // A new game started: the launcher writes its output from the beginning again.
    if(size<m_position){m_position=0;m_partial.clear();m_lines.clear();m_view->clear();}
    if(m_position<0){m_position=qMax<qint64>(0,size-firstRead);if(m_position>0){file.seek(m_position);file.readLine();m_position=file.pos();}m_view->clear();}
    if(size==m_position)return;
    file.seek(m_position);const auto data=m_partial+file.read(qMin<qint64>(size-m_position,8*1024*1024));m_position=file.pos();
    // Only whole lines: the rest waits for the next read, so a character split between reads is never cut.
    const auto end=data.lastIndexOf('\n');if(end<0){m_partial=data;return;}
    m_partial=data.mid(end+1);auto text=QString::fromUtf8(data.left(end));text.remove('\r');
    append(text.split('\n'));
}
void LogView::append(const QStringList &lines){
    m_lines+=lines;if(m_lines.size()>maxLines)m_lines.erase(m_lines.begin(),m_lines.begin()+(m_lines.size()-maxLines));
    const auto filter=m_filter->text().trimmed();QStringList shown;
    for(const auto &line:lines)if(filter.isEmpty()||line.contains(filter,Qt::CaseInsensitive))shown<<line;
    if(shown.isEmpty())return;
    // Follow the end only when the view was already there: reading something higher up is not interrupted.
    auto bar=m_view->verticalScrollBar();const bool atEnd=bar->value()>=bar->maximum()-4;const int keep=bar->value();
    m_view->appendPlainText(shown.join('\n'));
    if(atEnd)bar->setValue(bar->maximum());else bar->setValue(keep);
}
void LogView::refilter(){
    const auto filter=m_filter->text().trimmed();QStringList shown;
    for(const auto &line:m_lines)if(filter.isEmpty()||line.contains(filter,Qt::CaseInsensitive))shown<<line;
    m_view->setPlainText(shown.join('\n'));m_view->verticalScrollBar()->setValue(m_view->verticalScrollBar()->maximum());
}
LogWindow::LogWindow(const QString &folder,const QString &title,std::function<bool()> running,QWidget *parent):QDialog(parent){
    setObjectName("logWindow");setProperty("logFolder",folder);setAttribute(Qt::WA_DeleteOnClose);setWindowTitle(title+" · "+t("Log"));setSizeGripEnabled(true);Ui::fitToScreen(this,{960,620});
    auto layout=new QVBoxLayout(this);layout->setContentsMargins(16,14,16,14);layout->setSpacing(10);
    auto view=new LogView;view->running=std::move(running);view->setFolder(folder);layout->addWidget(view,1);
    auto bottom=new QHBoxLayout;bottom->addStretch();layout->addLayout(bottom);
    auto close=new QPushButton(Language::key("Close"));bottom->addWidget(close);connect(close,&QPushButton::clicked,this,&QDialog::close);
}
