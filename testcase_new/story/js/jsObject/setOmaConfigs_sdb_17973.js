﻿/******************************************************************************
*@Description : test Oma function: setIniConfigs getIniConfigs                                       
*               TestLink: seqDB-17868:sdb和sdbcm的setIniConfigs支持ini格式，ini字段数据类型测试 
*@Author      : 2019-3-7  XiaoNi Huang
*@Info		  : sdb interface test
******************************************************************************/

main( test );

function test ()
{
   // init and prepare data
   var cmd = new Cmd();

   var filePath = WORKDIR + "/" + "config17973_sdb.conf";
   initWorkDir( cmd );
   cmd.run( "rm -f " + filePath );

   var iniData = {
      "Int.v1": -2147483648, "Int.v2": 2147483647, "Int.v3": -2147483649, "Int.v4": 2147483648,
      "Number.v1": -9007199254740992, "Number.v2": 9007199254740992, "Number.v3": -9007199254740993, "Number.v4": 9007199254740993,
      "Long.v1": -9223372036854775808, "Long.v2": 9223372036854775807, "Long.v3": -9223372036854776000, "Long.v4": 9223372036854776000,
      "Float.v1": -1.7E+308, "Float.v2": 1.7E+308, "Float.v3": -1.8E+308, "Float.v4": 1.8E+308,
      "String.v1": "ALL", "String.v2": "", "String.v3": " ",
      "Bool.v1": true, "Bool.v2": false,
      //"Null.v1": null,    ----not support
      "Nu.v1": undefined,
      "A": 1
   };
   var expFileData = '["Int.v1=-2147483648","Int.v2=2147483647","Int.v3=-2147483649","Int.v4=2147483648","Number.v1=-9007199254740992","Number.v2=9007199254740992","Number.v3=-9007199254740992","Number.v4=9007199254740992","Long.v1=-9.223372036854776e+18","Long.v2=9.223372036854776e+18","Long.v3=-9.223372036854776e+18","Long.v4=9.223372036854776e+18","Float.v1=-1.7e+308","Float.v2=1.7e+308","Float.v3=-inf","Float.v4=inf","String.v1=ALL","String.v2=","String.v3= ","Bool.v1=TRUE","Bool.v2=FALSE","A=1",""]';
   var expGetData = '{"A":"1","Bool.v1":"TRUE","Bool.v2":"FALSE","Float.v1":"-1.7e+308","Float.v2":"1.7e+308","Float.v3":"-inf","Float.v4":"inf","Int.v1":"-2147483648","Int.v2":"2147483647","Int.v3":"-2147483649","Int.v4":"2147483648","Long.v1":"-9.223372036854776e+18","Long.v2":"9.223372036854776e+18","Long.v3":"-9.223372036854776e+18","Long.v4":"9.223372036854776e+18","Number.v1":"-9007199254740992","Number.v2":"9007199254740992","Number.v3":"-9007199254740992","Number.v4":"9007199254740992","String.v1":"ALL","String.v2":"","String.v3":""}';

   // sdb test
   sdbSetOmaConf( iniData, filePath );

   var actData = readLocalFile( cmd, filePath );
   checkResult( expFileData, actData );

   var actData = sdbGetOmaConf( filePath );
   checkResult( expGetData, actData );

   // clear local data
   //cmd.run( "rm -f "+ filePath );
}

function sdbSetOmaConf ( data, filePath )
{
   Oma.setOmaConfigs( data, filePath );
}

function sdbGetOmaConf ( filePath )
{
   var rc = Oma.getOmaConfigs( filePath );
   return JSON.stringify( rc.toObj() );
}

function readLocalFile ( cmd, filePath )
{
   var rc = cmd.run( 'cat ' + filePath ).split( "\n" );
   return JSON.stringify( rc );
}

function checkResult ( expData, actData )
{
   assert.equal( expData, actData );
}

function initWorkDir ( cmd, remote ) 
{
   try
   {
      cmd.run( "ls " + WORKDIR );
   }
   catch( e ) 
   {
      if( 2 == e.message )   // 2: No such file or directory
      {
         cmd.run( "mkdir -p " + WORKDIR );
      }
      else
      {
         throw e;
      }
   }
}