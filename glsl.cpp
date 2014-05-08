#include "glsl.h"

namespace glsl {
	namespace {
		std::pair<GLenum, const char*> c_valueType[] = {
			{GL_FLOAT, "GL_FLOAT"},
			{GL_FLOAT_VEC2, "GL_FLOAT_VEC2"},
			{GL_FLOAT_VEC3, "GL_FLOAT_VEC3"},
			{GL_FLOAT_VEC4, "GL_FLOAT_VEC4"},
			{GL_FLOAT_MAT2, "GL_FLOAT_MAT2"},
			{GL_FLOAT_MAT3, "GL_FLOAT_MAT3"},
			{GL_FLOAT_MAT4, "GL_FLOAT_MAT4"},
			{GL_FLOAT_MAT2x3, "GL_FLOAT_MAT2x3"},
			{GL_FLOAT_MAT2x4, "GL_FLOAT_MAT2x4"},
			{GL_FLOAT_MAT3x2, "GL_FLOAT_MAT3x2"},
			{GL_FLOAT_MAT3x4, "GL_FLOAT_MAT3x4"},
			{GL_FLOAT_MAT4x2, "GL_FLOAT_MAT4x2"},
			{GL_FLOAT_MAT4x3, "GL_FLOAT_MAT4x3"},
			{GL_INT, "GL_INT"},
			{GL_INT_VEC2, "GL_INT_VEC2"},
			{GL_INT_VEC3, "GL_INT_VEC3"},
			{GL_INT_VEC4, "GL_INT_VEC4"},
			{GL_UNSIGNED_INT, "GL_UNSIGNED_INT"},
			{GL_UNSIGNED_INT_VEC2, "GL_UNSIGNED_INT_VEC2"},
			{GL_UNSIGNED_INT_VEC3, "GL_UNSIGNED_INT_VEC3"},
			{GL_UNSIGNED_INT_VEC4, "GL_UNSIGNED_INT_VEC4"},
			{GL_DOUBLE, "GL_DOUBLE"},
			{GL_DOUBLE_VEC2, "GL_DOUBLE_VEC2"},
			{GL_DOUBLE_VEC3, "GL_DOUBLE_VEC3"},
			{GL_DOUBLE_VEC4, "GL_DOUBLE_VEC4"},
			{GL_DOUBLE_MAT2, "GL_DOUBLE_MAT2"},
			{GL_DOUBLE_MAT3, "GL_DOUBLE_MAT3"},
			{GL_DOUBLE_MAT4, "GL_DOUBLE_MAT4"},
			{GL_DOUBLE_MAT2x3, "GL_DOUBLE_MAT2x3"},
			{GL_DOUBLE_MAT2x4, "GL_DOUBLE_MAT2x4"},
			{GL_DOUBLE_MAT3x2, "GL_DOUBLE_MAT3x2"},
			{GL_DOUBLE_MAT3x4, "GL_DOUBLE_MAT3x4"},
			{GL_DOUBLE_MAT4x2, "GL_DOUBLE_MAT4x2"},
			{GL_DOUBLE_MAT4x3, "GL_DOUBLE_MAT4x3"}
		};
	}
	const char* GetValueTypeStr(GLenum type) {
		for(auto& p : c_valueType) {
			if(p.first == type)
				return p.second;
		}
		return nullptr;
	}
	// ------------------ MissingEntry ------------------
	MissingEntry::MissingEntry(const QString& entName):
		std::runtime_error(QString("missing entry (%1)").arg(entName).toStdString())
	{}
	// ------------------ InvalidFormat ------------------
	InvalidFormat::InvalidFormat(const QString& entName, const QString& type):
		std::runtime_error(QString("invalid entry value (%1 as %2)").arg(entName).arg(type).toStdString())
	{}
}
