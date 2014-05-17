#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "glsl.h"
#include "syntaxhighlighter.h"
#include "glctxnotify.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QOpenGLShader>

class MainWindow::TabEnt {
	using UPHL = std::unique_ptr<glsl::SyntaxHighlighter>;
	using UPDoc = std::unique_ptr<QTextDocument>;
	private:
		MainWindow*		_pMain = nullptr;
		UPHL			_hl;
		QTextEdit*		_tedit = nullptr;
		QString			_path,		//!< シェーダーファイルパス
						_extension,
						_baseTitle,
						_filter;
		UPDoc			_savedDoc;	//!< ファイルに保存してあるテキスト
		int				_tabIndex = -1;

		bool _isModified() const {
			if(_tedit) {
				// セーブ or ロードした時の文章と比較
				auto* doc = _tedit->document();
				if(_savedDoc->characterCount() != doc->characterCount() ||
					_savedDoc->toPlainText() != doc->toPlainText())
				{
					return true;
				}
			}
			return false;
		}
	public:
		void reset() {
			// onTextChangeが呼ばれるので先にクリアしておく
			_tedit = nullptr;
			// 全てのメンバをクリアして真っ更な状態にする
			this->~TabEnt();
			new(this) TabEnt();
		}
		void init(QTextEdit* te, const QString& filter, const QString& ext, int tabIndex, const QString& baseTitle, MainWindow* w) {
			_tedit = te;
			_filter = filter;
			_tabIndex = tabIndex;
			_extension = ext;
			_pMain = w;
			_baseTitle = baseTitle;
			_savedDoc.reset(te->document()->clone());

			_hl.reset(new glsl::SyntaxHighlighter(te->document()));
			auto appPath = QApplication::applicationDirPath();
			_hl->loadUserFormat(appPath + "/usercfg.json");
			_hl->loadDefine(appPath + "/defs");
			_hl->loadBlockDefine(appPath + "/block.json");
			QTextCharFormat& fmt = _hl->defaultFormat();
			fmt.setForeground(Qt::darkGreen);
			QObject::connect(te, &QTextEdit::textChanged, [this](){
				onTextChange();
			});
			te->clear();
			onTextChange();
		}
		static QStringRef ExtractFileName(const QString& path) {
			QRegExp re(R"([\w\d_]+(\.)?[\w\d]+$)");
			int idx = path.indexOf(re);
			if(idx >= 0)
				return QStringRef(&path, idx, re.matchedLength());
			return QStringRef(&path, 0, path.length());
		}
		static QString ExtractExtension(const QString& path) {
			QRegExp re(R"(\.([\w\d_]+)$)");
			int idx = path.indexOf(re);
			if(idx >= 0)
				return QStringRef(&path, idx, re.matchedLength()).toString();
			return QString();
		}
		QString makeTitle() {
			QChar mc(' ');
			if(_isModified())
				mc = '*';
			return QString("%1(%2)%3").arg(_baseTitle).arg(ExtractFileName(_path).toString()).arg(mc);
		}
		void onTextChange() {
			QMetaObject::invokeMethod(_pMain, "onTabTitleChanged", Qt::QueuedConnection,
									  Q_ARG(int, _tabIndex), Q_ARG(QString, makeTitle()));
		}
		bool clear() {
			// 未保存だったら確認する
			if(_isModified()) {
				int res = QMessageBox::question(_pMain, "save confirm", "the document is modified. would you like save it now?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No, QMessageBox::StandardButton::Cancel);
				if(res == QMessageBox::Yes) {
					save();
				} else if(res == QMessageBox::No) {}
				else {
					// 何もせず終了
					return false;
				}
			}
			_path.clear();
			_tedit->clear();
			_savedDoc.reset(_tedit->document()->clone());
			return true;
		}
		void load(const QString& name, const QByteArray& data) {
			// 未保存だったら現在の文章を保存するか確認
			if(clear()) {
				_path = name;
				_tedit->setText(data);
				_savedDoc.reset(_tedit->document()->clone());
				onTextChange();
			}
		}
		bool save() {
			if(_path.isEmpty()) {
				// ファイルダイアログを開く
				QFileDialog dlg(_pMain);
				dlg.setFileMode(QFileDialog::AnyFile);
				dlg.setFilter(QDir::Readable | QDir::Files);
				dlg.setViewMode(QFileDialog::ViewMode::List);
				dlg.setNameFilter(_filter);
				if(dlg.exec() == QFileDialog::Rejected)
					return false;
				_path = dlg.selectedFiles()[0];
			}
			QFile file;
			file.setFileName(_path);
			if(file.open(QFile::WriteOnly)) {
				QString str = _tedit->toPlainText();
				file.write(str.toUtf8());
				_savedDoc.reset(_tedit->document()->clone());
				onTextChange();
			} else {
				QMessageBox::warning(_pMain, "error", QString("can't open file %1").arg(file.fileName()));
				return false;
			}
			return true;
		}
		void saveAs() {
			QFileDialog dlg;
			dlg.setFileMode(QFileDialog::AnyFile);
			dlg.setFilter(QDir::Writable | QDir::Files);
			dlg.setViewMode(QFileDialog::ViewMode::List);
			dlg.setNameFilter(_filter);
			if(dlg.exec() == QFileDialog::Accepted) {
				QString tmp = _path;
				_path = dlg.selectedFiles()[0];
				if(ExtractExtension(_path).isEmpty()) {
					_path.append('.');
					_path.append(_extension);
				}
				save();
				_path = tmp;
			}
		}
};

// --------------------- MainWindow ---------------------
MainWindow::MainWindow(QWidget *parent):
	QMainWindow(parent),
	_tab(std::make_shared<TabV>(glsl::Shader::_Num)),
	_ui(std::make_shared<Ui::MainWindow>())
{
	_ui->setupUi(this);
	_ui->glwidget->hide();
	QObject::connect(_ui->glwidget, &glsl::GLCtxNotify::onContextInitialized, [this](QOpenGLContext* ctx){
		_ctx = ctx;
		initializeOpenGLFunctions();
	});
	(*_tab)[glsl::Shader::Vertex].init(_ui->teVS, "Vertex Shader (*.vsh)", "vsh", glsl::Shader::Vertex, "VertexShader: ", this);
	(*_tab)[glsl::Shader::Fragment].init(_ui->teFS, "Fragment Shader (*.fsh)", "fsh", glsl::Shader::Fragment, "FragmentShader: ", this);
}
MainWindow::~MainWindow() {
	for(auto& t : *_tab)
		t.reset();
}
void MainWindow::onTabTitleChanged(int index, const QString& title) {
	_ui->tabWidget->setTabText(index, title);
}
void MainWindow::loadShader() {
	// 拡張子でVSかFSを判断
	QFileDialog dlg(this);
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setFilter(QDir::Readable | QDir::Files);
	dlg.setViewMode(QFileDialog::ViewMode::List);
	dlg.setNameFilter("Shader files (*.vsh *.fsh)");
	if(dlg.exec() == QFileDialog::Accepted) {
		QRegExp re(R"(\.([\w\d]+)$)");
		QStringList files = dlg.selectedFiles();
		for(auto& f : files) {
			int idx = f.indexOf(re);
			if(idx >= 0) {
				QString ext = re.capturedTexts()[1];
				glsl::Shader::Type type;
				if(ext == "vsh")
					type = glsl::Shader::Vertex;
				else if(ext == "fsh")
					type = glsl::Shader::Fragment;
				else
					continue;

				QFile file;
				file.setFileName(f);
				if(file.open(QFile::ReadOnly)) {
					auto s = file.readAll();
					(*_tab)[type].load(file.fileName(), s);
				} else
					QMessageBox::warning(this, "error", QString("can't open file %1").arg(file.fileName()));
			}
		}
	}
}
void MainWindow::saveCurrent() {
	int index = _ui->tabWidget->currentIndex();
	(*_tab)[static_cast<glsl::Shader::Type>(index)].save();
}
void MainWindow::saveAll() {
	for(auto& t : *_tab) {
		if(!t.save())
			break;
	}
}
void MainWindow::saveAs() {
	int index = _ui->tabWidget->currentIndex();
	(*_tab)[static_cast<glsl::Shader::Type>(index)].saveAs();
}
void MainWindow::doCompile() {
	_ui->teOutput->clear();

	try {
		_ui->trAttribute->clear();
		_ui->trUnifom->clear();

		QString vs_str(_ui->teVS->toPlainText());
		QOpenGLShader vs_sh(QOpenGLShader::Vertex, this);
		if(!vs_sh.compileSourceCode(vs_str))
			throw std::runtime_error(vs_sh.log().toStdString());

		QString fs_str(_ui->teFS->toPlainText());
		QOpenGLShader fs_sh(QOpenGLShader::Fragment, this);
		if(!fs_sh.compileSourceCode(fs_str))
			throw std::runtime_error(fs_sh.log().toStdString());

		QOpenGLShaderProgram prog(this);
		prog.addShader(&vs_sh);
		prog.addShader(&fs_sh);
		if(!prog.link()) {
			throw std::runtime_error(prog.log().toStdString());
		}

		QStringList sl;
		GLuint id = prog.programId();
		GLint n;
		GLsizei len;
		GLint size;
		GLenum type;
		GLchar buff[256];
		glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &n);
		for(int i=0 ; i<n ; i++) {
			glGetActiveAttrib(id, i, sizeof(buff), &len, &size, &type, buff);

			sl.clear();
			sl << QString("%1").arg(glGetAttribLocation(id, buff))
				<< buff
				<< glsl::GetValueTypeStr(type)
				<< QString("%1").arg(size);
			_ui->trAttribute->addTopLevelItem(new QTreeWidgetItem(sl));
		}

		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &n);
		for(int i=0 ; i<n ; i++) {
			glGetActiveUniform(id, i, sizeof(buff), &len, &size, &type, buff);
			sl.clear();
			sl << QString("%1").arg(glGetUniformLocation(id, buff))
				<< buff
				<< glsl::GetValueTypeStr(type)
				<< QString("%1").arg(size);
			_ui->trUnifom->addTopLevelItem(new QTreeWidgetItem(sl));
		}
	} catch(const std::exception& e) {
		_ui->teOutput->append("compile error:");
		_ui->teOutput->append(e.what());
	}
}
void MainWindow::quit() {
	qApp->quit();
}
