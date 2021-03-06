/***************************************************************************
 * 
 * 
 **************************************************************************/



/**
 * @file ConstraintFunction.h
 * @brief 
 *  
 **/


#ifndef  __CONSTRAINTFUNCTION_H_
#define  __CONSTRAINTFUNCTION_H_

#include <string>
#include <list>
#include <map>
#include "Configure.h"
#include "ConfigGroup.h"
#include "ConfigUnit.h"

namespace confIDL{
	struct cons_func_t;
	struct var_t;
	struct idl_t;
}

namespace comcfg{
	//typedef std::string str_t;
	typedef std::list <str_t> param_list_t;

	//Constraint function pointer
	typedef int (* CF_T)(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit*);

	enum CFLevel{
		CF_STRUCT = 0,	//最高级别的约束，必须最早执行，如array，约束某个字段的类型
		CF_KEY,			//字段约束，如default，约束某个字段的基本情况
		CF_TYPE,		//类型约束，int16, int32, int64...,也是一种值的约束(CF_VALUE)
		CF_VALUE,		//普通级别的约束，如ip(), length()等，约束某个字段值的内容
		CF_DEFAULT,
	};

	class ConstraintFunction{
		int _level;
		public:
			virtual int check(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup*, ConfigUnit*);
			void setLevel(int lv);
			int getLevel();
	};
	typedef std::map<str_t, ConstraintFunction*> CFMAP_T;

	//Constraint Library is Singleton
	class ConstraintLibrary{
		ConstraintLibrary();
		~ConstraintLibrary();
		static ConstraintLibrary * lib;
		public:
			/**
			 * @brief 获取Library的实例
			 *			【用户不用关心本接口】
			 * @return  ConstraintLibrary* 
			 * @retval   
			 * @see 
			**/
			static ConstraintLibrary* getInstance();
			/**
			 * @brief 获取某个函数的级别，对不存在的函数返回default
			 *			【用户不用关心本接口】
			 *
			 * @param [in] function   : const str_t&
			 * @return  int 
			 * @retval   
			 * @see 
			**/
			int getLevel(const str_t& function);
			static int getLevel_static(const str_t& function);
			/**
			 * @brief 根据函数检查一个内容 
			 * 			内容以父节点的形式传入（因为被检查的字段不一定存在，如default就是在父节点下新建子结点）
			 * 			【用户不用关心本接口】
			 *
			 * @param [in] function   : const str_t& 约束函数名
			 * @param [in]    	: const confIDL::var_t& 约束信息
			 * @param [in] 		: ConfigGroup * 传入的父节点
			 * @return  int 
			 * @retval   
			 * @see 
			**/
			int check(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup*, ConfigUnit*);
			/**
			 * @brief 注册一个约束函数 【用于用户的扩展】
			 *
			 * @param [in] fname   : const str_t& 约束函数的名字
			 * @param [in] cons   : const ConstraintFunction& 约束函数实体，相当于仿函数
			 * 			【注意】请new出来再传进去，然后ConstraintLibrary会delete之
			 * @param [in] overwrite   : bool 是否覆盖同名的函数
			 * @return  int 
			 * @retval   
			 * @see 
			**/
			int registConstraint(const str_t& fname, ConstraintFunction* cons, bool overwrite = false);

			int registConstraint(const str_t& fname, CF_T cf, bool overwrite = false){
				return registConstraint(fname, cf, CF_VALUE, overwrite);
			}
		private:
			friend class Constraint;
			friend class ConsUnitControl;
			int registConstraint(const str_t& fname, CF_T cf, int cfLevel, bool overwrite = false);
		protected:
			CFMAP_T cfMap;
			class WrapFunction : public ConstraintFunction{
				public:
					virtual int check(const confIDL::cons_func_t& cons, 
							const confIDL::var_t& param, ConfigGroup* father, ConfigUnit* self){
						return _cf(cons, param, father, self);
					}
					void setCF(CF_T cf){
						_cf = cf;
					}
					WrapFunction(){}
				protected:
					CF_T _cf;
			};
	};
}




#endif  //__CONSTRAINTFUNCTION_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
