#include <iostream>
#include <vector>


/*

���� Ž�� Ʈ�� ( BST, Binary Search Tree )

< ���� >
1. ��� ���� ������ Ű�� ���´�.
2. ���� ���� Ʈ���� Ű���� ��Ʈ�� Ű���� �۴�.
3. ������ ���� Ʈ���� Ű���� ��Ʈ�� Ű���� ũ��.
4. ���ʰ� ������ ���� Ʈ���� ���� Ž�� Ʈ���̴�.

*/


/* ���� */
//1. Node : Key �� Value�� ����, ���� left �� Right ���� ��带 ����
class BSTNode {
public : 
	BSTNode(int key, int value);

public:
	int key;
	int value;
	BSTNode* left;
	BSTNode* right;
	BSTNode* parent;
	int LeftOrRight; // 0 : �θ� ��� ����, 1 : �θ� ����� left �ڽ�, 2 : �θ� ����� right �ڽ�
};

BSTNode::BSTNode(int key, int value) {
	this->key = key;
	this->value = value;

	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
	LeftOrRight = 0;
}


//2. BST : ��Ʈ ��尪�� ���ڷ� �޾Ƽ� �ʱ�ȭ��. Ž��, ����, ���� ���� ����.
class BST {
public:
	BST(int key, int value);

public: 
	BSTNode* RootNode;
	BSTNode* find(int key);
	BSTNode* insert(int key, int value);
	BSTNode* DeleteNode(int key);

	void printTree();

private:
	BSTNode* find_internal(int key, BSTNode* CurrentNode);

	bool IsLeafNode(BSTNode* CurrentNode);

	bool IsNodesInVectorAllNull(std::vector<BSTNode*>& vNodes);

	BSTNode* swapNode(BSTNode* NodeA, BSTNode* NodeB);

};

// ������ : key, value ���� ���ڷ� �޾Ƽ�, �ش� ���� ������ RootNode�� ������.
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

// Ʈ���� ����ϴ� �Լ�
void BST::printTree()
{
	//1. vector ����.
	std::vector<BSTNode*> vNodes;
	std::vector<BSTNode*> vTmp;

	//2. ��Ʈ ��� ���.
	if (this->RootNode) {
		vNodes.push_back(this->RootNode);
	}
	else {
		return;
	}

	std::cout << "\n=============\n";

	//3. �ݺ�
	while (!IsNodesInVectorAllNull(vNodes)) {

		//3.1 �� ��� ���� key�� ���.
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

		// 3.2 ���๮�� ���.
		std::cout << std::endl;

		// 3.3 vNodes ����.
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


// Ž��
// 1. key == ���� ��� -> ���� ��尡 ã�� ���.
// 2. key < ���� ��� -> ã�� ���� ���� ����� ���ʿ� ����.
// 3. key > ���� ��� -> ã�� ���� ���� ����� �����ʿ� ����.
BSTNode* BST::find(int key) 
{
	//1. ��Ʈ ���κ��� ����.
	if ((this->RootNode)) {
		//1.A ���� ����� �������� ������.
		if ((key < this->RootNode->key)) {
			return find_internal(key, this->RootNode->left); // ���� ��尡 �����ϴ����� internal �Լ����� ó��.
		}
		//1.B      ""     ������   "".
		else if (key > this->RootNode->key) {
			return find_internal(key, this->RootNode->right);
		}
		//1.C ��Ʈ ��尡 ����.
		else return this->RootNode;
	}
	else {
		return nullptr;
	}
}


BSTNode* BST::find_internal(int key, BSTNode* CurrentNode)
{
	//0. ���� ��� null check
	if (!CurrentNode) {
		return nullptr;
	}

	//1.A ���� ����� �������� ������.
	if ((key < CurrentNode->key)) {
		return find_internal(key, CurrentNode->left); // ���� ��尡 �����ϴ����� internal �Լ����� ó��
	}
	//1.B      ""     ������   ""
	else if (key > CurrentNode->key) {
		return find_internal(key, CurrentNode->right);
	}
	//1.C ��Ʈ ��尡 ����.
	else return CurrentNode;

}


// �Է����� ���� ��尡 leaf ������� üũ�ϴ� �Լ�
bool BST::IsLeafNode(BSTNode* CurrentNode)
{
	// 0. �Ķ���� null check
	if (!CurrentNode) return false;

	// 1. leafnode���� üũ( ���� ����� right�� left�� ��� nullptr�̸� leafnode )
	if (!(CurrentNode->right) && !(CurrentNode->left)) return true;
	return false;
}


// ����
// find ������ ���󰡸鼭, ���� ��尡 nullptr�� �ɶ����� �ݺ��ϰ�, ���� ��尡 nullptr�̸�, �� ��ġ�� ��带 �����.
BSTNode* BST::insert(int key, int value)
{
	//1. ���� ��� == ��Ʈ ���.
	if (!(this->RootNode)) return nullptr;
	BSTNode* CurrentNode = this->RootNode;
	BSTNode* ParentNode = nullptr;


	//2. ���ǿ� �´� ParentNode�� ã�� ������ �ݺ�.
	while (true) {

		//2.1 key�� ���� ���� ���� Ž��.
		//2.1.A ���� ����� �������� ������.
		if ((key < CurrentNode->key)) {
			//2.1.A.A ���� ��尡 nullptr -> ����ٰ� ���Ӱ� �����.
			if (!(CurrentNode->left)) {
				BSTNode* newNode = new BSTNode(key, value);
				CurrentNode->left = newNode;
				newNode->parent = CurrentNode;
				newNode->LeftOrRight = 1;

				return CurrentNode->left;
			}

			//2.1.A.B nullptr -> ���� ��� Ž��.
			else {
				ParentNode = CurrentNode;
				CurrentNode = CurrentNode->left;
			}
		}
		//2.1.B      ""     ������   ""
		else if (key > CurrentNode->key) {
			//2.1.B.A ������ ��尡 nullptr -> ����ٰ� ���Ӱ� �����.
			if (!(CurrentNode->right)) {
				BSTNode* newNode = new BSTNode(key, value);
				CurrentNode->right = newNode;
				newNode->parent = CurrentNode;
				newNode->LeftOrRight = 2;

				return CurrentNode->right;
			}

			//2.1.B.B nullptr -> ���� ��� Ž��.
			else {
				ParentNode = CurrentNode;
				CurrentNode = CurrentNode->right;
			}
		}
		//2.1.C ���� ��尡 ���� �����ϰ��� �ߴ� key .
		else return CurrentNode;

	}
}

// �� ����� ��ġ�� �¹ٲٴ� �Լ�
BSTNode* BST::swapNode(BSTNode* NodeA, BSTNode* NodeB)
{
	// 0. null üũ
	if (!NodeA || !NodeB) return nullptr;

	BSTNode* tmp = nullptr;
	int nTmp = -1;

	//1. parent �¹ٲٱ�
	//1.1 �θ� -> ��� �� �ٲٱ�
	if (NodeA->LeftOrRight == 1) { // �� ���� �θ� ����� ���� ���
		if (NodeB->LeftOrRight == 1) { // ���⵵ ����
			tmp = NodeA->parent->left;
			NodeA->parent->left = NodeB->parent->left;
			NodeB->parent->left = tmp;
		}
		else if (NodeB->LeftOrRight == 2) {
			tmp = NodeA->parent->left;
			NodeA->parent->left = NodeB->parent->right;
			NodeB->parent->right = tmp;
		}
		else { // NodeB�� ��Ʈ ���
			NodeA->parent->left = NodeB;
		}
	}
	else if (NodeA->LeftOrRight = 2) { // ������
		if (NodeB->LeftOrRight == 1) { // ����
			tmp = NodeA->parent->right;
			NodeA->parent->right = NodeB->parent->left;
			NodeB->parent->left = tmp;
		}
		else if (NodeB->LeftOrRight == 2) {
			tmp = NodeA->parent->right;
			NodeA->parent->right = NodeB->parent->right;
			NodeB->parent->right = tmp;
		}
		else { // NodeB�� ��Ʈ ���
			NodeA->parent->right = NodeB;
		}
	}
	else { // ��� A�� ��Ʈ���
		if (NodeB->LeftOrRight == 1) { // ����
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

	//1.2 ��� -> �θ� �� �ٲٱ�
	tmp = NodeA->parent;
	NodeA->parent = NodeB->parent;
	NodeB->parent = tmp;

	//1.3 LeftOrRight �� �ٲٱ�
	nTmp = NodeA->LeftOrRight;
	NodeA->LeftOrRight = NodeB->LeftOrRight;
	NodeB->LeftOrRight = nTmp;


	//2. �ڽ� ��� �¹ٲٱ�
	//2.1 �ڽ� -> ���
	//2.1.A �Ѵ� ���� ���
	if (IsLeafNode(NodeA) && IsLeafNode(NodeB)) {
		// �Ұ� ����
	}
	//2.1.B NodeA�� ���� ���
	if (IsLeafNode(NodeA) && !IsLeafNode(NodeB)) {
		if (NodeB->left) {
			NodeB->left->parent = NodeA;
		}
		if (NodeB->right) {
			NodeB->right->parent = NodeA;
		}
	}
	//2.1.C NodeB�� ���� ���
	if (!IsLeafNode(NodeA) && !IsLeafNode(NodeB)) {
		if (NodeA->left) {
			NodeA->left->parent = NodeB;
		}
		if (NodeA->right) {
			NodeA->right->parent = NodeB;
		}
	}
	//2.1.D �Ѵ� ������� �ƴ�
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

	//2.2 ��� -> �ڽ�
	tmp = NodeA->left;
	NodeA->left = NodeB->left;
	NodeB->left = tmp;

	tmp = NodeA->right;
	NodeA->right = NodeB->right;
	NodeB->right = tmp;

	return NodeA;
}


//����
//1. ������ ��尡 Leaf ��� -> �θ� ��� ���� ���� �� ����
//2. �ڽ� ��尡 �ϳ��� ����� ���� -> �ڽ� �ڸ��� �ڽ� ��带 ����, �����ϸ� ��.
//3. �ڽĳ�尡 2���� ����� ����
// -> ������ ��带 �������� �ϴ� ���� Ʈ���� �����غ���, ��ü�� �� �߾Ӱ��� ������� ���̹Ƿ�,
// -> ���ο� �߾Ӱ��� ã�´�.
// -> ��, ������ ����� ���� ����Ʈ�� �� �ִ밪�� ���ų�, ������ ���� Ʈ�� �� �ּҰ��� ���� �ȴ�.
BSTNode* BST::DeleteNode(int key)
{
	//0. ã�� ��尡 �����ϴ��� üũ
	BSTNode* CurrentNode = find(key);
	if (!CurrentNode) return nullptr;

	//1. ó��
	//1.A ������ ��尡 Leaf���
	if (IsLeafNode(CurrentNode)) {
		//1.A.1 �θ� ��忡�� �ش� �ڽ� ��带 null
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = nullptr;
		}
		else {
			CurrentNode->parent->right = nullptr;
		}
		delete CurrentNode;
	}
	//1.B �ڽĳ�尡 �ϳ���.
	//���ʸ� ����
	else if ( ((CurrentNode->left) && !(CurrentNode->right)) ) {
		BSTNode* tmp = CurrentNode;
		//1.B.1 ������ ����� �θ� ��� ó��
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = CurrentNode->left;
		}
		else {
			CurrentNode->parent->right = CurrentNode->left;
		}

		//1.B.2 �ڽ� ����� �θ� ���� �θ�� �߰�
		CurrentNode->left->parent = CurrentNode->parent;
		delete tmp;
	}
	//�����ʸ� ����
	else if ( !((CurrentNode->left) && (CurrentNode->right)) ) {
		BSTNode* tmp = CurrentNode;
		//1.B.1 ������ ����� �θ� ��� ó��
		if (CurrentNode->parent->left == CurrentNode) {
			CurrentNode->parent->left = CurrentNode->right;
		}
		else {
			CurrentNode->parent->right = CurrentNode->right;
		}

		//1.B.2 �ڽ� ����� �θ� ���� �θ�� �߰�
		CurrentNode->right->parent = CurrentNode->parent;
		delete tmp;
	}

	// �Ѵ� ����
	else if (((CurrentNode->left) && (CurrentNode->right))) {
		//1. ���� ����Ʈ������ ���� ū ���� ã�´�. ( ���� ������ )
		BSTNode* leftMaxNode = CurrentNode->left;
		while (!IsLeafNode(leftMaxNode)) {
			leftMaxNode = leftMaxNode->right;
		}

		// 2. �ش� ���� ������ ����� ��ġ�� �����Ѵ�.
		// 2.1 �̵��� ����� �θ� ��� ó��, �̵��� ��忡���� �θ� ���� ����
		//leftMaxNode->parent->right = nullptr;
		//leftMaxNode->parent = nullptr;

		//2.2 ���� ��� �޸� ������ ���� tmp
		BSTNode* tmp = CurrentNode;

		// 2.3 ���� ��带 ���� ����Ʈ�� �ִ밪���� ġȯ
		// 2.3.1 ���� ����� key�� value�� ���� ����Ʈ�� �ִ밪���� ġȯ
		CurrentNode->key = leftMaxNode->key;
		CurrentNode->value = leftMaxNode->value;

		// 2.3.2 ���� ���� ����Ʈ�� �ִ밪 ��带 ���ֱ�
		// 2.3.2.A �ش� ��尡 ���� ��� -> 
		if (IsLeafNode(leftMaxNode)) {
			//1.A.1 �θ� ��忡�� �ش� �ڽ� ��带 null 
			leftMaxNode->parent->right = nullptr;
			delete leftMaxNode;
		}
		//2.3.2.B �ش� ��尡 ������尡 �ƴ�( ���� �ڽ� ��常 ���� )
		else {
			//2.3.2.B.1 ������ ����� �θ� ��� ó��
			leftMaxNode->parent->right = leftMaxNode->left;

			//1.B.2 �ڽ� ����� �θ� ���� �θ�� �߰�
			leftMaxNode->left->parent = leftMaxNode->parent;
			delete leftMaxNode;
		}
	}

	//exceptional cases
	else {
		;
	}
}


int main()
{
	BST BinarySearchTree = BST(1, 2);
	BinarySearchTree.printTree();
	
	BinarySearchTree.insert(5, 8);
	BinarySearchTree.printTree();

	BinarySearchTree.insert(10, 12);
	BinarySearchTree.printTree();

	BinarySearchTree.insert(2, 6);
	BinarySearchTree.printTree();

	BinarySearchTree.insert(3, 4); 
	BinarySearchTree.printTree();

	BinarySearchTree.insert(5, 5);
	BinarySearchTree.printTree();

	BinarySearchTree.DeleteNode(5);
	BinarySearchTree.printTree();
	
	return 0;
}