/***************************************************************************
 * 
 * Copyright (c) 2008 Baidu.com, Inc. All Rights Reserved
 * $Id: ts_sock.h,v 1.1 2008/08/22 04:04:06 yanghy2 Exp $ 
 * 
 **************************************************************************/



/**
 * @file tinyse_interface.h
 * @author yanghy(yanghuaiyuan@baidu.com)
 * @date 2008/06/20 09:56:28
 * @version $Revision: 1.1 $ 
 * @brief socket接口
 *  
 **/


#ifndef  __TS_SOCK_H_
#define  __TS_SOCK_H_



/**
* @brief 查询类命令号
*/
enum _ts_query_cmd_t
{
    qcAll = 100001,     ///< 检索全部数据
    qcDel = 100002,     ///< 检索已删除数据
    qcUndel = 100003,   ///< 检索未删除数据

};
/**
* @brief 提交类命令号
*/
enum _ts_commit_cmd_t
{
    cmDataAdd = 10001,      ///< 添加数据
    cmDataMod = 10002,      ///< 修改数据
    cmDataModBasic = 10003, ///< 仅修改基本数据，不影响索引和di
    cmDataDel = 10004,      ///< 批量删除
    cmDataUndel = 10005,    ///< 批量反删除
    cmDataRebuild = 10006   ///< 用于命令回放等重建索引的命令
};

/**
* @brief 错误号
*/
enum _ts_errinfo_t
{
    SUCCESS = 0,        ///< 成功
    errNet = 10,        ///< 网络io错误
    errCmdno = 11,      ///< 命令号错误
    errIllegalReq = 12, ///< 请求的结构体数据有问题
    errConnect = 13,    ///< 连接服务错误
    errRWTimeout = 14,  ///< 读写数据超时
    errNoPower = 15,    ///< 无权限
    errNoExist = 16,    ///< 数据不存在 
    errDataExist = 17,  ///< 该数据已存在
    errDataType = 18,   ///< 该数据类型不正确
    errDataWrong = 19,  ///< 发送数据存在问题: 重复/删除/不存在/错误/范围不对..
    errInternal = 20    ///< 内部错误，如文件io错误.(读/写)
};




/**
* @brief tinyse统一的命令接口
*   
*   +-------+------------------------+-------------------+
*   |nshead | 定长部分 ts_head_t     |  变长部分mcpack   |
*   +-------+------------------------+-------------------+
*/
//typedef struct ts_head_t
//{
    //uint32 cmd_no;                         /**< 命令号     */
    //uint32 err_no;                         /**< 错误号     */
    //char data_type[TS_TYPENAME_MAXLEN];    /**< 数据类型      */

    //uint32 page_no;                        /**< 页号      */
    //uint32 num_per_page;                   /**< 每页显示条数       */

    //uint32 ret_num;                        /**< 返回个数      */
    //uint32 disp_num;                       /**< 总共查询到的个数      */


//} ts_head_t;










#endif //__TS_SOCK_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
