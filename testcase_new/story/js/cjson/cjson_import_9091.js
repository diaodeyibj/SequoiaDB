﻿/************************************************************************
*@Description:   seqDB-9091:  json类型字段名为空
*@Author:        2016-7-20  wuyan
************************************************************************/
var clName = COMMCLNAME + "_9091";
main( test );
function test ()
{
   var cl = readyCL( COMMCSNAME, clName );
   cmd.run( 'rm -rf ./sdbimport.log' );

   //import datas          
   var imprtFile = tmpFileDir + "9091.json";
   var srcDatas = "{:'test'}"
   var rcInfos = importData( COMMCSNAME, clName, imprtFile, srcDatas );

   //check the Return Infos of the import datas
   var parseFail = 1;
   var importRes = 0;
   checkImportReturn( rcInfos, parseFail, importRes );

   //check sdbimport.log 
   var matchInfos = 'find ./ -maxdepth 1 -name "sdbimport.log" |xargs grep "Failed to parse JSON key"';
   var expLogInfo = 'Failed to parse JSON key';
   checkSdbimportLog( matchInfos, expLogInfo );

   commDropCL( db, COMMCSNAME, clName );
   removeTmpDir();
}