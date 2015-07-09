#include <string>
#include <stdio.h>
#include <stdlib.h>

unsigned int computeHash(unsigned short input);
unsigned int computeHash(std::string input);

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity)
	{
		m_capacity = capacity;
		m_buckets = numBuckets;
		curNodes = 0;
		arr = new Node*[m_buckets];
		for (int i = 0; i < m_buckets; i++)
			arr[i] = nullptr;
		headOfRecentlyWritten = nullptr;
	}

	~HashTable()
	{
		for (int i = 0; i < m_buckets; i++)		// destruct hash table
		{
			if (arr[i] == nullptr)
				continue;
			Node* p = arr[i];
			while (p != nullptr)
			{
				Node* t = p->next;
				if (p->toPointer != nullptr)
					delete p->toPointer;
				delete p;
				p = t;
			}
		}
		delete[] arr;
	}

	bool isFull()	const
	{
		return curNodes == m_capacity;
	}

	bool set(const KeyType&	key, const ValueType& value, bool permanent = false)		// insert
	{
		Node* p = findInArr(key);
		if (p != nullptr)								// if key already in hash table
		{
			p->m_value = value;
			// Update recently-updated list
			if (p->permanance == false)
			{
				pointerToNode* updated = p->toPointer;
				// Fix next, previous, and head pointers			
				moveUpdatedToTop(updated);
			}
		}
		else											// if key not in hash table, make new node
		{
			if (isFull())
				return false;
			Node* w = new Node;
			w->m_key = key;
			w->m_value = value;							
			w->permanance = permanent;
			w->next = nullptr;
			w->previous = nullptr;

			unsigned int computeHash(KeyType);
			unsigned int buck = computeHash(key) % m_buckets;
			if (arr[buck] == nullptr)					// if hash table list empty
				arr[buck] = w;
			else										// if hash table list has nodes
			{
				Node* p = arr[buck];
				while (p->next != nullptr)
				{
					p = p->next;
				}
				w->previous = p;
				p->next = w;
			}
			if (permanent == false)
			{
				// Update recently-updated list
				pointerToNode* q = new pointerToNode;
				q->node = w;
				w->toPointer = q;
				q->next = nullptr;
				q->previous = nullptr;
				// If list is empty, first node added to front
				if (headOfRecentlyWritten == nullptr)
				{
					headOfRecentlyWritten = q;
					tailOfRecentlyWritten = q;
				}
				// If list already has nodes, add on to its front
				else
				{
					headOfRecentlyWritten->previous = q;
					q->next = headOfRecentlyWritten;
					headOfRecentlyWritten = q;
				}
			}
			curNodes++;
		}
		return true;
	}

	bool get(const KeyType& key, ValueType& value) const
	{
		Node* p = findInArr(key);
		if (p != nullptr)
		{
			value = p->m_value;
			return true;
		}
		return false;
	}

	bool touch(const KeyType& key)
	{
		Node* p = findInArr(key);
		if (p != nullptr && !p->permanance)
		{
			moveUpdatedToTop(p->toPointer);
			return true;
		}
		return false;
	}

	bool discard(KeyType& key, ValueType& value)
	{
		// Delete node in hash table
		pointerToNode* willDiscard = tailOfRecentlyWritten;
		if (willDiscard == nullptr || willDiscard->node->permanance)
			return false;
		unsigned int computeHash(KeyType);
		unsigned int a = computeHash(willDiscard->node->m_key) % m_buckets;
		if (willDiscard->node->previous == nullptr  && willDiscard->node->next != nullptr)		// node in hash table first in list of this array element
		{
			arr[a] = willDiscard->node->next;		// used to be arrspot
			willDiscard->node->next->previous = nullptr;
		}
		else if (willDiscard->node->previous == nullptr && willDiscard->node->next == nullptr)	// only one node in hash table list at this array element
			arr[a] = nullptr;
		else if (willDiscard->node->previous != nullptr && willDiscard->node->next == nullptr)	// last node in hash table list for this array element
			willDiscard->node->previous->next = nullptr;
		else																					// in middle
		{
			willDiscard->node->previous->next = willDiscard->node->next;
			willDiscard->node->next->previous = willDiscard->node->previous;
		}
		key = willDiscard->node->m_key;
		value = willDiscard->node->m_value;
		// Fix pointers in recently updated list
		Node* willTrash = willDiscard->node;
		if (headOfRecentlyWritten == tailOfRecentlyWritten)
			headOfRecentlyWritten = nullptr;
		tailOfRecentlyWritten = tailOfRecentlyWritten->previous;
		if (tailOfRecentlyWritten != nullptr)
			tailOfRecentlyWritten->next = nullptr;
		// Delete Node and pointerToNode
		delete willDiscard;
		delete willTrash;
		curNodes--;
		return true;
	}

	//void dump()
	//{
	//	cerr << "------------Hash-Table Contents-----------------" << endl;
	//	for (int i = 0; i < m_buckets; i++)
	//	{
	//		if (arr[i] == nullptr)
	//			continue;
	//		Node* p = arr[i];
	//		while (p != nullptr)
	//		{
	//			cerr << "Key: " << p->m_key << " Value: " << p->m_value << endl;
	//			p = p->next;
	//		}
	//	}
	//	cerr << "---------- Recently-Updated--------------" << endl;
	//	pointerToNode* u = headOfRecentlyWritten;
	//	while (u != nullptr)
	//	{
	//		cerr << u->node->m_key << endl;
	//		u = u->next;
	//	}
	//}

private:
	//	We	prevent a	HashTable from	being	copied	or	assigned	by	declaring	the
	//	copy	constructor	and	assignment	operator	private	and	not	implementing	them.
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);

	int m_buckets;
	int m_capacity;
	int curNodes;

	struct pointerToNode;
	struct Node
	{
		KeyType m_key;				// data storing ie int, string
		ValueType m_value;			// hash number
		bool permanance;
		Node* next;
		Node* previous;
		pointerToNode* toPointer;
		Node() { toPointer = nullptr; }
	};
	struct pointerToNode
	{
		Node* node;
		pointerToNode* next;
		pointerToNode* previous;
	};
	Node** arr;
	pointerToNode* headOfRecentlyWritten;
	pointerToNode* tailOfRecentlyWritten;
	
	Node* findInArr(const KeyType& k) const
	{
		Node* check;
		unsigned int computeHash(KeyType);
		unsigned int b = computeHash(k) % m_buckets;
		check = arr[b];
		while (check != nullptr)
		{
			if (check->m_key == k)
				return check;
			check = check->next;
		}
		return nullptr;
	}

	void moveUpdatedToTop(pointerToNode* updated)
	{
		if (updated->next == nullptr && updated->previous != nullptr)	// node is last
		{
			tailOfRecentlyWritten = updated->previous;
			updated->previous->next = nullptr;
			updated->next = headOfRecentlyWritten;
			headOfRecentlyWritten->previous = updated;
			headOfRecentlyWritten = updated;
			tailOfRecentlyWritten = updated->previous;
			updated->previous = nullptr;
		}
		else if (updated->next != nullptr && updated->previous != nullptr)	// node is in middle somewhere
		{
			updated->next->previous = updated->previous;
			updated->previous->next = updated->next;
			updated->next = headOfRecentlyWritten;
			updated->previous = nullptr;
			headOfRecentlyWritten->previous = updated;
			headOfRecentlyWritten = updated;
		}
	}
};