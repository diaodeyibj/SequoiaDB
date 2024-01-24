/************************************
*@Description: decimal data use $mod
*@author:      zhaoyu
*@createdate:  2016.5.3
**************************************/
main( test )
function test ()
{
   var clName = COMMCLNAME + "_7772";
   //clean environment before test
   commDropCL( db, COMMCSNAME, clName );

   //create cl
   var dbcl = commCreateCL( db, COMMCSNAME, clName );

   //insert decimal data
   var doc = [{ a: { $decimal: "-9223372036854775808456", $precision: [1000, 2] } },
   { a: { $decimal: "9223372036854775807123" } },
   { a: { $decimal: "-1.71E-309" } },
   { a: { $decimal: "1.72E-310", $precision: [1000, 320] } },
   { a: "123" },
   { a: { $decimal: "MAX" } },
   { a: { $decimal: "MIN" } },
   { a: { $decimal: "NAN" } },
   { a: { $decimal: "-INF" } },
   { a: { $decimal: "INF" } }];
   dbcl.insert( doc );

   //check result
   var expRecs0 = [{ a: { $decimal: "-456.00" } },
   { a: { $decimal: "2147482771" } },
   { a: { $decimal: "-0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000171" } },
   { a: { $decimal: "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000017200000000" } },
   { a: null },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } }];
   checkResult( dbcl, {}, { a: { $mod: { $decimal: "-2147483648" } } }, expRecs0, { _id: 1 } );

   var expRecs1 = [{ a: { $decimal: "-1333.00" } },
   { a: { $decimal: "0.00" } },
   { a: { $decimal: "-0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000171" } },
   { a: { $decimal: "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000017200000000" } },
   { a: null },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } }];
   checkResult( dbcl, {}, { a: { $mod: { $decimal: "9223372036854775807123", $precision: [100, 2] } } }, expRecs1, { _id: 1 } );

   var expRecs2 = [{ a: { $decimal: "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005" } },
   { a: { $decimal: "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000025" } },
   { a: { $decimal: "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000016" } },
   { a: { $decimal: "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000021" } },
   { a: null },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } }];
   checkResult( dbcl, {}, { a: { $mod: { $decimal: "5.9E-324" } } }, expRecs2, { _id: 1 } );

   var expRecs3 = [{ a: { $decimal: "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005" } },
   { a: { $decimal: "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000025" } },
   { a: { $decimal: "-0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000016" } },
   { a: { $decimal: "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000021" } },
   { a: null },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } },
   { a: { $decimal: "NaN" } }];
   checkResult( dbcl, {}, { a: { $mod: { $decimal: "-5.9E-324" } } }, expRecs3, { _id: 1 } );

   //mod 0,check result
   InvalidArgCheck( dbcl, {}, { a: { $mod: { $decimal: "0" } } }, -6 );
   commDropCL( db, COMMCSNAME, clName );
}
