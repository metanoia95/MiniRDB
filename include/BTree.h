#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <cassert>


template<typename K, typename V>
struct Node {

	bool isLeaf;
	std::size_t degree; // 차수. 자식 수
	std::size_t size; // 현재 개수
	K* keys;  // 키 배열
	V* values; // 값 배열

	Node<K, V>** children; //이중포인터.
	// 자식 노드 포인터들의 배열을 가리키는 포인터

	Node<K, V>* parent;

	Node<K, V>* prev; //왼쪽 노드
	Node<K, V>* next; //오른쪽 노드


public:
	//constructor
	Node(std::size_t _degree) {
		isLeaf = false;
		degree = _degree;
		size = 0;

		keys = new K[degree]{};

		values = new V[degree]{};

		children = new Node<K, V>* [degree + 1] {};

		parent = nullptr;

		prev = nullptr;
		next = nullptr;

	}

	~Node() {
		delete[] keys;
		delete[] values;
		delete[] children;
	}

};


template<typename K, typename V>
class BPlusTree {

	size_t degree;

	Node<K, V>* root;

public:


	//생성자
	BPlusTree(std::size_t _degree) {
		degree = _degree;
		root = nullptr;
	};

	// 소멸자
	~BPlusTree() {
		clearNode(root);
	};

	// 템플릿 파일은 헤더파일에 전체를 작성함.
	//1. insert
	void insert(const K& key, const V& value) {

		// 빈 트리인 경우
		if (root == nullptr) {
			createNewRoot(key, value);

			return;
		}

		// 리프 노드 찾기.
		Node<K, V>* leafNode = findLeafNode(key);

		// 리프에 삽입
		insertIntoLeafNode(leafNode, key, value);


		// overflow 발생 시 split
		Node<K, V>* cursor = leafNode;
		// 리프노드 split
		if (cursor->size == degree) {
			Node<K, V>* p = splitLeafNode(cursor);
			cursor = p; //커서 업데이트
		}
		// 내부노드 스플릿
		while (cursor->size == degree) { //overflow 발생 시
			Node<K, V>* p = splitInternalNode(cursor);
			cursor = p; //커서 업데이트
		}

	}


	//2. search 
	// 단건 조회
	V search(const K& key) {

		Node<K, V>* leafNode = findLeafNode(key);
		auto& values = leafNode->values;

		std::size_t idx = lowerBound(leafNode, key);
		if (idx < leafNode->size && leafNode->keys[idx] == key) {
			return values[idx];
		}

		return V{}; // 키가 없는 경우 빈 값 반환
	}

	// 범위 탐색
	std::vector<V> rangeSearch(const K& start, const K& end) {
		std::vector<V> result;
		Node<K, V>* cursor = findLeafNode(start);
		if (cursor == nullptr) return result;

		std::size_t idx = lowerBound(cursor, start);

		// 커서가 유효한 경우에만 처리
		while (cursor != nullptr) {

			while (idx < cursor->size) {
				if (cursor->keys[idx] > end) {
					return result; // 범위 벗어나면 종료
				}
				result.push_back(cursor->values[idx]);
				idx++;
			}
			idx = 0; //다음 노드로 이동 시 인덱스 초기화
			cursor = cursor->next; // 다음 리프 노드로 이동

		}

		return result;

	}


	//3. update 
	bool update(const K& key, const V& newValue) {

		if (root == nullptr) return false;

		Node<K, V>* cursor = findLeafNode(key);
		if (cursor == nullptr) return false;

		// 인덱스 찾기
		std::size_t idx = lowerBound(cursor, key);

		// 인덱스 조건 체크 *가능하면 반드시 인덱스 범위 체크를 할 것.
		if (idx < cursor->size && cursor->keys[idx] == key) {
			cursor->values[idx] = newValue;
			return true;
		};

		return false;


	}


	//4. erase
	bool erase(const K& key) {


		// 0. 리프노드 찾기
		Node<K, V>* cursor = findLeafNode(key);
		assert(cursor != nullptr); // assert는 참이어야 통과

		// 1. 리프노드에서 값 삭제
		if (!eraseFromLeaf(cursor, key)) {
			return false;
		};

		// 2. 리프노드 언더플로우 핸들링
		cursor = handleLeafUnderflow(cursor, key);

		// 3. 내부노드 언더플로우 핸들링
		if (cursor != nullptr) {
			handleInternalUnderflow(cursor, key);
		}


		return true;

	}


	//5. print 함수
	// 전체 출력
	void print() {
		if (root == nullptr) {
			std::cout << "빈 트리" << std::endl;
			return;
		}

		std::queue<Node<K, V>*> q;
		q.push(root);

		int level = 0;
		while (!q.empty()) {
			int levelSize = q.size();
			std::cout << "[레벨 " << level << "] ";

			for (int i = 0; i < levelSize; i++) {
				Node<K, V>* cur = q.front();
				q.pop();

				std::cout << "(";
				for (std::size_t j = 0; j < cur->size; j++) {
					std::cout << cur->keys[j];
					if (j < cur->size - 1) std::cout << ",";
				}
				std::cout << ") ";

				if (!cur->isLeaf) {
					for (std::size_t j = 0; j <= cur->size; j++) {
						if (cur->children[j] != nullptr)
							q.push(cur->children[j]);
					}
				}
			}
			std::cout << std::endl;
			level++;
		}
	};

	// 리프 노드만 순서대로 출력
	void printLeaf() {
		if (root == nullptr) return;

		// 가장 왼쪽 리프 찾기
		Node<K, V>* cur = root;
		while (!cur->isLeaf) {
			cur = cur->children[0];
		}

		std::cout << "[리프 순회] ";
		while (cur != nullptr) {
			for (std::size_t i = 0; i < cur->size; i++) {
				std::cout << cur->keys[i] << " ";
			}
			cur = cur->next;
		}
		std::cout << std::endl;
	}

	// 리프 노드 역방향 출력
	void printLeafReverse() {
		if (root == nullptr) return;

		// 가장 오른쪽 리프 찾기
		Node<K, V>* cur = root;
		while (!cur->isLeaf) {
			cur = cur->children[cur->size];
		}
		std::cout << "[리프 역방향 순회]";
		while (cur != nullptr) {
			// -- 반복문에서 인수는 int를 사용
			// size_t 는 unsigned Integer. 0 -1 => max값이 됨. 즉 무한루프
			//std::size_t k = 0;
			//std::cout << k - 1 << std::endl;
			for (int i = cur->size - 1; i >= 0; i--) {
				std::cout << cur->keys[i] << " ";
			}
			cur = cur->prev;
		}
		std::cout << std::endl;
	}


	//6. clear 함수
	void clear() {
		clearNode(root);
		root = nullptr;
	}


private:

	//1. 핸들러 함수
	// 리프노드 언더플로우 핸들링
	Node<K, V>* handleLeafUnderflow(Node<K, V>* cursor, const K& key) {
		// 공용 변수 설정
		Node<K, V>* parent = cursor->parent;
		Node<K, V>* donor = nullptr;
		// 2. 리프 노드 언더플로우 검사.
		if (cursor->size < 1) { //최소 차수 1 // TODO : 차후 최소 차수 값을 변수화할 것.
			if (parent == nullptr) {
				delete cursor;
				root = nullptr;
				return nullptr;
			}
			donor = findDonor(cursor);
			// 빌린 값이 있으면 현재 노드에 추가.
			if (donor != nullptr) {
				redistributeLeaf(cursor, donor);
				return nullptr; // 재분배 처리됨. 추가 처리 건 없음.
			}
			else { // 빌린 값이 없는 경우 - merge 커서 객체 변함.
				return mergeLeaf(cursor); // 병합 후 parent 객체 리턴
			}
		}
		else { // key값은 삭제됐지만 언더플로우는 미발생
			updateParentBoundaryKey(cursor, key);
			return nullptr;
		}

	}

	// 내부노드 언더플로우 핸들링
	void handleInternalUnderflow(Node<K, V>*& cursor, const K& key) {
		Node<K, V>* parent = cursor->parent;

		Node<K, V>* donor = nullptr;
		// 3. 내부노드 처리 재귀적으로 처리.
		if (cursor == root && cursor->size == 0) {
			root = cursor->children[0];
			root->parent = nullptr;
			delete cursor;
			cursor = root;
		};
		while (cursor != root) { //cursor가 루트일때까지 반복
			parent = cursor->parent;
			if (cursor->size < 1) { // 언더플로우 발생 시
				donor = findDonor(cursor);

				if (donor != nullptr) { // 재분배
					redistributeInternal(cursor, donor);
				}
				else { // 빌릴 곳이 없는 경우 merge
					cursor = mergeInternal(cursor); // 병합 후 parent 리턴

					if (cursor == root && cursor->size == 0) {
						root = cursor->children[0];
						root->parent = nullptr;
						delete parent;
						break;
					}

				}

			}
			else { // 언더플로우 미발생시
				break;
			}
			// cursor가 root인지 검사해서 parent 객체로 이동 재귀처리

		}
	}


	//2. 조회 함수 ======================================================
	// 리프노드 조회
	Node<K, V>* findLeafNode(const K& key) {

		Node<K, V>* cursor = root;
		assert(cursor != nullptr);

		while (!cursor->isLeaf) { //현재 객체가 리프노드가 아닌 경우

			//키 값을 기준으로 어디로 들어갈지 찾아야 함. 

			// ex key = { 10 } ; 10보다 작으면 왼쪽[0], 크면 [1]. key의 인덱스 = 0;
			//  key값 이상인 경우 오른쪽. key보다 작을 경우 왼쪽.
			//  keys[id] <= key =>  
			//  15, 20  key<15  [0], 15<= key <20 이면 [1], 20<= key [2]
			int i = upperBound(cursor, key);

			cursor = cursor->children[i];

		}

		// 리프노드를 찾은 경우. 
		return cursor;


	};

	// 좌측 형제노드 조회
	Node<K, V>* findLeftSiblingNode(Node<K, V>* cursor) {

		Node<K, V>* parent = cursor->parent;
		if (parent == nullptr) return nullptr;

		for (size_t i = 0; i <= parent->size; i++) {
			if (parent->children[i] == cursor) {
				if (i > 0) {
					return parent->children[i - 1];
				}
			}
		}
		return nullptr;


	}

	// 우측 형제노드 조회
	Node<K, V>* findRightSiblingNode(Node<K, V>* cursor) {

		Node<K, V>* parent = cursor->parent;
		if (parent == nullptr) return nullptr;

		for (size_t i = 0; i <= parent->size; i++) {
			if (parent->children[i] == cursor) {
				if (i < parent->size) {
					return parent->children[i + 1];
				}
			}
		}
		return nullptr;

	}

	// 빌릴 수 있는 형제 노드 조회 
	Node<K, V>* findDonor(Node<K, V>* cursor) {

		Node<K, V>* leftNode = findLeftSiblingNode(cursor);
		Node<K, V>* rightNode = findRightSiblingNode(cursor);
		if (!cursor->isLeaf && cursor->parent->size <= 0) { // 내부노드이면서 parent의 size가 0인 경우 
			if (leftNode != nullptr && leftNode->size > 2) {
				return leftNode;
			}
			if (rightNode != nullptr && rightNode->size > 2) {
				return rightNode;
			}
			return nullptr;
		}
		else {
			if (leftNode != nullptr && leftNode->size > 1) { //최소차수 상수|변수화 필요
				return leftNode;
			}
			if (rightNode != nullptr && rightNode->size > 1) {
				return rightNode;
			}
			return nullptr;
		}
	}

	// 커서의 부모 내(children 배열) 인덱스 조회
	int findIndexInParent(Node<K, V>* cursor) {

		Node<K, V>* parent = cursor->parent;

		for (int i = 0; i <= parent->size; i++) {
			if (parent->children[i] == cursor) {
				return i;
			}
		}

		return -1; //부모에 없음.
	}

	// key값이 해당 노드의 keys 배열에 존재하는지 확인. 있으면 인덱스 조회. 없으면 -1 
	int findKeyIndexInKeys(Node<K, V>* cursor, const K& key) {

		for (size_t i = 0; i < cursor->size; i++) {
			if (cursor->keys[i] == key) {
				return i;
			}
		}
		return -1;

	}



	//3. 삽입 함수 ======================================================
	// 자식 노드에 삽입
	void insertIntoLeafNode(Node<K, V>* leafNode
		, const K& key, const V& value) {
		// 삽입 위치
		std::size_t i = 0;

		while (i < leafNode->size&& key >= leafNode->keys[i]) {
			i++;
		}

		//다른 노드 위치 수정.
		// [0][1] size 2 이 있는 경우 && i = 0 =>  [0] -> [1][2]
		// i = 1 => [0]+[1] -> [2]
		// 뒤에서 부터 한 칸씩 밀기
		for (int j = leafNode->size; j > i; j--) { //i는 삽입위치. 
			//0 삽입인 경우 1까지 
			leafNode->keys[j] = leafNode->keys[j - 1];
			leafNode->values[j] = leafNode->values[j - 1];
		}
		// size = 0, i = 0 =>  j=0 jump 
		// size = 1, i = 0 => j = 1 j > i => keys[1] = keys[0] 

		leafNode->keys[i] = key;
		leafNode->values[i] = value;
		leafNode->size++;

	}

	// 부모 노드에 삽입
	Node<K, V>* insertIntoParentNode(
		Node<K, V>* cursor
		, const K& upKey
		, Node<K, V>* newBrotherNode) {

		// 부모 노드가 없는 경우 새로 생성
		if (cursor->parent == nullptr) {
			Node<K, V>* newParentNode = new Node<K, V>(degree);
			cursor->parent = newParentNode;
			root = newParentNode; //새 루트로 할당
			newParentNode->children[0] = cursor;
			newParentNode->isLeaf = false;
		}
		Node<K, V>* p = cursor->parent;
		newBrotherNode->parent = p;

		// 부모에 값 올리기 
		//삽입 위치
		std::size_t i = 0;
		while (i < p->size&& upKey >= p->keys[i]) {
			i++;
		}
		//기존 키 밀기
		for (int j = p->size; j > i; j--) { //i는 삽입위치. 
			//0 삽입인 경우 1까지 
			p->keys[j] = p->keys[j - 1];
		}
		p->keys[i] = upKey; // key값 삽입


		// 자식포인터
		std::size_t pPos = 0;
		while (pPos <= p->size && p->children[pPos] != cursor) { // cursor 노드 위치 찾기
			pPos++;
		}
		p->size++;
		for (std::size_t j = p->size; j > pPos + 1; j--) {
			p->children[j] = p->children[j - 1];
		}
		p->children[pPos + 1] = newBrotherNode;

		return p; //커서 업데이트


	}

	// 루트 노드 생성
	void createNewRoot(const K& key, const V& value) {
		root = new Node<K, V>(degree);
		root->values[0] = value;
		root->keys[0] = key;
		root->isLeaf = true;
		root->size = 1;
	};

	//4. 분할 함수 ======================================================
	// 리프노드 분할
	Node<K, V>* splitLeafNode(Node<K, V>* cursor) {

		// 새 형제 노드. 
		Node<K, V>* newBrotherNode = new Node<K, V>(degree);
		newBrotherNode->isLeaf = true;

		// 새 형제 노드로 값 복사
		// degree = 3 => 3/2 => 1 : 1번 인덱스(2번째 값부터 분할)
		std::size_t oldSize = cursor->size;
		std::size_t splitIdx = degree / 2;
		K upKey = cursor->keys[splitIdx];
		for (std::size_t i = splitIdx; i < oldSize; i++) {
			newBrotherNode->keys[i - splitIdx] = cursor->keys[i];
			newBrotherNode->values[i - splitIdx] = cursor->values[i];
			newBrotherNode->size++;
		}
		cursor->size = splitIdx;

		// 연결리스트 포인터 연결 
		newBrotherNode->next = cursor->next;
		newBrotherNode->prev = cursor;
		if (cursor->next != nullptr) {
			cursor->next->prev = newBrotherNode;
		}
		cursor->next = newBrotherNode;

		return insertIntoParentNode(cursor, upKey, newBrotherNode);

	}

	// 내부 노드 분할
	Node<K, V>* splitInternalNode(Node<K, V>* cursor) {
		// 새 형제 노드. 
		Node<K, V>* newBrotherNode = new Node<K, V>(degree);
		newBrotherNode->isLeaf = false;
		// 리프노드 아님.

		//split 기준 인덱스 : 차수 / 2
		std::size_t oldSize = cursor->size;
		std::size_t splitIdx = degree / 2;
		K upKey = cursor->keys[splitIdx];
		//새 형제 노드로 키 값을 복사. 기준 인덱스 다음 값 분할. 기준 인덱스는 부모로 올라감. 
		for (std::size_t idx = splitIdx + 1; idx < oldSize; idx++) {
			newBrotherNode->keys[idx - (splitIdx + 1)] = cursor->keys[idx];
			newBrotherNode->size++;
		}
		//children 값 복사.
		for (std::size_t idx = splitIdx + 1; idx <= oldSize; idx++) {
			newBrotherNode->children[idx - (splitIdx + 1)] = cursor->children[idx];

			// 새형제로 옮겨진 자식 객체의 부모 노드 수정
			if (newBrotherNode->children[idx - (splitIdx + 1)] != nullptr) {
				newBrotherNode->children[idx - (splitIdx + 1)]->parent = newBrotherNode;
			}
		}

		cursor->size = splitIdx;
		// cursor에서 넘긴 children 정리
		for (std::size_t idx = splitIdx + 1; idx <= oldSize; idx++) {
			cursor->children[idx] = nullptr;
		}

		Node<K, V>* p = insertIntoParentNode(cursor, upKey, newBrotherNode);

		return p;

	}



	//5. 삭제 함수 ======================================================
	// 리프노드에서 삭제
	bool eraseFromLeaf(Node<K, V>*& cursor, const K& key) {

		//이분탐색으로 리프노드에서 해당 키 값 찾기.
		std::size_t idx = lowerBound(cursor, key);
		if (cursor->keys[idx] != key) {
			// 해당되는 키가 없음. 
			return false;
		}
		// 리프노드 배열에서 해당 위치 뒤 원소들을 한 칸씩 앞으로 당김. 
		for (size_t i = idx; i + 1 < cursor->size; i++) {
			cursor->keys[i] = cursor->keys[i + 1];
			cursor->values[i] = cursor->values[i + 1];
		}
		cursor->size -= 1;

		return true;
	}

	// 내부노드에서 삭제. 삭제 후 children node 포인터를 리턴
	Node<K, V>* eraseFromInternal(Node<K, V>* cursor, const K& key) {
		if (cursor->isLeaf) return nullptr;

		int idx = findKeyIndexInKeys(cursor, key);
		Node<K, V>* outChild = nullptr;
		if (idx == -1) { //키가 없는 경우
			return nullptr;
		}
		else if (idx == 0) { // 첫 키인 경우
			outChild = cursor->children[idx];
			for (size_t i = idx; i < cursor->size; i++) {
				cursor->keys[i] = cursor->keys[i + 1];
				cursor->children[i] = cursor->children[i + 1];
			}
			--cursor->size;
			return outChild;
		}
		else { // 그 외 경우
			outChild = cursor->children[idx + 1];
			for (size_t i = idx; i < cursor->size; i++) {
				cursor->keys[i] = cursor->keys[i + 1];
				cursor->children[i + 1] = cursor->children[i + 2];
			}
			--cursor->size;
			return outChild;
		}



	}


	//6. 재분배 함수 ======================================================
	// 리프 재분배 함수 - 리팩토링 버전.
	Node<K, V>* redistributeLeaf(Node<K, V>* cursor, Node<K, V>* donor) {
		Node<K, V>* parent = cursor->parent;
		int idx = findIndexInParent(cursor); // 현재노드의 인덱스

		if (donor == findLeftSiblingNode(cursor)) { // 왼쪽
			cursor->keys[0] = donor->keys[donor->size - 1];  //빌린 키
			cursor->values[0] = donor->values[donor->size - 1]; // 빌린 값.
			eraseFromLeaf(donor, cursor->keys[0]);
			++cursor->size;
			parent->keys[idx - 1] = cursor->keys[0];
		}
		else { //오른쪽.
			cursor->keys[cursor->size] = donor->keys[0];
			cursor->values[cursor->size] = donor->values[0];
			eraseFromLeaf(donor, cursor->keys[0]);
			++cursor->size;
			// idx==0 갱신할 경계키가 없음.
			if (idx > 0) parent->keys[idx - 1] = cursor->keys[0];
			parent->keys[idx] = donor->keys[0];
		}
		return cursor;

	};

	// 내부노드 재분배 함수
	Node<K, V>* redistributeInternal(Node<K, V>* cursor, Node<K, V>* donor) {
		//std::cout << "인터널 재분배 발생" << std::endl;
		int idx = findIndexInParent(cursor); // 현재노드의 인덱스
		Node<K, V>* parent = cursor->parent;
		K outKey;
		Node<K, V>* outChild;
		if (donor == findLeftSiblingNode(cursor)) { // 왼쪽 노드
			// 테스트 케이스 차수 4, 1~10 삽입, 9,10 삭제 | -> 8 삭제 시 발생
			outKey = donor->keys[donor->size - 1];
			outChild = eraseFromInternal(donor, outKey);
			cursor->keys[0] = parent->keys[idx - 1];
			parent->keys[idx - 1] = outKey;
			++cursor->size;
			for (size_t i = 0; i < cursor->size; i++) {
				cursor->children[i + 1] = cursor->children[i];
			}
			cursor->children[0] = outChild;
			if (outChild) outChild->parent = cursor;   // 이거 필수
		}
		else { // 오른쪽 노드
			outKey = donor->keys[0];
			outChild = eraseFromInternal(donor, outKey);
			cursor->keys[0] = parent->keys[idx];
			parent->keys[idx] = outKey;
			++cursor->size;
			cursor->children[cursor->size] = outChild;
			if (outChild) outChild->parent = cursor;   // 이거 필수
		}
		//printKeys(cursor);
		//printChildren(cursor);

		return cursor;

	}


	//7. 병합 함수 ======================================================
	Node<K, V>* mergeLeaf(Node<K, V>* cursor) {
		//TODO
		/*	현재 최소 차수가 1로 이 분기에선 남은 데이터가 없어 노드에 있는 데이터를 옮겨줄 필요가 없음
			만약 추후 최소 차수가 1이상이 되는 형태로 바꿀 경우(cursor->size < 2)
			옆 노드로 데이터를 이관하는 로직을 추가해야함.
			-> 아마도 경계값 미만 값일테니 왼쪽 노드로 몽땅 옮기면 될 듯.
		*/
		//std::cout << "리프 머지 발생" << std::endl;
		Node<K, V>* parent = cursor->parent;
		assert(parent != nullptr);
		int idx = findIndexInParent(cursor); // 현재노드의 인덱스
		assert(idx >= 0);

		if (idx == 0) { //오른쪽으로 병합
			deleteEntry(parent, 0, 0);
		}
		else { //왼쪽으로 병합
			deleteEntry(parent, idx - 1, idx);
		}

		// leafNode 커서 연결
		if (cursor->prev != nullptr) cursor->prev->next = cursor->next;
		if (cursor->next != nullptr) cursor->next->prev = cursor->prev;


		// 자식 메모리 해제
		delete cursor;


		return parent;
	}

	// 내부노드 병합 함수
	Node<K, V>* mergeInternal(Node<K, V>* cursor) {
		Node<K, V>* parent = cursor->parent;
		Node<K, V>* leftNode = findLeftSiblingNode(cursor);
		Node<K, V>* rightNode = findRightSiblingNode(cursor);

		int idx = findIndexInParent(cursor); // 현재노드의 인덱스

		// 왼쪽 노드가 비어있지 않다면 무조건 왼쪽으로 머지
		if (leftNode != nullptr) {
			cursor = mergeInternalWithLeft(cursor);
		}
		else if (rightNode != nullptr) {
			cursor = mergeInternalWithRight(cursor);
		}

		return cursor->parent;

	}

	// 왼쪽 노드로 병합
	Node<K, V>* mergeInternalWithLeft(Node <K, V>* cursor) {
		Node<K, V>* parent = cursor->parent;
		Node<K, V>* leftNode = findLeftSiblingNode(cursor);
		assert(parent == leftNode->parent);

		int idx = findIndexInParent(cursor);
		assert(idx >= 0);
		K parentKey = parent->keys[idx - 1]; // cursor와 leftNode 사이 경계키
		int parentKeyIdx = findKeyIndexInKeys(parent, parentKey);

		// 왼쪽노드에 부모 키 및 커서의 자식 포인터 삽입
		insertAt(leftNode, leftNode->size, parentKey, cursor->children[0]);

		// 3. 부모 노드 정리
		// 부모에서 사용된 키와 자식(cursor) 포인터를 제거하고 당기기
		// 부모키가 내려오므로 부모에서 해당 키를 지우고 index도 삭제.
		deleteEntry(parent, parentKeyIdx, idx);


		// 메모리 해제 및 커서 갱신.
		delete cursor;
		cursor = leftNode;

		return cursor;
	};

	// 오른쪽 노드로 병합
	Node<K, V>* mergeInternalWithRight(Node<K, V>* cursor) { // 병합된 노드 리턴

		assert(cursor->size == 0);

		Node<K, V>* parent = cursor->parent;
		Node<K, V>* rightNode = findRightSiblingNode(cursor);
		int idx = findIndexInParent(cursor);
		K parentKey = parent->keys[idx]; // cursor와 rightNode 사이 경계키
		int parentKeyIdx = findKeyIndexInKeys(parent, parentKey);

		// 오른쪽 노드에 부모키 및 커서의 자식 포인터 삽입
		insertAt(rightNode, 0, parentKey, cursor->children[0]);
		assert(rightNode->children[0] == cursor->children[0]);

		// 4. 부모 정리
		// idx는 지워지는 cursor의 인덱스.
		// 부모키가 내려오므로 부모에서 해당 키를 지우고 index도 삭제.
		deleteEntry(parent, parentKeyIdx, idx);
		// rightNode로 병합하는 케이스는 

		delete cursor;
		cursor = rightNode; // ← 병합된 노드로 갱신

		return cursor;
	}


	//8. 탐색 함수 ======================================================
	// 키 이분탐색. key 이상(>=) 이 처음 나오는 위치
	int lowerBound(Node<K, V>* node, const K& key) {

		int lo = 0;
		int hi = node->size;
		auto* keys = node->keys;

		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			if (keys[mid] == key) {
				return mid;
			}
			else if (keys[mid] < key) {
				lo = mid + 1;
			}
			else if (keys[mid] > key) {
				hi = mid;
			}
		}
		return lo;

	}

	// 키 이분탐색. key 초과(>)가 처음 나오는 위치
	int upperBound(Node<K, V>* node, const K& key) {

		int lo = 0;
		int hi = node->size;
		auto* keys = node->keys;

		// key = 2 일때
		// keys = [2]  
		// children =  1 / 2,3 
		//            [0, 1] -> 1을 골라야함.
		// lo = 0; hi = 1;
		while (lo < hi) {
			int mid = lo + (hi - lo) / 2;
			// mid = 0+(1-0)/2 => 0
			if (keys[mid] <= key) {
				lo = mid + 1;
			}
			else if (keys[mid] > key) {
				hi = mid;
			}

		}
		return lo;


	}

	//10. 수정함수 
	// 경계키 수정함수
	void updateParentBoundaryKey(Node<K, V>* cursor, const K& key) {
		Node<K, V>* parent = cursor->parent;
		// key는 이미 삭제된 상태. 
		// 경계값에 key가 있는지 확인
		// 언더플로우가 발생하지 않은 경우 - 값만 삭제된 경우
		if (parent != nullptr) {
			int keyIdx = findKeyIndexInKeys(parent, key); // parent keys에서의 key 인덱스값. -> 경계값
			if (keyIdx != -1) {
				parent->keys[keyIdx] = cursor->keys[0];
			}
		}

	}


	//11. 메모리 소거 함수 ======================================================
	// 노드 삭제. 자식 노드들도 재귀적으로 삭제.
	void clearNode(Node<K, V>* node) {
		if (node == nullptr) return;

		if (!node->isLeaf) {
			for (std::size_t i = 0; i <= node->size; i++) {
				clearNode(node->children[i]);
			}
		}
		delete node; //소멸자 자동으로 호출됨.
	}

	//12. 헬퍼 함수
	// 노드 삽입함수
	void insertAt(Node < K, V>* targetNode, int idx, const K& key, Node<K, V>* child) {
		// idx는 키 기준으로 들어옴.
		// 키 삽입
		shiftKey(targetNode, idx);
		targetNode->keys[idx] = key;

		// 자식 포인터 삽입
		if (!targetNode->isLeaf) { // 내부 노드인 경우 자식 포인터 삽입.
			if (idx == 0) { // 가장 왼쪽자식으로 삽입하는 경우 0번 위치부터 밀어야 함.
				shiftChild(targetNode, idx);
				setChild(targetNode, idx, child);
			}
			else { // 그 외 경우. idx 위치는 키 기준으로 들어오므로 idx+1 위치에 자식 포인터 삽입해야 함.
				shiftChild(targetNode, idx + 1);
				setChild(targetNode, idx + 1, child);
			}
		}

		// 노드 크기 증가
		targetNode->size++;

	}



	// 노드에서 특정 위치 삭제
	void deleteAt(Node <K, V>* targetNode, int idx) {
		// idx는 키 기준.
		assert(idx >= 0 && idx < targetNode->size);
		deleteKey(targetNode, idx);

		if (!targetNode->isLeaf) { // 리프 노드가 아닌 경우에만 자식 삭제
			if (idx == 0) {
				deleteChildPtr(targetNode, idx);
			}
			else {
				deleteChildPtr(targetNode, idx + 1);
			}
		}

		--targetNode->size;
	}

	// 인터널 노드에서 특정 값 삭제.
	void deleteEntry(Node <K, V>* node, int keyIdx, int childIdx) {
		assert(!node->isLeaf);
		assert(keyIdx >= 0 && keyIdx < node->size);
		assert(childIdx >= 0 && childIdx <= node->size);

		deleteKey(node, keyIdx);
		deleteChildPtr(node, childIdx);

		--node->size;
	}



	// 자식 세팅 함수.
	void setChild(Node <K, V>* node, int idx, Node<K, V>* child) {
		assert(child != nullptr);
		node->children[idx] = child;
		if (child == nullptr) return; // 방어용 함수
		child->parent = node;
	}

	// 키 및 자식 포인터 삭제 헬퍼 함수. ======================================================
	// size는 삭제 전 크기. 삭제 후 --node->size 처리 해야함.
	// 키 밀어내는 함수. idx 위치부터 한 칸 밀어냄.
	void shiftKey(Node <K, V>* node, int idx) {
		assert(idx >= 0 && idx <= node->size);
		for (int i = node->size; i > idx; i--) {
			node->keys[i] = node->keys[i - 1];
		}
	}

	// 자식 포인터 밀어내는 함수. idx 위치부터 한 칸 밀어냄.
	void shiftChild(Node <K, V>* node, int idx) {
		assert(idx >= 0 && idx <= node->size + 1);
		// 자식은 최대 size + 1까지 있을 수 있으므로 idx는 그 사이여야 함

		for (int i = node->size; i >= idx; i--) {
			node->children[i + 1] = node->children[i];
		}
	}

	// 키 삭제 함수. idx 위치의 키 삭제, 뒤에 칸을 당김.
	void deleteKey(Node<K, V>* node, int idx) {
		assert(idx >= 0 && idx < node->size);

		for (int i = idx; i < node->size - 1; i++) {
			node->keys[i] = node->keys[i + 1];
		}
		//TODO 방어용으로 마지막 칸 초기화 고려해볼 것.
	}

	// 자식 포인터 삭제 함수. idx 위치의 자식 포인터 삭제, 뒤의 칸들을 당김.
	void deleteChildPtr(Node<K, V>* node, int idx) {
		assert(idx >= 0 && idx <= node->size + 1);

		for (int i = idx; i < node->size; i++) {
			node->children[i] = node->children[i + 1];
		}
		node->children[node->size] = nullptr;

	}

	// 13. 테스트용 프린트 함수
	// 현재 노드 키 출력
	void printKeys(Node<K, V>* node) {
		std::cout << "(";
		for (std::size_t i = 0; i < node->size; i++) {
			std::cout << node->keys[i] << ",";
		}
		std::cout << ")" << " \ 사이즈 :" << node->size << std::endl;


	};

	// 자식노드 출력
	void printChildren(Node<K, V>* node) {
		if (node->isLeaf) return;
		for (std::size_t i = 0; i <= node->size; i++) {
			std::cout << "(" << i << " : " << node->children[i]->keys[0] << ")";
		}
		std::cout << std::endl;

	}

	// 자식노드 개수 
	size_t countChildren(Node<K, V>* node) {
		size_t count = 0;
		for (std::size_t i = 0; i <= node->size; i++) {
			count++;
		}
		return count;
	}


};