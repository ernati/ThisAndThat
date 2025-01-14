#include <iostream>
#include <vector>
#include <algorithm>


/*

자가 균형 이진 탐색 트리 ( AVL Tree )

< 정의 >
1. 모든 노드는 유일한 키를 갖는다.
2. 왼쪽 서브 트리의 키들은 루트의 키보다 작다.
3. 오른쪽 서브 트리의 키들은 루트의 키보다 크다.
4. 왼쪽과 오른쪽 서브 트리도 이진 탐색 트리이다.

<AVL Tree>
5. 두 자식 서브 트리의 높이는 항상 최대 1만큼 차이난다.
6. 어떤 시점에서 높이 차이가 1보다 커지면 이 속성을 유지하기 위해서 스스로 균형을 잡는다. 

*/


/* 구현 */
//1. Node : Key 와 Value로 구성, 각각 left 와 Right 하위 노드를 가짐
class AVLTreeNode {
public:
	AVLTreeNode(int key, int value);

public:
	int key;
	int value;
	AVLTreeNode* left;
	AVLTreeNode* right;
	AVLTreeNode* parent;
	int LeftOrRight; // 0 : 부모 노드 없음, 1 : 부모 노드의 left 자식, 2 : 부모 노드의 right 자식

	int heights;
};

AVLTreeNode::AVLTreeNode(int key, int value) {
	this->key = key;
	this->value = value;

	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
	LeftOrRight = 0;
	int heights = 0;
}


//2. AVLTree : 루트 노드값을 인자로 받아서 초기화됨. 탐색, 삽입, 삭제 연산 보유.
class AVLTree {
public:
	AVLTree(int key, int value);

public:
	AVLTreeNode* RootNode;
	AVLTreeNode* find(int key);
	AVLTreeNode* insert(int key, int value);
	AVLTreeNode* DeleteNode(int key);

	void printTree();
	void MeasureHeights();

private:
	AVLTreeNode* find_internal(int key, AVLTreeNode* CurrentNode);

	bool IsLeafNode(AVLTreeNode* CurrentNode);

	bool IsNodesInVectorAllNull(std::vector<AVLTreeNode*>& vNodes);

	AVLTreeNode* swapNode(AVLTreeNode* NodeA, AVLTreeNode* NodeB);

	
	void VisitNode(AVLTreeNode* Node, std::vector<AVLTreeNode*>& vAVLTreeNodes);
	void VisitAllNodesAndPutLeafNodesIntoVector(std::vector<AVLTreeNode*>& vAVLTreeNodes);
};

// 생성자 : key, value 값을 인자로 받아서, 해당 값을 가지는 RootNode를 생성함.
AVLTree::AVLTree(int key, int value) {
	AVLTreeNode* TmpNode = new AVLTreeNode(key, value);
	this->RootNode = TmpNode;
}

bool AVLTree::IsNodesInVectorAllNull(std::vector<AVLTreeNode*>& vNodes)
{
	for (int i = 0; i < vNodes.size(); i++) {
		if (vNodes[i]) return false;
	}

	return true;
}

// 트리를 출력하는 함수
void AVLTree::printTree()
{
	//1. vector 선언.
	std::vector<AVLTreeNode*> vNodes;
	std::vector<AVLTreeNode*> vTmp;

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
			if (nullptr != vNodes[i]) {
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
AVLTreeNode* AVLTree::find(int key)
{
	//1. 루트 노드로부터 시작.
	if ((this->RootNode)) {
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


AVLTreeNode* AVLTree::find_internal(int key, AVLTreeNode* CurrentNode)
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


// 입력으로 들어온 노드가 leaf 노드인지 체크하는 함수
bool AVLTree::IsLeafNode(AVLTreeNode* CurrentNode)
{
	// 0. 파라미터 null check
	if (!CurrentNode) return false;

	// 1. leafnode인지 체크( 현재 노드의 right와 left가 모두 nullptr이면 leafnode )
	if (!(CurrentNode->right) && !(CurrentNode->left)) return true;
	return false;
}


// 삽입
// find 로직을 따라가면서, 현재 노드가 nullptr이 될때까지 반복하고, 현재 노드가 nullptr이면, 그 위치에 노드를 만든다.
AVLTreeNode* AVLTree::insert(int key, int value)
{
	//1. 현재 노드 == 루트 노드.
	if (!(this->RootNode)) return nullptr;
	AVLTreeNode* CurrentNode = this->RootNode;
	AVLTreeNode* ParentNode = nullptr;


	//2. 조건에 맞는 ParentNode를 찾을 때까지 반복.
	while (true) {

		//2.1 key를 통한 진행 방향 탐색.
		//2.1.A 현재 노드의 왼쪽으로 가야함.
		if ((key < CurrentNode->key)) {
			//2.1.A.A 왼쪽 노드가 nullptr -> 여기다가 새롭게 만들기.
			if (!(CurrentNode->left)) {
				AVLTreeNode* newNode = new AVLTreeNode(key, value);
				CurrentNode->left = newNode;
				newNode->parent = CurrentNode;
				newNode->LeftOrRight = 1;

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
				AVLTreeNode* newNode = new AVLTreeNode(key, value);
				CurrentNode->right = newNode;
				newNode->parent = CurrentNode;
				newNode->LeftOrRight = 2;

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

// 두 노드의 위치를 맞바꾸는 함수
AVLTreeNode* AVLTree::swapNode(AVLTreeNode* NodeA, AVLTreeNode* NodeB)
{
	// 0. null 체크
	if (!NodeA || !NodeB) return nullptr;

	AVLTreeNode* tmp = nullptr;
	int nTmp = -1;

	//1. parent 맞바꾸기
	//1.1 부모 -> 노드 값 바꾸기
	if (NodeA->LeftOrRight == 1) { // 이 노드는 부모 노드의 왼쪽 노드
		if (NodeB->LeftOrRight == 1) { // 여기도 왼쪽
			tmp = NodeA->parent->left;
			NodeA->parent->left = NodeB->parent->left;
			NodeB->parent->left = tmp;
		}
		else if (NodeB->LeftOrRight == 2) {
			tmp = NodeA->parent->left;
			NodeA->parent->left = NodeB->parent->right;
			NodeB->parent->right = tmp;
		}
		else { // NodeB가 루트 노드
			NodeA->parent->left = NodeB;
		}
	}
	else if (NodeA->LeftOrRight = 2) { // 오른쪽
		if (NodeB->LeftOrRight == 1) { // 왼쪽
			tmp = NodeA->parent->right;
			NodeA->parent->right = NodeB->parent->left;
			NodeB->parent->left = tmp;
		}
		else if (NodeB->LeftOrRight == 2) {
			tmp = NodeA->parent->right;
			NodeA->parent->right = NodeB->parent->right;
			NodeB->parent->right = tmp;
		}
		else { // NodeB가 루트 노드
			NodeA->parent->right = NodeB;
		}
	}
	else { // 노드 A가 루트노드
		if (NodeB->LeftOrRight == 1) { // 왼쪽
			NodeB->parent->left = NodeA;
			NodeB->parent = nullptr;
			NodeB->LeftOrRight = 0;
		}
		else if (NodeB->LeftOrRight == 2) {
			NodeB->parent->right = NodeA;
			NodeB->parent = nullptr;
			NodeB->LeftOrRight = 0;
		}
		else {
			//No cases
		}
	}

	//1.2 노드 -> 부모 값 바꾸기
	tmp = NodeA->parent;
	NodeA->parent = NodeB->parent;
	NodeB->parent = tmp;

	//1.3 LeftOrRight 값 바꾸기
	nTmp = NodeA->LeftOrRight;
	NodeA->LeftOrRight = NodeB->LeftOrRight;
	NodeB->LeftOrRight = nTmp;


	//2. 자식 노드 맞바꾸기
	//2.1 자식 -> 노드
	//2.1.A 둘다 리프 노드
	if (IsLeafNode(NodeA) && IsLeafNode(NodeB)) {
		// 할거 없음
	}
	//2.1.B NodeA만 리프 노드
	if (IsLeafNode(NodeA) && !IsLeafNode(NodeB)) {
		if (NodeB->left) {
			NodeB->left->parent = NodeA;
		}
		if (NodeB->right) {
			NodeB->right->parent = NodeA;
		}
	}
	//2.1.C NodeB만 리프 노드
	if (!IsLeafNode(NodeA) && !IsLeafNode(NodeB)) {
		if (NodeA->left) {
			NodeA->left->parent = NodeB;
		}
		if (NodeA->right) {
			NodeA->right->parent = NodeB;
		}
	}
	//2.1.D 둘다 리프노드 아님
	else {
		if (NodeA->left) {
			NodeA->left->parent = NodeB;
		}
		if (NodeA->right) {
			NodeA->right->parent = NodeB;
		}
		if (NodeB->left) {
			NodeB->left->parent = NodeA;
		}
		if (NodeB->right) {
			NodeB->right->parent = NodeA;
		}
	}

	//2.2 노드 -> 자식
	tmp = NodeA->left;
	NodeA->left = NodeB->left;
	NodeB->left = tmp;

	tmp = NodeA->right;
	NodeA->right = NodeB->right;
	NodeB->right = tmp;

	return NodeA;
}


//삭제
//1. 삭제할 노드가 Leaf 노드 -> 부모 노드 정보 수정 후 삭제
//2. 자식 노드가 하나인 노드의 삭제 -> 자신 자리에 자식 노드를 놓고, 삭제하면 됨.
//3. 자식노드가 2개인 노드의 삭제
// -> 삭제할 노드를 투르노드로 하는 서브 트리를 생각해보면, 전체값 중 중앙값이 사라지는 셈이므로,
// -> 새로운 중앙값을 찾는다.
// -> 즉, 삭제할 노드의 왼쪽 서브트리 중 최대값을 고르거나, 오른쪽 서브 트리 중 최소값을 고르면 된다.
AVLTreeNode* AVLTree::DeleteNode(int key)
{
	//0. 찾는 노드가 존재하는지 체크
	AVLTreeNode* CurrentNode = find(key);
	if (!CurrentNode) return nullptr;

	//1. 처리
	//1.A 삭제할 노드가 Leaf노드
	if (IsLeafNode(CurrentNode)) {
		//1.A.1 부모 노드에서 해당 자식 노드를 null
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = nullptr;
		}
		else {
			CurrentNode->parent->right = nullptr;
		}
		delete CurrentNode;
	}
	//1.B 자식노드가 하나임.
	//왼쪽만 존재
	else if (((CurrentNode->left) && !(CurrentNode->right))) {
		AVLTreeNode* tmp = CurrentNode;
		//1.B.1 삭제할 노드의 부모 노드 처리
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = CurrentNode->left;
		}
		else {
			CurrentNode->parent->right = CurrentNode->left;
		}

		//1.B.2 자식 노드의 부모를 기존 부모로 추가
		CurrentNode->left->parent = CurrentNode->parent;
		delete tmp;
	}
	//오른쪽만 존재
	else if (!((CurrentNode->left) && (CurrentNode->right))) {
		AVLTreeNode* tmp = CurrentNode;
		//1.B.1 삭제할 노드의 부모 노드 처리
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = CurrentNode->right;
		}
		else {
			CurrentNode->parent->right = CurrentNode->right;
		}

		//1.B.2 자식 노드의 부모를 기존 부모로 추가
		CurrentNode->right->parent = CurrentNode->parent;
		delete tmp;
	}

	// 둘다 존재
	else if (((CurrentNode->left) && (CurrentNode->right))) {
		//1. 왼쪽 서브트리에서 가장 큰 값을 찾는다. ( 가장 오른쪽 )
		AVLTreeNode* leftMaxNode = CurrentNode->left;
		while (!IsLeafNode(leftMaxNode)) {
			leftMaxNode = leftMaxNode->right;
		}

		// 2. 해당 값을 삭제할 노드의 위치로 변경한다.
		// 2.1 이동할 노드의 부모 노드 처리, 이동할 노드에서도 부모 정보 삭제
		//leftMaxNode->parent->right = nullptr;
		//leftMaxNode->parent = nullptr;

		//2.2 현재 노드 메모리 해제를 위한 tmp
		AVLTreeNode* tmp = CurrentNode;

		// 2.3 현재 노드를 왼쪽 서브트리 최대값으로 치환
		// 2.3.1 현재 노드의 key와 value를 왼쪽 서브트리 최대값으로 치환
		CurrentNode->key = leftMaxNode->key;
		CurrentNode->value = leftMaxNode->value;

		// 2.3.2 기존 왼쪽 서브트리 최대값 노드를 없애기
		// 2.3.2.A 해당 노드가 리프 노드 -> 
		if (IsLeafNode(leftMaxNode)) {
			//1.A.1 부모 노드에서 해당 자식 노드를 null 
			leftMaxNode->parent->right = nullptr;
			delete leftMaxNode;
		}
		//2.3.2.B 해당 노드가 리프노드가 아님( 왼쪽 자식 노드만 존재 )
		else {
			//2.3.2.B.1 삭제할 노드의 부모 노드 처리
			leftMaxNode->parent->right = leftMaxNode->left;

			//1.B.2 자식 노드의 부모를 기존 부모로 추가
			leftMaxNode->left->parent = leftMaxNode->parent;
			delete leftMaxNode;
		}
	}

	//exceptional cases
	else {
		;
	}
}

void AVLTree::VisitNode(AVLTreeNode* Node, std::vector<AVLTreeNode*>& vAVLTreeNodes)
{
	//0. null check
	if (!Node) return;

	//1. Node가 leafnode인지 체크
	//1.A leafNode -> 벡터에 노드를 담고 종료
	if (IsLeafNode(Node)) {
		vAVLTreeNodes.push_back(Node);
	}
	//1.B leafNode가 아님 -> 좌우 둘다 진행
	else {
		VisitNode(Node->left, vAVLTreeNodes);
		VisitNode(Node->right, vAVLTreeNodes);
	}

	return;
}

void AVLTree::VisitAllNodesAndPutLeafNodesIntoVector(std::vector<AVLTreeNode*>& vAVLTreeNodes)
{
	//0. 루트 노드 null check
	if (!this->RootNode) return;

	//1. 루트 노드 방문 시작
	VisitNode(this->RootNode, vAVLTreeNodes);
}

void AVLTree::MeasureHeights()
{
	//0. 루트 노드가 비어있는지 체크한다.
	if (!this->RootNode) return;

	std::vector<AVLTreeNode*> vAVLTreeNodes;
	//1. 현재 트리에 존재하는 모든 leafnode들을 구한다.
	//1.1 트리에 존재하는 모든 노드들을 방문하여, leafnode들을 벡터에 담는다.
	VisitAllNodesAndPutLeafNodesIntoVector(vAVLTreeNodes);

	//2. 모든 leafnode들로부터 RootNode까지 올라가는데, 올라갈 때마다 노드들의 높이 값을 갱신한다.
	AVLTreeNode* TmpNode = nullptr;
	for (auto it = vAVLTreeNodes.begin(), end = vAVLTreeNodes.end(); it != end; it++) {
		//2.1 leafnode의 heights는 0
		TmpNode = *it;
		TmpNode->heights = 0;

		//2.2 leafnode 부터 root 까지 올라간다.
		while (TmpNode->parent != nullptr) {
			//2.2.1 부모의 heights를 갱신한다. ( 기존 height와, 자식노드height +1 중 최대값으로 갱신 )
			TmpNode->parent->heights = std::max(TmpNode->parent->heights, TmpNode->heights + 1);

			//2.2.2 임시 노드를 부모 노드로 바꾼다.
			TmpNode = TmpNode->parent;
		}
	}

	return;
}



int main()
{
	AVLTree AVL = AVLTree(7, 7);
	AVL.printTree();

	AVL.insert(4, 4);
	AVL.printTree();

	AVL.insert(8, 8);
	AVL.printTree();

	AVL.insert(2, 2);
	AVL.printTree();

	AVL.insert(6, 6);
	AVL.printTree();

	AVL.insert(10, 10);
	AVL.printTree();

	AVL.DeleteNode(5);
	AVL.printTree();

	AVL.MeasureHeights();
	AVL.printTree();

	return 0;
}