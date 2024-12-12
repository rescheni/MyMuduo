#pragma once 





/*
 *  noncopyable 被 继承 后 派生类对象 可以正常的构造和 析构 但是 派生类对象无法进行构造和 赋值操作
 *
 * */

namespace mymuduo {

	// 可以正常的构造和 析构 但是 派生类对象无法进行构造和 赋值操作
	class noncopyable
	{
		public:
			noncopyable(const noncopyable&) = delete;
			noncopyable& operator=(const noncopyable&) = delete;
		protected:
			noncopyable() = default;
			~noncopyable() = default;
	};

}
