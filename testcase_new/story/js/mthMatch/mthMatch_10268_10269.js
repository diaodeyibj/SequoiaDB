/************************************
*@Description: many fields use divide to match,
*@author:      zhaoyu
*@createdate:  2016.10.13
*@testlinkCase: 
**************************************/
main( test );
function test ()
{
   //clean environment before test
   commDropCL( db, COMMCSNAME, COMMCLNAME, true, true, "drop CL in the beginning" );

   //create cl
   var dbcl = commCreateCL( db, COMMCSNAME, COMMCLNAME );

   //insert data 
   var doc = [{ No: 1, a: 1.2, b: -2.5 },
   { No: 2, a: -1.2, b: -2.5 },
   { No: 3, a: [1.2, 2.5], b: [-1.2, -2.5] },
   { No: 4, a: [-1.2, -2.5], b: [1.2, 2.5] },
   { No: 5, a: [{ 0: 1.2 }, { 1: 2.5 }], b: [{ 0: -1.2 }, { 1: -2.5 }] },
   { No: 6, a: [{ 0: -1.2 }, { 1: -2.5 }], b: [{ 0: 1.2 }, { 1: 2.5 }] },
   { No: 7, a: { 0: 1.2, 1: 2.5 }, b: { 0: -1.2, 1: -2.5 } },
   { No: 8, a: { 0: -1.2, 1: -2.5 }, b: { 0: 1.2, 1: 2.5 } }];
   dbcl.insert( doc );

   var findCondition1 = { a: { $divide: 2, $et: 0.6 }, b: { $divide: -0.5, $et: 5 } };
   var expRecs1 = [{ No: 1, a: 1.2, b: -2.5 },
   { No: 3, a: [1.2, 2.5], b: [-1.2, -2.5] }];
   checkResult( dbcl, findCondition1, null, expRecs1, { _id: 1 } );

   var findCondition2 = { a: { $divide: 2, $et: 0.6 }, b: { $divide: -0.5, $et: 5 }, c: { $divide: 1, $et: 1 } };
   var expRecs2 = [];
   checkResult( dbcl, findCondition2, null, expRecs2, { _id: 1 } );

   var findCondition3 = { "a.0": { $divide: 2, $et: 0.6 }, "b.0": { $divide: -2, $et: 0.6 } };
   var expRecs3 = [{ No: 3, a: [1.2, 2.5], b: [-1.2, -2.5] },
   { No: 5, a: [{ 0: 1.2 }, { 1: 2.5 }], b: [{ 0: -1.2 }, { 1: -2.5 }] },
   { No: 7, a: { 0: 1.2, 1: 2.5 }, b: { 0: -1.2, 1: -2.5 } }];
   checkResult( dbcl, findCondition3, null, expRecs3, { _id: 1 } );

   var findCondition4 = { "a.0": { $divide: 2, $et: 0.6 }, "b.0": { $divide: -2, $et: 0.6 }, "c.0": { $divide: 1, $et: -1 } };
   var expRecs4 = [];
   checkResult( dbcl, findCondition4, null, expRecs4, { _id: 1 } );
}