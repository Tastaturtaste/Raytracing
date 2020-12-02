#pragma once
#include<concepts>
#include<type_traits>
#include<functional>
#include<utility>

template<class F> class func_ref;

template<class R, class... Args>
class func_ref<R(Args...)> {
public:
	constexpr func_ref() = delete;
	constexpr func_ref(func_ref<R(Args...)>& rhs) = default;
	constexpr func_ref<R(Args...)>& operator=(func_ref<R(Args...)>& rhs) = default;

	template<class F>
	requires requires{
		std::same_as<std::invoke_result_t<F, Args...>, R>;
	}
	constexpr func_ref(F&& f) noexcept
		: obj_(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(f))))
	{
		callback_ = [](void* obj, Args... args) -> R {
			return std::invoke(
				*reinterpret_cast<typename std::add_pointer<F>::type>(obj),
				std::forward<Args>(args)...);
		};
	}
	R operator()(Args... args) const noexcept {
		return callback_(obj_, std::forward<Args>(args)...);
	}

private:
	void* obj_ = nullptr;
	R(*callback_)(void*, Args...) = nullptr;
};