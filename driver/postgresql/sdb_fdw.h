#ifndef SDB_FDW_H__
#define SDB_FDW_H__

#include "client.h"
#include "fmgr.h"
#include "catalog/pg_foreign_server.h"
#include "catalog/pg_foreign_table.h"
#include "nodes/execnodes.h"


/* Connection related options */
#define OPTION_NAME_ADDRESS          "address"
#define OPTION_NAME_SERVICE          "service"
#define OPTION_NAME_USER             "user"
#define OPTION_NAME_PASSWORD         "password"
#define OPTION_NAME_TOKEN            "token"
#define OPTION_NAME_CIPHER           "cipher"
#define OPTION_NAME_CIPHERFILE       "cipherfile"
#define OPTION_NAME_TRANSACTION      "transaction"
/******************************/

/* Table related options */
#define OPTION_NAME_COLLECTIONSPACE          "collectionspace"
#define OPTION_NAME_COLLECTION               "collection"
#define OPTION_NAME_USEDECIMAL               "decimal"
#define OPTION_NAME_PUSHDOWNSORT             "pushdownsort"
#define OPTION_NAME_PUSHDOWNLIMIT            "pushdownlimit"
#define OPTION_NAME_PREFEREDINSTANCE         "preferedinstance"
#define OPTION_NAME_PREFERREDINSTANCE        "preferredinstance"
#define OPTION_NAME_PREFEREDINSTANCE_MODE    "preferedinstancemode"
#define OPTION_NAME_PREFERREDINSTANCE_MODE   "preferredinstancemode"
#define OPTION_NAME_SESSION_TIMEOUT          "sessiontimeout"



#define DEFAULT_PREFEREDINSTANCE      ""
#define DEFAULT_PREFEREDINSTANCE_MODE ""
#define DEFAULT_SESSION_TIMEOUT       -1
/*************************/

#define DEFAULT_HOSTNAME            "localhost"
#define DEFAULT_SERVICENAME         "30010"
#define DEFAULT_USERNAME            ""
#define DEFAULT_PASSWORDNAME        ""

#define SDB_OPTION_ON               "on"
#define SDB_OPTION_OFF              "off"

#define INITIAL_ARRAY_CAPACITY         8
#define SDB_TUPLE_COST_MULTIPLIER      5

#define SDB_MAX_KEY_COLUMN_COUNT       (10)
#define SDB_MAX_KEY_COLUMN_LENGTH      (30)

#define SDB_MAX_INDEX_NUM              (20)

#define SDB_MAX_SERVICE_LENGTH         (256)

#define POSTGRES_TO_UNIX_EPOCH_DAYS (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE)
#define POSTGRES_TO_UNIX_EPOCH_USECS (POSTGRES_TO_UNIX_EPOCH_DAYS * USECS_PER_DAY)
struct SdbInputOption
{
   const CHAR *optionName ;
   Oid   optionContextID ;
} ;
typedef struct SdbInputOption SdbInputOption ;

static const SdbInputOption SdbInputOptionList[] =
{
   { OPTION_NAME_ADDRESS,                 ForeignServerRelationId },
   { OPTION_NAME_SERVICE,                 ForeignServerRelationId },
   { OPTION_NAME_USER,                    ForeignServerRelationId },
   { OPTION_NAME_PASSWORD,                ForeignServerRelationId },
   { OPTION_NAME_TOKEN,                   ForeignServerRelationId },
   { OPTION_NAME_CIPHER,                  ForeignServerRelationId },
   { OPTION_NAME_CIPHERFILE,              ForeignServerRelationId },
   { OPTION_NAME_PREFEREDINSTANCE,        ForeignServerRelationId },
   { OPTION_NAME_PREFERREDINSTANCE,       ForeignServerRelationId },
   { OPTION_NAME_PREFEREDINSTANCE_MODE,   ForeignServerRelationId },
   { OPTION_NAME_PREFERREDINSTANCE_MODE,  ForeignServerRelationId },
   { OPTION_NAME_SESSION_TIMEOUT,         ForeignServerRelationId },
   { OPTION_NAME_TRANSACTION,             ForeignServerRelationId },

   { OPTION_NAME_COLLECTIONSPACE,         ForeignTableRelationId },
   { OPTION_NAME_COLLECTION,              ForeignTableRelationId },
   { OPTION_NAME_USEDECIMAL,              ForeignTableRelationId },
   { OPTION_NAME_PUSHDOWNSORT,            ForeignTableRelationId },
   { OPTION_NAME_PUSHDOWNLIMIT,           ForeignTableRelationId }
} ;

struct SdbInputOptions
{
   CHAR  *serviceList[ INITIAL_ARRAY_CAPACITY ] ;
   INT32 serviceNum ;
   CHAR  *user ;
   CHAR  *password ;
   CHAR  *token ;
   INT32 isUseCipher ;                          /* use cipherfile */
   CHAR  *cipherfile ;
   CHAR  *collectionspace ;
   CHAR  *collection ;
   CHAR  *preference_instance ;
   CHAR  *preference_instance_mode ;
   INT32 sessionTimeout ;
   CHAR  *transaction ;
   INT32 isUseDecimal ;                         /* use decimal in sdb */
   INT32 isPushDownSort ;                       /* push down sort or not */
   INT32 isPushDownLimit ;                      /* push down limit or not */
} ;
typedef struct SdbInputOptions SdbInputOptions ;

/* SdbColumnMapping is for mapping between sdbbson's field name and PG's column
 * name
 */
struct SdbColumnMapping
{
   CHAR columnName [ NAMEDATALEN ] ;
   UINT32 columnIndex ;
   Oid columnTypeId ;
   INT32 columnTypeMod ;
   Oid columnArrayTypeId ;
} ;
typedef struct SdbColumnMapping SdbColumnMapping ;

enum SDB_PLAN_TYPE
{
   SDB_PLAN_SCAN = 0,
   SDB_PLAN_INSERT,
   SDB_PLAN_UPDATE,
   SDB_PLAN_DELETE,

   SDB_PLAN_UNKNOWN = 100
} ;
typedef enum SDB_PLAN_TYPE SDB_PLAN_TYPE ;

typedef struct PgColumnDesc_s
{
   char *pgname; /* PostgreSQL column name */
   int pgattnum; /* PostgreSQL attribute number */
   int attnum_in_target;
   Oid pgtype;   /* PostgreSQL data type */
   int pgtypmod; /* PostgreSQL type modifier */

   bool isDropped;
   bool isInReturningList;
}PgColumnsDesc;

typedef struct SdbExprTreeState_s
{
   Index foreign_table_index;   /* range table index of foreign table */
   Oid foreign_table_id;
   INT32 total_unsupport_count;
   INT32 and_unsupport_count ;  /* AND's child node, just impact NOT node*/
   INT32 or_unsupport_count ;   /* OR/NOT 's child node, impact OR/NOT node*/

   INT32 is_use_decimal ;       /* use decimal in sdb */

   List *range_table ;          /* List of RangeTblEntry */
}SdbExprTreeState;

typedef struct PgTableDesc_s
{
   char *name;    /* table name */
   int ncols;     /* number of columns */
   PgColumnsDesc *cols;
}PgTableDesc;

/* SdbExecState represents the runtime state for sdb cursor
 */
struct SdbExecState
{
   SDB_PLAN_TYPE planType ;
   struct HTAB *columnMappingHash ; /* map sdbbson fields to columns */
   sdbCursorHandle hCursor ;
   sdbConnectionHandle hConnection ;
   sdbCollectionHandle hCollection ;
   sdbbson queryDocument ; /* query request */
   sdbbson sortDocument ; /* sort request */
   INT64 offset ;
   INT64 limit ;
   int isPushDownSort ;
   int isPushDownLimit ;

   /* sdb server options */
   char *sdbServerList[ INITIAL_ARRAY_CAPACITY ];
   int sdbServerNum;
   char *usr;
   char *passwd;
   char *token;
   int isUseCipher;
   char *cipherfile;
   char *preferenceInstance;
   char *preferenceInstanceMode;
   int sessionTimeout;
   char *transaction;

   char *sdbcs;
   char *sdbcl;

   int isUseDecimal;                          /* use decimal in sdb */

   Oid tableID ;
   Index relid ;

   /* pg table column's description */
   PgTableDesc *pgTableDesc;

   /* foreign table's rowcount */
   SINT64 row_count;

   /* the bson record's index in SdbRecordCache */
   UINT64 bson_record_index ;
   /* the SdbExecState's key address */
   UINT64 keyAddress ;

   /* save the key column name to identify the specify column(0 for _id, others for the sharding key) */
   int key_num;
   char key_name[SDB_MAX_KEY_COLUMN_COUNT][SDB_MAX_KEY_COLUMN_LENGTH];
} ;
typedef struct SdbExecState SdbExecState ;


/* Sdb represents connection handle for each host:service */
struct SdbConnection
{
   /* transaction name include address:service */
   CHAR *connName ;
   sdbConnectionHandle hConnection ;
   /* by default 0 means there's no transaction started on this connection */
   INT32 isTransactionOn ;
   INT32 transLevel ;
} ;
typedef struct SdbConnection SdbConnection ;

/* SdbTransList represents list of active transactions */
struct SdbConnectionPool
{
   INT32 numConnections ;
   INT32 poolSize ;
   SdbConnection *connList ;
} ;
typedef struct SdbConnectionPool  SdbConnectionPool ;

typedef struct
{
   int keyNum ;
   char indexKey[SDB_MAX_KEY_COLUMN_COUNT][SDB_MAX_KEY_COLUMN_LENGTH];
} sdbIndexInfo;

struct SdbCLStatistics
{
   Oid tableID ;
   char shardingKeys[SDB_MAX_KEY_COLUMN_COUNT][SDB_MAX_KEY_COLUMN_LENGTH] ;
   int keyNum ;
   SINT64 recordCount ;

   int indexNum;
   sdbIndexInfo indexInfo[SDB_MAX_INDEX_NUM + 1];

   /// clHandle should not exist here actually.
   sdbCollectionHandle clHandle ;
} ;
typedef struct SdbCLStatistics SdbCLStatistics ;

struct SdbStatisticsCache
{
   struct HTAB *ht ;
} ;
typedef struct SdbStatisticsCache SdbStatisticsCache ;

INT32 sdbRecurExprTree( Node *node, SdbExprTreeState *expr_state,
                        sdbbson *condition, ExprContext *exprContext ) ;

int sdbSetBsonValue( sdbbson *bsonObj, const char *name, Datum valueDatum,
                     Oid columnType, INT32 columnTypeMod, INT32 isUseDecimal ) ;

List *serializeSdbExecState( SdbExecState *fdwState ) ;

SdbStatisticsCache *SdbGetStatisticsCache();
SdbCLStatistics * SdbGetCLStatFromCache( Oid foreignTableId ) ;



extern Datum sdb_fdw_handler(PG_FUNCTION_ARGS);
extern Datum sdb_fdw_validator(PG_FUNCTION_ARGS);
#endif

