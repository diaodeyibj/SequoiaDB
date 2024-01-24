/***************************************************************************************************
 * @Description: 删除最后一个用户
 * @ATCaseID: dropUsr_at_1
 * @Author: Zhou Hongye
 * @TestlinkCase: 无
 * @Change    Activity:
 * Date       Who         Description
 * ========== =========== =========================================================
 * 08/04/2023 Zhou Hongye Test drop user
 **************************************************************************************************/

/*********************************************测试用例***********************************************
 * 环境准备：集群环境
 * 测试场景：删除最后一个非_root用户；删除最后一个_root用户
 * 测试步骤：
 *  1. 集群中创建非_root用户
 *  2. 删除该非_root用户
 *  3. 集群中创建_root用户
 *  4. 删除该_root用户
 * 期望结果：
 *  删除最后一个非_root用户成功；删除最后一个_root用户成功
 **************************************************************************************************/
main(test);
function test() {
   var user1 = "test_user1"
   var user_root = "test_user_root"
   var pwd = "123";
   db.createUsr(user1, pwd);
   db.dropUsr(user1, pwd);
   db.createUsr(user_root, pwd, { Roles: ["_root"] });
   db.dropUsr(user_root, pwd);
}