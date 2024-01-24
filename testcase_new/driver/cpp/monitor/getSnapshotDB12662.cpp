/**************************************************************
 * @Description: get all kind of snapshot(database)
 *               seqDB-12662 : get all kind of snapshot
 * @Modify     : Suqiang Ling
 *               2017-09-11
 ***************************************************************/
#include <gtest/gtest.h>
#include <client.hpp>
#include <iostream>
#include <vector>
#include "testcommon.hpp"
#include "arguments.hpp"
#include "testBase.hpp"

using namespace sdbclient ;
using namespace bson ;
using namespace std ;

class getSnapshotDB12662 : public testBase 
{
protected:
   sdbCursor cursor ;
   BSONObj res ;

   void TearDown()
   {
      INT32 rc = SDB_OK ;
      rc = cursor.close() ;
      ASSERT_EQ( SDB_OK, rc ) ;

      testBase::TearDown() ;
   }
} ;

TEST_F( getSnapshotDB12662, snapshotDatabase )
{
   // snapshot database
   INT32 rc = SDB_OK ;
   rc = db.getSnapshot( cursor, SDB_SNAP_DATABASE ) ;
   ASSERT_EQ( SDB_OK, rc ) ;

   // check snapshot
   rc = cursor.next( res ) ;
   ASSERT_EQ( SDB_OK, rc ) ;
   ASSERT_TRUE( res.hasField( "TotalInsert" ) ) ;
}