#pragma once
#include <vector>
#include <utility>
#include <algorithm>

class EmptyBinomialHeap : public std::exception {
};

template<typename Comparable, typename ComparFunc = std::less<>>
class BinomialHeap {
	struct BinomialNode {
		struct BinomialNode		*m_leftChild;		/* ���� */
		struct BinomialNode		*m_nextSibling;		/* ���ֵ� */
		Comparable				 m_key;				/* ��ֵ */
	public:
		BinomialNode(Comparable const &key) 
			: m_leftChild(nullptr), m_nextSibling(nullptr), m_key(key) { }

		BinomialNode(Comparable &&key)
			noexcept(noexcept(Comparable(std::declval<Comparable>()))) /* �����ֵ�����쳣�����쳣 */
			: m_leftChild(nullptr), m_nextSibling(nullptr), m_key(std::move(key)) { }
	};
private:
	std::vector<BinomialNode *>		m_treeArray;	/* ������ */
	std::size_t						m_size;			/* ���� */
private:
	/*
	 * ����˵��:		�������Ŷ�����, �����µĶ�����ָ��
	 * @first:		������ָ��
	 * @second:		������ָ��
	 */
	static BinomialNode *combine_trees(BinomialNode *first, BinomialNode *second)
	{
		if (!ComparFunc()(first->m_key, second->m_key) && first->m_key != second->m_key)
			return combine_trees(second, first);

		second->m_nextSibling = first->m_leftChild;
		first->m_leftChild = second;
		return first;
	}

	/*
	 * ����˵��:		Ѱ�� m_treeArray �����е�������ȼ�������±�
	 */
	int find_high_priority_index() const
	{
		int minIndex = 0;
		
		while (m_treeArray[minIndex] == nullptr)
			++minIndex;

		for (int i = minIndex + 1; i < m_treeArray.size(); ++i) {
			if (m_treeArray[i] != nullptr
				&& ComparFunc()(m_treeArray[i]->m_key, m_treeArray[minIndex]->m_key))
				minIndex = i;
		}

		return minIndex;
	}

	/*
	 * ����˵��:		�ݹ���ս���ڴ�
	 * @node:		���
	*/
	static void reclaim_memory(BinomialNode *node)
	{
		if (node == nullptr)
			return;

		reclaim_memory(node->m_leftChild);
		reclaim_memory(node->m_nextSibling);
		delete node;
	}

	/*
	 * ����˵��:		�������, ���ؿ�����Ľ��ָ��
	 * @node:		�������
	 */
	static BinomialNode *clone(BinomialNode *node)
	{
		if (node == nullptr)
			return nullptr;

		BinomialNode *newNode = new BinomialNode(node->m_key);
		newNode->m_leftChild = clone(node->m_leftChild);
		newNode->m_nextSibling = clone(node->m_nextSibling);

		return newNode;
	}
public:

	BinomialHeap(std::size_t capacity = 10) 
		: m_size(0), m_treeArray(capacity, nullptr)
	{ }

	BinomialHeap(BinomialHeap const &other)
		: m_size(other.m_size)
	{
		m_treeArray.resize(other.m_treeArray.size());
		
		for (std::size_t i = 0; i < other.m_treeArray.size(); ++i)
			m_treeArray[i] = clone(other.m_treeArray[i]);
	}

	BinomialHeap(BinomialHeap &&other) noexcept
		: m_treeArray(std::move(other.m_treeArray)), m_size(other.m_size)
	{
		other.m_size = 0;
	}

	BinomialHeap &operator=(BinomialHeap other)
	{
		swap(*this, other);
		return *this;
	}

	BinomialHeap &operator=(BinomialHeap &&other) noexcept
	{
		BinomialHeap tmp = std::move(other);
		swap(tmp, *this);
		return *this;
	}

	~BinomialHeap() { make_empty(); }

	/*
	 * ����˵��:		�Զ�����н��кϲ�����, �ϲ��� other ������н����ÿ�
	 * @other:		��Ҫ�ϲ��Ķ������
	 */
	void meger(BinomialHeap &other)
	{
		/* ��ֹ���� */
		if (this == &other)
			return;

		/* ����ռ䲻�������� */
		m_size += other.m_size;
		if (m_size > capacity()) {
			std::size_t oldNumTrees = m_size;
			std::size_t newNumTrees = std::max(m_treeArray.size(), other.m_treeArray.size()) + 1;
			m_treeArray.resize(newNumTrees);
			for (std::size_t i = oldNumTrees; i < newNumTrees; ++i)
				m_treeArray[i] = nullptr;
		}

		/* �ϲ� */
		BinomialNode *carry = nullptr;
		for (std::size_t i = 0, j = 1; j <= m_size; ++i, j *= 2) {
			BinomialNode *t1 = m_treeArray[i];
			BinomialNode *t2 = (i < other.m_treeArray.size() ? other.m_treeArray[i] : nullptr);

			int whichCase = (t1 == nullptr ? 0 : 1);
			whichCase += (t2 == nullptr ? 0 : 2);
			whichCase += (carry == nullptr ? 0 : 4);

			switch (whichCase) {
			case 0:		/* ����Ҫ�ϲ� */
			case 1:		/* this ��� */
				break;
			case 2:		/* other ��� */
				m_treeArray[i] = t2;
				other.m_treeArray[i] = nullptr;
				break;
			case 4:		/* carry ��� */
				m_treeArray[i] = carry;
				carry = nullptr;
				break;
			case 3:		/* this �� other ��� */
				carry = combine_trees(t1, t2);
				m_treeArray[i] = other.m_treeArray[i] = nullptr;
				break;
			case 5:		/* this �� carry ��� */
				carry = combine_trees(t1, carry);
				m_treeArray[i] = nullptr;
				break;
			case 6:		/* other �� carry ��� */
			case 7:		/* ȫ����� */
				carry = combine_trees(t2, carry);
				other.m_treeArray[i] = nullptr;
				break;
			}
		}

		/* �ÿ� other */
		for (auto &root : other.m_treeArray)
			root = nullptr;

		other.m_size = 0;
	}
	
	/* 
	 * ����˵��:		���ض�������е�������ȼ���Ԫ��, ����������Ϊ��, �׳� EmptyBinomialHeap �쳣
	 */
	Comparable const &top() const
	{
		if (empty())
			throw EmptyBinomialHeap();

		int minIndex = find_high_priority_index();
		return m_treeArray[minIndex]->m_key;
	}


	/*
	 * ����˵��:		����һ��Ԫ�ص����������
	 */
	void push(Comparable const &key)
	{
		BinomialHeap newHeap(1);

		newHeap.m_treeArray[0] = new BinomialNode(key);
		newHeap.m_size = 1;

		meger(newHeap);
	}

	/*
	 * ����˵��:		����һ��Ԫ�ص����������
	*/
	void push(Comparable &&key)
	{
		BinomialHeap newHeap(1);

		newHeap.m_treeArray[0] = new BinomialNode(std::move(key));
		newHeap.m_size = 1;

		meger(newHeap);
	}

	/*
	 * ����˵��:		ɾ�����������������ȼ��Ľ��
	 */
	void pop()
	{
		if (empty())
			throw EmptyBinomialHeap();

		int minIndex = find_high_priority_index();

		BinomialNode *oldRoot = m_treeArray[minIndex];
		BinomialNode *deletedTrees = oldRoot->m_leftChild;
		delete oldRoot;
		oldRoot = m_treeArray[minIndex] = nullptr;

		BinomialHeap newHeap(minIndex + 1);
		newHeap.m_size = (1 << minIndex) - 1;
		
		for (int j = minIndex - 1; j >= 0; --j) {
			newHeap.m_treeArray[j] = deletedTrees;
			deletedTrees = deletedTrees->m_nextSibling;
			newHeap.m_treeArray[j]->m_nextSibling = nullptr;
		}

		m_size -= newHeap.m_size + 1;

		meger(newHeap);
	}

	/*
	 * ����˵��:		��ն���
	*/
	void make_empty()
	{
		if (empty())
			return;

		for (auto &root : m_treeArray) {
			reclaim_memory(root);
			root = nullptr;
		}

		m_size = 0;
	}

	/* 
	 * ����˵��:		���ص�ǰ������е�����
	 */ 
	std::size_t capacity() const
	{
		return (1 << m_treeArray.size()) - 1;
	}

	/*
	 * ����˵��:		����������Ϊ��, ���� true, ���򷵻� false
	 */
	bool empty() const
	{
		return m_size == 0;
	}

	/*
	 * ����˵��:		���ض��ж��е�Ԫ������
	 */
	std::size_t size() const
	{
		return m_size;
	}

	/*
	 * ����˵��:		�ػ� swap 
	 */
	friend void swap(BinomialHeap &first, BinomialHeap &second) noexcept
	{
		std::swap(first.m_treeArray, second.m_treeArray);
		std::swap(first.m_size, second.m_size);
	}
};