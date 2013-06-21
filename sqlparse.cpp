#include "sqlparse.h"
#include "sqlscanner.h"

enum IndexParseState{
    IPPrepair,
    IPCreate,
    IPUnique,
    IPIndex,
    IPIf,
    IPIfNotExists,
    IPDBName,
    IPDNDot,
    IPName,
    IPOn,
    IPTableName,
    IPCollist,
    IPColName,
    IPColCollate,
    IPCollateName,
    IPColSort,
    IPEnd
};

enum TableDefState
{
    TbDefCreate,
    TbDefTemp,
    TbDefTable,
    TbDefIf,
    TbDefIfNotExists,
    TbDefName
};

enum ColumnDefState
{
    ColDefName,
    ColDefType,
    ColDefLength,
    ColDefLenFirst,
    ColDefLenComma,
    ColDefLenSecond,
    ColDefTypeEnd,
    ColDefNotNull,
    ColDefPKey,
    ColDefUnique,
    ColDefCheck
};

SqliteColumn::SqliteColumn()
{
    auto_inc = false;
    check=QString();
    conlate=QString();
    defaultValue=QString();
    isPK=false;
    isUnique=false;
    length=0;
    length2=0;
    name=QString();
    notnull=false;
    notnullconflict=QString();
    pkconflict=QString();
    pksort_order=QString();
    type=QString();
    unique_conflict = QString();
    state = 0;
}

SqliteColumn& SqliteColumn::operator =(const SqliteColumn &other)
{
    auto_inc = other.auto_inc;
    check = other.check;
    conlate = other.conlate;
    defaultValue = other.defaultValue;
    isPK = other.isPK;
    isUnique = other.isUnique;
    length = other.length;
    length2 = other.length2;
    name = other.name;
    notnull = other.notnull;
    notnullconflict = other.notnullconflict;
    pkconflict = other.pkconflict;
    pksort_order = other.pksort_order;
    type = other.type;
    unique_conflict = other.unique_conflict;
    state = other.state;

    return *this;
}

//Q_DECLARE_METATYPE(SqliteIndex);

void SqlParse::reset()
{
    state = Prepair;
    //重置table里的内容
    QVector<SqliteColumn*>::iterator iter = table.column_list.begin();
    for(;iter!=table.column_list.end();++iter)
    {
        delete (*iter);
    }
    table.column_list.clear();
}

bool SqlParse::parse(const QString &sql)
{
    SqlScanner scanner;
    if(!scanner.scan(sql))
        return false;

    reset();

    SqliteToken *token=0;
    SqliteToken *next=0;
    SqliteColumn *column =0;
    bool error = false;

    while(true)
    {
        token = scanner.next();

        if(state == Prepair){
            if(token->id == TokenCreate){
                parse_table(&scanner);
            }else{
                //error
                break;
            }
        }
        else if(state == ParseColumnList){
            if(token->id == TokenID){
                column = new SqliteColumn();
                table.column_list.append(column);
                column->name = token->value;
                parse_column(&scanner,column);
            }
            else if (token->id == TokenPrimary)
            {
                next = scanner.next();
                if(next->id == TokenLeft){
                    state = ParsePKey;
                }else{
                    //错误的PKey约束格式
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenUnique)
            {
                next = scanner.next();
                if(next->id == TokenLeft){
                    state = ParseUnique;
                }else{
                    //错误的Unique约束格式
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenCheck)
            {
                next = scanner.next();
                if(next->id == TokenLeft){
                    state = ParseCheck;
                }else{
                    //错误的Check约束格式
                    error = true;
                    break;
                }
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(state == ParsePKey){
            if(token->id == TokenID){
                //SqliteColumn *col = findBy(token->value);
                //if(!col)
                //    col->isPK = true;
                table.pkey.column_list.push_back(token->value);
                state = ParsePKeyCol;
            }else{
                error = true;
                break;
                //PKey格式错误
            }
        }
        else if(state == ParsePKeyCol){
            if(token->id == TokenComma){
                state = ParsePKey;
            }
            else if(token->id == TokenRight){
                state = ParsePKeyColEnd;
            }else{
                error = true;
                break;
                //错误的PKey格式
            }
        }
        else if(state == ParsePKeyColEnd){
            if(token->id == TokenOn){
                if(!parse_conflict(&scanner,table.pkey.conflict))
                {
                    break;
                }
            }else if(token->id == TokenComma){
                state = ParseColumnList;
            }else if(token->id == TokenRight){
                state = ParseEnd;
            }else{
                error = true;
                break;
                //错误格式
            }
        }
        else if(state == ParseUnique){
            if(token->id == TokenID){
                //SqliteColumn *col = findBy(token->value);
                //if(!col)
                //    col->isPK = true;
                table.uq.column_list.push_back(token->value);
                state = ParseUniqueCol;
            }else{
                error = true;
                break;
                //PKey格式错误
            }
        }
        else if(state == ParseUniqueCol){
            if(token->id == TokenComma){
                state = ParseUnique;
            }
            else if(token->id == TokenRight){
                state = ParseUniqueColEnd;
            }else{
                error = true;
                break;
                //错误的Unique格式
            }
        }
        else if(state == ParseUniqueColEnd){
            if(token->id == TokenOn){
                if(!parse_conflict(&scanner,table.uq.conflict))
                {
                    break;
                }
            }else if(token->id == TokenComma){
                state = ParseColumnList;
            }else if(token->id == TokenRight){
                state = ParseEnd;
            }else{
                error = true;
                break;
                //错误格式
            }
        }
        else if(state == ParseCheck){
            if(token->id == TokenRight){
                state = ParseCheckEnd;
            }else{
                table.check_expr.push_back(token->value);
            }

        }
        else if(state == ParseCheckEnd){
            if(token->id == TokenComma){
                state = ParseColumnList;
            }else if(token->id == TokenRight){
                state = ParseEnd;
            }else{
                error = true;
                break;
            }
        }
        else if(state == ParseEnd)
        {
            break;
        }
    }
    return !error;
}

bool SqlParse::parse_table(SqlScanner *scanner)
{
    TableDefState def_state = TbDefCreate;

    bool error = false;
    SqliteToken *token = 0;

    while(true){
        token = scanner->next();
        if(token==0)break;

        if(def_state == TbDefCreate)
        {
            if(token->id == TokenTemp)
            {
                def_state = TbDefTemp;
                continue;
            }
            else if(token->id == TokenTable)
            {
                def_state = TbDefTable;
                continue;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(def_state == TbDefTemp)
        {
            if(token->id == TokenTable){
                def_state = TbDefTable;
                continue;
            }
            else{
                error = true;
                break;
            }

        }
        else if(def_state == TbDefTable)
        {
            if(token->id == TokenIf){
                def_state = TbDefIf;
                continue;
            }
            else if(token->id == TokenID){
                def_state = TbDefName;
                table.name = token->value;
                continue;
            }
        }
        else if(def_state == TbDefIf)
        {
            SqliteToken *next = scanner->next();

            if(token->id == TokenNot && next->id == TokenExists)
            {
                def_state = TbDefIfNotExists;
            }
            else {
                error = true;
                break;
            }
        }
        else if(def_state == TbDefIfNotExists)
        {
            if(token->id == TokenID)
            {
                def_state = TbDefName;
                table.name = token->value;
            }
        }
        else if(def_state == TbDefName)
        {
            if(token->id == TokenLeft)
            {
                state = ParseColumnList;
                break;
            }
            else
            {
                error = true;
                break;
            }
        }
    }
    return error;
}

bool SqlParse::parse_column(SqlScanner *scanner,SqliteColumn *col)
{
    ColumnDefState def_state = ColDefName;

    SqliteToken *token = scanner->next();
    SqliteToken *next = 0;
    QList<SqliteToken*> exp_stack;
    bool error = false;

    while(true)
    {
        if(token==0)break;

        if(def_state == ColDefName)
        {
            if(token->id == TokenChar
               ||token->id == TokenVarchar
               || token->id == TokenText
               || token->id == TokenBlob
               || token->id == TokenInt
               || token->id == TokenFloat
               || token->id == TokenDecimal
               || token->id == TokenDateTime)
            {
                def_state = ColDefType;
                col->type = token->value;
            }
            else if(token->id == TokenID){
                def_state = ColDefType;
                col->type = token->value;
            }
            else if(token->id == TokenComma)
            {
                state = ParseColumnList;
                //当前列定义结束
                break;
            }
            else if(token->id == TokenRight){
                state = ParseEnd;
                break;
            }
            else if(token->id == TokenNot)
            {
                //not null 约束定义
            }
        }
        else if(def_state == ColDefType)
        {
            if(token->id == TokenLeft)
            {
                def_state = ColDefLength;
            }
            else if (token->id == TokenComma)
            {
                state = ParseColumnList;
                //当前列定义结束
                break;
            }else if(token->id == TokenRight){
                state = ParseEnd;
                break;
            }else if(token->id == TokenNot)
            {
                next = scanner->next();
                if(next->id==TokenNull){
                    col->notnull = true;
                    def_state = ColDefNotNull;
                }
                else
                {
                    //错误的not null列约束定义
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenPrimary)
            {
                next = scanner->next();
                if(next->id == TokenKey){
                    def_state = ColDefPKey;
                    col->isPK = true;
                }else{
                    //错误的主键声明
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenUnique)
            {
                col->isUnique = true;
                def_state = ColDefUnique;
            }
            else if(token->id == TokenCheck)
            {
                next = scanner->next();
                if(next->id == TokenLeft){
                    def_state = ColDefCheck;
                }
                else
                {
                    //未定义的Check约束
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenDefault)
            {
                col->defaultValue = scanner->next()->value;
            }
            else if(token->id == TokenCollate)
            {
                col->conlate = scanner->next()->value;
            }
            else{
                error = true;
                break;
            }
        }
        else if(def_state == ColDefLength)
        {
            if(token->id == TokenIntConst){
                col->length = token->value.toInt();
                def_state = ColDefLenFirst;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(def_state == ColDefLenFirst)
        {
            if(token->id == TokenComma)
            {
                def_state = ColDefLenComma;
            }
            else if(token->id == TokenRight)
            {
                def_state = ColDefTypeEnd;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(def_state == ColDefLenComma)
        {
            if(token->id == TokenIntConst){
                col->length2 = token->value.toInt();
                def_state = ColDefLenSecond;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(def_state == ColDefLenSecond)
        {
            if(token->id == TokenRight){
                def_state = ColDefTypeEnd;
            }else{
                error = true;
                break;
            }
        }
        //TYPEEND:
        else if(def_state == ColDefTypeEnd)
        {
            if(token->id == TokenComma)
            {
                state = ParseColumnList;
                //当前列定义结束
                break;
            }
            else if(token->id == TokenRight){
                state = ParseEnd;
                break;
            }
            else if(token->id == TokenNot)
            {
                next = scanner->next();
                if(next->id==TokenNull){
                    col->notnull = true;
                    def_state = ColDefNotNull;
                }
                else
                {
                    //错误的not null列约束定义
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenPrimary)
            {
                next = scanner->next();
                if(next->id == TokenKey){
                    def_state = ColDefPKey;
                    col->isPK = true;
                }else{
                    //错误的主键声明
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenUnique)
            {
                col->isUnique = true;
                def_state = ColDefUnique;
            }
            else if(token->id == TokenCheck)
            {
                next = scanner->next();
                if(next->id == TokenLeft){
                    def_state = ColDefCheck;
                    exp_stack.clear();
                }
                else
                {
                    //未定义的Check约束
                    error = true;
                    break;
                }
            }
            else if(token->id == TokenDefault)
            {
                col->defaultValue = scanner->next()->value;
            }
            else if(token->id == TokenCollate)
            {
                col->conlate = scanner->next()->value;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(def_state == ColDefNotNull)
        {
            if(token->id == TokenOn)
            {                
                if(!parse_conflict(scanner,col->notnullconflict)){
                    break;
                }
            }
            else
            {
                def_state = ColDefTypeEnd;
                continue;
                //goto TYPEEND;
            }
        }
        else if(def_state == ColDefPKey)
        {
            if(token->id == TokenOn)
            {
                if(!parse_conflict(scanner,col->pkconflict)){
                    break;
                }
            }
            else if(token->id == TokenAutoIncrement){
                col->auto_inc = true;
            }
            else {
                def_state = ColDefTypeEnd;
                continue;
                //goto TYPEEND;
            }
        }
        else if(def_state == ColDefUnique)
        {
            if(token->id == TokenOn)
            {
                if(!parse_conflict(scanner,col->unique_conflict)){
                    break;
                }
            }
            else
            {
                def_state = ColDefTypeEnd;
                continue;
                //goto TYPEEND;
            }
        }
        else if(def_state == ColDefCheck)
        {
            if(token->id == TokenLeft){
                col->check.push_back(token->value);
                exp_stack.push_back(token);
            }
            else if(token->id==TokenRight){
                if(exp_stack.isEmpty()){
                    def_state = ColDefTypeEnd;
                }else{
                    exp_stack.pop_back();
                    col->check.push_back(token->value);
                }
            }
            else
            {
                col->check.append(token->value);
            }

        }
        token = scanner->next();
    }
    return error;
}

bool SqlParse::parse_conflict(SqlScanner *scanner,QString &conflict)
{
    SqliteToken *next = scanner->next();
    if(next->id == TokenConflict){
        conflict = scanner->next()->value;
        return true;
    }
    else{
        //错误的conflict声明
        return false;
    }    
}

bool SqlParse::parse_index(const QString &src,
                           const QString &dbname,
                           SqliteIndex &index)
{
    if(src.isEmpty())
        return false;
    SqlScanner scanner;
    if(!scanner.scan(src))
        return false;

    index.name.clear();
    index.unique = false;
    index.collist.clear();

    IndexParseState ip_state = IPPrepair;
    SqliteIndexColumn *ic=0;

    bool error = false;
    SqliteToken *token = scanner.next();

    while(true){

        if(token==0)break;

        if(ip_state == IPPrepair)
        {
            if(token->id == TokenCreate)
            {
                ip_state = IPCreate;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(ip_state == IPCreate)
        {
            if(token->id ==TokenUnique){
                ip_state = IPUnique;
                index.unique = true;
            }
            else if(token->id == TokenIndex){
                ip_state = IPIndex;
                index.unique = false;
            }else{
                error = true;
                break;
            }

        }
        else if(ip_state == IPUnique){
            if(token->id == TokenIndex){
                ip_state = IPIndex;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPIndex)
        {
            if(token->id == TokenIf){
                ip_state = IPIf;
            }
            else if(token->id == TokenID){
                if(token->value==dbname){
                    ip_state = IPDBName;

                }else{
                    ip_state = IPName;
                    index.name = token->value;
                }
            }else
            {
                error = true;
                break;
            }
        }
        else if(ip_state == IPIf)
        {
            SqliteToken *next = scanner.next();

            if(token->id == TokenNot && next->id == TokenExists)
            {
                ip_state = IPIfNotExists;
            }
            else {
                error = true;
                break;
            }
        }
        else if(ip_state == IPDBName)
        {
            SqliteToken *next = scanner.next();
            if(token->id == TokenMember && next->id == TokenID){
                ip_state = IPName;
                index.name = next->value;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPIfNotExists)
        {
            if(token->id == TokenID)
            {
                if(token->value == dbname){
                    ip_state = IPDBName;
                }else{
                    ip_state = IPName;
                    index.name = token->value;
                }
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPName)
        {
            if(token->id == TokenOn)
            {
                ip_state = IPOn;
            }
            else
            {
                error = true;
                break;
            }
        }
        else if(ip_state == IPOn){
            if(token->id == TokenID){
                index.tbname = token->value;
                ip_state = IPTableName;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPTableName){
            if(token->id == TokenLeft){
                ip_state = IPCollist;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPCollist){
            if(token->id == TokenID){
                ip_state = IPColName;
                ic = new SqliteIndexColumn;
                ic->name = token->value;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPColName){
            if(token->id == TokenCollate){
                ip_state = IPColCollate;
            }
            else if(token->id == TokenAsc || token->id == TokenDesc){
                ip_state = IPColSort;
                ic->sort = token->value;
            }
            else if(token->id == TokenComma){
                ip_state = IPCollist;
                index.collist.push_back(ic);
            }
            else if(token->id == TokenRight){
                ip_state = IPEnd;
                index.collist.push_back(ic);
                break;
            }
            else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPColCollate){
            if(token->id == TokenID){
                ip_state = IPCollateName;
                ic->collate = token->value;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPCollateName){
            if(token->id == TokenAsc || token->id == TokenDesc){
                ip_state = IPColSort;
                ic->sort = token->value;
            }
            else if(token->id == TokenComma){
                ip_state = IPCollist;
                index.collist.push_back(ic);
            }
            else if(token->id == TokenRight){
                ip_state = IPEnd;
                index.collist.push_back(ic);
                break;
            }else{
                error = true;
                break;
            }
        }
        else if(ip_state == IPColSort){
            if(token->id == TokenComma)
            {
                ip_state = IPCollist;
                index.collist.push_back(ic);
            }
            else if(token->id == TokenRight){
                ip_state = IPEnd;
                index.collist.push_back(ic);
                break;
            }else{
                error = true;
                break;
            }
        }

        token = scanner.next();
    }
    return !error;
}

//SqliteColumn* SqlParse::findBy(const QString &col_name)
//{
//    foreach(SqliteColumn *col,table.column_list){
//        if(col->name==col_name)
//            return col;
//    }
//    return 0;
//}
