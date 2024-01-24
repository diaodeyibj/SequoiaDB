package com.sequoiadb.fulltext.parallel;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.bson.BSONObject;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.annotations.Test;

import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.DBCursor;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.fulltext.utils.FullTextDBUtils;
import com.sequoiadb.fulltext.utils.FullTextUtils;
import com.sequoiadb.testcommon.FullTestBase;
import com.sequoiadb.testcommon.SdbTestBase;
import com.sequoiadb.threadexecutor.ResultStore;
import com.sequoiadb.threadexecutor.ThreadExecutor;
import com.sequoiadb.threadexecutor.annotation.ExecuteOrder;

/**
 * @FileName: seqDB-12118:创建集合空间与创建/删除全文索引并发，创建新的集合空间
 * @Author zhaoyu
 * @Date 2019-05-11
 */

public class Fulltext12118B extends FullTestBase {
    private List< String > csNames = new ArrayList< String >();
    private List< String > clNames = new ArrayList< String >();
    private String csCreateName = "cs12118B_2";
    private String csBasicName = "cs12118B";
    private String clBasicName = "cl12118B";
    private int csNum = 2;
    private int clNum = 4;
    private String indexName = "fulltext12118B";
    private int insertNum = 20000;
    private ThreadExecutor te = new ThreadExecutor(
            FullTextUtils.THREAD_TIMEOUT );

    @Override
    protected void initTestProp() {
        caseProp.setProperty( IGNORESTANDALONE, "true" );
    }

    @Override
    protected void caseInit() {
        for ( int i = 0; i < csNum; i++ ) {
            String csName = csBasicName + "_" + i;
            csNames.add( csName );
        }

        for ( int i = 0; i < clNum; i++ ) {
            String clName = clBasicName + "_" + i;
            clNames.add( clName );
        }
        // 每个集合空间下一半集合用于删除全文索引，一半集合用于创建全文索引
        for ( String csName : csNames ) {
            if ( sdb.isCollectionSpaceExist( csName ) ) {
                sdb.dropCollectionSpace( csName );
            }
            CollectionSpace cs = sdb.createCollectionSpace( csName );
            for ( int i = 0; i < clNum; i++ ) {
                DBCollection cl = cs.createCollection( clNames.get( i ) );
                cl.createIndex( "id", "{id:1}", false, false );
                insertRecord( cl, insertNum );
                if ( i < clNum / 2 ) {
                    cl.createIndex( indexName, "{a:'text',b:'text'}", false,
                            false );
                }
            }
        }
    }

    @Override
    protected void caseFini() throws Exception {
        List< String > esIndexNames = new ArrayList< String >();
        List< String > cappedCLNames = new ArrayList< String >();
        for ( String csName : csNames ) {
            CollectionSpace cs = sdb.getCollectionSpace( csName );
            for ( String clName : clNames ) {
                DBCollection cl = cs.getCollection( clName );
                if ( cl.isIndexExist( indexName ) ) {
                    String esIndexName = FullTextDBUtils.getESIndexName( cl,
                            indexName );
                    esIndexNames.add( esIndexName );
                    String cappedCLName = FullTextDBUtils.getCappedName( cl,
                            indexName );
                    cappedCLNames.add( cappedCLName );
                }
            }

        }
        csNames.add( csCreateName );
        for ( String csName : csNames ) {
            FullTextDBUtils.dropCollectionSpace( sdb, csName );
        }
        if ( !esIndexNames.isEmpty() && !cappedCLNames.isEmpty() ) {
            Assert.assertTrue( FullTextUtils.isIndexDeleted( sdb, esIndexNames,
                    cappedCLNames ) );
        }
    }

    @Test
    public void test() throws Exception {
        // 执行并发测试,集合空间下的集合一半用于创建全文索引、一半用于删除全文索引
        for ( String csName : csNames ) {
            for ( int i = 0; i < clNum; i++ ) {
                if ( i < clNum / 2 ) {
                    te.addWorker( new DropFullIndexThread( csName,
                            clNames.get( i ) ) );
                } else {
                    te.addWorker( new CreateFullIndexThread( csName,
                            clNames.get( i ) ) );
                }
            }
        }
        te.addWorker( new CreateCS() );
        te.run();

    }

    private class DropFullIndexThread extends ResultStore {
        private String csName = null;
        private String clName = null;
        private String cappedCLName = null;
        private String esIndexName = null;
        private SimpleDateFormat df = new SimpleDateFormat(
                "yyyy-MM-dd HH:mm:ss.S" );

        public DropFullIndexThread( String csName, String clName ) {
            super();
            this.csName = csName;
            this.clName = clName;
        }

        @ExecuteOrder(step = 1, desc = "删除全文索引")
        public void dropFullIndex() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            System.out.println( this.getClass().getName().toString()
                    + " start at:" + df.format( new Date() ) );
            try {
                DBCollection cl = db.getCollectionSpace( csName )
                        .getCollection( clName );
                cappedCLName = FullTextDBUtils.getCappedName( cl, indexName );
                esIndexName = FullTextDBUtils.getESIndexName( cl, indexName );
                cl.dropIndex( indexName );
            } catch ( BaseException e ) {
                if ( e.getErrorCode() != -34 && e.getErrorCode() != -23
                        && e.getErrorCode() != -248 && e.getErrorCode() != -47
                        && e.getErrorCode() != -147 ) {
                    e.printStackTrace();
                    Assert.fail( e.getMessage() );
                }
                saveResult( e.getErrorCode(), e );
            } finally {
                db.close();
            }
            System.out.println( this.getClass().getName().toString()
                    + " stop at:" + df.format( new Date() ) );
        }

        @ExecuteOrder(step = 2, desc = "结果校验")
        public void checkResult() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                DBCollection cl = db.getCollectionSpace( csName )
                        .getCollection( clName );
                if ( getRetCode() == 0 ) {
                    Assert.assertTrue( FullTextUtils.isIndexDeleted( db,
                            esIndexName, cappedCLName ) );
                    // 全文检索数据报错-52、-6
                    try {
                        cl.query( "{'':{'$Text':{query:{match_all:{}}}}}",
                                "{a:1,c:1}", null, null );
                    } catch ( BaseException e ) {
                        if ( e.getErrorCode() != -6 && e.getErrorCode() != -52
                                && e.getErrorCode() != -10 ) {
                            Assert.fail( e.getMessage() );
                        }
                    }
                } else {
                    // 同步符合预期
                    Assert.assertTrue( FullTextUtils.isIndexCreated( cl,
                            indexName, insertNum ) );
                    // 全文检索数据符合预期
                    DBCursor cursor = cl.query(
                            "{'':{'$Text':{query:{match_all:{}}}}}",
                            "{a:1,c:1}", null, null );
                    int actualRecordNum = 0;
                    while ( cursor.hasNext() ) {
                        cursor.getNext();
                        actualRecordNum++;
                    }
                    Assert.assertEquals( actualRecordNum, insertNum );
                }
            } catch ( Exception e ) {
                e.printStackTrace();
                Assert.fail( e.getMessage() );
            } finally {
                db.close();
            }
        }
    }

    private class CreateFullIndexThread extends ResultStore {
        private String csName = null;
        private String clName = null;
        private SimpleDateFormat df = new SimpleDateFormat(
                "yyyy-MM-dd HH:mm:ss.S" );

        public CreateFullIndexThread( String csName, String clName ) {
            super();
            this.csName = csName;
            this.clName = clName;
        }

        @ExecuteOrder(step = 1, desc = "创建全文索引")
        public void createFullIndex() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            System.out.println( this.getClass().getName().toString()
                    + " start at:" + df.format( new Date() ) );
            try {
                DBCollection cl = db.getCollectionSpace( csName )
                        .getCollection( clName );
                cl.createIndex( indexName, "{a:'text',b:'text'}", false,
                        false );
            } catch ( BaseException e ) {
                if ( e.getErrorCode() != -34 && e.getErrorCode() != -23
                        && e.getErrorCode() != -248 ) {
                    e.printStackTrace();
                    Assert.fail( e.getMessage() );
                }
                saveResult( e.getErrorCode(), e );
            } finally {
                db.close();
            }
            System.out.println( this.getClass().getName().toString()
                    + " stop at:" + df.format( new Date() ) );
        }

        @ExecuteOrder(step = 2, desc = "结果校验")
        public void checkResult() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                DBCollection cl = db.getCollectionSpace( csName )
                        .getCollection( clName );
                if ( getRetCode() == 0 ) {
                    // 同步符合预期
                    Assert.assertTrue( FullTextUtils.isIndexCreated( cl,
                            indexName, insertNum ) );
                    // 全文检索数据符合预期
                    DBCursor cursor = cl.query(
                            "{'':{'$Text':{query:{match_all:{}}}}}",
                            "{a:1,c:1}", null, null );
                    int actualRecordNum = 0;
                    while ( cursor.hasNext() ) {
                        cursor.getNext();
                        actualRecordNum++;
                    }
                    Assert.assertEquals( actualRecordNum, insertNum );

                } else {
                    Assert.assertFalse( cl.isIndexExist( indexName ) );
                }
            } catch ( Exception e ) {
                e.printStackTrace();
                Assert.fail( e.getMessage() );
            } finally {
                db.close();
            }

        }

    }

    private class CreateCS extends ResultStore {
        private SimpleDateFormat df = new SimpleDateFormat(
                "yyyy-MM-dd HH:mm:ss.S" );

        @ExecuteOrder(step = 1, desc = "创建集合空间、集合、全文索引、插入记录")
        public void createCS() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                System.out.println( this.getClass().getName().toString()
                        + " start at:" + df.format( new Date() ) );
                if ( db.isCollectionSpaceExist( csCreateName ) ) {
                    db.dropCollectionSpace( csCreateName );
                }
                CollectionSpace cs = db.createCollectionSpace( csCreateName );
                for ( String clName : clNames ) {
                    DBCollection cl = cs.createCollection( clName );
                    cl.createIndex( indexName, "{a:'text',b:'text'}", false,
                            false );
                    insertRecord( cl, insertNum );
                }
                System.out.println( this.getClass().getName().toString()
                        + " stop at:" + df.format( new Date() ) );
            } finally {
                db.close();
            }
        }

        @ExecuteOrder(step = 2, desc = "结果校验")
        public void checkResult() {
            Sequoiadb db = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
            try {
                CollectionSpace cs = db.getCollectionSpace( csCreateName );
                for ( String clName : clNames ) {
                    DBCollection cl = cs.getCollection( clName );
                    // 同步符合预期
                    Assert.assertTrue( FullTextUtils.isIndexCreated( cl,
                            indexName, insertNum ) );
                    // 全文检索数据符合预期
                    DBCursor cursor = cl.query(
                            "{'':{'$Text':{query:{match_all:{}}}}}",
                            "{a:1,c:1}", null, null );
                    int actualRecordNum = 0;
                    while ( cursor.hasNext() ) {
                        cursor.getNext();
                        actualRecordNum++;
                    }
                    Assert.assertEquals( actualRecordNum, insertNum );

                }
            } catch ( Exception e ) {
                e.printStackTrace();
                Assert.fail( e.getMessage() );
            } finally {
                db.close();
            }
        }

    }

    public void insertRecord( DBCollection cl, int insertNums ) {
        List< BSONObject > insertObjs = new ArrayList<>();
        for ( int i = 0; i < 100; i++ ) {
            for ( int j = 0; j < insertNums / 100; j++ ) {
                int k = i * 100 + j;
                insertObjs.add( ( BSONObject ) JSON.parse( "{id:" + k
                        + ",a: 'test_11981_" + i * 100 + j
                        + "', b: 'test_11981_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa "
                        + i * 100 + j + "',c:'text'}" ) );
            }
            cl.insert( insertObjs, 0 );
            insertObjs.clear();
        }
    }
}