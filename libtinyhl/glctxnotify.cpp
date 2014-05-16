#include "glctxnotify.h"
#include <QOpenGLContext>

namespace glsl {
	void GLCtxNotify::initializeGL() {
		auto* ctx = QOpenGLContext::currentContext();
		emit onContextInitialized(ctx);
	}
}
