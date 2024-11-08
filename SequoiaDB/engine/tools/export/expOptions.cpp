/*******************************************************************************

   Copyright (C) 2023-present SequoiaDB Ltd.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Affero General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Source File Name = expOptions.cpp

   Dependencies: N/A

   Restrictions: N/A

   Change Activity:
   defect Date        Who          Description
   ====== =========== ============ =============================================
          28/07/2016  Lin Yuebang  Initial Draft

   Last Changed =

*******************************************************************************/
#include "ossIO.hpp"
#include "expOptions.hpp"
#include "expUtil.hpp"
#include "utilParam.hpp"
#include "ossUtil.h"
#include "pd.hpp"
#include "utilPasswdTool.hpp"
#include <iostream>

namespace exprt
{
   using namespace engine ;

   #ifndef W_OK
   #define W_OK         2
   #endif

   // general
   #define OPTION_HELP              "help"
   #define OPTION_VERSION           "version"
   #define OPTION_HOSTNAME          "hostname"
   #define OPTION_SVCNAME           "svcname"
   #define OPTION_HOSTS             "hosts"
   #define OPTION_USER              "user"
   #define OPTION_PASSWORD          "password"
   #define OPTION_DELRECORD         "delrecord"
   #define OPTION_FILELIMIT         "filelimit"
   #define OPTION_FIELDS            "fields"
   #define OPTION_TYPE              "type"
   #define OPTION_WITHID            "withid"
   #define OPTION_ERRORSTOP         "errorstop"
   #define OPTION_SSL               "ssl"
   #define OPTION_FLOATFMT          "floatfmt"
   #define OPTION_REPLACE           "replace"
   #define OPTION_CIPHERFILE        "cipherfile"
   #define OPTION_CIPHER            "cipher"
   #define OPTION_TOKEN             "token"

   // single collection
   #define OPTION_COLLECTSPACE      "csname"
   #define OPTION_COLLECTION        "clname"
   #define OPTION_SELECT            "select"
   #define OPTION_FILTER            "filter"
   #define OPTION_SORT              "sort"
   #define OPTION_FILENAME          "file"
   #define OPTION_SKIP              "skip"
   #define OPTION_LIMIT             "limit"

   // multi collection
   #define OPTION_CSCL              "cscl"
   #define OPTION_EXCLUDECSCL       "excludecscl"
   #define OPTION_DIRNAME           "dir"

   //json
   #define OPTION_STRICT            "strict"

   // csv
   #define OPTION_DELCHAR           "delchar"
   #define OPTION_DELFIELD          "delfield"
   #define OPTION_INCLUDE           "included"
   #define OPTION_INCLUDEBINARY     "includebinary"
   #define OPTION_INCLUDEREGEX      "includeregex"
   #define OPTION_FORCE             "force"
   #define OPTION_KICKNULL          "kicknull"
   #define OPTION_CHECKDELIMETER    "checkdelimeter"

   // conf
   #define OPTION_CONF              "conf"
   #define OPTION_GENCONF           "genconf"
   #define OPTION_GENFIELDS         "genfields"


   // general
   #define EXPLAIN_HELP             "help information"
   #define EXPLAIN_VERSION          "version"
   #define EXPLAIN_HOSTNAME         "host name, default: localhost"
   #define EXPLAIN_SVCNAME          "service name, default: 11810"
   #define EXPLAIN_HOSTS            "host addresses(hostname:svcname), separated by ',', such as 'localhost:11810,localhost:11910', default: 'localhost:11810'"
   #define EXPLAIN_USER             "username"
   #define EXPLAIN_PASSWORD         "password"
   #define EXPLAIN_CIPHER           "input password using a cipher file"
   #define EXPLAIN_TOKEN            "password encryption token"
   #define EXPLAIN_CIPHERFILE       "cipher file location, default ~/sequoiadb/passwd"
   #define EXPLAIN_FILELIMIT        "the limit of max size for one file, in K/k/M/m/G/g, default: 16G"
   #define EXPLAIN_DELRECORD        "record delimiter, default: '\\n' "
   #define EXPLAIN_TYPE             "type of file to output, default: csv (json,csv)"
   #define EXPLAIN_WITHID           "keep the fields with '_id' when force to export or generate the fields"
   #define EXPLAIN_ERRORSTOP        "whether stop by hitting error, default false"
   #define EXPLAIN_SSL              "use SSL connection (arg: [true|false], e.g. --ssl true)"
   #define EXPLAIN_FIELDS           "field names for each collections, " \
                                    "format as <field>[,<field>,...] for single collection, " \
                                    "or format as <csName>.<clName>:<field>[,<field>,...] for each collection " \
                                    "when specify multi collections"
   #define EXPLAIN_FLOATFMT         "float format, default: '%.16g', input 'db2' is '%+.14E', " \
                                    "format %[+][.precision](f|e|E|g|G) ( float only )"
   #define EXPLAIN_REPLACE          "whether to overwrite the output file"

   //json
   #define EXPLAIN_STRICT           "strict export of data types, default: false"

   // csv
   #define EXPLAIN_DELCHAR          "string delimiter, default: '\"'"
   #define EXPLAIN_DELFIELD         "field delimiter, default: ',' "
   #define EXPLAIN_INCLUDE          "whether to include field names as csv head-line, default: true "
   #define EXPLAIN_INCLUDEBINARY    "whether to output a compelete binary, default: false"
   #define EXPLAIN_INCLUDEREGEX     "whether to output a compelete regex, default: false"
   #define EXPLAIN_FORCE            "for csv, force to export collections without field-names being specified, " \
                                    "the fields(except '_id') of first record in collection will be taken by default"
   #define EXPLAIN_KICKNULL         "whether kick null value, default: false"
   #define EXPLAIN_CHECKDEL         "whether check delimeter strictly, default: true"

   // single collection
   #define EXPLAIN_COLLECTSPACE     "collection space name"
   #define EXPLAIN_COLLECTION       "collection name"
   #define EXPLAIN_SELECT           "the select rule(e.g. --select '{ age:\"\", address:{$trim:1} }')"
   #define EXPLAIN_FILTER           "the matching rule(e.g. --filter '{ age: 18 }')"
   #define EXPLAIN_SORT             "the ordered rule(e.g. --sort '{ name: 1 }')"
   #define EXPLAIN_FILENAME         "output file name for one collection"
   #define EXPLAIN_SKIP             "set the number of skip records, default: 0"
   #define EXPLAIN_LIMIT            "set the number of return records, default: -1 ( all return )"

   // multi collection
   #define EXPLAIN_CSCL             "collection full name or collection space name to export, seperated by ','"
   #define EXPLAIN_EXCLUDECSCL      "collection full name or collection space name to exclude, seperated by ','"
   #define EXPLAIN_DIRNAME          "output dir path for collections"

   // conf
   #define EXPLAIN_CONF             "the name of configure file"
   #define EXPLAIN_GENCONF          "the name of configure file to generate"
   #define EXPLAIN_GENFIELDS        "whether to generate option \"fields\" for each collection, default: true"

   #define DEFAULT_HOSTNAME         "localhost"
   #define DEFAULT_SVCNAME          "11810"
   #define DEFAULT_HOST             "localhost:11810"
   #define DEFAULT_DELCHAR_CHAR     "\""
   #define DEFAULT_DELFIELD_CHAR    ","
   #define DEFAULT_FILELIMIT        ( 16LL * 1024 * 1024 * 1024 ) // 16G

   #define FILELIMIT_MAX            ( 16LL * 1024 * 1024 * 1024 * 1024 ) // 16T

   #define _TYPE(T) po::value< T >()
   #define _IMPLICIT_TYPE(T,V) implicit_value<T>(V)

   vector<string> passwdVec ;

   #define EXP_GENERAL_OPTIONS \
      ( OPTION_HELP",h",               /* no arg */      EXPLAIN_HELP ) \
      ( OPTION_VERSION,                /* no arg */      EXPLAIN_VERSION ) \
      ( OPTION_HOSTNAME",s",           _TYPE(string),    EXPLAIN_HOSTNAME ) \
      ( OPTION_SVCNAME",p",            _TYPE(string),    EXPLAIN_SVCNAME ) \
      ( OPTION_HOSTS,                  _TYPE(string),    EXPLAIN_HOSTS ) \
      ( OPTION_USER",u",               _TYPE(string),    EXPLAIN_USER ) \
      ( OPTION_PASSWORD",w",           po::value< vector<string> >(&passwdVec)->multitoken()->zero_tokens(), EXPLAIN_PASSWORD ) \
      ( OPTION_CIPHER,                 _TYPE(bool),      EXPLAIN_CIPHER ) \
      ( OPTION_TOKEN,                  _TYPE(string),    EXPLAIN_TOKEN ) \
      ( OPTION_CIPHERFILE,             _TYPE(string),    EXPLAIN_CIPHERFILE ) \
      ( OPTION_DELRECORD",r",          _TYPE(string),    EXPLAIN_DELRECORD ) \
      ( OPTION_FILELIMIT,              _TYPE(string),    EXPLAIN_FILELIMIT ) \
      ( OPTION_TYPE,                   _TYPE(string),    EXPLAIN_TYPE ) \
      ( OPTION_WITHID,                 _TYPE(bool),      EXPLAIN_WITHID ) \
      ( OPTION_FIELDS,         _TYPE(vector<string>),    EXPLAIN_FIELDS ) \
      ( OPTION_SSL,                    _TYPE(bool),      EXPLAIN_SSL) \
      ( OPTION_FLOATFMT,               _TYPE(string),    EXPLAIN_FLOATFMT ) \
      ( OPTION_REPLACE,                /* no arg */      EXPLAIN_REPLACE )

   #define EXP_SINGLE_COLLECTION_OPTIONS \
      ( OPTION_COLLECTSPACE",c",       _TYPE(string),    EXPLAIN_COLLECTSPACE )\
      ( OPTION_COLLECTION",l",         _TYPE(string),    EXPLAIN_COLLECTION ) \
      ( OPTION_SELECT,                 _TYPE(string),    EXPLAIN_SELECT ) \
      ( OPTION_FILTER,                 _TYPE(string),    EXPLAIN_FILTER ) \
      ( OPTION_SORT,                   _TYPE(string),    EXPLAIN_SORT ) \
      ( OPTION_FILENAME,               _TYPE(string),    EXPLAIN_FILENAME ) \
      ( OPTION_SKIP,                   _TYPE(INT64),     EXPLAIN_SKIP ) \
      ( OPTION_LIMIT,                  _TYPE(INT64),     EXPLAIN_LIMIT )

   #define EXP_MULTI_COLLECTION_OPTIONS \
      ( OPTION_CSCL,                   _TYPE(string),    EXPLAIN_CSCL ) \
      ( OPTION_EXCLUDECSCL,            _TYPE(string),    EXPLAIN_EXCLUDECSCL ) \
      ( OPTION_DIRNAME,                _TYPE(string),    EXPLAIN_DIRNAME )

   #define EXP_JSON_OPTIONS \
      ( OPTION_STRICT,                 _TYPE(bool),      EXPLAIN_STRICT )

   #define EXP_CSV_OPTIONS \
      ( OPTION_DELCHAR",a",            _TYPE(string),    EXPLAIN_DELCHAR ) \
      ( OPTION_DELFIELD",e",           _TYPE(string),    EXPLAIN_DELFIELD ) \
      ( OPTION_INCLUDE,                _TYPE(bool),      EXPLAIN_INCLUDE ) \
      ( OPTION_INCLUDEBINARY,          _TYPE(bool),      EXPLAIN_INCLUDEBINARY)\
      ( OPTION_INCLUDEREGEX,           _TYPE(bool),      EXPLAIN_INCLUDEREGEX )\
      ( OPTION_FORCE,                  _TYPE(bool),      EXPLAIN_FORCE ) \
      ( OPTION_KICKNULL,               _TYPE(bool),      EXPLAIN_KICKNULL ) \
      ( OPTION_CHECKDELIMETER,         _TYPE(bool),      EXPLAIN_CHECKDEL )

   #define EXP_CONF_OPTIONS \
      ( OPTION_CONF,                   _TYPE(string),    EXPLAIN_CONF ) \
      ( OPTION_GENCONF,                _TYPE(string),    EXPLAIN_GENCONF ) \
      ( OPTION_GENFIELDS,              _TYPE(bool),      EXPLAIN_GENFIELDS )

   #define WRITE_STR_OPTION( buf, option, value, has ) \
      if ( has ) \
      { \
         buf += option ; \
         buf += " = " ; \
         buf += value ; \
         buf += OSS_NEWLINE ; \
      }
   #define WRITE_BOOL_OPTION( buf, option, value, has ) \
      if ( has ) \
      { \
         buf += option ; \
         buf += " = " ; \
         buf += ( value ? "true" : "false" ) ; \
         buf += OSS_NEWLINE ; \
      }

   inline void WRITE_INT32_OPTION( string &buf, const CHAR *pOption,
                                   INT32 value, BOOLEAN has )
   {
      if( has )
      {
         CHAR tmpBuff[32] = { 0 } ;
         buf += pOption ;
         buf += " = " ;
         ossSnprintf( tmpBuff, 32, "%d", value ) ;
         buf += tmpBuff ;
         buf += OSS_NEWLINE ;
      }
   }

   inline void WRITE_INT64_OPTION( string &buf, const CHAR *pOption,
                                   INT64 value, BOOLEAN has )
   {
      if( has )
      {
         CHAR tmpBuff[32] = { 0 } ;
         buf += pOption ;
         buf += " = " ;
         ossSnprintf( tmpBuff, 32, "%lld", value ) ;
         buf += tmpBuff ;
         buf += OSS_NEWLINE ;
      }
   }

   static const CHAR *formatNames[FORMAT_COUNT] = { "csv", "json" } ;
   INT32 formatOfName( const string & name, EXP_FILE_FORMAT &format )
   {
      INT32 rc = SDB_INVALIDARG ;
      for ( INT32 i = 0; i < FORMAT_COUNT; ++i )
      {
         if ( formatNames[i] == name )
         {
            format = (EXP_FILE_FORMAT)i ;
            rc = SDB_OK ;
            break ;
         }
      }
      return rc ;
   }

   static INT32 getFileLimit( const string &fileLimitStr, UINT64 &fileLimit )
   {
      INT32 rc = SDB_OK ;
      CHAR last = 0 ;
      UINT64 unit = 1 ;
      INT64 fileLimitLL = ossAtoll( fileLimitStr.c_str() ) ;
      if ( fileLimitLL <= 0 )
      {
         PD_LOG( PDERROR, "Invalid value for filelimit" ) ;
         goto error ;
      }

      fileLimit = fileLimitLL ;
      SDB_ASSERT( !fileLimitStr.empty(), "" ) ;
      last = fileLimitStr[ fileLimitStr.size() - 1 ] ;
      if      ( 'k' == last || 'K' == last ) { unit = 1024 ; }
      else if ( 'm' == last || 'M' == last ) { unit = 1024 * 1024 ; }
      else if ( 'g' == last || 'G' == last ) { unit = 1024 * 1024 * 1024 ; }
      else
      {
         PD_LOG( PDERROR, "Invalid value for filelimit" ) ;
         goto error ;
      }

      if ( FILELIMIT_MAX / unit < fileLimit )
      {
         PD_LOG( PDERROR, "Invalid value for filelimit" ) ;
         goto error ;
      }

      fileLimit *= unit ;

   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }

   expOptions::expOptions() : _cmdParsed     (FALSE),
                              _confParsed    (FALSE),
                              _hostName      (DEFAULT_HOSTNAME),
                              _svcName       (DEFAULT_SVCNAME),
                              _hostsString   (DEFAULT_HOST),
                              _delRecord     ("\n"),
                              _typeName      (formatNames[FORMAT_CSV]),
                              _type          (FORMAT_CSV),
                              _withId        (FALSE),
                              _errorStop     (FALSE),
                              _useSSL        (FALSE),
                              _fileLimit     (DEFAULT_FILELIMIT),
                              _skip          (0),
                              _limit         (-1),
                              _strict        (FALSE),
                              _delChar       (DEFAULT_DELCHAR_CHAR),
                              _delField      (DEFAULT_DELFIELD_CHAR),
                              _headLine      (TRUE),
                              _includeBinary (FALSE),
                              _includeRegex  (FALSE),
                              _force         (FALSE),
                              _kickNull      (FALSE),
                              _strictCheckDel(TRUE),
                              _genFields     (TRUE)
   {
   }

   expOptions::~expOptions()
   {
   }

   BOOLEAN expOptions::hasHelp() const
   {
      return _cmdHas(OPTION_HELP) ;
   }

   BOOLEAN expOptions::hasVersion() const
   {
      return _cmdHas(OPTION_VERSION) ;
   }

   BOOLEAN expOptions::hasConf() const
   {
      return _cmdHas(OPTION_CONF) ;
   }

   BOOLEAN expOptions::hasGenConf() const
   {
      return _cmdHas(OPTION_GENCONF) ;
   }

   void expOptions::printHelpInfo() const
   {
      po::options_description general("General Options") ;
      po::options_description sCL("Single-collection Options") ;
      po::options_description mCL("Multi-collection Options") ;
      po::options_description json("JSON Options") ;
      po::options_description csv("CSV Options") ;
      po::options_description conf("Configure-file Options") ;

      SDB_ASSERT( _cmdParsed, "Cannot be used before parsing cmd" ) ;

      general.add_options()EXP_GENERAL_OPTIONS ;
      sCL.add_options()EXP_SINGLE_COLLECTION_OPTIONS ;
      mCL.add_options()EXP_MULTI_COLLECTION_OPTIONS ;
      json.add_options()EXP_JSON_OPTIONS ;
      csv.add_options()EXP_CSV_OPTIONS ;
      conf.add_options()EXP_CONF_OPTIONS ;

      cout << general << endl ;
      cout << sCL << endl ;
      cout << mCL << endl ;
      cout << json << endl ;
      cout << csv << endl ;
      cout << conf << endl ;
   }

   INT32 expOptions::writeToConf( const expCLSet &clSet )
   {
      INT32 rc = SDB_OK ;
      string writeBuf ;

      {
         vector<Host>::const_iterator it ;

         _hostsString = "" ;

         for ( it = _hosts.begin(); it != _hosts.end(); ++it )
         {
            const Host& host = *it ;

            if( it != _hosts.begin() )
            {
               _hostsString += "," ;
            }

            _hostsString += host.hostname + ":" + host.svcname ;
         }
      }

      // general options
      WRITE_STR_OPTION( writeBuf, OPTION_HOSTS, _hostsString, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_USER, _user, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_TYPE, _typeName, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_FILELIMIT, _fileLimit, _has(OPTION_FILELIMIT));
      WRITE_BOOL_OPTION( writeBuf, OPTION_SSL, _useSSL, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_FLOATFMT, _floatFmt, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_REPLACE, "", _has( OPTION_REPLACE ) ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_WITHID, _withId, _has(OPTION_WITHID) ) ;

      // json options
      WRITE_BOOL_OPTION( writeBuf, OPTION_STRICT, _strict, _has(OPTION_STRICT) ) ;

      // csv options
      WRITE_STR_OPTION( writeBuf, OPTION_DELCHAR, _delChar, TRUE ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_DELFIELD, _delField, TRUE ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_INCLUDE, _headLine, TRUE ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_INCLUDEBINARY, _includeBinary, TRUE );
      WRITE_BOOL_OPTION( writeBuf, OPTION_INCLUDEREGEX, _includeRegex, TRUE ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_FORCE, _force, FALSE ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_KICKNULL, _kickNull, TRUE ) ;
      WRITE_BOOL_OPTION( writeBuf, OPTION_CHECKDELIMETER, _strictCheckDel, TRUE ) ;

      // single collection options
      WRITE_STR_OPTION( writeBuf, OPTION_COLLECTSPACE, _csName, _has(OPTION_COLLECTSPACE) ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_COLLECTION, _clName, _has(OPTION_COLLECTION) ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_SELECT, _select, _has(OPTION_SELECT) );
      WRITE_STR_OPTION( writeBuf, OPTION_FILTER, _filter, _has(OPTION_FILTER) );
      WRITE_STR_OPTION( writeBuf, OPTION_SORT,   _sort, _has(OPTION_SORT) ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_FILENAME, _file,_has(OPTION_FILENAME));
      WRITE_INT64_OPTION( writeBuf, OPTION_SKIP, _skip,_has(OPTION_SKIP));
      WRITE_INT64_OPTION( writeBuf, OPTION_LIMIT, _limit,_has(OPTION_LIMIT));

      // multi collection options
      WRITE_STR_OPTION( writeBuf, OPTION_CSCL, _cscl, _has(OPTION_CSCL) ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_EXCLUDECSCL, _excludeCscl, _has(OPTION_EXCLUDECSCL) ) ;
      WRITE_STR_OPTION( writeBuf, OPTION_DIRNAME, _dir,_has(OPTION_DIRNAME) ) ;
      // fields
      if ( _genFields )
      {
         for ( expCLSet::const_iterator i = clSet.begin(); clSet.end() != i;++i)
         {
            writeBuf += OPTION_FIELDS ;
            writeBuf += " = " ;
            writeBuf += i->csName ;
            writeBuf += "." ;
            writeBuf += i->clName ;
            writeBuf += EXPCL_FIELDS_SEP_CHAR ;
            writeBuf += i->fields ;
            writeBuf += OSS_NEWLINE ;
         }
      }

      // write conf
      rc = utilWriteConfigFile( _genConf.c_str(), writeBuf.c_str(), TRUE ) ;
      if ( SDB_FE == rc )
      {
         cerr << "File " << _genConf <<" already existed" << endl ;
         goto error ;
      }
      else if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to write conf file, rc = ", rc ) ;
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 expOptions::parseCmd( INT32 argc, CHAR* argv[] )
   {
      INT32 rc = SDB_OK ;

      SDB_ASSERT( !_cmdParsed, "Can't parse cmd again" ) ;

      _cmdDesc.add_options()
         EXP_GENERAL_OPTIONS
         EXP_SINGLE_COLLECTION_OPTIONS
         EXP_MULTI_COLLECTION_OPTIONS
         EXP_JSON_OPTIONS
         EXP_CSV_OPTIONS
         EXP_CONF_OPTIONS ;

      rc = utilReadCommandLine( argc, argv, _cmdDesc, _cmdVm, FALSE ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to read command line , rc = %d", rc ) ;
         goto error ;
      }
      _cmdParsed = TRUE ;

      if ( _cmdHas(OPTION_HELP) || _cmdHas(OPTION_VERSION) )
      {
         goto done ;
      }

      rc = _setConfOptions() ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to set conf options" ) ;
         goto error ;
      }

      if ( _cmdHas(OPTION_CONF) )
      {
         rc = _parseConf( _conf.c_str() ) ;
         if ( SDB_OK != rc )
         {
            PD_LOG( PDERROR, "Failed to parse conf file , rc = %d", rc ) ;
            goto error ;
         }
      }

      rc = _setOptions( argc ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Invalid options , rc = %d", rc ) ;
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }

   INT32 expOptions::_parseConf( const CHAR* fileName )
   {
      INT32 rc = SDB_OK ;

      SDB_ASSERT( !_confParsed, "Can't parse conf again" ) ;

      _confDesc.add_options()
         EXP_GENERAL_OPTIONS
         EXP_SINGLE_COLLECTION_OPTIONS
         EXP_MULTI_COLLECTION_OPTIONS
         EXP_JSON_OPTIONS
         EXP_CSV_OPTIONS
         EXP_CONF_OPTIONS ;

      rc = utilReadConfigureFile( fileName, _confDesc, _confVm ) ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to Read conf file , rc = %d", rc ) ;
         goto error;
      }
      _confParsed = TRUE ;

   done:
      return rc ;
   error:
      goto done ;
   }

   BOOLEAN expOptions::_cmdHas( const CHAR *option ) const
   {
      SDB_ASSERT( option, "" ) ;
      return _cmdParsed && _cmdVm.count(option) > 0 ;
   }

   BOOLEAN expOptions::_confHas( const CHAR *option ) const
   {
      SDB_ASSERT( option, "" ) ;
      return _confParsed && _confVm.count(option) > 0 ;
   }

   BOOLEAN expOptions::_has( const CHAR *option ) const
   {
      return _cmdHas(option) || _confHas(option) ;
   }

   template <typename T>
   T expOptions::_get( const CHAR *option ) const
   {
      SDB_ASSERT( _has(option), "Has the option" );
      if ( _cmdHas(option) )
         return _cmdVm[option].as<T>() ;
      else
         return _confVm[option].as<T>() ;
   }

   #define IS_HEX( c )  \
      ( ( (c) >= '0' && (c) <= '9' ) || \
        ( (c) >= 'a' && (c) <= 'f' ) || \
        ( (c) >= 'A' && (c) <= 'F' ) )

   static INT32 hexValue( CHAR c )
   {
      INT32 i = 0 ;
      if ( c >= '0' && c <= '9' )
      {
         i = c - '0' ;
      }
      else if ( c >= 'a' && c <= 'f' )
      {
         i = 10 + c - 'a' ;
      }
      else
      {
         i = 10 + c - 'A' ;
      }
      return i ;
   }

/*
   static INT32 getDel( const string &rawChar, CHAR &chr )
   {
      INT32 rc = SDB_OK ;
      if ( 1 == rawChar.size() )
      {
         chr = rawChar[0] ;
         goto done ;
      }
      if ( rawChar.size() < 3 || rawChar.size() > 4 )
      {
         PD_LOG( PDERROR, "Invalid value for char" ) ;
         goto error ;
      }
      if ( '0' != rawChar[0] || 'x' != rawChar[1] )
      {
         PD_LOG( PDERROR, "Invalid value for char" ) ;
         goto error ;
      }

      if ( 3 == rawChar.size() && IS_HEX( rawChar[2] ) )
      {
         chr = (CHAR)hexValue( rawChar[2] ) ;
      }
      else if ( 4 == rawChar.size() &&
                IS_HEX( rawChar[2] ) && IS_HEX( rawChar[3] ) )
      {
         chr = (CHAR)( 16 * hexValue( rawChar[2] ) + hexValue( rawChar[3] ) );
      }
      else
      {
         PD_LOG( PDERROR, "Invalid value for char" ) ;
         goto error ;
      }


   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }
*/

   static INT32 _convertAsciiChar( const string& in, string& out )
   {
      INT32 rc = SDB_OK ;
      stringstream ss ;
      const CHAR* str = in.c_str() ;
      INT32 len = in.length() ;
      BOOLEAN hasEscape = FALSE ;
      BOOLEAN hasHex = FALSE ;

      while ( len > 0 )
      {
         CHAR ch = *str ;

         if ( '\\' == ch )
         {
            CHAR nextCh = *( str + 1 ) ;
            str++ ;
            len-- ;

            // escape ascii char
            if ( isdigit( nextCh ) )
            {
               INT64 c = 0 ;
               INT64 newC = 0 ;

               while ( len > 0 && isdigit( *str ) )
               {
                  newC = c * 10 + ( *str - '0' ) ;
                  // the max ascii is 127
                  if ( newC < 0 || newC > 127 )
                  {
                     break ;
                  }
                  c = newC ;
                  str++ ;
                  len-- ;
               }

               ss << ( CHAR )c ;
               hasEscape = true ;
               continue ;
            }
            else if ( 'n' == nextCh )
            {
               str++ ;
               len-- ;
               ss << '\n' ;
               hasEscape = true ;
               continue ;
            }
            else if ( 'r' == nextCh )
            {
               str++ ;
               len-- ;
               ss << '\r' ;
               hasEscape = true ;
               continue ;
            }
            else if ( 't' == nextCh )
            {
               str++ ;
               len-- ;
               ss << '\t' ;
               hasEscape = true ;
               continue ;
            }
            else if ( '\\' == nextCh )
            {
               str++ ;
               len-- ;
               ss << '\\' ;
               hasEscape = true ;
               continue ;
            }

            rc = SDB_INVALIDARG ;
            goto error ;
         }
         // detect 0x
         else if ( '0' == ch )
         {
            CHAR nextCh = *( str + 1 ) ;
            str++ ;
            len-- ;

            if ( 'x' == nextCh )
            {
               INT64 c = 0 ;
               INT64 newC = 0 ;

               if ( IS_HEX( *( str + 1 ) ) )
               {
                  str++ ;
                  len-- ;

                  while ( len > 0 && IS_HEX( *str ) )
                  {
                     if ( '0' == *str && len > 1 && 'x' == *( str + 1 ) )
                     {
                        break ;
                     }

                     newC = c * 16 + hexValue( *str ) ;

                     // the max ascii is 127
                     if ( newC < 0 || newC > 127 )
                     {
                        break ;
                     }
                     c = newC ;
                     str++ ;
                     len-- ;
                  }

                  ss << ( CHAR )c ;
                  hasHex = true ;

                  continue ;
               }
               else
               {
                  str-- ;
                  len++ ;
               }
            }
            else
            {
               str-- ;
               len++ ;
            }
         }

         ss << ch ;
         str++ ;
         len-- ;
      }

      if ( TRUE == hasEscape && TRUE == hasHex )
      {
         cerr << "Doesn't support value in mixed format for option "  OPTION_DELFIELD
              << endl ;
         PD_LOG( PDERROR, "Doesn't support value in mixed format for option "
                          OPTION_DELFIELD ) ;
         rc = SDB_INVALIDARG ;
         goto error ;
      }

      out = ss.str() ;

   done:
      return rc ;
   error:
      goto done ;
   }

   // check and set the delimiter options
   INT32 expOptions::_setDelOptions()
   {
      INT32 rc = SDB_OK ;

      if ( _has(OPTION_CHECKDELIMETER) )
      {
         _strictCheckDel = _get<bool>(OPTION_CHECKDELIMETER) ;
      }

      if ( _has(OPTION_DELCHAR) )
      {
         string rawStr = _get<string>(OPTION_DELCHAR) ;

         rc = _convertAsciiChar( rawStr, _delChar ) ;
         if ( SDB_OK != rc )
         {
            cerr << "Invalid value for option "  OPTION_DELCHAR
                 << endl ;
            PD_LOG( PDERROR, "Invalid value for option "
                             OPTION_DELCHAR ) ;
            goto error ;
         }
      }
      if ( _has(OPTION_DELFIELD) )
      {
         string rawStr = _get<string>(OPTION_DELFIELD) ;

         if ( rawStr.empty() )
         {
            rc = SDB_INVALIDARG ;
            std::cerr << OPTION_DELFIELD << " can't be empty" << std::endl ;
            PD_LOG( PDERROR, "Invalid value for option "
                             OPTION_DELFIELD ) ;
            goto error ;
         }

         rc = _convertAsciiChar( rawStr, _delField ) ;
         if ( SDB_OK != rc )
         {
            cerr << "Invalid value for option "  OPTION_DELFIELD
                 << endl ;
            PD_LOG( PDERROR, "Invalid value for option "
                             OPTION_DELFIELD ) ;
            goto error ;
         }
      }
      if ( _has(OPTION_DELRECORD) )
      {
         string rawStr = _get<string>(OPTION_DELRECORD) ;

         if ( rawStr.empty() )
         {
            rc = SDB_INVALIDARG ;
            std::cerr << OPTION_DELRECORD << " can't be empty" << std::endl ;
            PD_LOG( PDERROR, "Invalid value for option "
                             OPTION_DELRECORD ) ;
            goto error ;
         }

         rc = _convertAsciiChar( rawStr, _delRecord ) ;
         if ( SDB_OK != rc )
         {
            cerr << "Invalid value for option "  OPTION_DELRECORD
                 << endl ;
            PD_LOG( PDERROR, "Invalid value for option "
                             OPTION_DELRECORD ) ;
            goto error ;
         }
      }

      if ( _delChar.size() > 0 && string::npos != _delField.find( _delChar ) )
      {
         cerr << "Option " << OPTION_DELCHAR << " cant be same as "
              << "option " << OPTION_DELFIELD << endl ;
         PD_LOG( PDERROR, "Option \"%s\" cant be same as option \"%s\"",
                 OPTION_DELCHAR, OPTION_DELFIELD ) ;
         goto error ;
      }
      if ( _delChar.size() > 0 && string::npos != _delRecord.find( _delChar ) )
      {
         cerr << "Option " << OPTION_DELCHAR << " cant be same as "
              << "option " << OPTION_DELRECORD << endl ;
         PD_LOG( PDERROR, "Option \"%s\" cant be same as option \"%s\"",
                 OPTION_DELCHAR, OPTION_DELRECORD ) ;
         goto error ;
      }
      if ( _delField == _delRecord )
      {
         cerr << "Option " << OPTION_DELFIELD << " cant be same as "
              << "option " << OPTION_DELRECORD << endl ;
         PD_LOG( PDERROR, "Option \"%s\" cant be same as option \"%s\"",
                 OPTION_DELFIELD, OPTION_DELRECORD ) ;
         goto error ;
      }

      if ( _strictCheckDel && FORMAT_CSV == _type )
      {
         if ( !_checkDelimeters( _delChar, _delField, _delRecord ) )
         {
            goto error ;
         }
      }

   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }

   BOOLEAN expOptions::_checkDelimeters( string &stringDelimiter,
                                         string &fieldDelimiter,
                                         string &recordDelimiter )
   {
      if ( stringDelimiter.find( fieldDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELCHAR << " can't contain "
                   << OPTION_DELFIELD << std::endl ;
         return FALSE ;
      }

      if ( stringDelimiter.find( recordDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELCHAR << " can't contain "
                   << OPTION_DELRECORD << std::endl ;
         return FALSE ;
      }

      if ( stringDelimiter.size() > 0 &&
           fieldDelimiter.find( stringDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELFIELD << " can't contain "
                   << OPTION_DELCHAR << std::endl ;
         return FALSE ;
      }

      if ( fieldDelimiter.find( recordDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELFIELD << " can't contain "
                   << OPTION_DELRECORD << std::endl ;
         return FALSE ;
      }

      if ( stringDelimiter.size() > 0 &&
           recordDelimiter.find( stringDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELRECORD << " can't contain "
                   << OPTION_DELCHAR << std::endl ;
         return FALSE ;
      }

      if ( recordDelimiter.find( fieldDelimiter ) != string::npos )
      {
         std::cerr << OPTION_DELRECORD << " can't contain "
                   << OPTION_DELFIELD << std::endl ;
         return FALSE ;
      }

      return TRUE ;
   }

   // check and set the configure-file options
   INT32 expOptions::_setConfOptions()
   {
      INT32 rc = SDB_OK ;

      if ( _cmdHas(OPTION_CONF) && _cmdHas(OPTION_GENCONF) )
      {
         cerr << "Option " << OPTION_CONF << " and "
              << "option " << OPTION_GENCONF << " "
              << "cannot be used at the same time" << endl ;
         PD_LOG( PDERROR, "Option \"%s\" and option \"%s\""
                          "cannot be used at the same time",
                 OPTION_CONF, OPTION_GENCONF ) ;
         goto error ;
      }

      if ( _cmdHas(OPTION_CONF) )
      {
         _conf = _get<string>(OPTION_CONF) ;
      }
      if ( _cmdHas(OPTION_GENCONF) )
      {
         _genConf = _get<string>(OPTION_GENCONF) ;
      }
      if ( _cmdHas(OPTION_GENFIELDS) )
      {
         _genFields= _get<bool>(OPTION_GENFIELDS) ;
      }

   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }

   INT32 expOptions::_setFilePathOptions()
   {
      INT32 rc = SDB_OK ;
      if ( _has(OPTION_FILENAME) && _has(OPTION_DIRNAME) )
      {
         cerr << "Option " << OPTION_FILENAME << " and "
              << "option " << OPTION_DIRNAME << " "
              << "cannot be used at the same time" << endl ;
         PD_LOG( PDERROR, "Option \"%s\" and option \"%s\""
                          "cannot be used at the same time",
                 OPTION_FILENAME, OPTION_DIRNAME ) ;
         goto error ;
      }
      else if ( !_has(OPTION_FILENAME) &&
                !_has(OPTION_DIRNAME) &&
                !_has(OPTION_GENCONF) )
      {
         cerr << "Option " << OPTION_FILENAME << " or "
              << "option " << OPTION_DIRNAME << " "
              << "must be specified" << endl ;
         PD_LOG( PDERROR, "Option \"%s\" or option \"%s\""
                          "must be specified ",
                 OPTION_FILENAME, OPTION_DIRNAME ) ;
         goto error ;
      }

      if ( _has(OPTION_DIRNAME) )
      {
         _dir = _get<string>(OPTION_DIRNAME) ;
         if ( _dir.empty() )
         {
            _dir = "." OSS_FILE_SEP ;
         }
         else if( OSS_FILE_SEP_CHAR != _dir[ _dir.size() - 1 ] )
         {
            _dir += OSS_FILE_SEP ;
         }

         rc = ossAccess( _dir.c_str(), W_OK ) ;
         if ( SDB_FNE == rc )
         {
            cerr << "Directory " << _dir << " does not exist" << endl ;
            PD_LOG( PDERROR, "Directory %s does not exist", _dir.c_str() ) ;
            goto error ;
         }
         else if ( SDB_OK != rc )
         {
            cerr << "Failed to access directory " << _dir << endl ;
            PD_LOG( PDERROR, "Failed to access directory %s", _dir.c_str() ) ;
            goto error ;
         }
      }
      if ( _has(OPTION_FILENAME) )
      {
         _file = _get<string>(OPTION_FILENAME) ;
      }

   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }

   // check and set the single collection options
   INT32 expOptions::_setCollectionOptions()
   {
      INT32 rc = SDB_OK ;

      // -c,-l must be specified together
      if ( ( !_has(OPTION_COLLECTSPACE) && _has(OPTION_COLLECTION) ) ||
           ( _has(OPTION_COLLECTSPACE) && !_has(OPTION_COLLECTION) ) )
      {
         cerr << "Option " << OPTION_COLLECTSPACE
              << " must be specified with "
              << "option " << OPTION_COLLECTION << endl ;
         PD_LOG( PDERROR, "Option \"%s\" must be specified with option \"%s\"",
                 OPTION_COLLECTSPACE, OPTION_COLLECTION ) ;
         goto error ;
      }

      // -c/-l cant be used mixing with --cscl/--excludecscl
      if ( _has(OPTION_COLLECTSPACE) &&
           ( _has(OPTION_CSCL) || _has(OPTION_EXCLUDECSCL) ) )
      {
         cerr << "Option " OPTION_COLLECTSPACE "/" OPTION_COLLECTION
              << " cannot be used mixing with option " OPTION_CSCL "/"
                 OPTION_EXCLUDECSCL << endl ;
         PD_LOG( PDERROR, "Option \"%s\"/\"%s\" cannot be used mixing with "
                          "option \"%s\"/\"%s\"",
                 OPTION_COLLECTSPACE, OPTION_COLLECTION,
                 OPTION_CSCL, OPTION_EXCLUDECSCL ) ;
         goto error ;
      }

      // --select cant be used with --fields
      if ( _has(OPTION_SELECT) && _has(OPTION_FIELDS) )
      {
         cerr << "Option " << OPTION_SELECT << " and "
              << "option " << OPTION_FIELDS << " "
              << "cannot be used at the same time" << endl ;
         PD_LOG( PDERROR, "Option \"%s\" and option \"%s\""
                          "cannot be used at the same time",
                 OPTION_SELECT, OPTION_FIELDS ) ;
         goto error ;
      }

      if ( _has(OPTION_COLLECTSPACE) )
      {
         _csName = _get<string>(OPTION_COLLECTSPACE) ;
         _clName = _get<string>(OPTION_COLLECTION) ;
         _cscl =  _csName ;
         _cscl += "." ;
         _cscl += _clName ;
      }
      if ( _has(OPTION_SELECT) )
      {
         _select = _get<string>(OPTION_SELECT) ;
      }
      if ( _has(OPTION_SORT) )
      {
         _sort = _get<string>(OPTION_SORT) ;
      }
      if ( _has(OPTION_FILTER) )
      {
         _filter = _get<string>(OPTION_FILTER) ;
      }
      if( _has( OPTION_SKIP ) )
      {
         _skip = _get<INT64>( OPTION_SKIP ) ;
      }
      if( _has( OPTION_LIMIT ) )
      {
         _limit = _get<INT64>( OPTION_LIMIT ) ;
      }

      if ( _has(OPTION_CSCL) )
      {
         _cscl = _get<string>(OPTION_CSCL) ;
      }
      if ( _has(OPTION_EXCLUDECSCL) )
      {
         _excludeCscl = _get<string>(OPTION_EXCLUDECSCL ) ;
      }

      if ( _cmdHas(OPTION_FIELDS) && _confHas(OPTION_FIELDS) )
      {
         vector<string> cmdCLFields ;
         cmdCLFields = _cmdVm[OPTION_FIELDS].as< vector<string> >() ;
         _fields = _confVm[OPTION_FIELDS].as< vector<string> >() ;
         _fields.reserve( _fields.size() + cmdCLFields.size() ) ;
         _fields.insert( _fields.begin(),
                           cmdCLFields.begin(), cmdCLFields.end() ) ;
      }
      else if ( _has(OPTION_FIELDS) )
      {
         _fields = _get< vector<string> >(OPTION_FIELDS) ;
      }

   done:
      return rc ;
   error:
      rc = SDB_INVALIDARG ;
      goto done ;
   }

   INT32 expOptions::_setOptions( INT32 argc )
   {
      INT32 rc = SDB_OK ;

      if ( _has( OPTION_HOSTS ) )
      {
         _hostsString = _get<string>( OPTION_HOSTS ) ;
      }

      if ( _has(OPTION_HOSTNAME) )
      {
         _hostName = _get<string>(OPTION_HOSTNAME) ;
      }

      if ( _has(OPTION_SVCNAME) )
      {
         _svcName = _get<string>(OPTION_SVCNAME) ;
      }

      // add hostname & svcname to hostsString,
      // so we can process them in one time
      if ( _has( OPTION_HOSTNAME ) || _has( OPTION_SVCNAME ) )
      {
         // it's ok if there are duplicate hostsString, it'll be processed.
         if ( _has( OPTION_HOSTS ) )
         {
            _hostsString += "," + _hostName + ":" + _svcName ;
         }
         else
         {
            _hostsString = _hostName + ":" + _svcName ;
         }
      }

      rc = Hosts::parse( _hostsString, _hosts ) ;
      if ( rc )
      {
         std::cerr << "Invalid value for hosts"  << std::endl;
         PD_LOG( PDERROR, "Invalid value for hosts : %s", _hostsString.c_str() ) ;
         rc = SDB_INVALIDARG;
         goto error;
      }
      Hosts::removeDuplicate( _hosts ) ;

      if ( _has(OPTION_CIPHERFILE) )
      {
         _cipherfile = _get<string>(OPTION_CIPHERFILE) ;
      }

      if ( _has(OPTION_TOKEN) )
      {
         _token = _get<string>(OPTION_TOKEN) ;
      }
      if ( _has(OPTION_USER) )
      {
         _user = _get<string>(OPTION_USER) ;

         if ( _has(OPTION_PASSWORD) )
         {
            string  passwd ;
            BOOLEAN isNormalInput = FALSE ;

            if ( 0 == passwdVec.size() )
            {
               isNormalInput = utilPasswordTool::interactivePasswdInput( passwd ) ;
            }
            else
            {
               isNormalInput = TRUE ;
               passwd = passwdVec[0] ;
            }

            if ( !isNormalInput )
            {
               rc = SDB_APP_INTERRUPT ;
               std::cerr << getErrDesp( rc ) << ", rc: " << rc << std::endl ;
               goto error ;
            }

            _password = passwd ;
         }
         else
         {
            utilPasswordTool passwdTool ;

            if ( _has(OPTION_CIPHER) && _get<bool>(OPTION_CIPHER) )
            {
               rc = passwdTool.getPasswdByCipherFile( _user, _token,
                                                      _cipherfile,
                                                      _password ) ;
               if ( SDB_OK != rc )
               {
                  std::cerr << "Failed to get user[" << _user.c_str()
                            << "] password from cipher file"
                            << "[" << _cipherfile.c_str() << "], rc: " << rc
                            << std::endl ;
                  PD_LOG( PDERROR, "Failed to get user[%s] password from cipher"
                          " file[%s], rc: %d", _user.c_str(),
                          _cipherfile.c_str(), rc ) ;
                  goto error ;
               }
               _user = utilGetUserShortNameFromUserFullName( _user ) ;
            }
            else
            {
               if ( _has(OPTION_TOKEN) || _has(OPTION_CIPHERFILE) )
               {
                  std::cout << "If you want to use cipher text, you should use"
                            << " \"--cipher true\"" << std::endl ;
               }
            }
         }
      }
      if ( _has(OPTION_SSL) )
      {
         _useSSL = _get<bool>(OPTION_SSL) ;
      }
      if ( _has(OPTION_INCLUDE) )
      {
         _headLine = _get<bool>(OPTION_INCLUDE) ;
      }
      if ( _has(OPTION_INCLUDEREGEX) )
      {
         _includeRegex = _get<bool>(OPTION_INCLUDEREGEX) ;
      }
      if ( _has(OPTION_INCLUDEBINARY) )
      {
         _includeBinary = _get<bool>(OPTION_INCLUDEBINARY) ;
      }
      if ( _has(OPTION_FORCE) )
      {
         _force = _get<bool>(OPTION_FORCE) ;
      }
      if ( _has(OPTION_KICKNULL) )
      {
         _kickNull = _get<bool>(OPTION_KICKNULL) ;
      }
      if ( _has(OPTION_STRICT) )
      {
         _strict = _get<bool>(OPTION_STRICT) ;
      }

      if ( _has(OPTION_TYPE) )
      {
         _typeName = _get<string>(OPTION_TYPE) ;
         rc = formatOfName( _typeName, _type ) ;
         if ( SDB_OK != rc )
         {
            cerr << "Invalid value for option " << OPTION_TYPE <<endl;
            PD_LOG( PDERROR, "Invalid value for option " OPTION_TYPE ) ;
            goto error ;
         }
      }

      if ( _has( OPTION_WITHID ) )
      {
         _withId = _get<bool>( OPTION_WITHID ) ;
      }
      else
      {
         if( FORMAT_CSV == _type )
         {
            _withId = FALSE ;
         }
         else if( FORMAT_JSON == _type )
         {
            _withId = TRUE ;
         }
      }

      if ( _has(OPTION_FILELIMIT) )
      {
         rc = getFileLimit( _get<string>(OPTION_FILELIMIT), _fileLimit ) ;
         if ( SDB_OK != rc )
         {
            cerr << "Invalid value for option "
                 << OPTION_FILELIMIT <<endl;
            PD_LOG( PDERROR, "Invalid value for option " OPTION_FILELIMIT);
            goto error ;
         }
      }

      if( _has( OPTION_FLOATFMT ) )
      {
         _floatFmt = _get<string>( OPTION_FLOATFMT ) ;
      }
      else
      {
         _floatFmt = "%.16g" ;
      }

      _replace = _has( OPTION_REPLACE ) ;

      rc = _setDelOptions() ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to set del options" ) ;
         goto error ;
      }
      rc = _setCollectionOptions() ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to set collection options" ) ;
         goto error ;
      }
      rc = _setFilePathOptions() ;
      if ( SDB_OK != rc )
      {
         PD_LOG( PDERROR, "Failed to set file-path options" ) ;
         goto error ;
      }

   done:
      return rc ;
   error:
      goto done ;
   }
}
