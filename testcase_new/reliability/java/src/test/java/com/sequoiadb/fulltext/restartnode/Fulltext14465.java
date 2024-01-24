package com.sequoiadb.fulltext.restartnode;

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
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.NodeRestart;
import com.sequoiadb.fulltext.FullTextDBUtils;
import com.sequoiadb.fulltext.FullTextUtils;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;

/**
 * @Description seqDB-14465: 删除全文索引时备节点正常重启
 * @author xiaoni Zhao
 * @date 2019/8/10
 */
public class Fulltext14465 extends SdbTestBase {
    private Sequoiadb sdb = null;
    private GroupMgr groupMgr = null;
    private String groupName = "";
    private List< String > cappedClNames = new ArrayList< String >();
    private List< String > esIndexNames = new ArrayList< String >();
    private CollectionSpace cs = null;

    @BeforeClass
    public void setUp() throws ReliabilityException {
        sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        groupMgr = GroupMgr.getInstance();
        if ( CommLib.isStandAlone( sdb ) ) {
            throw new SkipException( "isStandAlone() TRUE, STANDALONE MODE" );
        }
        if ( !groupMgr.checkBusiness( 120 ) ) {
            throw new SkipException( "checkBusiness() FAIL, GROUP ERROR" );
        }
        if ( !FullTextUtils.checkAdapter() ) {
            throw new SkipException( "Check adapter failed" );
        }
        List< String > groupNames = CommLib.getDataGroupNames( sdb );
        groupName = groupNames.get( 0 );
        cs = sdb.getCollectionSpace( csName );
        for ( int i = 0; i < 10; i++ ) {
            DBCollection cl = cs.createCollection( "cl_14465_" + i,
                    ( BSONObject ) JSON
                            .parse( "{'Group':'" + groupName + "'}" ) );
            cl.createIndex( "fullTextIndex_14465_" + i, "{a:'text'}", false,
                    false );
            FullTextDBUtils.insertData( cl, 10000 );
            cappedClNames.add( FullTextDBUtils.getCappedName( cl,
                    "fullTextIndex_14465_" + i ) );
            esIndexNames.add( FullTextDBUtils.getESIndexName( cl,
                    "fullTextIndex_14465_" + i ) );
        }
    }

    @Test
    public void Test() throws Exception {
        NodeWrapper node = groupMgr.getGroupByName( groupName ).getSlave();
        FaultMakeTask faultMakeTask = NodeRestart.getFaultMakeTask( node, 1,
                10 );
        TaskMgr taskMgr = new TaskMgr( faultMakeTask );
        taskMgr.addTask( new InsertTask() );
        taskMgr.addTask( new DropIndexTask() );
        taskMgr.execute();

        Assert.assertTrue( taskMgr.isAllSuccess(), taskMgr.getErrorMsg() );
        Assert.assertTrue( groupMgr.checkBusinessWithLSN( 600 ) );
        Assert.assertTrue( FullTextUtils.checkAdapter() );

        for ( int i = 0; i < 10; i++ ) {
            DBCollection cl = cs.getCollection( "cl_14465_" + i );
            Assert.assertTrue( FullTextUtils.isIndexDeleted( sdb,
                    esIndexNames.get( i ), cappedClNames.get( i ) ) );
            Assert.assertTrue( FullTextUtils.isCLConsistency( cl ) );
            Assert.assertTrue( FullTextUtils.isCLDataConsistency( cl ) );
        }
    }

    @AfterClass
    public void tearDown() {
        try {
            for ( int i = 0; i < 10; i++ ) {
                cs.dropCollection( "cl_14465_" + i );
            }
        } finally {
            sdb.close();
        }
    }

    private class InsertTask extends OperateTask {
        private Sequoiadb db = null;

        @Override
        public void exec() throws Exception {
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                for ( int i = 0; i < 10; i++ ) {
                    DBCollection cl = db.getCollectionSpace( csName )
                            .getCollection( "cl_14465_" + i );
                    FullTextDBUtils.insertData( cl, 10000 );
                }
            } finally {
                db.close();
            }
        }
    }

    private class DropIndexTask extends OperateTask {
        private Sequoiadb db = null;

        @Override
        public void exec() throws Exception {
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            for ( int i = 0; i < 10; i++ ) {
                DBCollection cl = db.getCollectionSpace( csName )
                        .getCollection( "cl_14465_" + i );
                FullTextDBUtils.dropFullTextIndex( cl,
                        "fullTextIndex_14465_" + i );
            }
            db.close();
        }
    }
}