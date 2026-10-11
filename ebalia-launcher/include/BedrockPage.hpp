#pragma once
#include <QWidget>
#include <QStringList>

namespace Bedrock {
struct Command {
    QString program;
    QStringList arguments;
    bool valid() const { return !program.isEmpty(); }
};
enum class Platform { Windows, Linux, MacOS, Unsupported };
Platform hostPlatform();
// Build an argument vector, never a shell command. macOS bundles are opened with /usr/bin/open.
Command localCommand(const QString &path, Platform platform);
}
#include <functional>
class BedrockPage : public QWidget {
public:
    explicit BedrockPage(const QString &dataRoot, QWidget *parent = nullptr);
    ~BedrockPage() override;
protected:
    // Looks for the game again each time the page opens (it may have been installed from the Store meanwhile).
    void showEvent(QShowEvent *event) override;
private:
    void refresh() {m_refresh();}
    std::function<void()> m_refresh;
};
