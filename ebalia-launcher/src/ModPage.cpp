#include "ModPage.hpp"
#include "md4c-html.h"
#include <QByteArray>
#include <QRegularExpression>
#include <QUrl>

namespace {
void collect(const MD_CHAR *text, MD_SIZE size, void *out) { static_cast<QByteArray *>(out)->append(text, qsizetype(size)); }
// Percent sizes and widths wider than the panel: the view fits the picture to the panel itself.
QString image(QString tag) {
    static const QRegularExpression style(R"re(\sstyle\s*=\s*("[^"]*"|'[^']*'))re", QRegularExpression::CaseInsensitiveOption);
    tag.remove(style);
    static const QRegularExpression width(R"re(\swidth\s*=\s*["']?\s*(\d+(?:\.\d+)?)\s*(%|px)?\s*["']?)re", QRegularExpression::CaseInsensitiveOption);
    static const QRegularExpression height(R"re(\sheight\s*=\s*["']?\s*\d+(?:\.\d+)?\s*(%|px)?\s*["']?)re", QRegularExpression::CaseInsensitiveOption);
    const auto w = width.match(tag);
    if (w.hasMatch() && (w.captured(2) == "%" || w.captured(1).toDouble() > 480)) {tag.remove(width);tag.remove(height);}
    else if (!w.hasMatch()) tag.remove(height);
    return tag;
}
}

QString ModPage::html(const QString &body, bool markdown) {
    QString out = body;
    if (markdown) {
        const auto source = body.toUtf8();
        QByteArray rendered;
        if (md_html(source.constData(), MD_SIZE(source.size()), collect, &rendered, MD_DIALECT_GITHUB, 0) != 0) return body.toHtmlEscaped();
        out = QString::fromUtf8(rendered);
    }
    static const QRegularExpression scripts(R"re(<(script|style|noscript)\b.*?</\1\s*>)re", QRegularExpression::CaseInsensitiveOption | QRegularExpression::DotMatchesEverythingOption);
    out.remove(scripts);
    // Embedded players (YouTube…) cannot play in the view: a link to the video instead.
    static const QRegularExpression frame(R"re(<iframe\b[^>]*?\bsrc\s*=\s*["']?([^"'\s>]+)[^>]*>(?:\s*</iframe\s*>)?)re", QRegularExpression::CaseInsensitiveOption);
    for (auto m = frame.match(out); m.hasMatch(); m = frame.match(out, m.capturedStart())) {
        auto url = m.captured(1);if (url.startsWith("//")) url.prepend("https:");
        const QUrl link(url);
        out.replace(m.capturedStart(), m.capturedLength(), link.scheme() == "https" ? QString("<p><a href=\"%1\">▶ %2</a></p>").arg(link.toString(QUrl::FullyEncoded).toHtmlEscaped(), link.host().toHtmlEscaped()) : QString());
    }
    static const QRegularExpression img(R"re(<img\b[^>]*>)re", QRegularExpression::CaseInsensitiveOption);
    QString result;qsizetype last = 0;
    for (auto it = img.globalMatch(out); it.hasNext();) {auto m = it.next();result += out.mid(last, m.capturedStart() - last) + image(m.captured());last = m.capturedEnd();}
    return result + out.mid(last);
}
