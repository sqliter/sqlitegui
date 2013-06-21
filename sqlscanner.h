#ifndef SQLSCANNER_H
#define SQLSCANNER_H
#include <QString>
#include <QMap>
#include <QVector>

enum SqliteTokenEnum{
    TokenChar,
    TokenVarchar,
    TokenText,
    TokenBlob,
    TokenInt,
    TokenFloat,
    TokenDecimal,
    TokenDateTime,
    TokenIntConst,
    TokenFloatConst,
    TokenStringConst,
    TokenDatetimeConst,
    TokenCreate,
    TokenTemp,
    TokenTable,
    TokenIf,
    TokenNot,
    TokenExists,
    TokenNull,
    TokenPrimary,
    TokenKey,
    TokenAutoIncrement,
    TokenUnique,
    TokenCheck,
    TokenDefault,
    TokenCollate,
    TokenOn,
    TokenConflict,
    TokenEscape,
    TokenIsNull,
    TokenNotNull,
    TokenBetween,
    TokenCase,
    TokenWhen,
    TokenThen,
    TokenElse,
    TokenEnd,
    TokenCast,
    TokenAs,
    TokenLeft,
    TokenRight,
    TokenMidLeft,
    TokenMidRight,
    TokenMember,
    TokenComma,
    TokenIn,
    TokenLike,
    TokenGlob,
    TokenRegexp,
    TokenConnect,
    TokenAdd,
    TokenSub,
    TokenMul,
    TokenDiv,
    TokenMod,
    TokenBitAnd,
    TokenBitOr,
    TokenBitNot,
    TokenBitLeft,
    TokenBitRight,
    TokenAnd,
    TokenOr,
    TokenLogicNot,
    TokenLess,
    TokenMore,
    TokenEqual,
    TokenNotEqual,
    TokenNotLess,
    TokenNotMore,
    TokenID,
    TokenIndex,
    TokenAsc,
    TokenDesc,
    TokenTrigger,
    TokenBefore,
    TokenAfter,
    TokenInstead,
    TokenOf,
    TokenFor,
    TokenEach,
    TokenRow,
    TokenStatement,
    TokenBegin,
    TokenError
};

struct SqliteToken
{
    SqliteTokenEnum id;
    QString value;
};

enum ScanState
{
    ScanPrepair,
    ScanZero,
    ScanInt,
    ScanIntDot,
    ScanReal,
    ScanChar,
    ScanString,
    ScanID,
    ScanKeyID,
    ScanConnect,
    ScanNot,
    ScanEqual,
    ScanLess,
    ScanMore,
    ScanEOF
};

class SqlScanner
{
public:
    SqlScanner();
    SqliteToken* next();
    bool scan(const QString &src);
private:
    void reset();
    void init_keywords();


    bool isAccept(QChar ch);
    SqliteTokenEnum getIDToken(const QString &id);

    QVector<SqliteToken*> token_table;
    QVector<SqliteToken*>::iterator token_iter;

    QString sourceSql;
    QMap<QString,SqliteTokenEnum> keywords;
    ScanState state;
};

#endif // SQLSCANNER_H
