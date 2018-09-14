#ifndef _KLICEN_UTILS_SINGLETON_H_
#define _KLICEN_UTILS_SINGLETON_H_

#include <mutex>
#include <boost/noncopyable.hpp>


		template <typename T>
		class Singleton : private boost::noncopyable
		{
		public:
			static T* GetInstance()
			{
				// double check
				if (nullptr == instance_)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					if (nullptr == instance_)
					{
						instance_ = new T;
					}
				}
				return instance_;
			}
		private:
			Singleton();
			static T* instance_;
			static std::mutex mutex_;
		};
		template <typename T> T* Singleton<T>::instance_ = nullptr;
		template <typename T> std::mutex Singleton<T>::mutex_;
#define DEFINE_SINGLETON_DEFAULT(class_name); \
	public: \
	friend class klicen::utils::Singleton<class_name>; \
	typedef klicen::utils::Singleton<class_name> singleton; \
	private: \
	class_name() {} \
	virtual ~class_name() {} \
	class_name(const class_name&){} \
	class_name& operator=(const class_name&){} 

		template <typename T>
		class SingletonConstruct : private boost::noncopyable
		{
		public:
			static T* GetInstance()
			{
				/*if (nullptr == instance_)
				{
				std::lock_guard<std::mutex> lock(mutex_);
				if (nullptr == instance_)
				{
				throw std::exception("Get singleton failed");
				}
				}*/
				return instance_;
			}
			template <typename... Args>
			static void Create(Args&&... args)
			{
				if (nullptr == instance_)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					if (nullptr == instance_)
					{
						instance_ = new T(std::forward<Args>(args)...);
					}
				}
			}
		private:
			SingletonConstruct();
			static T* instance_;
			static std::mutex mutex_;
		};
		template <typename T> T* SingletonConstruct<T>::instance_ = nullptr;
		template <typename T> std::mutex SingletonConstruct<T>::mutex_;
#define DEFINE_SINGLETON_CONSTRUCT(class_name); \
	public: \
	friend class klicen::utils::SingletonConstruct<class_name>; \
	typedef klicen::utils::SingletonConstruct<class_name> singleton; \
	private: \
	virtual ~class_name() {} \
	class_name& operator=(const class_name&){} 

		template <typename T>
		class SingletonConstructNoParam : private boost::noncopyable
		{
		public:
			static T* GetInstance()
			{
				if (nullptr == instance_)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					if (nullptr == instance_)
					{
						instance_ = new T;
					}
				}
				return instance_;
			}
		private:
			SingletonConstructNoParam();
			static T* instance_;
			static std::mutex mutex_;
		};
		template <typename T> T* SingletonConstructNoParam<T>::instance_ = nullptr;
		template <typename T> std::mutex SingletonConstructNoParam<T>::mutex_;
#define DEFINE_SINGLETON_CONSTRUCT_NO_PARAM(class_name); \
	public: \
	friend class klicen::utils::SingletonConstructNoParam<class_name>; \
	typedef klicen::utils::SingletonConstructNoParam<class_name> singleton; \
	private: \
	class_name(); \
	virtual ~class_name() {} \
	class_name& operator=(const class_name&){} 

		template <typename T>
		class SingletonConstructWithDefault : private boost::noncopyable
		{
		public:
			static T* GetInstance()
			{
				if (nullptr == instance_)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					if (nullptr == instance_)
					{
						instance_ = new T;
					}
				}
				return instance_;
			}
			template <typename... Args>
			static void Create(Args&&... args)
			{
				if (nullptr == instance_)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					if (nullptr == instance_)
					{
						instance_ = new T(std::forward<Args>(args)...);
					}
				}
			}
		private:
			SingletonConstructWithDefault();
			static T* instance_;
			static std::mutex mutex_;
		};
		template <typename T> T* SingletonConstructWithDefault<T>::instance_ = nullptr;
		template <typename T> std::mutex SingletonConstructWithDefault<T>::mutex_;
#define DEFINE_SINGLETON_CONSTRUCT_WITH_DEFAULT(class_name); \
	public: \
	friend class SingletonConstructWithDefault<class_name>; \
	typedef SingletonConstructWithDefault<class_name> singleton; \
	private: \
	virtual ~class_name() {} \
	class_name& operator=(const class_name&){} 


#endif // _KLICEN_UTILS_SINGLETON_H_
