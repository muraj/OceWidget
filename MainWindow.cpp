#include <QFileDialog>
#include <QDir>

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <TopTools_HSequenceOfShape.hxx>
#include <AIS_Shape.hxx>
#include <STEPControl_Reader.hxx>

static Handle_TopTools_HSequenceOfShape importBREP(const QString& filename);
static Handle_TopTools_HSequenceOfShape importSTEP(const QString& filename);
static Handle_TopTools_HSequenceOfShape importIGES(const QString& filename);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    lastFolder(QDir::homePath())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
  QString filterChosen;
  QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
      lastFolder, tr("BREP (*.brep *.rle);;"
                     "STEP (*.step *.stp);;"
                     "IGES (*.iges *.igs)"), &filterChosen);
  filterChosen = filterChosen.split(" ")[0];  // Get the format type
  // TODO: Replace with I/O plugins...
  // TODO: Background thread
  QApplication::setOverrideCursor(Qt::WaitCursor);
  Handle_TopTools_HSequenceOfShape shapes;
  try { 
    if     (filterChosen.startsWith("BREP")) {
      shapes = importBREP(filename);
    }
    else if(filterChosen.startsWith("STEP")) {
      shapes = importSTEP(filename);
    }
    else if(filterChosen.startsWith("IGES")) {
      shapes = importIGES(filename);
    }
  } catch(Standard_Failure) {

  }
  if(!shapes.IsNull() && shapes->Length() != 0) {
    Handle_AIS_InteractiveContext ctx = ui->oceWidget->getContext();
    for(int i=1; i<=shapes->Length(); i++)
    {
      Handle(AIS_Shape) shape = new AIS_Shape(shapes->Value(i));
      ctx->SetMaterial(shape, Graphic3d_NOM_GOLD);
      ctx->SetColor(shape, Quantity_NOC_RED);
      ctx->SetDisplayMode(shape, 1, Standard_False);
      ctx->Display(shape, Standard_False);
    }
    ctx->UpdateCurrentViewer();
  }
  QApplication::restoreOverrideCursor();
}

Handle_TopTools_HSequenceOfShape importBREP(const QString& filename)
{
  return Handle_TopTools_HSequenceOfShape(0);
}
Handle_TopTools_HSequenceOfShape importSTEP(const QString& filename)
{
  const bool printOnlyFailures = false;
  Handle_TopTools_HSequenceOfShape retShapes;
  STEPControl_Reader reader;
  if(reader.ReadFile(filename.toUtf8().constData()) != IFSelect_RetDone) {
    return retShapes;
  }
  //Interface_TraceFile::SetDefault();
  reader.PrintCheckLoad(printOnlyFailures, IFSelect_ItemsByEntity);
  int n = reader.NbRootsForTransfer();
  reader.PrintCheckTransfer(printOnlyFailures, IFSelect_ItemsByEntity);
  retShapes = new TopTools_HSequenceOfShape();
  for(int i=1; i<=n; i++) {
    if(!reader.TransferRoot(i)) continue;
    int ns = reader.NbShapes();
    for(int j=1; j<=ns; j++)
      retShapes->Append(reader.Shape(j));
  }
  return retShapes;
}
Handle_TopTools_HSequenceOfShape importIGES(const QString& filename)
{
  return Handle_TopTools_HSequenceOfShape(0);
}
