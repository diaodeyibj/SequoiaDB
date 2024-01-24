package com.sequoiadb.datasync.killnode;

import com.sequoiadb.base.*;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.datasync.OprLobTask;
import com.sequoiadb.datasync.Utils;
import com.sequoiadb.datasync.AddNodeTask;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.KillNode;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;
import org.bson.BSONObject;
import org.bson.types.ObjectId;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import java.util.Date;
import java.util.List;
import java.util.Random;

/**
 * @FileName seqDB-3228: LOB写入加新建节点过程中备节点节点异常重启，该主节点为同步的源节点 seqDB-3237:
 *           LOB写入加新建节点过程中备节点节点异常重启，该主节点为同步的目的节点
 * @Author linsuqiang
 * @Date 2017-03-20
 * @Version 1.00
 */

/*
 * 1.创建CS，CL 2.循环增删LOB 3.往副本组中新增节点 4.过程中构造节点异常重启(kill -9) 5.选主成功后，继续写入 6.过程中故障恢复
 * 7.验证结果 注：ReplSize = 2,随机断一个备节点时，该节点有可能是同步的源节点，也有可能是同步的目的节点。
 */

public class OprLobAndAddNode3228 extends SdbTestBase {
    private GroupMgr groupMgr = null;
    private boolean runSuccess = false;
    private String clName = "cl_3228";
    private String clGroupName = null;
    private String randomHost = null;
    private int randomPort;
    private AddNodeTask aTask = null ;

    @BeforeClass
    public void setUp() {
        Sequoiadb db = null;
        try {
            db = new Sequoiadb( coordUrl, "", "" );
            groupMgr = GroupMgr.getInstance();

            if ( !groupMgr.checkBusiness() ) {
                throw new SkipException( "checkBusiness failed" );
            }

            clGroupName = groupMgr.getAllDataGroupName().get( 0 );
            DBCollection cl = createCL( db );
            putLobs( cl ); // prepare data for sync

            // node info, which will be used at AddNodeTask and teardown
            Random ran = new Random();
            List< String > hosts = groupMgr.getAllHosts();
            randomHost = hosts.get( ran.nextInt( hosts.size() ) );
            randomPort = ran.nextInt( reservedPortEnd - reservedPortBegin )
                    + reservedPortBegin;

            Utils.makeReplicaLogFull( clGroupName );
        } catch ( ReliabilityException e ) {
            Assert.fail( this.getClass().getName()
                    + " setUp error, error description:" + e.getMessage()
                    + "\r\n" + Utils.getKeyStack( e, this ) );
        } finally {
            if ( db != null ) {
                db.close();
            }
        }
    }

    @Test
    public void test() {
        Sequoiadb db = null;
        try {
            GroupWrapper dataGroup = groupMgr.getGroupByName( clGroupName );
            NodeWrapper slvNode = dataGroup.getSlave();

            FaultMakeTask faultTask = KillNode.getFaultMakeTask(
                    slvNode.hostName(), slvNode.svcName(), 1 );
            TaskMgr mgr = new TaskMgr( faultTask );
            OprLobTask oTask = new OprLobTask(clName);
            aTask = new AddNodeTask(clGroupName, randomHost, randomPort);
            mgr.addTask( oTask );
            mgr.addTask( aTask );
            mgr.execute();
            Assert.assertEquals( mgr.isAllSuccess(), true, mgr.getErrorMsg() );

            if ( !groupMgr.checkBusinessWithLSN( 600 ) ) {
                Assert.fail( "checkBusiness occurs timeout" );
            }

            if ( !dataGroup.checkInspect( 1 ) ) {
                Assert.fail(
                        "data is different on " + dataGroup.getGroupName() );
            }
            runSuccess = true;
        } catch ( ReliabilityException e ) {
            e.printStackTrace();
            Assert.fail( e.getMessage() );
        } finally {
            if ( db != null ) {
                db.close();
            }
        }
    }

    @AfterClass
    public void tearDown() {
        if ( !runSuccess ) {
            throw new SkipException( "to save environment" );
        }
        Sequoiadb db = null;
        try {
            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            CollectionSpace cs = db.getCollectionSpace( csName );
            cs.dropCollection( clName );
            aTask.removeNode();
        } catch ( BaseException e ) {
            Assert.fail(
                    e.getMessage() + "\r\n" + Utils.getKeyStack( e, this ) );
        } finally {
            if ( db != null ) {
                db.close();
            }
        }
    }

    private DBCollection createCL( Sequoiadb db ) {
        BSONObject option = ( BSONObject ) JSON
                .parse( "{ ReplSize: 2, Group: '" + clGroupName + "' }" );
        CollectionSpace cs = db.getCollectionSpace( csName );
        return cs.createCollection( clName, option );
    }

    private void putLobs( DBCollection cl ) {
        int lobSize = 1 * 1024 * 1024;
        byte[] lobBytes = new byte[ lobSize ];
        new Random().nextBytes( lobBytes );

        int lobNum = 100;
        for ( int i = 0; i < lobNum; i++ ) {
            DBLob lob = cl.createLob();
            lob.write( lobBytes );
            lob.close();
        }
    }
}