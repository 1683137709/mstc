#include "BinomialHeap.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
	BinomialHeap<int> heap;

	srand(time(nullptr));

	/* ���Բ��� */
	for (int i = 0; i < 10; ++i)
		heap.push(rand() % 100);

#if 0
	/* ����ɾ�� */
	while (!heap.empty()) {
		std::cout << heap.top() << std::ends;
		heap.pop();
	}
#endif

	/* ���Կ������� */
	BinomialHeap<int> heap1 = heap;

	/* �����ƶ����� */
	BinomialHeap<int> heap2 = std::move(heap1);


	system("pause");
}