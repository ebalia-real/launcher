#pragma once
#include <QString>
namespace ModPage {
// A mod's description as HTML the launcher's text view shows well. Markdown (with the HTML Modrinth allows inside
// it) goes through md4c, the CommonMark parser Qt itself uses; CurseForge's HTML only gets the same clean-up:
// no scripts, embedded videos become links and image sizes the view cannot honour are dropped.
QString html(const QString &body, bool markdown);
}
