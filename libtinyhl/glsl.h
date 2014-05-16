#pragma once
#include <QOpenGLFunctions>

namespace glsl {
	//! シェーダー種別
	struct Shader {
		enum Type {
			Vertex,
			Fragment,
			_Num
		};
	};
	class MissingEntry : public std::runtime_error {
		public:
			MissingEntry(const QString& entName);
	};
	class InvalidFormat : public std::runtime_error {
		public:
			InvalidFormat(const QString& entName, const QString& type);
	};
	//! GLSL値フォーマットを示すEnum値の文字列表現を取得
	const char* GetValueTypeStr(GLenum type);
}
