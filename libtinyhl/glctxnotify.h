#pragma once
#include <QGLWidget>

class QOpenGLContext;
namespace glsl {
	//! OpenGLコンテキストの初期化と、それを外部に通知する
	/*! QGLWidget以外でOpenGL-APIを呼んで色々処理したい時に使用 */
	class GLCtxNotify : public QGLWidget {
		Q_OBJECT
		protected:
			void initializeGL() override;
		public:
			using QGLWidget::QGLWidget;
		signals:
			void onContextInitialized(QOpenGLContext* ctx);
	};
}
