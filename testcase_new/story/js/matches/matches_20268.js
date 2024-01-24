/******************************************************************************
@Description: [seqDB-20268] Query by $or with index;
              使用 $or 查询, 走索引查询
@Author: 2020/08/06 Zixian Yan
******************************************************************************/
testConf.clName = COMMCLNAME + "_20268";
main( test );

function test ( testPara )
{
   var cl = testPara.testCL;

   var data = [{ number: 7 }, { number: 5 }, { number: 3 }, { number: 6 },
   { number: 4 }, { number: 8 }, { number: 2 }, { number: 1 }]

   cl.insert( data );

   cl.createIndex( "numIndex", { number: 1 } );

   var rc1 = cl.find( { $or: [{ number: { $lt: 3 } }, { number: { $gt: 6 } }] } );
   var rc2 = cl.find( { $or: [{ number: { $lt: 3 } }, { number: { $gt: 6 } }] } ).sort( { number: 1 } );

   var expectationOne = [{ number: 7 },
   { number: 8 },
   { number: 2 },
   { number: 1 }];

   var expectationTwo = [{ number: 1 },
   { number: 2 },
   { number: 7 },
   { number: 8 }];

   checkRec( rc1, expectationOne );
   checkRec( rc2, expectationTwo );
}