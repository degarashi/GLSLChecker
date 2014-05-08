#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include "syntaxhighlighter.h"

namespace glsl {
	// ------------------ SyntaxHighlighter ------------------
	SyntaxHighlighter::TextFormat::TextFormat(const QJsonObject& o) {
		loadFromJson(o);
	}
	void SyntaxHighlighter::TextFormat::loadFromJson(const QJsonObject& o) {
		auto itr = o.find("italic");
		bool b = false;
		if(itr != o.end())
			b = itr.value().toBool(false);
		setFontItalic(b);

		int w = QFont::Normal;
		itr = o.find("bold");
		if(itr != o.end())
			w = itr.value().toBool(false) ? QFont::Bold : QFont::Normal;
		setFontWeight(w);

		b = false;
		itr = o.find("underline");
		if(itr != o.end())
			b = itr.value().toBool(false);
		setFontUnderline(b);

		QColor col(128,128,128);
		itr = o.find("color");
		if(itr != o.end()) {
			QJsonArray ar = itr.value().toArray();
			if(ar.size() == 3)
				col.setRgb(ar[0].toInt(), ar[1].toInt(), ar[2].toInt());
		}
		setForeground(col);
	}
	SyntaxHighlighter::Keywords::Keywords(const QJsonObject& o) {
		loadFromJson(o);
	}
	void SyntaxHighlighter::Keywords::loadFromJson(const QJsonObject& o) {
		_strV.clear();
		_regV.clear();
		QString strType = o.value("type").toString("string");
		bool bIsRegex = strType == "regex";
		QJsonArray ar = o.value("words").toArray();
		for(const auto& w : ar) {
			QString word = w.toString();
			if(!word.isEmpty()) {
				if(bIsRegex) {
					// 正規表現によるキーワード指定
					_regV.emplace_back(word);
				} else {
					// 文字列によるキーワード指定
					_strV.emplace_back(word);
				}
			}
		}
	}

	void SyntaxHighlighter::highlightBlock(const QString& text) {
		enum class TokenType {
			Keyword,
			CommentStart,
			CommentLine,
			_Num
		};
		struct Token {
			int			offset;
			TokenType	type;
			int			length;
		};

		enum class SyntaxState {
			Normal,
			InCommentBlock
		};
		auto& fmt_comment = _getFormat("comment");
		int length = text.length();
		QString startMark("/*"),
				endMark("*/"),
				lineMark("//");
		QRegExp keywordRE("[#a-zA-Z0-9_]+");
		SyntaxState state = (previousBlockState() == 1) ?
								SyntaxState::InCommentBlock :
								SyntaxState::Normal;
		int cursor = 0;
		setCurrentBlockState(0);
		for(;;) {
			switch(state) {
				case SyntaxState::Normal: {
					Token tokens[static_cast<int>(TokenType::_Num)] = {
						{text.indexOf(keywordRE, cursor), TokenType::Keyword, keywordRE.matchedLength()},
						{text.indexOf(startMark, cursor), TokenType::CommentStart, startMark.length()},
						{text.indexOf(lineMark, cursor), TokenType::CommentLine, lineMark.length()}
					};
					for(auto& t : tokens) {
						if(t.offset < 0)
							t.offset = std::numeric_limits<int>::max();
					}
					std::sort(tokens, tokens+sizeof(tokens)/sizeof(tokens[0]), [](const Token& t0, const Token& t1){
						return t0.offset < t1.offset;
					});
					auto& cur_token = tokens[0];
					// 何も見つからなければ終了
					if(cur_token.offset == std::numeric_limits<int>::max())
						return;

					switch(cur_token.type) {
						case TokenType::Keyword: {
							bool bFound = false;
							// どのキーワードに該当するか探索
							std::pair<int,int> kwd_result{std::numeric_limits<int>::max(), -1};
							QTextCharFormat fmt;
							for(auto& p : _pairV) {
								auto res = p.keyword->match(text, cur_token.offset);
								if(res.second > 0) {
									if(res.first < kwd_result.first) {
										bFound = true;
										kwd_result = res;
										fmt = *p.format;
									}
								}
							}
							if(bFound) {
								// キーワードに色付け
								setFormat(kwd_result.first, kwd_result.second, fmt);
							}
						} break;
						case TokenType::CommentStart:
							setFormat(cur_token.offset, cur_token.length, fmt_comment);
							state = SyntaxState::InCommentBlock;
							break;
						case TokenType::CommentLine:
							// 行最後までコメントアウト
							setFormat(cur_token.offset, length-cur_token.offset, fmt_comment);
							return;
						default:
							break;
					}
					cursor = cur_token.offset + cur_token.length;
				} break;

				case SyntaxState::InCommentBlock: {
					// CommentEndを探す
					int idx = text.indexOf(endMark, cursor);
					if(idx >= 0) {
						// endMarkまでコメントアウト
						setFormat(cursor, idx+endMark.length() - cursor, fmt_comment);
						cursor = idx+endMark.length();
						state = SyntaxState::Normal;
					} else {
						// 次の行へコメントが続いている
						setFormat(cursor, length-cursor, fmt_comment);
						setCurrentBlockState(1);
						return;
					}
				} break;
			}
		}
	}
	namespace {
		struct KeywordMatch {
			static int Length(const QRegExp& r) { return r.matchedLength(); }
			static int Length(const QString& s) { return s.length(); }
			static QString String(const QString& s) { return s; }
			static QString String(const QRegExp& r) { return r.pattern(); }

			const QString&	_text;
			int				_baseOffset,
							_offset,
							_length;
			KeywordMatch(const QString& text, int ofs):
				_text(text), _baseOffset(ofs), _offset(text.length()), _length(-1)
			{}

			template <class T>
			void proc(const T& t) {
				int idx = _text.indexOf(const_cast<T&>(t), _baseOffset);
				if(idx >= 0) {
					int len = Length(t);
					if((idx + len >= _text.length() ||
						!_text.at(idx+len).isLetterOrNumber()) &&
						(idx == 0 || !_text.at(idx-1).isLetterOrNumber()))
					{
						if(_offset > idx) {
							_offset = idx;
							_length = len;
						} else if(_offset == idx) {
							if(_length < len)
								_length = len;
						}
					}
				}
			}
		};
	}
	std::pair<int,int> SyntaxHighlighter::Keywords::match(const QString& text, int offset) const {
		KeywordMatch m(text, offset);
		for(auto& k : _strV)
			m.proc(k);
		for(auto& r : _regV)
			m.proc(r);
		return std::make_pair(m._offset, m._length);
	}
	void SyntaxHighlighter::_refreshPairV() {
		_pairV.clear();
		for(auto& k : _keywordMap) {
			Pair p;
			p.keywordName = k.first;
			QString str = QString::fromStdString(k.first);
			p.format = &_getFormat(k.first);
			p.keyword = &k.second;
			_pairV.push_back(std::move(p));
		}
	}
	const QTextCharFormat& SyntaxHighlighter::_getFormat(const std::string& name) const {
		auto itr = _formatMap.find(name);
		if(itr != _formatMap.end())
			return itr->second;
		return _formatDefault;
	}
	QJsonDocument SyntaxHighlighter::_LoadJson(const QString& path) {
		QFile file;
		file.setFileName(path);
		if(!file.open(QFile::ReadOnly))
			throw std::runtime_error("can't read file");

		QJsonParseError err;
		QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
		if(err.error != QJsonParseError::NoError)
			throw std::runtime_error(err.errorString().toStdString());
		return std::move(doc);
	}
	void SyntaxHighlighter::loadUserFormat(const QString& path) {
		QJsonDocument doc = _LoadJson(path);
		_formatMap.clear();
		QJsonObject root = doc.object();
		auto itr = root.find("highlights");
		if(itr != root.end()) {
			QJsonObject ent = itr.value().toObject();
			for(auto itr2 = ent.begin() ; itr2 != ent.end() ; itr2++)
				_formatMap.emplace(itr2.key().toStdString(), TextFormat(itr2.value().toObject()));
		}
		_refreshPairV();
	}
	void SyntaxHighlighter::loadDefine(const QString& path) {
		QDir dir(path);
		QStringList filter;
		filter << "*.json";
		QStringList files = dir.entryList(filter);
		for(auto& f : files) {
			// ファイル名から拡張子を除く
			QRegExp re(R"(([\w\d_]+)\.json)");
			f.indexOf(re);
			QString fileName = re.capturedTexts()[1];

			_keywordMap.emplace(fileName.toStdString(), Keywords(_LoadJson(path + '/' + f).object()));
		}
		_refreshPairV();
	}
}
