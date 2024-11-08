/******************************************************************************
 * @Description   : seqDB-33891:删除数据再查询导致锁升级，锁升级时读取的是内存的记录
 * @Author        : tangtao
 * @CreateTime    : 2023.11.07
 * @LastEditTime  : 2023.11.07
 * @LastEditors   : tangtao
 ******************************************************************************/
testConf.clName = COMMCLNAME + "_lockEscalation_33891";

main( test );
function test ( testPara )
{
   var cl = testPara.testCL;
   var indexName = "test_33891";

   cl.insert( { _id: 1, a: 1 } );
   cl.insert( { _id: 2, a: 2 } );
   cl.insert( { _id: 3, a: 3 } );
   cl.insert( { _id: 4, a: 4 } );

   cl.createIndex( indexName, { a: 1 } );

   db.setSessionAttr( { TransMaxLockNum: 1, TransIsolation: 2 } );
   db.transBegin();
   cl.remove( { _id: 2, a: 2 }, { "": indexName } );
   checkLockEscalated( db, false );
   var res = commCursor2Array( cl.find().sort( { a: 1 } ).hint( { "": indexName } ) );
   // read id:2 in memory trigger the lock escalation
   assert.equal( res, [{ _id: 1, a: 1 }, { _id: 3, a: 3 }, { _id: 4, a: 4 }] );
   checkLockEscalated( db, true );
   db.transCommit();
}