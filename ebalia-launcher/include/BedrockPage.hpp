#pragma once
#include <QWidget>
#include <functional>
// Bedrock · Beta: plays the player's own copy of Minecraft Bedrock (see BedrockInstaller).
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
