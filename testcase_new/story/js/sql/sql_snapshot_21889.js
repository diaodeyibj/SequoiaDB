/******************************************************************************
@Description seqDB-21889:内置SQL语句查询$SNAPSHOT_CATA
@author liyuanyue
@date 2020-3-20
******************************************************************************/
testConf.skipStandAlone = true;

main( test );

function test ()
{
   var groupName = commGetDataGroupNames( db )[0];

   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_0" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_1" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_2" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_3" );

   var options = {
      ShardingKey: { a: 1 }, ShardingType: "hash", Partition: 8192, ReplSize: -1,
      Compressed: true, CompressionType: "snappy", Group: groupName
   };
   commCreateCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_0", options );
   commCreateCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_1", options );
   commCreateCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_2", options );
   commCreateCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_3", options );

   // 使用内置SQL语句查询快照信息
   var cur = db.exec( "select * from $SNAPSHOT_CATA" );

   var sqlCLCount = 0;
   while( cur.next() )
   {
      var tmpObj = cur.current().toObj();
      for( var i = 0; i < 4; i++ )
      {
         // 找到自己创建的集合
         if( tmpObj["Name"] === COMMCSNAME + "." + CHANGEDPREFIX + "_21889_" + i )
         {
            sqlCLCount++;
            var actObj = {
               ReplSize: tmpObj["ReplSize"], CompressionTypeDesc: tmpObj["CompressionTypeDesc"], ShardingKey: tmpObj["ShardingKey"]["a"],
               ShardingType: tmpObj["ShardingType"], Partition: tmpObj["Partition"], GroupName: tmpObj["CataInfo"][0]["GroupName"]
            };
            var expObj = {
               ReplSize: -1, CompressionTypeDesc: "snappy", ShardingKey: 1,
               ShardingType: "hash", Partition: 8192, GroupName: groupName
            };
            if( !( commCompareObject( expObj, actObj ) ) )
            {
               throw new Error( "$SNAPSHOT_CATA result error\n" + "expObj :" + JSON.stringify( expObj ) + "\nactObj :" + JSON.stringify( actObj ) );
            }
         }
      }
   }

   if( sqlCLCount != 4 )
   {
      throw new Error( "result is " + 4 + ",but actually result is " + sqlCLCount );
   }

   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_0" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_1" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_2" );
   commDropCL( db, COMMCSNAME, CHANGEDPREFIX + "_21889_3" );
}