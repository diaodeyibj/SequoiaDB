/*******************************************************************************
*@Description : Test domain C driver, include sdbCreateDomain/sdbDropDomain/
*               /sdbGetDomain/sdbListDomains/sdbAlterDomain/
*               /sdbListCollectionSpacesInDomain/sdbListCollectionsInDomain
*@Modify List :
*               2014-7-15   xiaojun Hu   Init
*******************************************************************************/

#include <stdio.h>
#include <gtest/gtest.h>
#include <string.h>
#include <assert.h>
#include "testcommon.h"
#include "client.h"

#define  SDB_LIST_GROUPS         7
#define  SDB_INVALIDARG         -6
#define  SDB_DOMAIN_NOTEXIST    -214

// Test domain use in abnormal
TEST( sdbDomainTest, abnormal )
{
   INT32 rc                       = SDB_OK ;
   sdbConnectionHandle db         = 0 ;
   sdbDomainHandle dom            = 0 ;
   sdbCursorHandle cursor         = 0 ;
   const CHAR *pDomainName1       = "Largerthan128aaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" ;
   const CHAR *pDomName2          = "NULLHandle" ;
   const CHAR *getDomName1        = "NoCreateDomainName" ;
   const CHAR *altDomName1        = "AlterCorrectDomainName" ;
   CHAR pDomainName[512] ;
   CHAR pDomName1[50] ;
   CHAR getDomName[50] ;
   CHAR altDomName[50] ;

   // Deal with domain name
   getUniqueName( pDomainName1, pDomainName ) ;
   getUniqueName( pDomName2, pDomName1 ) ;
   getUniqueName( getDomName1, getDomName ) ;
   getUniqueName( altDomName1, altDomName ) ;

   rc = sdbConnect( HOST, SERVER, USER, PASSWD, &db ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to connect database, rc = " << rc  ;

   // Run mode is standalone
   rc = sdbGetList( db, SDB_LIST_GROUPS, NULL, NULL, NULL, &cursor ) ;
   if ( SDB_RTN_COORD_ONLY == rc )
   {
      printf( "Run mode is standalone.\n" ) ;
      return ;
   }
   ASSERT_EQ( SDB_OK, rc ) << "Failed to getList 'SDB_LIST_GROUPS', "
                               "rc = " << rc ;

   rc = sdbCreateDomain( db, NULL, NULL, &dom ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to create domain NULL, rc = "
                                       << rc ;
   // Domain name size is more than 128
   // printf( "get Name : %s\n", pDomainName ) ;
   rc = sdbCreateDomain( db, pDomainName, NULL, &dom ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to create domain large than"
                                          " 128, rc = " << rc ;
   // Option handle is null
   rc = sdbCreateDomain( db, pDomName1, NULL, NULL ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to create NULL domain handle,"
                                          "rc = " << rc ;
   // Drop domain have no name
   rc = sdbDropDomain( db, NULL ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to execute dropDomain specify"
                                          "nothing, rc = " << rc ;
   // Get domain abnormal
   rc = sdbGetDomain( db, NULL, &dom ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to execute getDomain specify"
                                          "nothing, rc = " << rc ;
   rc = sdbGetDomain( db, getDomName, &dom ) ;
   EXPECT_TRUE( SDB_DOMAIN_NOTEXIST == rc ) << "Failed to execute getDomain"
                                               "NotExist Name, rc = " << rc ;
   // Alter domain
   rc = sdbDropDomain( db, altDomName ) ;
   EXPECT_TRUE( SDB_OK == rc || SDB_DOMAIN_NOTEXIST == rc )
              << "Failed to drop domain for alter, rc = " << rc ;
   rc = sdbCreateDomain( db, altDomName, NULL, &dom ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to create domain for alter" << rc ;
   rc = sdbAlterDomain( dom, NULL ) ;
   EXPECT_TRUE( SDB_INVALIDARG == rc ) << "Failed to execute alterDomain"
                                          "specify no name, rc = " << rc ;
   // printf( "Drop Alter Dom Name : %s\n", altDomName ) ;
   rc = sdbDropDomain( db, altDomName ) ;
   EXPECT_TRUE( SDB_OK == rc ) << "Failed to drop domain for alter, rc = " << rc ;

   sdbReleaseDomain( dom ) ;
   sdbReleaseCursor( cursor ) ;
   sdbDisconnect( db ) ;
   sdbReleaseConnection( db ) ;
}

// Test domain in normal and all
TEST( sdbDomainTest, normal )
{
   sdbConnectionHandle db         = 0 ;
   sdbDomainHandle dom            = 0 ;
   sdbCursorHandle cursor         = 0 ;
   const CHAR *pDomainName1       = "DomainNameNormalRunAll" ;
   INT32 rc                       = SDB_OK ;
   CHAR  bson_itValue[5][1000] ;
   CHAR  pDomainName[50] ;
   const CHAR * key ;
   bson  domObj ;
   bson  altObj ;
   bson  condRG ;
   bson  selectRG ;
   bson  obj ;
   bson_iterator it ;

   // Deal with domain name
   getUniqueName( pDomainName1, pDomainName ) ;

   bson_init( &obj ) ;
   bson_finish( &obj ) ;

   // Domain option bson
   bson_init( &domObj ) ;
   bson_append_bool( &domObj, "AutoSplit", true ) ;
   bson_finish( &domObj ) ;
   //bson_print( &domObj ) ;

   // Group condition
   bson_init( &condRG ) ;
   bson_append_int( &condRG, "Role", 0 ) ;
   bson_finish( &condRG ) ;
   //bson_print( &condRG ) ;

   // Group selector
   bson_init( &selectRG ) ;
   bson_append_string( &selectRG, "GroupName", "" ) ;
   bson_finish( &selectRG ) ;
   //bson_print( &selectRG ) ;

   //rc = initEnv( HOST, SERVER, USER, PASSWD ) ;
   //ASSERT_EQ( SDB_OK, rc ) ;

   // Connect to database
   rc = sdbConnect( HOST, SERVER, USER, PASSWD, &db ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to create connection, rc = " << rc ;

   // Run mode is standalone
   rc = sdbGetList( db, SDB_LIST_GROUPS, NULL, NULL, NULL, &cursor ) ;
   if ( SDB_RTN_COORD_ONLY == rc )
   {
      printf( "Run mode is standalone.\n" ) ;
      return ;
   }
   ASSERT_EQ( SDB_OK, rc ) << "Failed to getList 'SDB_LIST_GROUPS', "
                                "rc = " << rc ;

   // Drop domain before create
   rc = sdbDropDomain( db, pDomainName ) ;
   EXPECT_TRUE( SDB_OK == rc || SDB_DOMAIN_NOTEXIST == rc )
              << "Failed to drop domain, rc = " << rc ;
   // printf( "NAMENAME : %s\n", pDomainName ) ;
   // Create domain
   rc = sdbCreateDomain( db, pDomainName, &domObj, &dom ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to create domain, rc = " << rc ;
   sdbReleaseDomain( dom ) ;
   dom = 0 ;              // let the domain handle is 0, test alter domain
   // List domain
   sdbReleaseCursor ( cursor ) ;
   rc = sdbListDomains( db, NULL, NULL, NULL, &cursor ) ;
   EXPECT_TRUE( SDB_OK == rc ) << "Failed to list domains, rc = " << rc ;
   // Get domain
   rc = sdbGetDomain( db, pDomainName, &dom ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to get domain, rc = " << rc ;

   // Get the datagroup where the database have
   sdbReleaseCursor ( cursor ) ;
   rc = sdbGetList( db, SDB_LIST_GROUPS, &condRG, &selectRG, NULL, &cursor) ;
   ASSERT_EQ( rc, SDB_OK ) ;
   int i = 0 ;
   while( SDB_OK == sdbNext( cursor, &obj ) )
   {
      //bson_print( &obj ) ;
      //printf( "Count : i = %d\n", i ) ;
      bson_iterator_init( &it , &obj ) ;
      strcpy( bson_itValue[i], bson_iterator_string( &it )) ;
      // printf( "Get Group: %s\n", bson_itValue[i] ) ;

      // Alter domain optin bson
      bson_init( &altObj ) ;
      bson_append_start_array( &altObj, "Groups" ) ;
      bson_append_string( &altObj, "", bson_itValue[i] ) ;
      bson_append_finish_array( &altObj ) ;
      bson_finish( &altObj ) ;
      bson_print( &altObj ) ;
      // Alter domain
      rc = sdbAlterDomain( dom, &altObj ) ;
      bson_destroy( &altObj ) ;
      ASSERT_EQ( rc, SDB_OK ) << "Failed to alter domain, rc =" << rc ;
      ++i ;
   }
   // List collection space
   sdbReleaseCursor ( cursor ) ;
   rc = sdbListCollectionSpacesInDomain( dom, &cursor ) ;
   EXPECT_TRUE( SDB_OK == rc ) << "Failed to list CS, rc = " << rc ;
   // List collection
   sdbReleaseCursor ( cursor ) ;
   rc = sdbListCollectionsInDomain( dom, &cursor ) ;
   EXPECT_TRUE( SDB_OK == rc ) << "Failed to list CL, rc = " << rc ;

   // Clear the environment
   rc = sdbDropDomain( db, pDomainName ) ;
   ASSERT_EQ( rc, SDB_OK ) << "Failed to clear domain in SDB, rc = " << rc ;

   bson_destroy( &domObj ) ;
   bson_destroy( &altObj ) ;
   bson_destroy( &condRG ) ;
   bson_destroy( &selectRG ) ;
   bson_destroy( &obj ) ;

   sdbReleaseDomain( dom ) ;
   sdbReleaseCursor ( cursor ) ;
   sdbDisconnect( db ) ;
   sdbReleaseConnection( db ) ;
}
