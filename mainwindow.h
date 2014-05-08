#pragma once
#include <QMainWindow>
#include <QOpenGLFunctions>
#include <memory>

namespace Ui {
	class MainWindow;
}
class MainWindow : public QMainWindow, public QOpenGLFunctions {
	Q_OBJECT
	private:
		class TabEnt;
		using TabV = std::vector<TabEnt>;
		using SPTabV = std::shared_ptr<TabV>;
		SPTabV	_tab;

		std::shared_ptr<Ui::MainWindow>	_ui;
		QOpenGLContext*	_ctx;
	public:
		explicit MainWindow(QWidget* parent=nullptr);
		~MainWindow();
	public slots:
		void doCompile();
		//! ファイルダイアログを開き、シェーダーファイルをロード
		/*! 種別は拡張子で判断 */
		void loadShader();
		//! 現在アクティブなシェーダーを上書き保存
		/*! 新規作成のシェーダーならダイアログを開いて入力を求める */
		void saveCurrent();
		//! 全てのタブのシェーダーを保存
		void saveAll();
		//! ファイルから開いたシェーダーであっても常にダイアログで保存先を指定
		void saveAs();
		void onTabTitleChanged(int index, const QString& title);
		void quit();
};
