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
};

BSTNode::BSTNode(int key, int value) {
	this->key = key;
	this->value = value;

	this->left = nullptr;
	this->right = nullptr;
	this->parent = nullptr;
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
		leftMaxNode->parent->right = nullptr;
		leftMaxNode->parent = nullptr;

		//2.2 ���� ��� �޸� ������ ���� tmp
		BSTNode* tmp = CurrentNode;

		// 2.3 ���� ��带 ���� ����Ʈ�� �ִ밪���� ġȯ
		// 2.3.1 �ִ밪 ����� �θ� CurrentNode�� �θ�� ġȯ
		leftMaxNode->parent = CurrentNode->parent;
		//2.3.2 �ڽ�
		leftMaxNode->right = CurrentNode->right;
		leftMaxNode->left = CurrentNode->left;

		printf("");
		//3. ���� ��� �޸� ����
		delete tmp;
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