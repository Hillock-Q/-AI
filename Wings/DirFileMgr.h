#ifndef DIRFILEMGR_H
#define DIRFILEMGR_H

#include <QObject>

#define WORK_DIR_NAME                       QString("work")
#define STANDARD_DIR_NAME                   QString("standard")

#define CUSTOM_DIR_NAME                     QString("custom")
#define BASEBLOCK_SUBDIR_NAME               QString("baseblock")
#define BACKBONE_SUBDIR_NAME                QString("backbone")
#define NET_SUBDIR_NAME                     QString("net")

#define PROJECT_DIR_NAME                    QString("project")

#define NET_SUFFIX                          QString("net")
#define PROJECT_SUFFIX                      QString("wgs")

#define DEFAULT_WORK_NET_FILE_NAME          QString("work.") + NET_SUFFIX
#define DEFAULT_DEMO_PROJECT_FILE_NAME      QString("demo.") + PROJECT_SUFFIX



class QJsonObject;

class CDirFileMgr: public QObject
{
    Q_OBJECT

public:
    static CDirFileMgr& ins();

    void Init();

    QStringList GetStandardCatalogs() const;
    QStringList GetStandardNetIds(QString const &catalogName) const;

    QStringList GetCustomCatalogs() const;
    QStringList GetCustomNetIds(QString const &catalogName) const;

    bool LoadWorkNet(QJsonObject &netJsonObj);
    bool SaveWorkNet(QJsonObject const &netJsonObj);

    bool LoadStandardNet(QString const &catalogName, QString const &netId, QJsonObject &netJsonObj);

    bool LoadCustomNet(QString const &catalogName, QString const &netId, QJsonObject &netJsonObj);
    bool SaveCustomNet(QString const &catalogName, QString const &netId, QJsonObject const &netJsonObj, QString const &paramCode, QString const &netCode);

    bool DelCustomNet(QString const &catalogName, QString const &netId);
    bool DelCustomCatalog(QString const &catalogName);


    bool CheckProjectFilePathUsable(QString const &catalog, QString const &name) const;
    QString MakeProjectFilePath(QString const &catalog, QString const &name) const;

    QStringList GetProjectsCatalogs() const;
    QStringList GetProjectsFilePath() const;

    bool LoadProject(QJsonObject &netJsonObj, QString const &filePath);
    bool SaveProject(QJsonObject const &netJsonObj, QString const &filePath);
    bool SaveProjectCode(QString const &project, QString const &code, QString const &dir);

    bool DelProject(QString const &project, QString const &dir);

signals:
    void showInfoSignal(QString const &info);
    void showMsgDialogSignal(QString const &msg);


private slots:


private:
    CDirFileMgr();
    ~CDirFileMgr();

    CDirFileMgr(CDirFileMgr &&rhs) = delete;
    CDirFileMgr(CDirFileMgr const &rhs) = delete;

    CDirFileMgr& operator=(CDirFileMgr &&rhs) = delete;
    CDirFileMgr& operator=(CDirFileMgr const &rhs) = delete;

    class Keeper
    {
    public:
        Keeper(CDirFileMgr *pt = 0) : p(pt){}
        CDirFileMgr* get() const { return p; }
        ~Keeper() { delete p; }

    private:
        CDirFileMgr *p;
    };

    QStringList doGetProjectsFilePath(QString const &dirPath) const;

    QStringList getSubDirs(QString const &dirPath) const;
    QStringList getFiles(QString const &dirPath, QString const &filter) const;

    bool readJson(QString const &filePath, QJsonObject &jsonObj);
    bool writeJson(QString const &filePath, QJsonObject const &jsonObj);
    bool writeCode(QString const &filePath, QString const &code);

    bool dirExists(QString const &dirPath) const;
    bool fileExists(QString const &filePath) const;

    bool makeDir(QString const &dirPath) const;

    bool renameDir(QString const &oldPath, QString const &newPath) const;
    bool renameFile(QString const &oldFilePath, QString const &newFilePath) const;

    bool removeDir(QString const &dirPath) const;
    bool removeFile(QString const &filePath) const;

private:
    QString                         m_workDir;
    QString                         m_StandardDir;
    QString                         m_CustomDir;
    QString                         m_ProjectDir;
};

#endif // DIRFILEMGR_H
