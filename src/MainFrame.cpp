#include "MainFrame.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::flush;

class UppercaseValidator : public QValidator
{
    public:
        UppercaseValidator(QObject *parent = nullptr) : QValidator(parent)
        {
        }

        State validate(QString &str, int&) const override
        {
            str = str.toUpper();
            return QValidator::Acceptable;
        }
};

MainFrame::MainFrame(const std::string &exePath)
{
    setWindowTitle("RL Challenge Manager");
    setFixedSize(windowWidth, windowHeight);

    /// MENU & ACTIONS

    auto fileMenu = menuBar()->addMenu("&File");

    auto loadRulesAction = fileMenu->addAction("&Open rules...");
    loadRulesAction->setShortcut(QKeySequence("Ctrl+O"));
    auto saveRulesAction = fileMenu->addAction("&Save rules...");
    saveRulesAction->setShortcut(QKeySequence("Ctrl+S"));

    fileMenu->addSeparator();

    auto quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));


    auto challengeMenu = menuBar()->addMenu("&Challenge");

    auto installTrainingRoomAction = challengeMenu->addAction("Install &training room");
    installTrainingRoomAction->setShortcut(QKeySequence("Ctrl+T"));

    challengeMenu->addSeparator();

    m_loadChallengeAction = challengeMenu->addAction("&Load current challenge");
    m_loadChallengeAction->setShortcut(QKeySequence("Ctrl+L"));
    auto applyChangeAction = challengeMenu->addAction("&Apply changes");
    applyChangeAction->setShortcut(QKeySequence("Ctrl+A"));
    auto resetChangeAction = challengeMenu->addAction("&Reset changes");
    resetChangeAction->setShortcut(QKeySequence("Ctrl+R"));


    /// CHALLENGE GROUP

    m_loadButton = new QPushButton("Load challenge", this);
    m_loadButton->setToolTip("Loads the current challenge and displays its rules");
    m_loadButton->setFixedSize(140, 30);
    m_loadButton->setFont(QFont(m_loadButton->font().defaultFamily(), 10));

    m_levelLabel = new QLabel("Level: N/A");
    m_levelLabel->setMargin(5);
    m_levelLabel->setEnabled(false);
    m_eventLabel = new QLabel("Event: N/A");
    m_eventLabel->setMargin(5);
    m_eventLabel->setEnabled(false);
    m_difficultyLabel = new QLabel("Difficulty: N/A");
    m_difficultyLabel->setMargin(5);
    m_difficultyLabel->setEnabled(false);

    m_seedLine = new QLineEdit("00 00 00 00", this);
    m_seedLine->setToolTip("The seed of the challenge in hexadecimal notation");
    m_seedLine->setInputMask("HH HH HH HH;0");
    m_seedLine->setFont(QFont("Consolas", 10));
    m_seedLine->setTextMargins(4,1,4,1);
    m_seedLine->setFixedWidth(95);

    auto validator = new UppercaseValidator();
    m_seedLine->setValidator(validator);

    auto seedLabel = new QLabel("Seed:");
    seedLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    auto seedLayout = new QHBoxLayout();
    seedLayout->addWidget(seedLabel);
    seedLayout->addWidget(m_seedLine);

    m_seedWidget = new QWidget(this);
    m_seedWidget->setLayout(seedLayout);
    m_seedWidget->setEnabled(false);

    m_randomButton = new QPushButton("Random seed", this);
    m_randomButton->setToolTip("Generates a random seed");
    m_randomButton->setFixedSize(100, 25);

    auto arrowLabel = new QLabel("←"); //⇦←
    arrowLabel->setFont(QFont("Consolas", 20));
    arrowLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    arrowLabel->setFixedWidth(20);

    auto randomLayout = new QHBoxLayout();
    randomLayout->addWidget(arrowLabel);
    randomLayout->addWidget(m_randomButton);

    m_randomWidget = new QWidget(this);
    m_randomWidget->setLayout(randomLayout);
    m_randomWidget->setEnabled(false);


    m_goalLine = new SpinBox(this);
    m_goalLine->setToolTip("The goal of the challenge in floating point notation");
    m_goalLine->setSingleStep(50);
    m_goalLine->setRange(0, 1000000000);

    m_goalTypeLabel = new QLabel(this);

    auto goalLayout = new QHBoxLayout();
    goalLayout->addWidget(new QLabel("Goal:"));
    goalLayout->addWidget(m_goalLine);
    goalLayout->addWidget(m_goalTypeLabel);

    m_goalWidget = new QWidget(this);
    m_goalWidget->setLayout(goalLayout);
    m_goalWidget->setEnabled(false);


    m_limitLine = new SpinBox(this);
    m_limitLine->setToolTip("The minimal score in floating point notation");
    m_limitLine->setSingleStep(5);
    m_limitLine->setRange(0, 1000000000);

    m_limitTypeLabel = new QLabel(this);

    auto limitLayout = new QHBoxLayout();
    limitLayout->addWidget(new QLabel("Limit:"));
    limitLayout->addWidget(m_limitLine);
    limitLayout->addWidget(m_limitTypeLabel);

    m_limitWidget = new QWidget(this);
    m_limitWidget->setLayout(limitLayout);
    m_limitWidget->setEnabled(false);


    m_applyButton = new QPushButton("Apply changes");
    m_applyButton->setToolTip("Writes modifications into the RAM");
    m_applyButton->setFixedSize(120, 25);
    m_applyButton->setEnabled(false);

    m_resetButton = new QPushButton("Reset");
    m_resetButton->setToolTip("Reset the rules from the RAM");
    m_resetButton->setFixedSize(120, 25);
    m_resetButton->setEnabled(false);

    m_loadingMovie = new QMovie(":/data/img/loading.gif");
    m_loadingLabel = new QLabel(this);
    m_loadingLabel->setMovie(m_loadingMovie);


    auto challengeLayout = new QGridLayout();
    challengeLayout->addWidget(m_loadButton, 0, 0, 1, 2, Qt::AlignCenter);
    challengeLayout->addWidget(m_loadingLabel, 0, 1, Qt::AlignCenter);
    challengeLayout->addWidget(m_levelLabel, 1, 0, 1, 2, Qt::AlignCenter);
    challengeLayout->addWidget(m_eventLabel, 2, 0, Qt::AlignCenter);
    challengeLayout->addWidget(m_difficultyLabel, 2, 1, Qt::AlignCenter);
    challengeLayout->addWidget(m_seedWidget, 3, 0, Qt::AlignRight);
    challengeLayout->addWidget(m_randomWidget, 3, 1, Qt::AlignLeft);
    challengeLayout->addWidget(m_goalWidget, 4, 0);
    challengeLayout->addWidget(m_limitWidget, 4, 1);
    challengeLayout->addWidget(m_applyButton, 5, 0, Qt::AlignRight);
    challengeLayout->addWidget(m_resetButton, 5, 1, Qt::AlignLeft);

    auto challengeGroup = new QGroupBox("Challenge");
    challengeGroup->setLayout(challengeLayout);


    /// TOOLS GROUP

    m_trainingCheck = new QCheckBox("Enable training room", this);
    m_trainingCheck->setToolTip("Install/uninstall the training room");

    auto outputCheck = new QCheckBox("Show output", this);
    outputCheck->setToolTip("Display/hide the process output");

    auto toolLayout = new QHBoxLayout();
    toolLayout->addWidget(m_trainingCheck);
    toolLayout->addWidget(outputCheck);

    auto toolGroup = new QGroupBox("Tools");
    toolGroup->setLayout(toolLayout);


    /// CENTRAL

    auto layout = new QVBoxLayout();
    layout->addWidget(challengeGroup);
    layout->addWidget(toolGroup);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);

    setCentralWidget(centralWidget);


    /// BOTTOM DOCK

    m_outputList = new ListWidget();
    m_outputList->horizontalScrollBar()->show();
    m_outputList->setStyleSheet("background-color:black;");
    m_outputList->setFont(QFont("Consolas", 8));
    m_outputList->setContextMenuPolicy(Qt::CustomContextMenu);

    new OutputStream(cout, m_outputList, Qt::white);
    new OutputStream(cerr, m_outputList, Qt::red);

    m_outputDock = new QDockWidget("Ouput", this);
    addDockWidget(Qt::BottomDockWidgetArea, m_outputDock);
    m_outputDock->setWidget(m_outputList);
    m_outputDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    m_outputDock->hide();

    /// PROCESS

    cout << "RL® Challenge Manager (2.0.b4)" << endl << "© 2014-2016 Olybri" << endl << endl;

    try
    {
        m_gameFolder = getGameFolder(exePath);

        Bundle bundle(m_gameFolder, bundleName);
        m_trainingCheck->setChecked(bundle.checkTrainingRoom());
    }
    catch(const std::exception &e)
    {
        showError(e.what());

        m_loadButton->setEnabled(false);
        m_trainingCheck->setEnabled(false);
    }

    /// CONNECTIONS

    connect(m_loadButton, SIGNAL(clicked()), m_loadChallengeAction, SLOT(trigger()));
    connect(m_loadChallengeAction, SIGNAL(triggered()), this, SLOT(loadChallenge()));
    connect(&m_loadWatcher, SIGNAL(finished()), this, SLOT(onLoadChallengeFinished()));

    connect(m_randomButton, SIGNAL(clicked()), this, SLOT(generateRandomSeed()));

    connect(m_trainingCheck, SIGNAL(clicked(bool)), this, SLOT(installTrainingRoom(bool)));
    connect(&m_trainingWatcher, SIGNAL(finished()), this, SLOT(onInstallTrainingRoomFinished()));

    connect(outputCheck, SIGNAL(clicked(bool)), this, SLOT(showOutput(bool)));
    connect(m_outputList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showOutputMenu(QPoint)));

    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetChanges()));

    connect(m_seedLine, SIGNAL(textChanged(QString)), this, SLOT(enableButtons()));
    connect(m_goalLine, SIGNAL(valueChanged(double)), this, SLOT(enableButtons()));
    connect(m_limitLine, SIGNAL(valueChanged(double)), this, SLOT(enableButtons()));
}

void MainFrame::showOutput(bool show)
{
    if(show)
    {
        setMaximumHeight(QWIDGETSIZE_MAX);
        setMinimumHeight(altWindowHeight);

        m_outputDock->show();
        m_outputList->scrollToBottom();
    }

    else
    {
        setFixedSize(windowWidth, windowHeight);

        m_outputDock->hide();
    }
}

void MainFrame::showOutputMenu(QPoint pos)
{
    if(m_outputList->itemAt(pos)->text().isEmpty())
        return;

    QMenu menu;
    menu.addAction("Copy");
    auto actionCopy = menu.exec(m_outputList->mapToGlobal(pos));
    if(actionCopy)
    {
        auto text = m_outputList->itemAt(pos)->text();
        text.replace('\n', "");
        QApplication::clipboard()->setText(text);
    }
}

QString MainFrame::loadChallengeThread()
{
    m_trainingWatcher.waitForFinished();

    try
    {
        Clock clock;
        m_challenge.openProcess(m_gameFolder + gameName);
        m_challenge.load();
        cout << clock.elapsed() << " seconds elapsed." << endl;
    }

    catch(const std::exception &e)
    {
        return e.what();
    }

    return "";
}

void MainFrame::loadChallenge()
{
    m_loadChallengeAction->setEnabled(false);
    m_loadButton->setEnabled(false);
    m_applyButton->setEnabled(false);
    m_resetButton->setEnabled(false);

    m_loadingLabel->show();
    m_loadingMovie->start();

    m_loadWatcher.setFuture(QtConcurrent::run(this, loadChallengeThread));
}

void MainFrame::onLoadChallengeFinished()
{
    m_goalLine->setFixedSize(m_goalLine->size());
    m_limitLine->setFixedSize(m_limitLine->size());

    m_loadChallengeAction->setEnabled(true);
    m_loadButton->setEnabled(true);

    m_loadingLabel->hide();

    auto result = m_loadWatcher.future().result();
    if(!result.isEmpty())
    {
        showError(result);
        return;
    }

    m_levelLabel->setEnabled(true);
    m_eventLabel->setEnabled(true);
    m_difficultyLabel->setEnabled(true);
    m_seedWidget->setEnabled(true);
    m_randomWidget->setEnabled(true);

    m_goalWidget->setEnabled(!m_challenge.getGoalType().empty());
    m_limitWidget->setEnabled(true);

    m_goalTypeLabel->setText(m_challenge.getGoalType().c_str());
    m_limitTypeLabel->setText(m_challenge.getLimitType().c_str());

    m_levelLabel->setText(QString("Level: ") + m_challenge.getLevelName().c_str());
    m_eventLabel->setText(QString("Event: ") + m_challenge.getEventName().c_str());
    m_difficultyLabel->setText(QString("Difficulty: ") + m_challenge.getDifficultyName().c_str());

    resetChanges();
}

QString MainFrame::installTrainingRoomThread(bool install)
{
    m_loadWatcher.waitForFinished();

    try
    {
        Clock clock;
        Bundle bundle(m_gameFolder, bundleName);
        bundle.installTrainingRoom(install);
        cout << clock.elapsed() << " seconds elapsed." << endl;

        m_trainingCheck->setChecked(bundle.checkTrainingRoom());
    }
    catch(const std::exception &e)
    {
        return e.what();
    }

    return "";
}

void MainFrame::installTrainingRoom(bool install)
{
    m_trainingCheck->setEnabled(false);
    m_trainingWatcher.setFuture(QtConcurrent::run(this, installTrainingRoomThread, install));
}

void MainFrame::onInstallTrainingRoomFinished()
{
    m_trainingCheck->setEnabled(true);

    auto result = m_trainingWatcher.future().result();
    if(!result.isEmpty())
    {
        showError(result);
        return;
    }
}

void MainFrame::generateRandomSeed()
{
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<unsigned> distribution;

    auto seed = distribution(generator);
    cout << endl << "Generated random seed: " << std::hex << std::showbase << seed << endl;
    m_seedLine->setText(seedToString(seed));
}

void MainFrame::enableButtons()
{
    bool enabled = false;

    const QString changedStyle = "color:red";
    const QString unchangedStyle = "color:black";

    if(stringToSeed(m_seedLine->displayText()) != m_challenge.getSeed())
    {
        m_seedLine->setStyleSheet(changedStyle);
        enabled = true;
    }
    else
        m_seedLine->setStyleSheet(unchangedStyle);

    if(m_goalWidget->isEnabled())
    {
        if(!m_goalLine->valueEquals(m_challenge.getGoal()))
        {
            m_goalLine->setStyleSheet(changedStyle);
            enabled = true;
        }
        else
            m_goalLine->setStyleSheet(unchangedStyle);
    }
    else
        m_goalLine->setStyleSheet("");

    if(!m_limitLine->valueEquals(m_challenge.getLimit()))
    {
        m_limitLine->setStyleSheet(changedStyle);
        enabled = true;
    }
    else
        m_limitLine->setStyleSheet(unchangedStyle);

    m_applyButton->setEnabled(enabled);
    m_resetButton->setEnabled(enabled);
}

void MainFrame::applyChanges()
{
    auto seed = stringToSeed(m_seedLine->displayText());

    if(seed != m_challenge.getSeed())
        easterEgg(seed);

    try
    {
        m_challenge.updateRules(seed, m_goalLine->value(), m_limitLine->value());

        enableButtons();
    }
    catch(const std::exception &e)
    {
        showError(e.what());
    }
}

void MainFrame::resetChanges()
{
    m_seedLine->setText(seedToString(m_challenge.getSeed()));
    m_goalLine->setValue(m_challenge.getGoal());
    m_limitLine->setValue(m_challenge.getLimit());
}

void MainFrame::showLastSeed()
{
    QDirIterator dir((m_gameFolder + "/gamesave/saveghost").c_str());

    QFileInfo mostRecent(dir.next());
    while(dir.hasNext())
    {
        if(!QFileInfo(dir.next()).isFile())
            continue;

        if(QFileInfo(dir.filePath()).lastModified() > mostRecent.lastModified())
            mostRecent.setFile(dir.filePath());
    }

    QFile file(mostRecent.filePath());
    file.open(QIODevice::ReadOnly);

    std::vector<char> data(file.size());
    file.read(data.data(), data.size());
    file.close();

    const std::string ext = ".isc";
    auto address = std::distance(data.begin(), std::search(data.begin(), data.end(), ext.begin(), ext.end()));

    data.erase(data.begin(), data.begin() + address + 16);
    data.erase(data.begin() + 4, data.end());
    auto seed = *reinterpret_cast<unsigned*>(data.data());

    /*QMessageBox msgBox;
    msgBox.setWindowTitle("Last challenge seed");
    msgBox.setText("The last finished challenge has seed:<br><big><font face=\"Consolas\">" + seedToString(seed) + "</font></big>");
    msgBox.setIcon(QMessageBox::Information);
    auto copyButton = msgBox.addButton("Copy", QMessageBox::YesRole);
    msgBox.addButton("Close", QMessageBox::NoRole);
    msgBox.exec();
    if(msgBox.clickedButton() == copyButton)
    {
        cout << "copied" << endl;
    }*/

    cout << "Last challenge seed: " << seed << endl;

    showMessage("The last finished challenge has seed:<br><big><font face=\"Consolas\">" + seedToString(seed) + "</font></big>",
        seedToString(seed), "Last challenge seed", QMessageBox::Information);
}

std::string MainFrame::getGameFolder(const std::string &exePath)
{
    std::string gameFolder;

    auto saveFilename = exePath.substr(0, exePath.find_last_of('\\') + 1) + "gamedir.sav";
    std::ifstream ifs(saveFilename);
    if(ifs)
    {
        std::ostringstream str;
        str << ifs.rdbuf();
        gameFolder = str.str();

        ifs.close();
    }

    if(!gameFolder.empty())
        cout << "Game directory:" << endl << gameFolder << endl;

    else
    {
        gameFolder = locateGameFolder();

        std::ofstream ofs(saveFilename, std::ios::out | std::ios::trunc);
        if(ofs)
        {
            ofs << gameFolder;
            ofs.close();
        }

        else
            cerr << "Warning: Failed to save game directory into file \"" << saveFilename << "\"!" << endl;
    }

    return gameFolder;
}

std::string MainFrame::locateGameFolder()
{
    cout << "Locating game directory... " << flush;

    auto gameFolder = Process::getProcessLocation(gameName);

    if(gameFolder.empty())
    {
        cout << "Failure!" << endl;
        throw std::runtime_error("Can't locate the game directory because \"" + gameName + "\" is not running!\n"
            + "Please launch the game.");
    }

    cout << "Success!" << endl << "Found: " << gameFolder << endl;

    return gameFolder;
}

QString MainFrame::seedToString(unsigned seed)
{
    std::ostringstream str;
    str << std::hex << std::uppercase << std::setfill('0')
        << std::setw(2) << seed / 0x100'0000 << " "
        << std::setw(2) << seed / 0x1'0000 % 0x100 << " "
        << std::setw(2) << seed / 0x100 % 0x100 << " "
        << std::setw(2) << seed % 0x100;

    return str.str().c_str();
}

unsigned MainFrame::stringToSeed(const QString &text)
{
    std::istringstream str(text.toStdString());
    unsigned seed {0};

    unsigned byte;
    while(str >> std::hex >> byte)
            seed = seed * 0x100 + byte;

    return seed;
}

void MainFrame::showMessage(const QString &msg, const QString &copiable, const QString &title, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(msg.size() ? msg : copiable);
    msgBox.setIcon(icon);
    auto copyButton = msgBox.addButton("Copy", QMessageBox::YesRole);
    msgBox.setDefaultButton(msgBox.addButton("Close", QMessageBox::NoRole));
    msgBox.exec();
    if(msgBox.clickedButton() == copyButton)
        QApplication::clipboard()->setText(copiable);
}

void MainFrame::showError(const QString &error)
{
    cerr << endl << "Error: " << error << endl;

    showMessage("", error, "Error", QMessageBox::Critical);
}

void MainFrame::showEasterEgg(const QString &text, const QString &imageFilename)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("( ͡° ͜ʖ ͡°)");
    msgBox.setIcon(QMessageBox::NoIcon);

    auto fontId = QFontDatabase::addApplicationFont(":/data/digital-7_mono.ttf");

    msgBox.setText("<center><font face=\"" + QFontDatabase::applicationFontFamilies(fontId).at(0)
        + "\" size=12>" + text + "</font><br><img src=\":/data/img/" + imageFilename + "\"></center>");

    msgBox.addButton("whatevs", QMessageBox::AcceptRole);
    msgBox.exec();

    QFontDatabase::removeApplicationFont(fontId);
}

void MainFrame::easterEgg(unsigned seed)
{
    switch(seed)
    {
        case 0xdeadbeef:
            showEasterEgg("DEAD BEEF", "deadbeef.png");
            break;

        case 0xcafebabe:
            showEasterEgg("CAFE BABE", "cafebabe.png");
            break;

        case 0xcafed00d:
            showEasterEgg("CAFE D00D", "cafebabe.png");
            break;

        case 0x00bada55:
        case 0x0bada550:
        case 0xbada5500:
            showEasterEgg("BAD A55", "badass.png");
            break;

        case 0xbaadf00d:
        case 0x0badf00d:
        case 0xbadf00d0:
            showEasterEgg("BAD F00D", "badfood.png");
            break;

        case 0x0d15ea5e:
        case 0xd15ea5e0:
            showEasterEgg("D15EA5E", "disease.png");
            break;

        case 0xfeedface:
            showEasterEgg("FEED FACE", "feedface.png");
            break;

        case 0xfaceb00c:
            showEasterEgg("FACE B00C", "facebooc.png");
            break;
    }
}
