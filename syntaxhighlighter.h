#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <unordered_map>

namespace glsl {
	//! GLSLの各キーワードをハイライトする
	class SyntaxHighlighter : public QSyntaxHighlighter {
		Q_OBJECT
		//! テキストをハイライトする時の色やフォント
		class TextFormat : public QTextCharFormat {
			public:
				using QTextCharFormat::QTextCharFormat;
				TextFormat(const QJsonObject& o);
				void loadFromJson(const QJsonObject& o);
		};
		//! キーワード定義 (string or regex)
		/*! JSONフォーマット:
			"type" : "string" or "regex"
			"words" : [
				"keyword0",
				"keyword1", ...
			]
		*/
		class Keywords {
			using StrV = std::vector<QString>;
			using RegV = std::vector<QRegExp>;
			// StrVかRegVのどちらか片方が使用される
			StrV	_strV;
			RegV	_regV;
			public:
				Keywords(const QJsonObject& o);
				//! JSONで記述されたキーワード定義を読み込む
				void loadFromJson(const QJsonObject& o);
				//! キーワードが文字列中に存在するかチェック
				/*! \param[in] text	チェックする文字列
					\param[in] offset チェック開始するオフセット
					\return <int: キーワードのオフセット(負数は無効), int: キーワード長> */
				std::pair<int,int>	match(const QString& text, int offset) const;
		};

		//! テキストハイライト定義が存在しない場合のデフォルト値
		QTextCharFormat	_formatDefault;
		//! ハイライト定義名に対応したフォーマットを取得
		/*! 定義名が見つからなければデフォルト値を返す */
		const QTextCharFormat& _getFormat(const std::string& name) const;

		using FormatMap = std::unordered_map<std::string, TextFormat>;
		using KeywordMap = std::unordered_map<std::string, Keywords>;
		struct Pair {
			std::string				keywordName;
			const QTextCharFormat*	format;
			const Keywords*			keyword;
		};
		using PairV = std::vector<Pair>;

		//! テキストハイライト定義マップ (定義名とその値)
		FormatMap		_formatMap;
		//! キーワード定義マップ (定義名とその値)
		KeywordMap		_keywordMap;
		//! (Format, Keyword)に対し、同じ定義名のエントリをvectorで纏めた物
		PairV			_pairV;

		//! JSONデータをファイルから読み込み、QJsonDocumentにして返す
		static QJsonDocument _LoadJson(const QString& path);
		//! PairVを作りなおす
		void _refreshPairV();

		protected:
			void highlightBlock(const QString& text) override;
		public:
			using QSyntaxHighlighter::QSyntaxHighlighter;
			//! テキスト装飾定義を読み込む
			void loadUserFormat(const QString& path);
			//! キーワード定義を指定パス以下全て読み込む
			void loadDefine(const QString& path);
	};
}
