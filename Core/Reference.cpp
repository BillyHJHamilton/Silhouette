#include "Reference.h"

#if DEBUG_MEMORY
RefControlBlock::RefControlBlock()
{
	++s_NumCreated;
}

RefControlBlock::~RefControlBlock()
{
	++s_NumDestroyed;
}

int RefControlBlock::s_NumCreated = 0;
int RefControlBlock::s_NumDestroyed = 0;

void RefControlBlock::CheckMemoryReleased()
{
	if (s_NumCreated == s_NumDestroyed)
	{
		std::cout << "Reference Memory OK." << std::endl;
	}
	else
	{
		std::cerr << "Reference Memory Error!  Created " << s_NumCreated
			<< " Control Blocks, Destroyed " << s_NumDestroyed << std::endl;
	}
}
#endif

#if UNIT_TESTS
void TestReferences()
{
	struct Resource
	{
		int x = 1;
	};

	WeakRef<Resource> ref1;
	assert(!ref1);
	assert(ref1 == nullptr);

	{
		RefOwner<Resource> owner1(new Resource);

		ref1 = owner1.GetReference();
		WeakRef<Resource> ref2(owner1);
		WeakRef<Resource> ref3(ref1);

		assert(owner1.IsValid());
		assert(ref1.IsValid());
		assert(ref2);
		assert(ref1 == ref2);
		assert(owner1 == ref1);
		assert(owner1.Get() == ref2.Get());

		assert(owner1->x == 1);
		assert(ref1->x == 1);
		assert(ref2->x == 1);
	}

	assert(!ref1.IsValid());

	RefOwner<Resource> owner2;
	assert(!owner2.IsValid());
	WeakRef<Resource> ref4(owner2);
	assert(!ref4.IsValid());
	assert(owner2 == ref4);

	{
		Resource rTemp;
		RefTracker tracker1;

		ref1 = tracker1.MakeReference(&rTemp);

		assert(ref1.IsValid());
		assert(ref1->x == 1);
		assert(ref1 == &rTemp);

		const Resource* rPtr = &rTemp;
		WeakRef<const Resource> constRef = tracker1.MakeReference<const Resource>(rPtr);
		assert(constRef->x == 1);
	}
	assert(!ref1.IsValid());
}
#endif // UNIT_TESTS
