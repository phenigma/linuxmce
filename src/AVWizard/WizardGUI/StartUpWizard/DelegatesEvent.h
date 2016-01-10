/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef DELEGATES_H
#define DELEGATES_H

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// DelegatesEvent implementation
#include <vector> 

// DelegatesEvent class - invoke operator() on a collection of objects.
// Note that only one argument is supported for the parameters of
// the target signature. However, it is trivial to extend this to
// multiple arguments by having multiple templates similar to this
// one.
template <typename Return, typename Arg1>
class DelegatesEvent
{
private:
	// Base class - specify the signature.
	class Base
	{
	public:
		// Required for polymorphic destruction to work properly.
		virtual ~Base() { };

		// Target signature. Invoked through base class pointers.
		virtual Return operator()(Arg1) = 0;
	};

public:  
	// Derived class - parametrise the implementation so we can specify the 
	// target class in client code. 
	template <typename Class>
	class T : public Base
	{
		// Signature applied to a pointer-to-member for target class.
		typedef Return (Class::*Func)(Arg1);

	private:
		Class* mThis; // Pointer to the object we are delegating to.
		Func   mFunc; // Address of the function on the delegate object.

	public:
		T(Class* aThis, Func aFunc) : mThis(aThis), mFunc(aFunc) { }

		// Invoke the function throught the pointer-to-member.
		virtual Return operator()(Arg1 arg1)
		{
			return (mThis->*mFunc)(arg1);
		}
	};

public:  
	// Derived class - this is not parametrised and is intended for 
	// static and free functions.
	class S : public Base
	{
		typedef Return (*Func)(Arg1);

	private:
		Func mFunc; 

	public:
		S(Func aFunc) : mFunc(aFunc) { }

		// Invoke the function throught the pointer-to-member.
		virtual Return operator()(Arg1 arg1)
		{
			return mFunc(arg1);
		}
	};

private:
	// Use a vector to make this similar to C# multicast events.
	std::vector<Base*> mPtrs;

private:
	// Can't allow copying unless we modify the class to make a deep
	// copy of each of the pointers it holds. Base class would need 
	// a virtual Clone() method to allow polymorphic copying.
	DelegatesEvent(const DelegatesEvent&);
	DelegatesEvent& operator=(const DelegatesEvent&);

public:
	// Compiler seems to require this because other default constructors
	// have been made explicit.
	DelegatesEvent() { }

	// Clean up the garbage.
	~DelegatesEvent()
	{
		typename std::vector<Base*>::iterator end = mPtrs.end();
		for (typename std::vector<Base*>::iterator i = mPtrs.begin();
			i != end; ++i)
		{
			delete (*i);
		}
	}

	// Add a new target (callee) to our list.
	DelegatesEvent& operator+=(Base* aPtr)
	{
		mPtrs.push_back(aPtr);
		return *this;
	}

	// Call all the targets - there will be horrible undefined behaviour
	// if the callee object no longer exists. You have been warned!
	Return operator()(Arg1 arg1)
	{
		// There are problems here:
		// 1. Which result should the multicast return? I say the last called item.
		// 2. We need to can't store a temporary when Return is void.
		typename std::vector<Base*>::iterator end = mPtrs.end();
		for (typename std::vector<Base*>::iterator i = mPtrs.begin();
			i != end; ++i)
		{
			// Probably a specialisation for Return == void would be better.
			if ((i + 1) == end)
				return (*(*i))(arg1);
			else
				(*(*i))(arg1); 
		}
	}
};

#endif
