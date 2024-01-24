package com.sequoiadb.basicoperation;

import org.bson.BSONObject;
import org.bson.types.BasicBSONList;
import org.bson.util.JSON;
import org.testng.Assert;
import org.testng.SkipException;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import com.sequoiadb.base.CollectionSpace;
import com.sequoiadb.base.DBCollection;
import com.sequoiadb.base.Sequoiadb;
import com.sequoiadb.exception.BaseException;
import com.sequoiadb.testcommon.SdbTestBase;

/**
 * FileName: TestSplit7111.java test interface: split(String sourceGroupName,
 * String destGroupName, double percent) testlink cases:seqDB-7111 testing
 * strategy: 1.create rangecl 2.insert records 3.split by percent 4.connect the
 * sourceGroup and targetGroup,check the results count
 * 
 * @author wuyan
 * @Date 2016.10.9
 * @version 1.00
 */
public class TestSplit7111 extends SdbTestBase {

    private String clName = "cl_7111";
    private static Sequoiadb sdb = null;
    private CollectionSpace cs = null;
    private DBCollection cl;
    String sourceRGName;
    String targetRGName;

    @BeforeClass
    public void setUp() {
        try {
            sdb = new Sequoiadb( SdbTestBase.coordUrl, "", "" );
        } catch ( BaseException e ) {
            Assert.assertTrue( false, "connect %s failed,"
                    + SdbTestBase.coordUrl + e.getMessage() );
        }

        if ( Commlib.isStandAlone( sdb ) ) {
            throw new SkipException( "is standalone skip testcase" );
        }

        if ( Commlib.OneGroupMode( sdb ) ) {
            throw new SkipException( "less two groups skip testcase" );
        }
        createCL();
    }

    public void createCL() {
        try {
            if ( !sdb.isCollectionSpaceExist( SdbTestBase.csName ) ) {
                sdb.createCollectionSpace( SdbTestBase.csName );
            }
        } catch ( BaseException e ) {
            // -33 CS exist,ignore exceptions
            Assert.assertEquals( -33, e.getErrorCode(), e.getMessage() );
        }
        String test = "{ShardingKey:{no:1},ShardingType:'range'}";
        BSONObject options = ( BSONObject ) JSON.parse( test );
        try {
            cs = sdb.getCollectionSpace( SdbTestBase.csName );
            cl = cs.createCollection( clName, options );
        } catch ( BaseException e ) {
            Assert.assertTrue( false, "create cl fail " + e.getErrorType() + ":"
                    + e.getMessage() );
        }
    }

    @DataProvider(name = "recordNumsProvider")
    public Object[][] recordNumsProvider() {
        return new Object[][] { { sourceRGName, 8 }, { targetRGName, 2 }, };
    }

    public void splitCL() {
        double percent = 20;
        try {
            sourceRGName = Commlib.getSourceRGName( sdb, SdbTestBase.csName,
                    clName );
            targetRGName = Commlib.getTarRgName( sdb, sourceRGName );
            cl.split( sourceRGName, targetRGName, percent );
        } catch ( BaseException e ) {
            Assert.assertTrue( false,
                    "split fail " + e.getErrorCode() + e.getMessage() );
        }
    }

    public void checkSplitResult() {
        BasicBSONList splitGroupNames = new BasicBSONList();
        splitGroupNames.add( sourceRGName );
        splitGroupNames.add( targetRGName );
        BasicBSONList listRecsNums = new BasicBSONList();
        Sequoiadb dataDB = null;
        for ( int i = 0; i < splitGroupNames.size(); i++ ) {
            try {
                String nodeName = sdb
                        .getReplicaGroup( ( String ) splitGroupNames.get( i ) )
                        .getMaster().getNodeName();
                dataDB = new Sequoiadb( nodeName, "", "" );
                DBCollection dataCL = dataDB
                        .getCollectionSpace( SdbTestBase.csName )
                        .getCollection( clName );
                long num = dataCL.getCount();
                listRecsNums.add( num );
            } catch ( BaseException e ) {
                Assert.assertTrue( false, e.getErrorCode() + e.getMessage() );
            } finally {
                if ( dataDB != null ) {
                    dataDB.disconnect();
                }
            }
        }
        // check the count of datas on each group,srcGroup is 8,targetGroup s 2
        Assert.assertEquals( ( long ) listRecsNums.get( 0 ), 8,
                "srcGroup count:" + ( long ) listRecsNums.get( 0 ) );
        Assert.assertEquals( ( long ) listRecsNums.get( 1 ), 2,
                "tarGroup count:" + ( long ) listRecsNums.get( 1 ) );
    }

    @AfterClass
    public void tearDown() {
        try {
            if ( cs.isCollectionExist( clName ) ) {
                cs.dropCollection( clName );
            }
            sdb.disconnect();
        } catch ( BaseException e ) {
            Assert.assertTrue( false, "clean up failed:" + e.getMessage() );
        }
    }

    @Test
    public void testSplit7111() {
        Commlib.bulkInsert( cl );
        splitCL();
        checkSplitResult();
    }

}