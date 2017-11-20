#pragma once

template <class T>
class Queue
{
public:
	Queue()
	{
		first = nullptr;
		last = nullptr;
		numberOfItems = 0;
	}
	~Queue()
	{
		while (numberOfItems > 0) dequeue();
	};

	void enqueue(const T item)
	{
		Node *newNode = new Node(item);

		if (numberOfItems == 0)
		{
			first = newNode;
			last = first;
		}
		else
		{
			last->mPrevious = newNode;
			last = newNode;
		}
		++numberOfItems;
	}

	T dequeue()
	{
		if (numberOfItems == 0) return T();

		Node* removedNode;
		T removedData;

		if (numberOfItems == 1)
		{
			removedNode = first;
			removedData = first->mData;
			first = last = nullptr;
			--numberOfItems;
			delete removedNode;
			removedNode = nullptr;
			return removedData;
		}
		else
		{
			removedNode = first;
			removedData = first->mData;
			first = first->mPrevious;
			--numberOfItems;
			delete removedNode;
			removedNode = nullptr;
			return removedData;
		}
	}

	void printQueue()
	{
		if (numberOfItems == 0)
		{
			std::cout << "Queue is empty" << std::endl;
			return;
		}

		Node *tempNode = first;
		for (unsigned int i = 0; i < numberOfItems; ++i)
		{
			std::cout << tempNode->mData << " ";
			tempNode = tempNode->mPrevious;
		}
		std::cout << std::endl;
	}

	int getAmountInQueue() { return numberOfItems; }

private:

	struct Node
	{
		Node(T aData) { mData = aData; mPrevious = nullptr; };
		T mData;
		Node *mPrevious;
	};

	Node *first, *last;
	int numberOfItems;
};