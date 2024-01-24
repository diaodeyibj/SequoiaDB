/************************************
*@Description: decimal data use addtoset
*@author:      zhaoyu
*@createdate:  2016.4.27
**************************************/
main( test )
function test ()
{
   var clName = COMMCLNAME + "_7757";
   //clean environment before test
   commDropCL( db, COMMCSNAME, clName );

   //create cl
   var dbcl = commCreateCL( db, COMMCSNAME, clName );

   //insert data
   var doc = [{
      a: [{ $decimal: "1" },
      { $decimal: "5" },
      { $decimal: "10", $precision: [10, 2] },
      { $decimal: "1024", $precision: [10, 2] },
      { $decimal: "2048", $precision: [10, 2] },
      { $decimal: "2049", $precision: [10, 2] },
         1000,
         100.75]
   }];
   dbcl.insert( doc );

   //check result
   var expRecs = [{
      a: [{ $decimal: "1" },
      { $decimal: "5" },
      { $decimal: "10.00", $precision: [10, 2] },
      { $decimal: "1024.00", $precision: [10, 2] },
      { $decimal: "2048.00", $precision: [10, 2] },
      { $decimal: "2049.00", $precision: [10, 2] },
         1000,
         100.75,
      { $decimal: "123" },
      { $decimal: "897.00", $precision: [5, 2] }]
   }];

   //update use operator:addtoset
   var operator = {
      $addtoset: {
         a: [{ $decimal: "1" },
         { $decimal: "5", $precision: [10, 2] },
         { $decimal: "10" },
         { $decimal: "1024", $precision: [8, 3] },
         { $decimal: "2048", $precision: [10, 2] },
            2049,
         { $decimal: "1000", $precision: [6, 2] },
         { $decimal: "123" },
         { $decimal: "897", $precision: [5, 2] }]
      }
   }
   dbcl.update( operator );

   //check result
   checkResult( dbcl, {}, {}, expRecs, { _id: 1 } );
   commDropCL( db, COMMCSNAME, clName );
}
