#pragma once
#include <QWidget>
#include <functional>
// Bedrock · Beta: plays the player's own copy of Minecraft Bedrock (see BedrockInstaller).
class BedrockPage : public QWidget {
public:
    explicit BedrockPage(const QString &dataRoot, QWidget *parent = nullptr);
    ~BedrockPage() override;
protected:
    // Looks for the game the first time the page opens, and again only after something done from here (install, update,
    // uninstall, or opening the Store or Minecraft Linux, where the game may change); "Check again" looks at any time.
    void showEvent(QShowEvent *event) override;
private:
    void refresh() {m_refresh();}
    std::function<void()> m_refresh;
    bool m_checked=false,m_stale=false;
};
