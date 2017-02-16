#!/bin/bash

#################### Project setup ####################

cd $2
cd $1
mkdir -p Plugin
cd Plugin
mkdir -p $1
cd $1

touch $6.cpp 
touch $6.h
touch mainview.cpp
touch mainview.h
touch mainview.ui
touch $1.pro
touch config_$1.ini
touch tst_$6.cpp
touch Main_$1.pro

#################### Main.cpp ########################

echo "#include \"$6.h\"
#include <stdio.h>

/**
 * @brief The constructor of the $1 class
 */
$1::$1() : main_ui(NULL)
{
}

/**
 * @brief The destructor of the $1 class
 */
$1::~$1(){
}

/**
 * @brief returns the Plugin-Version
 * @return 
 */
const QString $1::getPluginVersion(){
    return \"0.1-ALPHA\";
}

/**
 * @brief returns the Plugin-ID
 * @return 
 */
const QString $1::getId(){
    return \"$1\";
}

/**
 * @brief returns the Plugin-Description
 * @return 
 */
const QString $1::getDescription(){
    return \"New Project\";
}

/**
 * @brief returns if the Plugin has a GUI
 * @return 
 */
bool $1::hasGUI(){
    return true;
}

/**
 * @brief returns if the Plugin should autostart
 * @return 
 */
bool $1::shouldAutostart(){
    return true;
}

/**
 * @brief returns the Widget with GUI-values
 * @param width
 * @param height
 * @param parent
 * @return 
 */
QWidget* $1::getWidget(const int width, const int height, QWidget *parent){
    logDebug(\"## init $1 ui\");
    printf(\"###%p\", main_ui);

    if(!main_ui) {
        (void)width;
        (void)height;
	main_ui = new MainView(parent);
    }

    return main_ui;
}

/**
 * @brief the mainloop function
 */
void $1::mainloop() {
    this->main_ui->setDisabled(false);
}

/**
 * @brief the stop function
 */
void $1::stop() {
    this->main_ui->setDisabled(true);
}

/**
 * @brief the shutdown function
 */
void $1::shutdown() {
    this->main_ui->close();
}

/**
 * @brief The communication function for the middleware
 * @param receiverpeer
 * @param receiverplugin
 * @param senderpeer
 * @param senderplugin
 * @param timestamp
 * @param id
 * @param expectingReply
 * @param message
 */
void $1::processMessage(QString receiverpeer,QString receiverplugin, QString senderpeer, QString senderplugin, QDateTime timestamp, QString id, bool expectingReply, QString message){
#ifdef QT_DEBUG
    logDebug(message2str(receiverpeer, receiverplugin, senderpeer, senderplugin, timestamp, id, expectingReply, message));
#endif
} " > $6.cpp

#################### Main.h ####################

echo "#ifndef $3_H
#define $3_H

#include \"../../Middleware/Plugin_Interface/plugin.h\"
#include \"../../Middleware/Plugin_Interface/sil_constants.h\"

#include <QtPlugin>
#include \"mainview.h\"

class $1: public Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID \"org.sil.ApplicationPlugin\")
    Q_INTERFACES(Plugin)

public:
    $1();
    virtual ~$1();
    const QString getPluginVersion();
    const QString getId();
    const QString getDescription();
    bool hasGUI();
    bool shouldAutostart();
    QWidget* getWidget(const int width, const int height, QWidget *parent);
        void mainloop();
    void stop();
    void shutdown();


protected:
    void processMessage(QString receiverpeer,QString receiverplugin, QString senderpeer, QString senderplugin, QDateTime timestamp, QString id, bool expectingReply, QString message);

private:
    MainView *main_ui;
};

#endif // $3_H " >$6.h

#################### MainView.cpp ####################

echo "#include \"mainview.h\"
#include \"ui_mainview.h\"

/**
 * @brief The constructor of the MainView Class
 * @param parent
 */
MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
}

/**
 * @brief The destructor of the MainView Class
 */
MainView::~MainView()
{
    delete ui;
}
" >mainview.cpp

#################### MainView.h #####################

echo "#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>

namespace Ui {
class MainView;
}

class MainView : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = 0);
    ~MainView();

private:
    Ui::MainView *ui;
};

#endif // MAINVIEW_H " >mainview.h

#################### MainView.ui ####################

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<ui version=\"4.0\">
 <class>MainView</class>
 <widget class=\"QMainWindow\" name=\"MainView\">
  <property name=\"geometry\">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>800</width>
    <height>600</height>
   </rect>
  </property>
  <property name=\"windowTitle\">
   <string>MainWindow</string>
  </property>
  <widget class=\"QWidget\" name=\"centralwidget\"/>
  <widget class=\"QMenuBar\" name=\"menubar\">
   <property name=\"geometry\">
    <rect>
     <x>0</x>
     <y>0</y>
     <width>800</width>
     <height>23</height>
    </rect>
   </property>
  </widget>
  <widget class=\"QStatusBar\" name=\"statusbar\"/>
 </widget>
 <resources/>
 <connections/>
</ui>" >mainview.ui

#################### Main.pro #####################

echo "TEMPLATE = lib
TARGET = ../../build/$6

CONFIG += plugin debug_and_release
QT += core widgets gui 

HEADERS += $6.h \\
	mainview.h

SOURCES += $6.cpp \\
	mainview.cpp 

FORMS += mainview.ui

unix:!macx: QMAKE_POST_LINK = doxygen ../../Documentation/Doxyfile

unix:!macx: LIBS += -L \$\$PWD/../../build/ -lPluginInterface

INCLUDEPATH += \$\$PWD/../../build
DEPENDPATH += \$\$PWD/../../build

unix:!macx: PRE_TARGETDEPS += \$\$PWD/../../build/libPluginInterface.a" >$1.pro

#################### config.ini ####################

echo "[General]
iface=eth0
ip=127.0.0.1
port=6666

logfilter=*
mode=NORMAL
peerid=demo
peers=\"demo:127.0.0.1:6666;\"

pluginpath=./
pluginrestrictions=\"lib$6.so;\"
Defaultplugins=\"\"

ui=true
uiheight=768
uiwidth=1024
GUIDefaultplugin=\"\" " >config_$1.ini 

#################### Test.pro #######################

echo "QT       += widgets testlib

TARGET = tst_$6
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_$6.cpp \

INCLUDEPATH += /$6

LIBS += ../../build/lib$6.so

unix:!macx: QMAKE_POST_LINK = ./tst_$6

DEFINES += SRCDIR=\\\\\\\"\$\$PWD/\\\\\\\" " > Test.pro

#################### tst_test.cpp###################

echo "#include <QString>
#include <QtTest>
#include <$6.h>

class Test_$1 : public QObject
{
    Q_OBJECT

public:
    Test_$1();

private Q_SLOTS:
    void initTestCase();
    void testCase1();

private:
    $1 * $6;
};

Test_$1::Test_$1()
{

}

void Test_$1::initTestCase()
{
    $6 = new $1();
}

void Test_$1::testCase1()
{
    QVERIFY($6->getId() == \"$1\");
}

QTEST_APPLESS_MAIN(Test_$1)

#include \"tst_$6.moc\"" > tst_$6.cpp

#################### Main_.pro######################

echo "TEMPLATE = subdirs
 CONFIG += ordered

 SUBDIRS = $1.pro\
           Test.pro " > Main_$1.pro

#################### Doxygen #######################

cd ../../
cd Documentation
chmod +rwx Doxyfile
sed -i "35s|.*|PROJECT_NAME           = "$1"|" Doxyfile
sed -i "61s|.*|OUTPUT_DIRECTORY       = "$4"|" Doxyfile
sed -i "784s|.*|INPUT                  = "$5"|" Doxyfile

doxygen

#################### Middleware ####################

cd ../

tar -xzf Middleware.tar.gz 

rm -f Middleware.tar.gz

#################### yml File   #####################

touch .gitlab-ci.yml

echo "stages:
  - build
  - test

job1:
  stage: build
  script:
    - ./.builder.sh
  only:
    - master
  tags:
    - shell
  artifacts:
   paths:
    - build/
    - Plugin/

job2:
  stage: test
  script:
    - ./.tester.sh
  only:
    - master
  tags:
    - shell" > .gitlab-ci.yml

################### builder-Script #################

touch .builder.sh
chmod +rwx .builder.sh

echo "#!/bin/bash

cd Plugin/$1/ && sed -i '/QMAKE_POST_LINK/d' $1.pro
sed -i '/QMAKE_POST_LINK/d' Test.pro

cd ../../Middleware/Plugin_Interface && qmake && make clean && make;
cd ../Middleware && qmake && make clean && make;
cd ../../Plugin/$1/ && qmake Main_$1.pro && make clean && make;" > .builder.sh

#################### tester-Script ################

touch .tester.sh
chmod +rwx .tester.sh

echo "#!/bin/bash

cd Plugin/$1/ && ./tst_$6" > .tester.sh



