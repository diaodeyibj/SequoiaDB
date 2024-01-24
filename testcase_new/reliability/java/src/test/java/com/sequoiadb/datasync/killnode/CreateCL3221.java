package com.sequoiadb.datasync.killnode;

import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.DBCursor;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.commlib.GroupMgr;
import com.sequoiadb.commlib.GroupWrapper;
import com.sequoiadb.commlib.NodeWrapper;
import com.sequoiadb.commlib.SdbTestBase;
import com.sequoiadb.datasync.Utils;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.ReliabilityException;
import com.sequoiadb.fault.KillNode;
import com.sequoiadb.task.FaultMakeTask;
import com.sequoiadb.task.OperateTask;
import com.sequoiadb.task.TaskMgr;
import org.bson.BSONObject;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;
import com.sequoiadb.datasync.CreateCLTask;
import java.util.*;

/**
 * @FileName seqDB-3221: 创建CL过程中备节点节点异常重启，该备节点为同步的源节点 seqDB-3230:
 *           创建CL过程中备节点节点异常重启，该备节点为同步的目的节点
 * @Author linsuqiang
 * @Date 2017-03-29
 * @Version 1.00
 */

/*
 * 1.指定所有选项(Compressed、AutoIndexId)，批量创建CL 2.过程中节点异常重启(kill -9) 3.继续创建 4.从故障中恢复
 * 5.继续创建部分CL，查看CL信息 6.随机选择CL插入数据 注：ReplSize =
 * 2,随机断一个备节点时，该节点有可能是同步的源节点，也有可能是同步的目的节点。
 */

public class CreateCL3221 extends SdbTestBase {
    private GroupMgr groupMgr = null;
    private boolean runSuccess = false;
    private String clNameBase = "cl_3221";
    private String clGroupName = null;
    private static final int CL_NUM = 500;

    @BeforeClass
    public void setUp() {
        Sequoiadb db = null;
        try {

            groupMgr = GroupMgr.getInstance();
            if ( !groupMgr.checkBusiness() ) {
                throw new SkipException( "checkBusiness failed" );
            }

            db = new Sequoiadb( coordUrl, "", "" );
            clGroupName = groupMgr.getAllDataGroupName().get( 0 );
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
            CreateCLTask cTask = new CreateCLTask(clNameBase, clGroupName, CL_NUM);
            mgr.addTask( cTask );
            mgr.execute();
            Assert.assertEquals( mgr.isAllSuccess(), true, mgr.getErrorMsg() );

            if ( !groupMgr.checkBusinessWithLSN( 600 ) ) {
                Assert.fail( "checkBusinessWithLSN() occurs timeout" );
            }

            db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            Utils.checkConsistencyCL(dataGroup, csName, clNameBase);
            checkUsable( db );
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
            dropCLs( db );
        } catch ( BaseException e ) {
            Assert.fail(
                    e.getMessage() + "\r\n" + Utils.getKeyStack( e, this ) );
        } finally {
            if ( db != null ) {
                db.close();
            }
        }
    }

    private void checkUsable( Sequoiadb db ) {
        for ( int i = 0; i < CL_NUM; i++ ) {
            String clName = clNameBase + "_" + i;
            CollectionSpace commCS = db.getCollectionSpace( csName );
            DBCollection cl = commCS.getCollection( clName );
            cl.insert( "{ a: 1 }" );
        }
    }

    private void dropCLs( Sequoiadb db ) {
        CollectionSpace commCS = db.getCollectionSpace( csName );
        for ( int i = 0; i < CL_NUM; i++ ) {
            String clName = clNameBase + "_" + i;
            commCS.dropCollection( clName );
        }
    }
}