#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDockWidget>
#include <QListWidget>
#include <QGroupBox>
#include <QValidator>
#include <QMessageBox>
#include <QtConcurrent>
#include <QScrollBar>
#include <QClipboard>
#include <QFontDatabase>
#include <QPixmap>
#include <QMenu>
#include <QMovie>
#include <QMenuBar>

#include "Challenge.hpp"
#include "Bundle.hpp"
#include "OutputStream.hpp"
#include "SpinBox.hpp"
#include "Clock.hpp"

inline std::ostream& operator<<(std::ostream& os, const QString& str)
{
    os << str.toStdString();
    return os;
}

class MainFrame : public QMainWindow
{
    Q_OBJECT

    public:
        MainFrame(const std::string &exePath);
        std::string getGameFolder(const std::string &exePath);
        std::string locateGameFolder();

        QString loadChallengeThread();
        QString installTrainingRoomThread(bool install);

        void showMessage(const QString &msg, const QString &copiable, const QString &title, QMessageBox::Icon icon);
        void showError(const QString &error);
        void showEasterEgg(const QString &text, const QString &imageFilename);

        static unsigned stringToSeed(const QString &text);
        static QString seedToString(unsigned seed);

        void easterEgg(unsigned seed);

    signals:

    public slots:
        void showOutput(bool show);
        void showOutputMenu(QPoint pos);

        void loadChallenge();
        void onLoadChallengeFinished();

        void installTrainingRoom(bool install);
        void onInstallTrainingRoomFinished();

        void generateRandomSeed();

        void enableButtons();
        void applyChanges();
        void resetChanges();

        void showLastSeed();

    private:
        QPushButton *m_loadButton;
        QAction *m_loadChallengeAction;

        QLabel *m_levelLabel;
        QLabel *m_eventLabel;
        QLabel *m_difficultyLabel;

        QLineEdit *m_seedLine;
        QWidget *m_seedWidget;

        QPushButton *m_randomButton;
        QWidget *m_randomWidget;

        QWidget *m_goalWidget;
        QWidget * m_limitWidget;

        SpinBox *m_goalLine;
        SpinBox *m_limitLine;

        QLabel *m_goalTypeLabel;
        QLabel *m_limitTypeLabel;

        QPushButton *m_applyButton;
        QPushButton *m_resetButton;

        QLabel *m_loadingLabel;
        QMovie *m_loadingMovie;

        QCheckBox *m_trainingCheck;

        QDockWidget *m_outputDock;
        ListWidget *m_outputList;

        QFutureWatcher<QString> m_loadWatcher;
        QFutureWatcher<QString> m_trainingWatcher;

        Challenge m_challenge;
        std::string m_gameFolder;

        const std::string gameName = "Rayman Legends.exe";
        const std::string bundleName = "Bundle_PC.ipk";

        const unsigned windowWidth = 424;
        const unsigned windowHeight = 347;
        const unsigned altWindowHeight = 521;
};

#endif // MAINFRAME_H
