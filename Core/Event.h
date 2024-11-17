#pragma once

#include "Core.h"

#include "Core/Reference.h"
#include "Util/VectorUtil.h"

// Delegate broadcaster

// Inspired by simoncoenen.com/blog/programming/CPP_Delegates

template<typename ReturnType, typename... ArgumentList>
class IDelegate
{
public:
	virtual ~IDelegate() = default;
	virtual ReturnType Execute(ArgumentList... Arguments) = 0;
	virtual const void* GetBoundObjectPointer() const = 0;
	virtual bool IsValid() const = 0;
};

// A delegate that binds to a raw pointer to the receiver.
// It is the responsibility of the binding object to unbind the delegate if the pointer is no longer valid.
template<typename ReceiverType, typename ReturnType, typename... ArgumentList>
class Delegate : public IDelegate<ReturnType, ArgumentList...>
{
public:
	using MemberFunctionType = ReturnType(ReceiverType::*)(ArgumentList...);

	Delegate(ReceiverType* NewReceiver, MemberFunctionType MemberFunctionToBind) :
		m_Receiver(NewReceiver),
		m_BoundFunction(MemberFunctionToBind)
	{}

	virtual ReturnType Execute(ArgumentList... Arguments) override
	{
		return (m_Receiver->*m_BoundFunction)(std::forward<ArgumentList>(Arguments)...);
	}

	virtual const void* GetBoundObjectPointer() const override
	{
		return static_cast<const void*>(m_Receiver);
	}

	virtual bool IsValid() const override
	{
		return m_Receiver != nullptr;
	}

private:
	ReceiverType* m_Receiver;
	MemberFunctionType m_BoundFunction;
};

// A delegate that holds a weak reference.
// This is safe to "bind and forget" since it will be cleaned up when the ref is invalidated.
template<typename ReceiverType, typename ReturnType, typename... ArgumentList>
class WeakRefDelegate : public IDelegate<ReturnType, ArgumentList...>
{
public:
	using MemberFunctionType = ReturnType(ReceiverType::*)(ArgumentList...);

	WeakRefDelegate(WeakRef<ReceiverType>&& NewReceiver, MemberFunctionType MemberFunctionToBind) :
		m_Receiver(std::move(NewReceiver)),
		m_BoundFunction(MemberFunctionToBind)
	{}

	virtual ReturnType Execute(ArgumentList... Arguments) override
	{
		ReceiverType* receiver = m_Receiver.Get();
		return (receiver->*m_BoundFunction)(std::forward<ArgumentList>(Arguments)...);
	}

	virtual const void* GetBoundObjectPointer() const override
	{
		return static_cast<const void*>(m_Receiver.Get());
	}

	virtual bool IsValid() const override
	{
		return m_Receiver.IsValid();
	}

private:
	WeakRef<ReceiverType> m_Receiver;
	MemberFunctionType m_BoundFunction;
};

// A delegate to a free function (non-member function).
template<typename ReturnType, typename... ArgumentList>
class FreeDelegate : public IDelegate<ReturnType, ArgumentList...>
{
public:
	using FreeFunctionType = ReturnType(*)(ArgumentList...);

	FreeDelegate(FreeFunctionType FreeFunctionToBind) :
		m_BoundFunction(FreeFunctionToBind)
	{}

	virtual ReturnType Execute(ArgumentList... Arguments) override
	{
		return (*m_BoundFunction)(std::forward<ArgumentList>(Arguments)...);
	}

	virtual const void* GetBoundObjectPointer() const override
	{
		return static_cast<const void*>(m_BoundFunction);
	}

	virtual bool IsValid() const override
	{
		return m_BoundFunction != nullptr;
	}

private:
	FreeFunctionType m_BoundFunction;
};

// An event that binds to a single delegate.
// The delegate can be executed and its return value received.
template<typename ReturnType, typename... ArgumentList>
class CallbackEvent
{
public:
	template<typename ReceiverType>
	void BindDelegate(ReceiverType* Receiver,
		typename Delegate<ReceiverType,ReturnType,ArgumentList...>::MemberFunctionType MemberFunctionToBind)
	{
		m_Delegate = std::make_unique<Delegate<ReceiverType,ReturnType,ArgumentList...>>(Receiver, MemberFunctionToBind);
	}

	template<typename ReceiverType>
	void BindWeakRef(WeakRef<ReceiverType> Receiver,
		typename WeakRefDelegate<ReceiverType,ReturnType,ArgumentList...>::MemberFunctionType MemberFunctionToBind)
	{
		m_Delegate = std::make_unique<WeakRefDelegate<ReceiverType,ReturnType,ArgumentList...>>(std::move(Receiver), MemberFunctionToBind);
	}

	void BindFreeFunction(typename FreeDelegate<ReturnType,ArgumentList...>::FreeFunctionType FreeFunctionToBind)
	{
		m_Delegate = std::make_unique<FreeDelegate<ReturnType,ArgumentList...>>(FreeFunctionToBind);
	}

	void Clear()
	{
		m_Delegate.reset();
	}

	bool IsBound() const
	{
		return m_Delegate && m_Delegate->IsValid();
	}

	ReturnType Execute(ArgumentList... Arguments)
	{
		if (IsBound())
		{
			return m_Delegate->Execute(Arguments...);
		}
		else
		{
			return ReturnType();
		}
	}

private:
	std::unique_ptr<IDelegate<ReturnType,ArgumentList...>> m_Delegate;
};

// A multicast event.  The return type of the event must be void.
template<typename... ArgumentList>
class MulticastEvent
{
public:
	using ReturnType = void;

	template<typename ReceiverType>
	void AddDelegate(ReceiverType* receiver,
		typename Delegate<ReceiverType,ReturnType,ArgumentList...>::MemberFunctionType MemberFunctionToBind)
	{
		m_DelegateList.push_back(
			std::make_unique<Delegate<ReceiverType,ReturnType,ArgumentList...>>(receiver, MemberFunctionToBind)
		);
	}

	template<typename ReceiverType>
	void AddWeakRef(WeakRef<ReceiverType> receiver,
		typename WeakRefDelegate<ReceiverType,ReturnType,ArgumentList...>::MemberFunctionType MemberFunctionToBind)
	{
		m_DelegateList.push_back(
			std::make_unique<WeakRefDelegate<ReceiverType,ReturnType,ArgumentList...>>(std::move(receiver), MemberFunctionToBind)
		);
	}
	
	void BindFreeFunction(typename FreeDelegate<ReturnType,ArgumentList...>::FreeFunctionType FreeFunctionToBind)
	{
		m_DelegateList.push_back(
			std::make_unique<FreeDelegate<ReturnType,ArgumentList...>>(FreeFunctionToBind)
		);
	}

	template<typename ReceiverType>
	void RemoveDelegatesForReceiver(const ReceiverType* receiver)
	{
		const void* const GenericObjectPointer = static_cast<const void*>(receiver);
		auto removeItr = std::remove_if(m_DelegateList.begin(), m_DelegateList.end(),
			[GenericObjectPointer](const std::unique_ptr<IDelegate<ReturnType, ArgumentList...>>& NextDelegate)
			{
				return NextDelegate->GetBoundObjectPointer() == GenericObjectPointer;
			}
		);
		m_DelegateList.erase(removeItr);
	}

	template<typename FreeFunctionType>
	void RemoveDelegateForFreeFunction(FreeFunctionType functionPointer)
	{
		const void* const GenericObjectPointer = static_cast<const void*>(functionPointer);
		auto removeItr = std::remove_if(m_DelegateList.begin(), m_DelegateList.end(),
			[GenericObjectPointer](const std::unique_ptr<IDelegate<ReturnType, ArgumentList...>>& NextDelegate)
			{
				return NextDelegate->GetBoundObjectPointer() == GenericObjectPointer;
			}
		);
		m_DelegateList.erase(removeItr);
	}

	void Reserve(int numElements)
	{
		m_DelegateList.reserve(numElements);
	}

	void Clear()
	{
		m_DelegateList.clear();
	}

	void Broadcast(ArgumentList... arguments)
	{
		for (int i = 0; i < m_DelegateList.size(); ++i)
		{
			std::unique_ptr<IDelegate<ReturnType, ArgumentList...>>& NextDelegate = m_DelegateList[i];
			if (NextDelegate && NextDelegate->IsValid())
			{
				NextDelegate->Execute(std::forward<ArgumentList>(arguments)...);
			}
			else
			{
				Util::RemoveSwap(m_DelegateList, i);
				--i;
			}
		}
	}

private:
	std::vector<std::unique_ptr<IDelegate<ReturnType,ArgumentList...>>> m_DelegateList;
};

// NOTE: Theoretically, I think we could make the delegate types declared inside the
// templated event class.  But then we couldn't reuse them between the CallbackEvent
// and the MulticastEvent.  There is no particular reason to expose them, but
// also probably no particular reason to hide them.

#ifdef UNIT_TESTS
void TestEvents();
#endif
