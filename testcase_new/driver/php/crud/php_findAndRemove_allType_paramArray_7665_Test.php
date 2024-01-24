/****************************************************
@description:      findAndRemove, cover all data type
@testlink cases:   seqDB-7664
@input:        1 test_createCL
               2 insert, cover all data type:
                  $null, $int32, $double, $string, $bool, $subobj, $array, 
                  $oid, $int64, $date, $binary, $regex, $timestamp, $minKey, $maxKey
               3 findAndRemove, with condition cover all data type, $condition/$selector/$orderby/$hint: array
               5 count, cover all parameter, $condition/$hint: array
               6 dropCL
@output:     success
@modify list:
        2016-4-22 XiaoNi Huang init
****************************************************/
<?php

include_once dirname(__FILE__).'/../func.php';

class DataOperator04 extends BaseOperator 
{  
   public function __construct()
   {
      parent::__construct();
   }
   
   function getErrno()
   {
      $this -> err = $this -> db -> getError();
      return $this -> err['errno'];
   }
   
   function createCL( $csName, $clName )
   {
      $options = null;
      return $this -> commCreateCL( $csName, $clName, $options, true );
   }
   
   function createIndex( $clDB, $idxName )
   {
      $indexDef = array( 'a' => 1 );
      $clDB -> createIndex( $indexDef, $idxName );
   }
   
   function insertRecs( $clDB )
   {
      //insert count: 18
      //nomal data type, count: 7
      $null   = array( 'a' => 0,  'b' => null, 'c' => 'null' );
      $int32  = array( 'a' => 1,  'b' => -2147483648, 'c' => 'int32' );
      $double = array( 'a' => 2,  'b' => -1.7E+308, 'c' => 'double' );
      $string = array( 'a' => 3,  'b' => 'test', 'c' => 'string' );
      $bool   = array( 'a' => 4,  'b' => true, 'c' => 'bool' );
      $array  = array( 'a' => 5,  'b' => array( array( 'b1' => 1 ) ), 'c' => 'array' );
      //special data type, count: 8
      $oid    = array( 'a' => 6,  'b' => new SequoiaID( '123abcd00ef12358902300ef' ), 'c' => 'oid' );
      $int64  = array( 'a' => 7,  'b' => new SequoiaINT64( "-9223372036854775808" ), 'c' => 'int64' );
      $date   = array( 'a' => 8,  'b' => new SequoiaDate( '1991-11-27' ), 'c' => 'date' );
      $binary = array( 'a' => 9,  'b' => new SequoiaBinary( 'aGVsbG8gd29ybGQ=', '1' ), 'c' => 'binary' );
      $regex  = array( 'a' => 10, 'b' => new SequoiaRegex( '^rg', 'i' ), 'c' => 'regex' );
      $timestamp = array( 'a' => 11, 'b' => new SequoiaTimestamp( '1901-12-15-00.00.00.000000' ), 'c' => 'timestamp' );
      $minKey = array( 'a' => 12, 'b' => new SequoiaMinKey(), 'c' => 'minKey' );
      $maxKey = array( 'a' => 13, 'b' => new SequoiaMaxKey(), 'c' => 'maxKey' );
      $subobj = array( 'a' => 14, 'b' => array( 'subobj' => "111" ), 'c' => 'subobj' );
      //others for find by condition, count: 3
      $tmp1 = array( 'a' => -1, 'c' => 'tmp1' );
      $tmp2 = array( 'a' => 15, 'c' => 'tmp2' );
      $tmp3 = array( 'a' => 16 );
      
      $recsArray = array( $null, $int32, $double, $string, $bool, $array, 
                          $oid, $int64, $date, $binary, $regex, $timestamp, $minKey, $maxKey, 
                          $subobj, 
                          $tmp1, $tmp2, $tmp3 );
      
      for( $i = 0; $i < count( $recsArray ); $i++ )
      {
         $clDB -> insert( $recsArray[$i] );
      }
      
      return $recsArray;
   }
   
   function findAndRemove( $clDB, $recsArray, $idxName )
   {
      //$condition, see below
      $selector  = array( 'a' => '', 'b' => '', 'c' => '', 'd' => 'hello' );
      $orderby   = array( 'a' => 1 );
      $hint      = array( '' => $idxName );
      $numToSkip = 0;
      $numToReturn = 15;
      $flag = 0;
      
      $updateReturnArray = array();
      for( $i = 0; $i < count( $recsArray ) - 3 ; $i++ )  //filter 3 records after find
      {
         if( $i === 10 ) //regex, match regex bson, must add $et
         {
            $condition = array( 'b' => array( '$et' => $recsArray[$i]['b'] ) ) ;
         }
         else if( $i < 12 || $i > 13 ) //last record is minKey/maxKey, can not match the records to remove 
         {
            $condition = array( 'b' => $recsArray[$i]['b'] );
         }
         else if( $i === 12 || $i === 13 ) //minKey maxKey
         {
            $condition = array( 'a' => $recsArray[$i]['a'] );
         }
         
         $cursor = $clDB -> findAndRemove( $condition, $selector, $orderby, $hint, 
                                           $numToSkip, $numToReturn, $flag );
         $errno = $this -> getErrno();
         if( $errno !== 0 )
         {
            echo "\nFailed to findAndRemove. Errno: ". $errno ."\n";
         }
         $conditionNum = 0 ;
         while( $tmpInfo = $cursor -> next() )
         {
            array_push( $updateReturnArray, $tmpInfo );
            $conditionNum++ ;
         }
         if( $conditionNum == 0 )
         {
            echo "\nfindAndRemove no match record. condition: ". json_encode( $condition ) ."\n";
         }
      }
      return $updateReturnArray;
   }
   
   function countRecs( $clDB, $idxName )
   {
      $condition1 = array( 'a' => array( '$lte' => 0 ) );
      $condition2 = array( '$and' => 
                    array( array( 'a' => array( '$gte' => 1 ) ), array( 'a' => array( '$lte' => 14 ) ) ) );
      $condition3 = array( 'a' => array( '$gte' => 15 ) );
      
      $hint      = array( '' => $idxName );
      
      $cnt1 = $clDB -> count( $condition1, $hint );
      $cnt2 = $clDB -> count( $condition2, $hint );
      $cnt3 = $clDB -> count( $condition3, $hint );
      $errno = $this -> getErrno();
      if( $errno !== 0 )
      {
         echo "\nFailed to count. Errno: ". $errno ."\n";
      }
      
      $cntArray = array( $cnt1, $cnt2, $cnt3 );
      return $cntArray;
   }
   
   function dropCL( $csName, $clName,$ignoreNotExist )
   {
      $this -> commDropCL( $csName, $clName, $ignoreNotExist );
   }
   
}

class TestData04 extends PHPUnit_Framework_TestCase
{
   protected static $dbh;
   private static $csName;
   private static $clName;
   private static $idxName;
   private static $clDB;
   private static $rawData;
   private static $beginTime;
   private static $endTime;
   
   public static function setUpBeforeClass()
   {
      date_default_timezone_set("Asia/Shanghai");
      self::$beginTime = microtime( true );
      echo "\n---Begin time: " . date( "Y-m-d H:i:s", self::$beginTime ) ."\n";
   
      self::$dbh = new DataOperator04();
      
      echo "\n---Begin to ready parameter.\n";
      self::$csName  = self::$dbh -> COMMCSNAME;
      self::$clName  = self::$dbh -> COMMCLNAME . '_7665_01';
      self::$idxName = self::$dbh -> CHANGEDPREFIX .'_index';
      
      echo "\n---Begin to drop cl in the begin.\n";
      self::$dbh -> dropCL( self::$csName, self::$clName, true );
      
      echo "\n---Begin to create cl.\n";
      self::$clDB = self::$dbh -> createCL( self::$csName, self::$clName );
   }
   
   function test_createIndex()
   {
      echo "\n---Begin to create index.\n";
      
      self::$rawData = self::$dbh -> createIndex( self::$clDB, self::$idxName );  //raw data
      
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
   
   function test_insert()
   {
      echo "\n---Begin to insert records.\n";
      
      self::$rawData = self::$dbh -> insertRecs( self::$clDB );  //raw data
      
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
   
   function test_findAndRemove()
   {
      echo "\n---Begin to findAndUpdata.\n";
      
      $removeReturnArray = self::$dbh -> findAndRemove( self::$clDB, self::$rawData, self::$idxName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( -29, $errno );
      
      //compare return count
      $this -> assertCount( 15, $removeReturnArray );
      
      //compare for $selector
      $this -> assertEquals( 'hello', $removeReturnArray[0]['d'] );
      $this -> assertEquals( 'hello', $removeReturnArray[14]['d'] );
   }
   
   function test_count()
   {
      echo "\n---Begin to count after findAndRemove.\n";
      
      $count = self::$dbh -> countRecs( self::$clDB, self::$idxName );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
      
      //compare results for findAndRemove
      $this -> assertEquals( 1, $count[0] ); //a <= 0
      $this -> assertEquals( 0, $count[1] ); //1 <= a && a >= 14
      $this -> assertEquals( 2, $count[2] ); //a >= 15
   }
   
   function test_dropCL()
   {
      echo "\n---Begin to drop cl in the end.\n";
      
      self::$dbh -> dropCL( self::$csName, self::$clName, false );
      $errno = self::$dbh -> getErrno();
      $this -> assertEquals( 0, $errno );
   }
   
   public static function tearDownAfterClass()
   {
      self::$endTime = microtime( true );
      echo "\n---End the Test,End time: " . date( "Y-m-d H:i:s", self::$endTime ) . "\n";
      echo "\n---Test 7665_01 spend time: " . ( self::$endTime - self::$beginTime ) . " seconds.\n";
   }
   
}
?>