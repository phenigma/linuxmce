#include <QtGui/QtGui>

#include "emu.h"
#include "debugger.h"

#include "debugqtwindow.h"
#include "debugqtlogwindow.h"
#include "debugqtdasmwindow.h"
#include "debugqtmemorywindow.h"

bool WindowQt::s_refreshAll = false;


WindowQt::WindowQt(running_machine* machine, QWidget* parent) :
	QMainWindow(parent),
	m_machine(machine)
{
	// The Debug menu bar
	QAction* debugActOpenMemory = new QAction("New &Memory Window", this);
	debugActOpenMemory->setShortcut(QKeySequence("Ctrl+M"));
	connect(debugActOpenMemory, SIGNAL(triggered()), this, SLOT(debugActOpenMemory()));

	QAction* debugActOpenDasm = new QAction("New &Dasm Window", this);
	debugActOpenDasm->setShortcut(QKeySequence("Ctrl+D"));
	connect(debugActOpenDasm, SIGNAL(triggered()), this, SLOT(debugActOpenDasm()));

	QAction* debugActOpenLog = new QAction("New &Log Window", this);
	debugActOpenLog->setShortcut(QKeySequence("Ctrl+L"));
	connect(debugActOpenLog, SIGNAL(triggered()), this, SLOT(debugActOpenLog()));

	QAction* dbgActRun = new QAction("Run", this);
	dbgActRun->setShortcut(Qt::Key_F5);
	connect(dbgActRun, SIGNAL(triggered()), this, SLOT(debugActRun()));

	QAction* dbgActRunAndHide = new QAction("Run And Hide Debugger", this);
	dbgActRunAndHide->setShortcut(Qt::Key_F12);
	connect(dbgActRunAndHide, SIGNAL(triggered()), this, SLOT(debugActRunAndHide()));

	QAction* dbgActRunToNextCpu = new QAction("Run to Next CPU", this);
	dbgActRunToNextCpu->setShortcut(Qt::Key_F6);
	connect(dbgActRunToNextCpu, SIGNAL(triggered()), this, SLOT(debugActRunToNextCpu()));

	QAction* dbgActRunNextInt = new QAction("Run to Next Interrupt on This CPU", this);
	dbgActRunNextInt->setShortcut(Qt::Key_F7);
	connect(dbgActRunNextInt, SIGNAL(triggered()), this, SLOT(debugActRunNextInt()));

	QAction* dbgActRunNextVBlank = new QAction("Run to Next VBlank", this);
	dbgActRunNextVBlank->setShortcut(Qt::Key_F8);
	connect(dbgActRunNextVBlank, SIGNAL(triggered()), this, SLOT(debugActRunNextVBlank()));

	QAction* dbgActStepInto = new QAction("Step Into", this);
	dbgActStepInto->setShortcut(Qt::Key_F11);
	connect(dbgActStepInto, SIGNAL(triggered()), this, SLOT(debugActStepInto()));

	QAction* dbgActStepOver = new QAction("Step Over", this);
	dbgActStepOver->setShortcut(Qt::Key_F10);
	connect(dbgActStepOver, SIGNAL(triggered()), this, SLOT(debugActStepOver()));

	QAction* dbgActStepOut = new QAction("Step Out", this);
	dbgActStepOut->setShortcut(QKeySequence("Shift+F11"));
	connect(dbgActStepOut, SIGNAL(triggered()), this, SLOT(debugActStepOut()));

	QAction* dbgActSoftReset = new QAction("Soft Reset", this);
	dbgActSoftReset->setShortcut(Qt::Key_F3);
	connect(dbgActSoftReset, SIGNAL(triggered()), this, SLOT(debugActSoftReset()));

	QAction* dbgActHardReset = new QAction("Hard Reset", this);
	dbgActHardReset->setShortcut(QKeySequence("Shift+F3"));
	connect(dbgActHardReset, SIGNAL(triggered()), this, SLOT(debugActHardReset()));

	QAction* dbgActClose = new QAction("Close &Window", this);
	dbgActClose->setShortcut(QKeySequence::Close);
	connect(dbgActClose, SIGNAL(triggered()), this, SLOT(debugActClose()));

	QAction* dbgActQuit = new QAction("&Quit", this);
	dbgActQuit->setShortcut(QKeySequence::Quit);
	connect(dbgActQuit, SIGNAL(triggered()), this, SLOT(debugActQuit()));

	// Construct the menu
	QMenu* debugMenu = menuBar()->addMenu("&Debug");
	debugMenu->addAction(debugActOpenMemory);
	debugMenu->addAction(debugActOpenDasm);
	debugMenu->addAction(debugActOpenLog);
	debugMenu->addSeparator();
	debugMenu->addAction(dbgActRun);
	debugMenu->addAction(dbgActRunToNextCpu);
	debugMenu->addAction(dbgActRunNextInt);
	debugMenu->addAction(dbgActRunNextVBlank);
	debugMenu->addSeparator();
	debugMenu->addAction(dbgActStepInto);
	debugMenu->addAction(dbgActStepOver);
	debugMenu->addAction(dbgActStepOut);
	debugMenu->addSeparator();
	debugMenu->addAction(dbgActSoftReset);
	debugMenu->addAction(dbgActHardReset);
	debugMenu->addSeparator();
	debugMenu->addAction(dbgActClose);
	debugMenu->addAction(dbgActQuit);
}


void WindowQt::debugActOpenMemory()
{
	MemoryWindow* foo = new MemoryWindow(m_machine, this);
	// A valiant effort, but it just doesn't wanna' hide behind the main window & not make a new toolbar icon
	// foo->setWindowFlags(Qt::Dialog);
	// foo->setWindowFlags(foo->windowFlags() & ~Qt::WindowStaysOnTopHint);
	foo->show();
}


void WindowQt::debugActOpenDasm()
{
	DasmWindow* foo = new DasmWindow(m_machine, this);
	// A valiant effort, but it just doesn't wanna' hide behind the main window & not make a new toolbar icon
	// foo->setWindowFlags(Qt::Dialog);
	// foo->setWindowFlags(foo->windowFlags() & ~Qt::WindowStaysOnTopHint);
	foo->show();
}


void WindowQt::debugActOpenLog()
{
	LogWindow* foo = new LogWindow(m_machine, this);
	// A valiant effort, but it just doesn't wanna' hide behind the main window & not make a new toolbar icon
	// foo->setWindowFlags(Qt::Dialog);
	// foo->setWindowFlags(foo->windowFlags() & ~Qt::WindowStaysOnTopHint);
	foo->show();
}


void WindowQt::debugActRun()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go();
}

void WindowQt::debugActRunAndHide()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go();
	// TODO: figure out hide
}

void WindowQt::debugActRunToNextCpu()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go_next_device();
}

void WindowQt::debugActRunNextInt()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go_interrupt();
}

void WindowQt::debugActRunNextVBlank()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go_vblank();
}

void WindowQt::debugActStepInto()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->single_step();
}

void WindowQt::debugActStepOver()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->single_step_over();
}

void WindowQt::debugActStepOut()
{
	debug_cpu_get_visible_cpu(*m_machine)->debug()->single_step_out();
}

void WindowQt::debugActSoftReset()
{
	m_machine->schedule_soft_reset();
}

void WindowQt::debugActHardReset()
{
	// TODO: Figure out segfault
	m_machine->schedule_hard_reset();
	debug_cpu_get_visible_cpu(*m_machine)->debug()->go();
}

void WindowQt::debugActClose()
{
	close();
}

void WindowQt::debugActQuit()
{
	m_machine->schedule_exit();
	qApp->closeAllWindows();
}
