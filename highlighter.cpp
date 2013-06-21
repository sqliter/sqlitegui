
#include <QtGui>

 #include "highlighter.h"

 Highlighter::Highlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     HighlightingRule rule;

     keywordFormat.setForeground(Qt::darkBlue);
     keywordFormat.setFontWeight(QFont::Bold);
     QStringList keywordPatterns;
     QFile file(QString(":/keyword.txt"));
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         qDebug()<<"Cannot read file keyword.txt.";
         return ;
     }
     QTextStream in(&file);
     QString keywords = in.readAll();
     QStringList keylist=keywords.split(",");

     foreach(QString key,keylist)
     {
         key = key.trimmed();
         if(!key.isEmpty())
         {
             key=QString("\\b(%1)\\b").arg(key);
             keywordPatterns.append(key);
         }
     }

     foreach (const QString &pattern, keywordPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }

     singleLineCommentFormat.setForeground(Qt::darkGreen);
     //rule.pattern = QRegExp(QString("--[^\n]*"));
     rule.pattern = QRegExp(QString("--.*"));
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     multiLineCommentFormat.setForeground(Qt::darkGreen);

     quotationFormat.setForeground(Qt::red);
     //rule.pattern = QRegExp(QString("\'.*\'"));

     //rule.format = quotationFormat;
     //highlightingRules.append(rule);

     commentStartExpression = QRegExp("/\\*");
     commentEndExpression = QRegExp("\\*/");

     quotationExp = QRegExp("\'");

 }

 void Highlighter::highlightBlock(const QString &text)
 {
     //qDebug()<<text;
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }

     setCurrentBlockState(0);

     int qe = 0;
     if(previousBlockState()!=1){
         qe = quotationExp.indexIn(text);
     }

     QChar c;
     int qs = 0;
     bool begin = true;
     int len = text.length();
     while(qe<len){
         c = text.at(qe);
         if(c == QChar('\'')){
             if(begin){
                 qs = qe;
                 begin = false;
             }
             else{
                 setFormat(qs,qe-qs+1,quotationFormat);
                 begin = true;
             }
         }
         qe ++;
     }

     setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
      startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
      int endIndex = commentEndExpression.indexIn(text, startIndex);
      int commentLength;
      if (endIndex == -1) {
          setCurrentBlockState(1);
          commentLength = text.length() - startIndex;
      } else {
          commentLength = endIndex - startIndex
                          + commentEndExpression.matchedLength();
      }
      setFormat(startIndex, commentLength, multiLineCommentFormat);
      startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }


 }
