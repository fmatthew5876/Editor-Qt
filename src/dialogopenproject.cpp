#include "dialogopenproject.h"
#include "ui_dialogopenproject.h"
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStyle>
#include "core.h"

DialogOpenProject::DialogOpenProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpenProject)
{
    setModal(true);
    ui->setupUi(this);

    // Hide Project Type
    ui->tableProjects->hideColumn(2);
}

DialogOpenProject::~DialogOpenProject()
{
    delete ui;
}

void DialogOpenProject::setDefDir(QString n_defDir)
{
    if (!n_defDir.endsWith('/'))
        n_defDir.append("/");
    m_defDir = n_defDir;
    ui->lineProjectPath->setText(n_defDir);
    RefreshProjectList();
}

QString DialogOpenProject::getDefDir()
{
    return m_defDir;
}

bool DialogOpenProject::isNativeProject()
{
    return (ui->tableProjects->item(ui->tableProjects->currentRow(),2)->text() == "1");
}

QString DialogOpenProject::getProjectFolder()
{
    return (ui->tableProjects->item(ui->tableProjects->currentRow(),0)->text());
}

void DialogOpenProject::RefreshProjectList()
{
    ui->tableProjects->setRowCount(0);
    QDir dir(m_defDir);
    if (!dir.exists())
        return;
    Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
        if (info.isDir())
        {
            QFileInfo f_project(info.absoluteFilePath()+"/"+EASY_DB);
            if (f_project.exists())
            {
                ui->tableProjects->insertRow(ui->tableProjects->rowCount());
                QTableWidgetItem *item = new QTableWidgetItem(f_project.absoluteDir().dirName());
                item->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                ui->tableProjects->setItem(ui->tableProjects->rowCount()-1,0,item);
                QSettings settings(m_defDir+info.baseName()+"/"+EASY_CFG,
                                    QSettings::IniFormat,
                                    this);
                item = new QTableWidgetItem(settings.value(GAMETITLE, "Untitled").toString());
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                ui->tableProjects->setItem(ui->tableProjects->rowCount()-1,1,item);

                item = new QTableWidgetItem("1");
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                ui->tableProjects->setItem(ui->tableProjects->rowCount()-1,2,item);
            } else {
                QFileInfo f_project(info.absoluteFilePath()+"/"+RM_DB);
                if (f_project.exists()) {
                    ui->tableProjects->insertRow(ui->tableProjects->rowCount());
                    QTableWidgetItem *item = new QTableWidgetItem(f_project.absoluteDir().dirName());
                    item->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    ui->tableProjects->setItem(ui->tableProjects->rowCount() - 1, 0, item);
                    QSettings settings(m_defDir + info.baseName() + "/" + RM_INI,
                                       QSettings::IniFormat,
                                       this);
                    settings.beginGroup("RPG_RT");
                    item = new QTableWidgetItem(settings.value(GAMETITLE, "Untitled").toString());
                    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                    ui->tableProjects->setItem(ui->tableProjects->rowCount()-1,1,item);

                    item = new QTableWidgetItem("0");
                    item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    ui->tableProjects->setItem(ui->tableProjects->rowCount()-1,2,item);
                }
            }
        }
    }
}

void DialogOpenProject::on_toolProjectPath_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select destination folder", m_defDir);
    if (path == QString())
        return;
    ui->lineProjectPath->setText(path+"/");
    m_defDir = ui->lineProjectPath->text();
    RefreshProjectList();
}

bool DialogOpenProject::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
                result = removeDir(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
            {
                QMessageBox::warning(this,
                                     "An error ocurred",
                                     QString("Could't delete %1").arg(info.absoluteFilePath()),
                                     QMessageBox::Ok, 0);
                return false;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void DialogOpenProject::on_tableProjects_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    emit ui->buttonBox->accepted();
}
