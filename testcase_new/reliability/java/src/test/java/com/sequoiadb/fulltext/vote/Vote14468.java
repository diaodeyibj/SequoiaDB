package com.sequoiadb.fulltext.vote;

import java.util.ArrayList;
import java.util.List;
import org.bson.BSONObject;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;
import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.commlib.CommLib;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fulltext.FullTextDBUtils;
import com.sequoiadb.fulltext.FullTextUtils;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;

/**
 * @FileName seqDB-14468:删除全文索引时选主
 * @Author Zhao xiaoni
 * @Date 2019-10-30
 */
public class Vote14468 extends SdbTestBase {
    private Sequoiadb sdb = null;
    private String csName = "cs14468";
    private String clName = "cl14468";
    private String indexName = "index14468";
    private CollectionSpace cs = null;
    private DBCollection cl = null;
    private GroupMgr groupMgr = null;
    private String groupName = null;
    private List< String > cappedNames = new ArrayList< String >();
    private List< String > esIndexNames = new ArrayList< String >();

    @BeforeClass
    public void setUp() throws ReliabilityException {
        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        groupMgr = GroupMgr.getInstance();
        groupName = groupMgr.getAllDataGroupName().get( 0 );

        if ( CommLib.isStandAlone( sdb ) ) {
            throw new SkipException( "StandAlone environment!" );
        }

        if ( !groupMgr.checkBusiness() ) {
            throw new SkipException( "checkBusiness failed" );
        }
        if ( sdb.isCollectionSpaceExist( csName ) ) {
            sdb.dropCollectionSpace( csName );
        }
        if ( !FullTextUtils.checkAdapter() ) {
            throw new SkipException( "Check adapter failed" );
        }

        cs = sdb.createCollectionSpace( csName );
        for ( int i = 0; i < 5; i++ ) {
            cl = cs.createCollection( clName + i, ( BSONObject ) JSON
                    .parse( "{Group:'" + groupName + "'}" ) );
            cl.createIndex( indexName + i,
                    "{a:'text',b:'text',c:'text',d:'text'}", false, false );
            String cappedName = FullTextDBUtils.getCappedName( cl,
                    indexName + i );
            cappedNames.add( cappedName );
            String esIndexName = FullTextDBUtils.getESIndexName( cl,
                    indexName + i );
            esIndexNames.add( esIndexName );
            FullTextDBUtils.insertData( cl, 10000 );
        }
    }

    @Test
    public void Test() throws Exception {

        TaskMgr taskMgr = new TaskMgr();
        taskMgr.addTask( new InsertTask() );
        taskMgr.addTask( new ReelectTask() );
        taskMgr.addTask( new DropIndexTask() );
        taskMgr.execute();

        Assert.assertTrue( taskMgr.isAllSuccess(), taskMgr.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 120 ) );
        GroupWrapper dataGroup = groupMgr.getGroupByName( groupName );
        Assert.assertEquals( dataGroup.checkInspect( 1 ), true );
        Assert.assertTrue( FullTextUtils.checkAdapter() );

        // 插入记录的同时删除全文索引并同时数据组选主，全文索引全部删除成功
        for ( int i = 0; i < 5; i++ ) {
            cl = cs.getCollection( clName + i );
            FullTextUtils.isIndexDeleted( sdb, esIndexNames.get( i ),
                    cappedNames.get( i ) );
        }
    }

    private class InsertTask extends OperateTask {
        private Sequoiadb db = null;
        private DBCollection cl = null;

        @Override
        public void exec() throws Exception {
            // TODO Auto-generated method stub
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );

            try {
                for ( int i = 0; i < 5; i++ ) {
                    cl = db.getCollectionSpace( csName )
                            .getCollection( clName + i );
                    FullTextDBUtils.insertData( cl, 10000 );
                }
            } finally {
                db.close();
            }
        }
    }

    private class DropIndexTask extends OperateTask {
        private Sequoiadb db = null;
        private DBCollection cl = null;

        @Override
        public void exec() throws Exception {
            // TODO Auto-generated method stub
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                for ( int i = 0; i < 5; i++ ) {
                    cl = db.getCollectionSpace( csName )
                            .getCollection( clName + i );
                    cl.dropIndex( indexName + i );
                }
            } finally {
                db.close();
            }
        }
    }

    private class ReelectTask extends OperateTask {
        private Sequoiadb db = null;

        @Override
        public void exec() throws Exception {
            // TODO Auto-generated method stub
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );

            try {
                for ( int i = 0; i < 5; i++ ) {
                    db.getReplicaGroup( groupName ).reelect(
                            ( BSONObject ) JSON.parse( "{Seconds: 60}" ) );
                }
            } finally {
                db.close();
            }
        }
    }

    @AfterClass
    public void tearDown() throws Exception {
        try {
            sdb.dropCollectionSpace( csName );
            for ( int i = 0; i < esIndexNames.size(); i++ ) {
                Assert.assertTrue( FullTextUtils.isIndexDeleted( sdb,
                        esIndexNames.get( i ), cappedNames.get( i ) ) );
            }
        } finally {
            if ( sdb != null ) {
                sdb.close();
            }
        }
    }
}