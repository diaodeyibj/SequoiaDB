/************************************
*@Description: insert doc that is size large than 64k 
               strong data consistency
*@author:     wangwenjing
**************************************/
function main ()
{
   try
   {
      var db = new Sdb( COORDHOSTNAME, COORDSVCNAME );
      if( commIsStandalone( db ) )
      {
         return;
      }

      var clName = COMMCLNAME + "_testinsdoc9";
      var mgr = new groupMgr( db );
      mgr.init();

      var nodeNum = 2;
      var group = selectGroupByNodeNum( mgr, nodeNum );

      var cl = new collection( COMMCSNAME, clName, w.ALL );
      cl.drop( db );
      cl.create( db, group.name );
      var pageSize = 64 * 1024;
      cl.insert( 3 * pageSize );
      if( !group.checkConsistency( cl ) )
      {
         throw new Error( "data is not consistency" ); 
      }
      if( !group.checkResult( true, group.checkDoc, cl, { id: 1 } ) )
      {
         throw new Error( "data is not consistency" );
      }
   }
   catch( e )
   {
      if( e.constructor === Error )
      {
         println( e.stack );
      }
      throw e;
   }
   finally
   {
      if( undefined !== db )
      {
         db.close();
      }
   }
}

main();