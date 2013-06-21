#include "sqlscanner.h"
#include <QtDebug>

SqlScanner::SqlScanner()
{
    init_keywords();
}

void SqlScanner::reset()
{
    state = ScanPrepair;

    //清空输出单词表
    QVector<SqliteToken*>::iterator iter = token_table.begin();
    for(;iter!=token_table.end();++iter)
    {
        delete (*iter);
    }
    token_table.clear();
}

void SqlScanner::init_keywords()
{
    keywords.insert(QString("char"),TokenChar);
    keywords.insert(QString("varchar"),TokenVarchar);
    keywords.insert(QString("text"),TokenText);
    keywords.insert(QString("blob"),TokenBlob);
    keywords.insert(QString("int"),TokenInt);
    keywords.insert(QString("float"),TokenFloat);
    keywords.insert(QString("decimal"),TokenDecimal);
    keywords.insert(QString("datetime"),TokenDateTime);
    keywords.insert(QString("create"),TokenCreate);
    keywords.insert(QString("temp"),TokenTemp);
    keywords.insert(QString("temporary"),TokenTemp);
    keywords.insert(QString("table"),TokenTable);
    keywords.insert(QString("if"),TokenIf);
    keywords.insert(QString("not"),TokenNot);
    keywords.insert(QString("exists"),TokenExists);
    keywords.insert(QString("null"),TokenNull);
    keywords.insert(QString("primary"),TokenPrimary);
    keywords.insert(QString("key"),TokenKey);
    keywords.insert(QString("autoincrement"),TokenAutoIncrement);
    keywords.insert(QString("unique"),TokenUnique);
    keywords.insert(QString("check"),TokenCheck);
    keywords.insert(QString("default"),TokenDefault);
    keywords.insert(QString("collate"),TokenCollate);
    keywords.insert(QString("on"),TokenOn);
    keywords.insert(QString("conflict"),TokenConflict);
    keywords.insert(QString("escapte"),TokenEscape);
    keywords.insert(QString("isnull"),TokenIsNull);
    keywords.insert(QString("notnull"),TokenNotNull);
    keywords.insert(QString("between"),TokenBetween);
    keywords.insert(QString("case"),TokenCase);
    keywords.insert(QString("when"),TokenWhen);
    keywords.insert(QString("then"),TokenThen);
    keywords.insert(QString("else"),TokenElse);
    keywords.insert(QString("end"),TokenEnd);
    keywords.insert(QString("cast"),TokenCast);
    keywords.insert(QString("as"),TokenAs);
    keywords.insert(QString("in"),TokenIn);
    keywords.insert(QString("and"),TokenAnd);
    keywords.insert(QString("or"),TokenOr);
    keywords.insert(QString("index"),TokenIndex);
    keywords.insert(QString("asc"),TokenAsc);
    keywords.insert(QString("desc"),TokenDesc);
    keywords.insert(QString("trigger"),TokenTrigger);
    keywords.insert(QString("before"),TokenBefore);
    keywords.insert(QString("after"),TokenAfter);
    keywords.insert(QString("instead"),TokenInstead);
    keywords.insert(QString("of"),TokenOf);
    keywords.insert(QString("for"),TokenFor);
    keywords.insert(QString("each"),TokenEach);
    keywords.insert(QString("row"),TokenRow);
    keywords.insert(QString("statement"),TokenStatement);
    keywords.insert(QString("begin"),TokenBegin);
    keywords.insert(QString("end"),TokenEnd);
}

SqliteTokenEnum SqlScanner::getIDToken(const QString &value)
{
    QString key(value);

    key = key.trimmed().toLower();

    if(keywords.contains(key)){
        return keywords[key];
    }else{
        return TokenID;
    }
}

bool SqlScanner::isAccept(QChar ch)
{
    //用户通过键盘输入的特殊但却是不可接受的字符。这个很难列全，因为用一些特殊的输入工具
    //可以输入大量的特殊符号
    if(ch=='@'||ch=='#'||ch=='$'
       ||ch=='^'||ch=='{'||ch=='}'
       ||ch=='\\'||ch==':'||ch==';'
       ||ch=='~'||ch=='_')
        return true;
    else
        return false;
}

bool SqlScanner::scan(const QString &src)
{
    sourceSql = src;
    qDebug()<<src;

    //复位扫描器:
    reset();

    int index=0;
    int count = sourceSql.count();

    QChar temp = sourceSql.at(index);
    QChar prev;

    SqliteToken *token=0;
    bool error_flag = false;
    bool eof = false;

    //不做错误处理，遇到错误就直接退出
    while(true){
        eof = (index>=count);
        if(eof)break;
        prev = sourceSql.at(index);
        index ++;
        temp = prev;

        //qDebug("%c %d",temp.unicode());
        //qDebug()<<temp<<"-"<<temp.unicode();

        PREPAIR:
        if(state == ScanPrepair)
        {
            if(eof){
                break;
            }            
            else if(temp == ' ')
            {
                continue;
            }
            else if(temp == '0')
            {
                state = ScanZero;
                token = new SqliteToken();
                token->id = TokenIntConst;
                token->value.push_back(temp);
            }            
            else if(temp >'0' && temp <='9')
            {
                state = ScanInt;
                token = new SqliteToken();
                token->id = TokenIntConst;
                token->value.push_back(temp);
            }
            else if(temp.isLetter())
            {
                state = ScanID;
                token = new SqliteToken();
                token->id = TokenID;
                token->value.push_back(temp);
            }
            else if(temp == '\"')
            {
                state = ScanString;
                token = new SqliteToken();
                token->id = TokenID;
                //token->value.push_back(temp);
            }
            else if(temp == '\'')
            {
                state = ScanChar;
                token = new SqliteToken();
                token->value.push_back(temp);
                token->id = TokenChar;
            }
            else if(temp == '(' )
            {
                token = new SqliteToken();
                token->id = TokenLeft;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == ')' )
            {
                token = new SqliteToken();
                token->id = TokenRight;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '[' )
            {
                state = ScanKeyID;
                token = new SqliteToken();
                token->id = TokenID;
            }
            else if(temp == '+')
            {
                token = new SqliteToken();
                token->id = TokenAdd;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '-')
            {
                token = new SqliteToken();
                token->id = TokenSub;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '*')
            {
                token = new SqliteToken();
                token->id = TokenMul;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '/')
            {
                token = new SqliteToken();
                token->id = TokenDiv;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '%')
            {
                token = new SqliteToken();
                token->id = TokenMod;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '&')
            {
                token = new SqliteToken();
                token->id = TokenBitAnd;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else if(temp == '|')
            {
                state = ScanConnect;
                token = new SqliteToken();
                token->id = TokenBitOr;
                token->value.push_back(temp);
            }
            else if(temp == '<')
            {
                state = ScanLess;
                token = new SqliteToken();
                token->id = TokenLess;
                token->value.push_back(temp);
            }
            else if(temp == '>')
            {
                state = ScanMore;
                token = new SqliteToken();
                token->id = TokenMore;
                token->value.push_back(temp);
            }
            else if(temp == '!')
            {
                state = ScanNot;
                token = new SqliteToken();
                token->id = TokenLogicNot;
                token->value.push_back(temp);
            }
            else if(temp == '~')
            {
                token = new SqliteToken();
                token->id = TokenBitNot;
                token->value.push_back(temp);;
            }
            else if(temp == '=')
            {
                state = ScanEqual;
                token = new SqliteToken();
                token->id = TokenEqual;
                token->value.push_back(temp);
            }
            else if(temp == ',')
            {
                token = new SqliteToken();
                token->id = TokenComma;
                token->value.push_back(temp);
                token_table.push_back(token);
            }
            else{
                error_flag = true;
                break;
            }
        }
        else if(state == ScanID)
        {
            if(eof)
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp == ' ')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp >='0' && temp <='9')
            {
                token->value.push_back(temp);
            }
            else if(temp.isLetter())
            {
                token->value.push_back(temp);
            }
            else if(temp == '_'){
                token->value.push_back(temp);
            }
            else if(temp == '\"' )
            {
                token_table.push_back(token);
                token->id = getIDToken(token->value);

                state = ScanString;
                token = new SqliteToken();
                token->id = TokenID;
            }
            else if(temp == '\'')
            {
                token_table.push_back(token);
                token->id = getIDToken(token->value);

                state = ScanChar;
                token = new SqliteToken();
                token->value.push_back(temp);
                token->id = TokenChar;
            }
            else if(temp == '.')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMember;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '(' )
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLeft;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == ')' )
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenRight;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '[' )
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();;
                state = ScanKeyID;
                token->id = TokenID;
            }
            else if(temp == '+')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenAdd;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '-')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenSub;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '*')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMul;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '/')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenDiv;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '%')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMod;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '&')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitAnd;
                token->value.push_back(temp);
                state = ScanPrepair;
            }
            else if(temp == '|')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitOr;
                token->value.push_back(temp);
                state = ScanConnect;
            }
            else if(temp == '<')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLess;
                token->value.push_back(temp);
                state = ScanLess;
            }
            else if(temp == '>')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMore;
                token->value.push_back(temp);
                state = ScanMore;
            }
            else if(temp == '=')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenEqual;
                token->value.push_back(temp);
                state = ScanEqual;
            }
            else if(temp == '!')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLogicNot;
                token->value.push_back(temp);
                state = ScanNot;
            }
            else if(temp == '~')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitNot;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == ',')
            {
                token->id = getIDToken(token->value);
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenComma;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token->value.push_back(temp);
                //error_flag = true;
                //break;
            }
        }
        else if(state == ScanZero)
        {
            if(eof)
            {
                token_table.push_back(token);
                state = ScanEOF;
                break;
            }
            else if(temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '(')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLeft;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == ')')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenRight;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == ',')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenComma;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '+')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenAdd;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '-')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenSub;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '*')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMul;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '/')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenDiv;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '%')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMod;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '>')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMore;
                token->value.push_back(temp);

                state = ScanMore;
            }
            else if(temp == '<')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLess;
                token->value.push_back(temp);

                state = ScanLess;
            }
            else if(temp == '=')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenEqual;
                token->value.push_back(temp);

                state = ScanEqual;
            }
            else if(temp == '!')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLogicNot;
                token->value.push_back(temp);

                state = ScanNot;
            }
            else if(temp == '~')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitNot;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '&')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitAnd;
                token->value.push_back(temp);

                state = ScanPrepair;
            }
            else if(temp == '|')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitOr;
                token->value.push_back(temp);

                state = ScanConnect;
            }
            else if(temp == '.')
            {
                state = ScanIntDot;
                token->id = TokenFloatConst;
                token->value.push_back(temp);
            }
            else
            {                
                error_flag = true;
                break;
            }
        }
        else if(state == ScanInt)
        {
            if(eof)
            {
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp == ' ')
            {
                state = ScanPrepair;
                token_table.push_back(token);
            }
            else if(temp >='0' && temp <='9')
            {
                token->value.push_back(temp);
            }
            else if(temp == '(')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLeft;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == ')')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenRight;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '+')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenAdd;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '-')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenSub;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '*')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMul;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '/')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenDiv;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '%')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMod;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '>')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMore;
                token->value.push_back(temp);

                state = ScanMore;
            }
            else if(temp == '<')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLess;
                token->value.push_back(temp);

                state = ScanLess;
            }
            else if(temp == '=')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenEqual;
                token->value.push_back(temp);

                state = ScanEqual;
            }
            else if(temp == '!')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLogicNot;
                token->value.push_back(temp);

                state = ScanNot;
            }
            else if(temp == '~')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitNot;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '&')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitAnd;
                token->value.push_back(temp);

                state = ScanPrepair;
            }
            else if(temp == '|')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitOr;
                token->value.push_back(temp);

                state = ScanConnect;
            }
            else if(temp == '.')
            {
                state = ScanIntDot;
                token->id = TokenFloatConst;
                token->value.push_back(temp);
            }
            else
            {
                error_flag = true;
                break;
            }
        }
        else if(state == ScanIntDot)
        {
            if(temp >= '0' && temp <='9')
            {
                state = ScanReal;
                token->id = TokenFloat;
                token->value.push_back(temp);
            }
            else
            {
                error_flag = true;
                break;
            }
        }
        else if (state == ScanReal)
        {
            if(eof)
            {
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp == '(')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLeft;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == ')')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenRight;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '+')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenAdd;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '-')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenSub;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '*')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMul;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '/')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenDiv;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '%')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMod;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '>')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenMore;
                token->value.push_back(temp);

                state = ScanMore;
            }
            else if(temp == '<')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLess;
                token->value.push_back(temp);

                state = ScanLess;
            }
            else if(temp == '=')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenEqual;
                token->value.push_back(temp);

                state = ScanEqual;
            }
            else if(temp == '!')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenLogicNot;
                token->value.push_back(temp);

                state = ScanNot;
            }
            else if(temp == '~')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitNot;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '&')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitAnd;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else if(temp == '|')
            {
                token_table.push_back(token);

                token = new SqliteToken();
                token->id = TokenBitOr;
                token->value.push_back(temp);

                state = ScanConnect;
            }
            else if(temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;

            }
            else if(temp >='0' && temp <='9')
            {
                token->value.push_back(temp);
            }
            else
            {
                error_flag = true;
                break;
            }
        }
        else if(state == ScanChar)
        {
            if(eof)
            {
                token->id = TokenStringConst;
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp=='\'')
            {
                token->id = TokenStringConst;
                token->value.push_back(temp);
                token_table.push_back(token);

                state = ScanPrepair;
            }
            else
            {
                token->value.push_back(temp);
            }
        }
        else if(state == ScanString)
        {
            if(eof)
            {
                token->id = TokenID;
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp == '\"')
            {
                token->id = TokenID;
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token->value.push_back(temp);
            }
        }
        else if(state == ScanKeyID)
        {
            if(eof)
            {
                token->id = TokenID;
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp == ']')
            {
                token->id = TokenID;
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token->value.push_back(temp);
            }
        }
        else if(state == ScanConnect)
        {
            if(eof)
            {
                token_table.push_back(token);
                state = ScanEOF;
                break;
            }
            else if(temp == '|')
            {
                token->id = TokenConnect;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token_table.push_back(token);
                state = ScanPrepair;
                goto PREPAIR;
            }
        }
        else if(state == ScanMore)
        {
            if(eof)
            {
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp=='=')
            {
                token->id = TokenNotLess;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp =='>')
            {
                token->id = TokenBitRight;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if( temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token_table.push_back(token);
                state = ScanPrepair;
                goto PREPAIR;
            }
        }
        else if(state == ScanLess)
        {
            if(eof)
            {
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp=='=')
            {
                token->id = TokenNotMore;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '<')
            {
                token->id = TokenBitLeft;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if(temp == '>')
            {
                token->id = TokenNotEqual;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if( temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token_table.push_back(token);
                state = ScanPrepair;
                goto PREPAIR;
            }
        }
        else if(state == ScanEqual)
        {
            if(eof)
            {
                token->id = TokenEqual;
                token_table.push_back(token);

                state = ScanEOF;
                break;
            }
            else if(temp=='=')
            {
                token->id = TokenEqual;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if( temp == ' ')
            {
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else
            {
                token_table.push_back(token);
                state = ScanPrepair;
                goto PREPAIR;
            }
        }
        else if(state == ScanNot)
        {
            if(eof)
            {
                state = ScanEOF;
                break;
            }
            else if(temp=='=')
            {
                token->id = TokenNotEqual;
                token->value.push_back(temp);
                token_table.push_back(token);
                state = ScanPrepair;
            }
            else if( temp == ' ')
            {
                state = ScanPrepair;
            }
            else
            {
                state = ScanPrepair;
                goto PREPAIR;
            }
        }

    }

    token_iter = token_table.begin();
    return !error_flag;
}

SqliteToken* SqlScanner::next()
{
    if(token_iter!=token_table.end())
    {
        qDebug()<<(*token_iter)->value;
        return *token_iter++;
    }
    else
        return 0;
}

