#include "DirFileMgr.h"
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <QDebug>

CDirFileMgr& CDirFileMgr::ins()
{
	//non-const-local-static
    static Keeper kp(new CDirFileMgr());
	return *kp.get();
}

CDirFileMgr::CDirFileMgr()
    : m_workDir(qApp->applicationDirPath() + "/" + WORK_DIR_NAME + "/"),
      m_StandardDir(qApp->applicationDirPath() + "/" + STANDARD_DIR_NAME + "/"),
      m_CustomDir(qApp->applicationDirPath() + "/" + CUSTOM_DIR_NAME + "/"),
      m_ProjectDir(qApp->applicationDirPath() + "/" + PROJECT_DIR_NAME + "/")
{
    QString filePath = m_workDir + DEFAULT_WORK_NET_FILE_NAME;
    if (!fileExists(filePath))
    {
        QJsonObject netJsonObj;
        SaveWorkNet(netJsonObj);
    }
}

CDirFileMgr::~CDirFileMgr()
{

}

void CDirFileMgr::Init()
{
   if (!dirExists(m_workDir))
       makeDir(m_workDir);

   if (!dirExists(m_StandardDir))
       makeDir(m_StandardDir);

   if (!dirExists(m_CustomDir))
   {
       if (makeDir(m_CustomDir))
       {
            makeDir(m_CustomDir + BASEBLOCK_SUBDIR_NAME);
            makeDir(m_CustomDir + BACKBONE_SUBDIR_NAME);
            makeDir(m_CustomDir + NET_SUBDIR_NAME);
       }
   }
   else
   {
       if (!dirExists(m_CustomDir + BASEBLOCK_SUBDIR_NAME))
           makeDir(m_CustomDir + BASEBLOCK_SUBDIR_NAME);

       if (!dirExists(m_CustomDir + BACKBONE_SUBDIR_NAME))
           makeDir(m_CustomDir + BACKBONE_SUBDIR_NAME);

       if (!dirExists(m_CustomDir + NET_SUBDIR_NAME))
           makeDir(m_CustomDir + NET_SUBDIR_NAME);
   }

   if (!dirExists(m_ProjectDir))
   {
       if (makeDir(m_ProjectDir))
       {
           makeDir(m_ProjectDir + BASEBLOCK_SUBDIR_NAME);
           makeDir(m_ProjectDir + BACKBONE_SUBDIR_NAME);
           makeDir(m_ProjectDir + NET_SUBDIR_NAME);
       }
   }
   else
   {
       if (!dirExists(m_ProjectDir + BASEBLOCK_SUBDIR_NAME))
           makeDir(m_ProjectDir + BASEBLOCK_SUBDIR_NAME);

       if (!dirExists(m_ProjectDir + BACKBONE_SUBDIR_NAME))
           makeDir(m_ProjectDir + BACKBONE_SUBDIR_NAME);

       if (!dirExists(m_ProjectDir + NET_SUBDIR_NAME))
           makeDir(m_ProjectDir + NET_SUBDIR_NAME);
   }
}

QStringList CDirFileMgr::GetStandardCatalogs() const
{
    return getSubDirs(m_StandardDir);
}

QStringList CDirFileMgr::GetStandardNetIds(QString const &catalogName) const
{
    return getFiles(m_StandardDir + catalogName, "*." + NET_SUFFIX);
}

QStringList CDirFileMgr::GetCustomCatalogs() const
{
    return getSubDirs(m_CustomDir);
}

QStringList CDirFileMgr::GetCustomNetIds(QString const &catalogName) const
{
    return getFiles(m_CustomDir + catalogName, "*." + NET_SUFFIX);
}

bool CDirFileMgr::LoadWorkNet(QJsonObject &netJsonObj)
{
    QString filePath = m_workDir + DEFAULT_WORK_NET_FILE_NAME;
    if (fileExists(filePath))
        return readJson(filePath, netJsonObj);

    return false;
}

bool CDirFileMgr::SaveWorkNet(QJsonObject const &netJsonObj)
{
    QString filePath = m_workDir + DEFAULT_WORK_NET_FILE_NAME;
    return writeJson(filePath, netJsonObj);
}

bool CDirFileMgr::LoadStandardNet(QString const &catalogName, QString const &netId, QJsonObject &netJsonObj)
{
    QString filePath = m_StandardDir + catalogName + "/" + netId + "." + NET_SUFFIX;
    if (fileExists(filePath))
        return readJson(filePath, netJsonObj);

    return false;
}

bool CDirFileMgr::LoadCustomNet(QString const &catalogName, QString const &netId, QJsonObject &netJsonObj)
{
    QString filePath = m_CustomDir;
    if (!catalogName.isEmpty())
    {
        filePath += catalogName + "/";
        if (!dirExists(filePath))
            return false;
    }
    filePath += netId + "." + NET_SUFFIX;

    if (fileExists(filePath))
        return readJson(filePath, netJsonObj);

    return false;
}

bool CDirFileMgr::SaveCustomNet(QString const &catalogName, QString const &netId, QJsonObject const &netJsonObj, QString const &paramCode, QString const &netCode)
{
    QString filePath = m_CustomDir;
    if (!catalogName.isEmpty())
    {
        filePath += catalogName + "/";
        if (!dirExists(filePath) && !makeDir(filePath))
            return false;
    }

    bool ret = writeJson(filePath + netId + "." + NET_SUFFIX, netJsonObj);
    ret &= writeCode(filePath + netId + "_param.py", paramCode);
    ret &= writeCode(filePath + netId + "_net.py", netCode);
    return ret;
}

bool CDirFileMgr::DelCustomNet(QString const &catalogName, QString const &netId)
{
    QString filePath = m_CustomDir + catalogName + "/";

    bool ret = removeFile(filePath + netId + "." + NET_SUFFIX);
    ret &= removeFile(filePath + netId + "_param.py");
    ret &= removeFile(filePath + netId + "_net.py");
    return ret;
}

bool CDirFileMgr::DelCustomCatalog(QString const &catalogName)
{
    QString dirPath = m_CustomDir + catalogName;
    return removeDir(dirPath);
}

bool CDirFileMgr::CheckProjectFilePathUsable(QString const &catalog, QString const &name) const
{
    if (name.isEmpty())
        return false;

    if (!dirExists(m_ProjectDir))
        makeDir(m_ProjectDir);

    if (catalog.isEmpty())
    {
        return !fileExists(m_ProjectDir + name + "." + PROJECT_SUFFIX);
    }
    else
    {
        return !dirExists(m_ProjectDir + catalog) || !fileExists(m_ProjectDir + catalog + "/" + name + "." + PROJECT_SUFFIX);
    }
}

QString CDirFileMgr::MakeProjectFilePath(QString const &catalog, QString const &name) const
{
    if (name.isEmpty() || (!dirExists(m_ProjectDir) && !makeDir(m_ProjectDir)))
        return "";

    if (catalog.isEmpty())
    {
        return (m_ProjectDir + name + "." + PROJECT_SUFFIX);
    }
    else
    {
        auto dir = m_ProjectDir + catalog + "/";
        if (!dirExists(dir) && !makeDir(dir))
            return "";

        return (dir + name + "." + PROJECT_SUFFIX);
    }
}

QStringList CDirFileMgr::GetProjectsCatalogs() const
{
    return getSubDirs(m_ProjectDir);
}

QStringList CDirFileMgr::GetProjectsFilePath() const
{
    return doGetProjectsFilePath(m_ProjectDir);
}

QStringList CDirFileMgr::doGetProjectsFilePath(QString const &dirPath) const
{
    QStringList fileList;

    QDir dir(dirPath);
    if (!dir.exists())
        return fileList;

    auto dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto const& item : dirs)
        fileList += doGetProjectsFilePath(item.filePath());

    dir.setNameFilters(QStringList("*." + PROJECT_SUFFIX));
    auto files = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (auto const& item : files)
        fileList.push_back(item.filePath());

    return fileList;
}

bool CDirFileMgr::LoadProject(QJsonObject &netJsonObj, QString const &filePath)
{
    if (fileExists(filePath))
        return readJson(filePath, netJsonObj);

    return false;
}

bool CDirFileMgr::SaveProject(QJsonObject const &netJsonObj, QString const &filePath)
{
    return writeJson(filePath, netJsonObj);
}

bool CDirFileMgr::SaveProjectCode(QString const &project, QString const &code, QString const &dir)
{
    return writeCode(dir + "/" + project + ".py", code);
}

bool CDirFileMgr::DelProject(QString const &project, QString const &dir)
{
    auto delPy = removeFile(dir + "/" + project + ".py");
    auto delWgs = removeFile(dir + "/" + project + ".wgs");
    return delPy && delWgs;
}

QStringList CDirFileMgr::getSubDirs(QString const &dirPath) const
{
    QDir dir(dirPath);
    if (dir.exists())
        return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    return QStringList();
}

QStringList CDirFileMgr::getFiles(QString const &dirPath, QString const &filter) const
{
    QStringList fileList;

    QDir dir(dirPath);
    if (!dir.exists())
        return fileList;

    dir.setNameFilters(QStringList(filter));
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (auto const& item : fileInfoList)
        fileList.push_back(item.completeBaseName());

    return fileList;
}

bool CDirFileMgr::readJson(QString const &filePath, QJsonObject &jsonObj)
{
    QFile file(filePath);
    if (file.open(QFile::ReadOnly))
    {
        auto data = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &jsonError);
        //QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromBase64(data), &jsonError);

        if (jsonDoc.isNull())
        {
            emit showMsgDialogSignal(tr("Document is null!"));
        }
        else if (jsonError.error != QJsonParseError::NoError || !jsonDoc.isObject())
        {
            emit showMsgDialogSignal(tr("Json interpret error!"));
        }
        else
        {
            auto obj = jsonDoc.object();
            jsonObj.swap(obj);
            return true;
        }
    }

    return false;
}

bool CDirFileMgr::writeJson(QString const &filePath, QJsonObject const &jsonObj)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        emit showMsgDialogSignal(tr("Save file failed!"));
        return false;
    }

    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObj);
    file.write(jsonDoc.toJson());
    //file.write(jsonDoc.toJson().toBase64());
    file.close();

    return true;
}

bool CDirFileMgr::writeCode(QString const &filePath, QString const &code)
{
    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        emit showMsgDialogSignal(tr("Save file failed!"));
        return false;
    }

    QByteArray data;
    data.append(code);
    file.write(data);
    file.close();

    return true;
}

bool CDirFileMgr::dirExists(QString const &dirPath) const
{
    QDir dir(dirPath);
    return dir.exists();
}

bool CDirFileMgr::fileExists(QString const &filePath) const
{
    return QFile::exists(filePath);
}

bool CDirFileMgr::makeDir(QString const &dirPath) const
{
    QDir dir(dirPath);

    if (!dir.exists())
        return dir.mkdir(dirPath);

    return true;
}

bool CDirFileMgr::renameDir(QString const &oldPath, QString const &newPath) const
{
    QDir old(oldPath);
    return old.rename(oldPath, newPath);
}

bool CDirFileMgr::renameFile(QString const &oldFilePath, QString const &newFilePath) const
{
    return QFile::rename(oldFilePath, newFilePath);
}

bool CDirFileMgr::removeDir(QString const &dirPath) const
{
    QDir dir(dirPath);
    return dir.removeRecursively();
}

bool CDirFileMgr::removeFile(QString const &filePath) const
{
    return QFile::remove(filePath);
}
