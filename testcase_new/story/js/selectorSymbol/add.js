/************************************
*@Description: use add:{field:{$add:xx}}
               1.one or two fields cover all type of numberic data,include numberic and Non-numberic;seqDB-8221
               2.add positive int;seqDB-5645/seqDB-5648/seqDB-5652
               3.add positive/negative float;seqDB-5646
               4.add negative int;seqDB-5647
               5.cover all type of Non-numberic;seqDB-5649
               6.field is Non-existent;seqDB-5651
               7.add Non-numberic,seqDB-5650
               8.add numberLong and decimal
*@author:      zhaoyu
*@createdate:  2016.7.15
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
   var doc = [{ a: -2147483648, b: -2147483648, c: -123, d: -123 },
   { a: 2147483647, b: 2147483647, c: 123, d: 123 },
   { a: { $numberLong: "-9223372036854775808" }, b: { $numberLong: "-9223372036854775808" }, c: { $numberLong: "-922337203685477" }, d: { $numberLong: "-922337203685477" } },
   { a: { $numberLong: "9223372036854775807" }, b: { $numberLong: "9223372036854775807" }, c: { $numberLong: "922337203685477" }, d: { $numberLong: "922337203685477" } },
   { a: -1.7E+308, b: -1.7E+308, c: -2113, d: -2113, e: -1.7E+308, f: -1.7E+308 },
   { a: -4.9E-324, b: -4.9E-324, c: 21.13, d: 21.13 },
   { a: 4.9E-324, b: 4.9E-324, c: -21.13, d: -21.13 },
   { a: 1.7E+308, b: 1.7E+308, c: 2113, d: 2113, e: 1.7E+308, f: 1.7E+308 },
   { a: { $decimal: "-1.7E+308" }, b: { $decimal: "-1.7E+308" }, c: { $decimal: "-2113" }, d: { $decimal: "-2113" } },
   { a: { $decimal: "-4.9E-324" }, b: { $decimal: "-4.9E-324" }, c: { $decimal: "-21.13" }, d: { $decimal: "-21.13" } },
   { a: { $decimal: "1.7E+308" }, b: { $decimal: "1.7E+308" }, c: { $decimal: "2113" }, d: { $decimal: "2113" } },
   { a: { $decimal: "4.9E-324" }, b: { $decimal: "4.9E-324" }, c: { $decimal: "21.13" }, d: { $decimal: "21.13" } },
   { a: 0 },
   { a: { $oid: "123abcd00ef12358902300ef" } },
   { a: { $date: "1900-01-01" } },
   { a: { $timestamp: "1902-01-01-00.00.00.000000" } },
   { a: { $binary: "aGVsbG8gd29ybGQ=", "$type": "1" } },
   { a: { $regex: "^z", "$options": "i" } },
   { a: { subobj: "value" } },
   { a: ["abc", 0, "def"] },
   { a: null },
   { a: "abc" },
   { a: MinKey() },
   { a: MaxKey() }];
   dbcl.insert( doc );

	/*1.one or two fields cover all type of numberic data,include numberic and Non-numberic;seqDB-8221
	  2.add positive int;seqDB-5645/seqDB-5648/seqDB-5652
	  3.add positive/negative float;seqDB-5646
	  4.add negative int;seqDB-5647
	  5.cover all type of Non-numberic;seqDB-5649
	  6.field is Non-existent;seqDB-5651*/
   var selectCondition1 = { a: { $add: 1 }, b: { $add: -1 }, c: { $add: 12.31 }, d: { $add: -12.31 }, e: { $add: 1.7E+308 }, f: { $add: -1.7E+308 }, g: { $add: 1 } };
   var expRecs1 = [{ a: -2147483647, b: -2147483649, c: -110.69, d: -135.31 },
   { a: 2147483648, b: 2147483646, c: 135.31, d: 110.69 },
   { a: { $numberLong: "-9223372036854775807" }, b: { "$decimal": "-9223372036854775809" }, c: -922337203685464.69, d: -922337203685489.31 },
   { a: { "$decimal": "9223372036854775808" }, b: { $numberLong: "9223372036854775806" }, c: 922337203685489.31, d: 922337203685464.69 },
   { a: -1.7E+308, b: -1.7E+308, c: -2100.69, d: -2125.31, e: -0, f: -Infinity },
   { a: 1, b: -1, c: 33.44, d: 8.819999999999999 },
   { a: 1, b: -1, c: -8.819999999999999, d: -33.44 },
   { a: 1.7E+308, b: 1.7E+308, c: 2125.31, d: 2100.69, e: Infinity, f: 0 },
   {
      a: { $decimal: "-169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999" },
      b: { $decimal: "-170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" },
      c: { $decimal: "-2100.69" },
      d: { $decimal: "-2125.31" }
   },
   {
      a: { $decimal: "0.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951" },
      b: { $decimal: "-1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049" },
      c: { $decimal: "-8.82" },
      d: { $decimal: "-33.44" }
   },
   {
      a: { $decimal: "170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" },
      b: { $decimal: "169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999" },
      c: { $decimal: "2125.31" },
      d: { $decimal: "2100.69" }
   },
   {
      a: { $decimal: "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049" },
      b: { $decimal: "-0.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951" },
      c: { $decimal: "33.44" },
      d: { $decimal: "8.82" }
   },
   { a: 1 },
   { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: [null, 1, null] }, { a: null }, { a: null }, { a: null }, { a: null }];
   checkResult( dbcl, null, selectCondition1, expRecs1, { _id: 1 } );

   var selectCondition2 = { a: { $add: { $numberLong: "922337203685477000" } }, b: { $add: { $numberLong: "-922337203685477000" } } };
   var expRecs2 = [{ a: { $numberLong: "922337201537993352" }, b: { $numberLong: "-922337205832960648" }, c: -123, d: -123 },
   { a: { $numberLong: "922337205832960647" }, b: { $numberLong: "-922337201537993353" }, c: 123, d: 123 },
   { a: { $numberLong: "-8301034833169298808" }, b: { "$decimal": "-10145709240540252808" }, c: -922337203685477, d: -922337203685477 },
   { a: { "$decimal": "10145709240540252807" }, b: { $numberLong: "8301034833169298807" }, c: 922337203685477, d: 922337203685477 },
   { a: -1.7E+308, b: -1.7E+308, c: -2113, d: -2113, e: -1.7E+308, f: -1.7E+308 },
   { a: 922337203685477000, b: -922337203685477000, c: 21.13, d: 21.13 },
   { a: 922337203685477000, b: -922337203685477000, c: -21.13, d: -21.13 },
   { a: 1.7E+308, b: 1.7E+308, c: 2113, d: 2113, e: 1.7E+308, f: 1.7E+308 },
   {
      a: { $decimal: "-169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000" },
      b: { $decimal: "-170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000" },
      c: { $decimal: "-2113" }, d: { $decimal: "-2113" }
   },
   {
      a: { $decimal: "922337203685476999.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951" },
      b: { $decimal: "-922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049" },
      c: { $decimal: "-21.13" }, d: { $decimal: "-21.13" }
   },
   {
      a: { $decimal: "170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000" },
      b: { $decimal: "169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000" },
      c: { $decimal: "2113" }, d: { $decimal: "2113" }
   },
   {
      a: { $decimal: "922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049" },
      b: { $decimal: "-922337203685476999.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951" },
      c: { $decimal: "21.13" }, d: { $decimal: "21.13" }
   },
   { a: { $numberLong: "922337203685477000" } },
   { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: [null, { $numberLong: "922337203685477000" }, null] }, { a: null }, { a: null }, { a: null }, { a: null }];
   checkResult( dbcl, null, selectCondition2, expRecs2, { _id: 1 } );

   var selectCondition3 = { a: { $add: { $decimal: "922337203685477000" } }, b: { $add: { $decimal: "-922337203685477000", $precision: [1000, 400] } } };
   var expRecs3 = [{ a: { $decimal: "922337201537993352" }, b: { $decimal: "-922337205832960648.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" }, c: -123, d: -123 },
   { a: { $decimal: "922337205832960647" }, b: { $decimal: "-922337201537993353.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" }, c: 123, d: 123 },
   { a: { $decimal: "-8301034833169298808" }, b: { $decimal: "-10145709240540252808.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" }, c: -922337203685477, d: -922337203685477 },
   { a: { $decimal: "10145709240540252807" }, b: { $decimal: "8301034833169298807.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" }, c: 922337203685477, d: 922337203685477 },
   {
      a: { $decimal: "-169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000" },
      b: { $decimal: "-170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: -2113, d: -2113, e: -1.7E+308, f: -1.7E+308
   },
   {
      a: { $decimal: "922337203685476999.99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999505934354158753" },
      b: { $decimal: "-922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049406564584124700000000000000000000000000000000000000000000000000000000000000" },
      c: 21.13, d: 21.13
   },
   {
      a: { $decimal: "922337203685477000.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000494065645841247" },
      b: { $decimal: "-922337203685476999.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999950593435415875300000000000000000000000000000000000000000000000000000000000000" },
      c: -21.13, d: -21.13
   },
   {
      a: { $decimal: "170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000" },
      b: { $decimal: "169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: 2113, d: 2113, e: 1.7E+308, f: 1.7E+308
   },
   {
      a: { $decimal: "-169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000" },
      b: { $decimal: "-170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: { $decimal: "-2113" }, d: { $decimal: "-2113" }
   },
   {
      a: { $decimal: "922337203685476999.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951" },
      b: { $decimal: "-922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: { $decimal: "-21.13" }, d: { $decimal: "-21.13" }
   },
   {
      a: { $decimal: "170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000922337203685477000" },
      b: { $decimal: "169999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999077662796314523000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: { $decimal: "2113" }, d: { $decimal: "2113" }
   },
   {
      a: { $decimal: "922337203685477000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000049" },
      b: { $decimal: "-922337203685476999.9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999951000000000000000000000000000000000000000000000000000000000000000000000000000" },
      c: { $decimal: "21.13" }, d: { $decimal: "21.13" }
   },
   { a: { $decimal: "922337203685477000" } },
   { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: null }, { a: [null, { $decimal: "922337203685477000" }, null] }, { a: null }, { a: null }, { a: null }, { a: null }];
   checkResult( dbcl, null, selectCondition3, expRecs3, { _id: 1 } );
   //add Non-numberic,seqDB-5650
   var selectCondition4 = { a: { $add: "a" } };
   InvalidArgCheck( dbcl, null, selectCondition4, -6 );
}