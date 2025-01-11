#include <iostream>
#include <vector>


/*

이진 탐색 트리 ( BST, Binary Search Tree )

< 정의 >
1. 모든 노드는 유일한 키를 갖는다.
2. 왼쪽 서브 트리의 키들은 루트의 키보다 작다.
3. 오른쪽 서브 트리의 키들은 루트의 키보다 크다.
4. 왼쪽과 오른쪽 서브 트리도 이진 탐색 트리이다.

*/


/* 구현 */
//1. Node : Key 와 Value로 구성, 각각 left 와 Right 하위 노드를 가짐
class BSTNode {
public : 
	BSTNode(int key, int value);

public:
	int key;
	int value;
	BSTNode* left;
	BSTNode* right;
};

BSTNode::BSTNode(int key, int value) {
	this->key = key;
	this->value = value;

	this->left = nullptr;
	this->right = nullptr;
}


//2. BST : 루트 노드값을 인자로 받아서 초기화됨. 탐색, 삽입, 삭제 연산 보유.
class BST {
public:
	BST(int key, int value);

public: 
	BSTNode* RootNode;
	BSTNode* find(int key);
	BSTNode* insert(int key, int value);

	void printTree();

private:
	BSTNode* find_internal(int key, BSTNode* CurrentNode);

	bool IsLeafNode(BSTNode* CurrentNode);

	bool IsNodesInVectorAllNull(std::vector<BSTNode*>& vNodes);

};

// 생성자 : key, value 값을 인자로 받아서, 해당 값을 가지는 RootNode를 생성함.
BST::BST(int key, int value) {
	BSTNode* TmpNode = new BSTNode(key, value);
	this->RootNode = TmpNode;
}

bool BST::IsNodesInVectorAllNull(std::vector<BSTNode*>& vNodes)
{
	for (int i = 0; i < vNodes.size(); i++) {
		if (vNodes[i]) return false;
	}

	return true;
}

void BST::printTree()
{
	//1. vector 선언.
	std::vector<BSTNode*> vNodes;
	std::vector<BSTNode*> vTmp;

	//2. 루트 노드 담기.
	if (this->RootNode) {
		vNodes.push_back(this->RootNode);
	}
	else {
		return;
	}

	std::cout << "\n=============\n";

	//3. 반복
	while (!IsNodesInVectorAllNull(vNodes)) {

		//3.1 각 노드 별로 key를 출력.
		for (int i = 0; i < vNodes.size(); i++) {
			if ( nullptr != vNodes[i] ) {
				std::cout << vNodes[i]->key;
			}
			else {
				std::cout << "X";
			}

			std::cout << " ";
			if (i % 2 == 1) std::cout << " ";
		}

		// 3.2 개행문자 출력.
		std::cout << std::endl;

		// 3.3 vNodes 갱신.
		vTmp.assign(vNodes.begin(), vNodes.end());
		vNodes.clear();
		for (int i = 0; i < vTmp.size(); i++) {
			//3.3.1 .
			if (!(vTmp[i])) {
				vNodes.push_back(nullptr);
				vNodes.push_back(nullptr);
			}
			else {
				if (!(vTmp[i]->left)) {
					vNodes.push_back(nullptr);
				}
				else {
					vNodes.push_back(vTmp[i]->left);
				}

				if (!(vTmp[i]->right)) {
					vNodes.push_back(nullptr);
				}
				else {
					vNodes.push_back(vTmp[i]->right);
				}
			}
		}

		vTmp.clear();
	}

}


// 탐색
// 1. key == 현재 노드 -> 현재 노드가 찾는 노드.
// 2. key < 현재 노드 -> 찾는 노드는 현재 노드의 왼쪽에 존재.
// 3. key > 현재 노드 -> 찾는 노드는 현재 노드의 오른쪽에 존재.
BSTNode* BST::find(int key) 
{
	//1. 루트 노드로부터 시작.
	if (!(this->RootNode)) {
		//1.A 현재 노드의 왼쪽으로 가야함.
		if ((key < this->RootNode->key)) {
			return find_internal(key, this->RootNode->left); // 왼쪽 노드가 존재하는지는 internal 함수에서 처리.
		}
		//1.B      ""     오른쪽   "".
		else if (key > this->RootNode->key) {
			return find_internal(key, this->RootNode->right);
		}
		//1.C 루트 노드가 정답.
		else return this->RootNode;
	}
	else {
		return nullptr;
	}
}


BSTNode* BST::find_internal(int key, BSTNode* CurrentNode)
{
	//0. 현재 노드 null check
	if (!CurrentNode) {
		return nullptr;
	}

	//1.A 현재 노드의 왼쪽으로 가야함.
	if ((key < CurrentNode->key)) {
		return find_internal(key, CurrentNode->left); // 왼쪽 노드가 존재하는지는 internal 함수에서 처리
	}
	//1.B      ""     오른쪽   ""
	else if (key > CurrentNode->key) {
		return find_internal(key, CurrentNode->right);
	}
	//1.C 루트 노드가 정답.
	else return CurrentNode;

}


bool BST::IsLeafNode(BSTNode* CurrentNode)
{
	// 0. 파라미터 null check
	if (!CurrentNode) return false;

	// 1. leafnode인지 체크( 현재 노드의 right와 left가 모두 nullptr이면 leafnode )
	if (!(CurrentNode->right) && !(CurrentNode->left)) return true;
	return false;
}


// 삽입
// find 로직을 따라가면서, 현재 노드가 nullptr이 될때까지 반복하고, 현재 노드가 nullptr이면, 그 위치에 노드를 만든다.
BSTNode* BST::insert(int key, int value)
{
	//1. 현재 노드 == 루트 노드.
	if (!(this->RootNode)) return nullptr;
	BSTNode* CurrentNode = this->RootNode;
	BSTNode* ParentNode = nullptr;


	//2. 조건에 맞는 ParentNode를 찾을 때까지 반복.
	while (true) {

		//2.1 key를 통한 진행 방향 탐색.
		//2.1.A 현재 노드의 왼쪽으로 가야함.
		if ((key < CurrentNode->key)) {
			//2.1.A.A 왼쪽 노드가 nullptr -> 여기다가 새롭게 만들기.
			if (!(CurrentNode->left)) {
				BSTNode* newNode = new BSTNode(key, value);
				CurrentNode->left = newNode;

				return CurrentNode->left;
			}

			//2.1.A.B nullptr -> 하위 노드 탐색.
			else {
				ParentNode = CurrentNode;
				CurrentNode = CurrentNode->left;
			}
		}
		//2.1.B      ""     오른쪽   ""
		else if (key > CurrentNode->key) {
			//2.1.B.A 오른쪽 노드가 nullptr -> 여기다가 새롭게 만들기.
			if (!(CurrentNode->right)) {
				BSTNode* newNode = new BSTNode(key, value);
				CurrentNode->right = newNode;

				return CurrentNode->right;
			}

			//2.1.B.B nullptr -> 하위 노드 탐색.
			else {
				ParentNode = CurrentNode;
				CurrentNode = CurrentNode->right;
			}
		}
		//2.1.C 현재 노드가 새로 삽입하고자 했던 key .
		else return CurrentNode;

	}
}


int main()
{
	BST BinarySearchTree = BST(1, 2);
	std::cout << "Root : " << BinarySearchTree.RootNode->key << "\n";
	
	BinarySearchTree.insert(5, 8);
	BinarySearchTree.insert(10, 12);
	BinarySearchTree.insert(2, 6);
	BinarySearchTree.insert(3, 4);

	BinarySearchTree.printTree();
	
	return 0;
}