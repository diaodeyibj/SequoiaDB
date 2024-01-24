﻿/************************************
*@Description: 指定group将统计信息重新加载至缓存再清空 
*@author:      zhaoyu
*@createdate:  2017.11.14
*@testlinkCase:seqDB-11633
**************************************/
main( test );
function test ()
{
   //判断独立模式
   if( true == commIsStandalone( db ) )
   {
      return;
   }

   var clName = COMMCLNAME + "_11633";
   var clFullName = COMMCSNAME + "." + clName;
   var insertNum = 2000;
   var sameValues = 9000;

   var findConf = { a: sameValues };
   var expAccessPlan1 = [{ ScanType: "tbscan", IndexName: "" }];
   var expAccessPlan2 = [{ ScanType: "ixscan", IndexName: "a" }];
   var expAccessPlan3 = [];

   //清理环境
   commDropCL( db, COMMCSNAME, clName, true, true, "drop CL in the beginning" );

   //创建cl
   var dbcl = commCreateCL( db, COMMCSNAME, clName );

   //创建索引
   commCreateIndex( dbcl, "a", { a: 1 } );

   //插入记录
   insertDiffDatas( dbcl, insertNum );
   insertSameDatas( dbcl, insertNum, sameValues );

   //获取主备节点
   var db1 = new Sdb( db );
   db1.setSessionAttr( { PreferedInstance: "m" } );
   var dbclPrimary = db1.getCS( COMMCSNAME ).getCL( clName );

   //执行统计
   db.analyze( { Collection: COMMCSNAME + "." + clName } );

   //检查主备同步
   checkConsistency( db, COMMCSNAME, clName );

   //检查统计信息
   checkStat( db, COMMCSNAME, clName, "a", true, true );

   //执行查询
   query( dbclPrimary, findConf, null, null, insertNum );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan1, actAccessPlan );

   //生成默认统计信息
   var groupName = getSrcGroup( COMMCSNAME, clName );
   db.analyze( { Mode: 3, Collection: COMMCSNAME + "." + clName, GroupName: groupName } );

   //检查主备同步
   checkConsistency( db, COMMCSNAME, clName );

   //检查统计信息
   checkStat( db, COMMCSNAME, clName, "a", true, false );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan3, actAccessPlan );

   //执行查询
   query( dbclPrimary, findConf, null, null, insertNum );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan2, actAccessPlan );

   //手工修改主节点统计信息
   var mcvValues = [{ a: 8000 }, { a: sameValues }, { a: 9001 }];
   var fracs = [500, 9000, 500];
   updateIndexStateInfo( db, COMMCSNAME, clName, "a", mcvValues, fracs );

   //统计信息加载至缓存
   db.analyze( { Mode: 4, GroupName: groupName } );

   //检查主备同步
   checkConsistency( db, COMMCSNAME, clName );

   //检查统计信息
   checkStat( db, COMMCSNAME, clName, "a", true, true );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan3, actAccessPlan );

   //执行查询
   query( dbclPrimary, findConf, null, null, insertNum );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan1, actAccessPlan );

   //再次更新统计信息
   var mcvValues = [{ a: 8000 }, { a: sameValues }, { a: 9001 }];
   var fracs = [500, 100, 9400];
   updateIndexStateInfo( db, COMMCSNAME, clName, "a", mcvValues, fracs );

   //检查主备同步
   checkConsistency( db, COMMCSNAME, clName );

   //检查统计信息
   checkStat( db, COMMCSNAME, clName, "a", true, true );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan1, actAccessPlan );

   //执行查询
   query( dbclPrimary, findConf, null, null, insertNum );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan1, actAccessPlan );

   //再次清空缓存
   db.analyze( { Mode: 5, GroupName: groupName } );

   //检查主备同步
   checkConsistency( db, COMMCSNAME, clName );

   //检查统计信息
   checkStat( db, COMMCSNAME, clName, "a", true, true );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan3, actAccessPlan );

   //执行查询
   query( dbclPrimary, findConf, null, null, insertNum );

   //检查访问计划快照
   var actAccessPlan = getCommonAccessPlans( db, { Collection: clFullName } );
   checkSnapShotAccessPlans( clFullName, expAccessPlan2, actAccessPlan );

   //清理环境
   commDropCL( db, COMMCSNAME, clName, true, true, "drop CL in the end" );
   db1.close();
}