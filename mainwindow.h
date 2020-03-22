#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Includes for this project
#include "structuredefinitions.h"
#include "sexysolver.h"

//system includes
#include "math.h"
#include <sys/mman.h>

//QT Includes
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QPointer>
#include <QScrollBar>
#include <QTime>
#include <QThread>
#include <QLineEdit>

//CFitsio Includes
#include "longnam.h"
#include "fitsio.h"

//KStars related includes
#include "stretch.h"
#include "math.h"
#include "dms.h"
#include "bayer.h"

#ifndef _WIN32 //Commented out for now on Windows due to compilation issues.
//Astrometry.net includes
extern "C"{
#include "tic.h"
#include "os-features.h"
#include "fileutils.h"
#include "ioutils.h"
#include "bl.h"
#include "an-bool.h"
#include "solver.h"
#include "fitsioutils.h"
#include "blindutils.h"
#include "astrometry/blind.h"
#include "log.h"
#include "engine.h"
#include "an-opts.h"
#include "gslutils.h"
#include "augment-xylist.h"
#include "anqfits.h"
#include "ioutils.h"
#include "fitsioutils.h"
#include "permutedsort.h"
#include "fitstable.h"
#include "sip_qfits.h"
#include "sip-utils.h"
#include "tabsort.h"
}
#endif

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow();
    ~MainWindow();

    void setStarList(QList<Star> starList){stars = starList;};

private:
    Ui::MainWindow *ui;

    QPointer<QProcess> solver;
    QPointer<SexySolver> internalSolver;
    QPointer<QProcess> sextractorProcess;
    QString fileToSolve;
    QString sextractorFilePath;
    QList<Star> stars;
    int selectedStar;

    //System File Paths
    QString dirPath;
    QString tempPath;
    QStringList indexFilePaths;
#if defined(Q_OS_OSX)
    QString sextractorBinaryPath = "/usr/local/bin/sex";
#elif defined(Q_OS_LINUX)
    QString sextractorBinaryPath = "/usr/bin/sextractor";
#else
     QString sextractorBinaryPath = "C:/cygwin64/bin/sextractor";
#endif

#if defined(Q_OS_OSX)
    QString confPath = "/Applications/KStars.app/Contents/MacOS/astrometry/bin/astrometry.cfg";
#elif defined(Q_OS_LINUX)
    QString confPath = "$HOME/.local/share/kstars/astrometry/astrometry.cfg";
#else
    //QString confPath = "C:/cygwin64/usr/etc/astrometry.cfg";
    QString confPath = QDir::homePath() + "/AppData/Local/cygwin_ansvr/etc/astrometry/backend.cfg";
#endif

#if defined(Q_OS_OSX)
    QString solverPath = "/usr/local/bin/solve-field";
#elif defined(Q_OS_LINUX)
    QString solverPath = "/usr/bin/solve-field";
#else
    //QString solverPath = "C:/cygwin64/bin/solve-field";
    QString solverPath = QDir::homePath() + "/AppData/Local/cygwin_ansvr/lib/astrometry/bin/solve-field.exe";
#endif

#if defined(Q_OS_OSX)
    QString wcsPath = "/usr/local/bin/wcsinfo";
#elif defined(Q_OS_LINUX)
    QString wcsPath = "/usr/bin/wcsinfo";
#else
    //QString wcsPath = "C:/cygwin64/bin/wcsinfo";
    QString wcsPath = QDir::homePath() + "/AppData/Local/cygwin_ansvr/lib/astrometry/bin/wcsinfo.exe";
#endif

    //Parameters for sextracting
    Shape apertureShape = SHAPE_CIRCLE;
    double kron_fact = 2.5;
    int subpix = 5;
    float r_min = 3.5;
    short inflags;
    float magzero = 20;
    float minarea = 5;
    int deblend_thresh = 32;
    float deblend_contrast = 1;
    int clean = 1;
    double clean_param = 1;
    double fwhm = 2;

    //Star Filter Parameters
    bool resort = true;  //NOTE: This is REQUIRED for the filters
    double maxEllipse = 0;
    double removeBrightest = 0;
    double removeDimmest = 0;

    //Parameters for solving
    bool use_scale = false;
    double fov_low, fov_high;
    QString units;
    bool use_position = false;
    double ra;
    double dec;
    double radius;

    bool inParallel = true;
    int solverTimeLimit = 600;
    double minwidth = 0.1;
    double maxwidth = 180;

    bool logToFile = false;
    QString logFile = QDir::tempPath() + "/AstrometryLog.txt";
    int logLevel = 0;

    //LogOdds Settings
    double logratio_tosolve = log(1e9);
    double logratio_tokeep  = log(1e9);
    double logratio_totune  = log(1e6);

    //Data about the image
    bool imageLoaded = false;
    Statistic stats;
    fitsfile *fptr { nullptr };
    QImage rawImage;
    QImage scaledImage;
    int currentWidth;
    int currentHeight;
    double currentZoom;
    int sampling = 2;
    /// FITS image data type (TBYTE, TUSHORT, TINT, TFLOAT, TLONG, TDOUBLE)
    uint32_t m_DataType { 0 };
    /// Number of channels
    uint8_t m_Channels { 1 };
    /// Generic data image buffer
    uint8_t *m_ImageBuffer { nullptr };
    /// Above buffer size in bytes
    uint32_t m_ImageBufferSize { 0 };
    StretchParams stretchParams;
    BayerParams debayerParams;
    bool checkDebayer();

    QTime solverTimer;

    void setupSolutionTable();
    void clearAstrometrySettings();
    void addSextractionToTable();

public slots:

    bool prepareForProcesses();
    void logOutput(QString text);
    void logSolver();
    void logSextractor();
    void clearLog();

    //These are the functions that run when the bottom buttons are clicked
    bool sextractImage();
    bool solveImage();
    bool sextractInternally();
    bool solveInternally();
    void abort();

    //These functions are for loading and displaying the image
    bool loadImage();
    bool loadFits();
    bool loadOtherFormat();
    bool saveAsFITS();
    bool debayer();
    bool debayer_8bit();
    bool debayer_16bit();
    void initDisplayImage();
    void doStretch(QImage *outputImage);
    void clearImageBuffers();

    void zoomIn();
    void zoomOut();
    void autoScale();
    void updateImage();

    //These functions are for the display of stars in the table
    void displayTable();
    void sortStars();
    void starClickedInTable();
    void updateStarTableFromList();
    bool getSextractorTable(QList<Star> *stars);

    void mouseOverStar(QPoint location);
    void mouseClickedOnStar(QPoint location);
    QRect getStarInImage(Star star);

    //These functions are for loading and parsing the options
    QStringList getSolverArgsList();
    bool getSolverOptionsFromFITS();

    //These functions are for the external sextractor and solver
    bool sextract(bool justSextract);
    bool solveField();
    bool externalSextractorComplete();
    bool internalSolverComplete(int x);

    //These functions are for the internal sextractor and solver
    bool runInnerSextractor();
    bool runInnerSolver();
    bool innerSextractorComplete();
    bool externalSolverComplete(int x);
    bool getSolutionInformation();

    //These are for both
    bool sextractorComplete();
    bool solverComplete(int x);
    void addSolutionToTable(Solution solution);

};

#endif // MAINWINDOW_H
