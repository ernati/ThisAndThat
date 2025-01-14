#include <iostream>
#include <vector>
#include <algorithm>


/*

�ڰ� ���� ���� Ž�� Ʈ�� ( AVL Tree )

< ���� >
1. ��� ���� ������ Ű�� ���´�.
2. ���� ���� Ʈ���� Ű���� ��Ʈ�� Ű���� �۴�.
3. ������ ���� Ʈ���� Ű���� ��Ʈ�� Ű���� ũ��.
4. ���ʰ� ������ ���� Ʈ���� ���� Ž�� Ʈ���̴�.

<AVL Tree>
5. �� �ڽ� ���� Ʈ���� ���̴� �׻� �ִ� 1��ŭ ���̳���.
6. � �������� ���� ���̰� 1���� Ŀ���� �� �Ӽ��� �����ϱ� ���ؼ� ������ ������ ��´�. 

*/


/* ���� */
//1. Node : Key �� Value�� ����, ���� left �� Right ���� ��带 ����
class AVLTreeNode {
public:
	AVLTreeNode(int key, int value);

public:
	int key;
	int value;
	AVLTreeNode* left;
	AVLTreeNode* right;
	AVLTreeNode* parent;
	int LeftOrRight; // 0 : �θ� ��� ����, 1 : �θ� ����� left �ڽ�, 2 : �θ� ����� right �ڽ�

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


//2. AVLTree : ��Ʈ ��尪�� ���ڷ� �޾Ƽ� �ʱ�ȭ��. Ž��, ����, ���� ���� ����.
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

// ������ : key, value ���� ���ڷ� �޾Ƽ�, �ش� ���� ������ RootNode�� ������.
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

// Ʈ���� ����ϴ� �Լ�
void AVLTree::printTree()
{
	//1. vector ����.
	std::vector<AVLTreeNode*> vNodes;
	std::vector<AVLTreeNode*> vTmp;

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
			if (nullptr != vNodes[i]) {
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
AVLTreeNode* AVLTree::find(int key)
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


AVLTreeNode* AVLTree::find_internal(int key, AVLTreeNode* CurrentNode)
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
bool AVLTree::IsLeafNode(AVLTreeNode* CurrentNode)
{
	// 0. �Ķ���� null check
	if (!CurrentNode) return false;

	// 1. leafnode���� üũ( ���� ����� right�� left�� ��� nullptr�̸� leafnode )
	if (!(CurrentNode->right) && !(CurrentNode->left)) return true;
	return false;
}


// ����
// find ������ ���󰡸鼭, ���� ��尡 nullptr�� �ɶ����� �ݺ��ϰ�, ���� ��尡 nullptr�̸�, �� ��ġ�� ��带 �����.
AVLTreeNode* AVLTree::insert(int key, int value)
{
	//1. ���� ��� == ��Ʈ ���.
	if (!(this->RootNode)) return nullptr;
	AVLTreeNode* CurrentNode = this->RootNode;
	AVLTreeNode* ParentNode = nullptr;


	//2. ���ǿ� �´� ParentNode�� ã�� ������ �ݺ�.
	while (true) {

		//2.1 key�� ���� ���� ���� Ž��.
		//2.1.A ���� ����� �������� ������.
		if ((key < CurrentNode->key)) {
			//2.1.A.A ���� ��尡 nullptr -> ����ٰ� ���Ӱ� �����.
			if (!(CurrentNode->left)) {
				AVLTreeNode* newNode = new AVLTreeNode(key, value);
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
				AVLTreeNode* newNode = new AVLTreeNode(key, value);
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
AVLTreeNode* AVLTree::swapNode(AVLTreeNode* NodeA, AVLTreeNode* NodeB)
{
	// 0. null üũ
	if (!NodeA || !NodeB) return nullptr;

	AVLTreeNode* tmp = nullptr;
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
AVLTreeNode* AVLTree::DeleteNode(int key)
{
	//0. ã�� ��尡 �����ϴ��� üũ
	AVLTreeNode* CurrentNode = find(key);
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
	else if (((CurrentNode->left) && !(CurrentNode->right))) {
		AVLTreeNode* tmp = CurrentNode;
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
	else if (!((CurrentNode->left) && (CurrentNode->right))) {
		AVLTreeNode* tmp = CurrentNode;
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
		AVLTreeNode* leftMaxNode = CurrentNode->left;
		while (!IsLeafNode(leftMaxNode)) {
			leftMaxNode = leftMaxNode->right;
		}

		// 2. �ش� ���� ������ ����� ��ġ�� �����Ѵ�.
		// 2.1 �̵��� ����� �θ� ��� ó��, �̵��� ��忡���� �θ� ���� ����
		//leftMaxNode->parent->right = nullptr;
		//leftMaxNode->parent = nullptr;

		//2.2 ���� ��� �޸� ������ ���� tmp
		AVLTreeNode* tmp = CurrentNode;

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

void AVLTree::VisitNode(AVLTreeNode* Node, std::vector<AVLTreeNode*>& vAVLTreeNodes)
{
	//0. null check
	if (!Node) return;

	//1. Node�� leafnode���� üũ
	//1.A leafNode -> ���Ϳ� ��带 ��� ����
	if (IsLeafNode(Node)) {
		vAVLTreeNodes.push_back(Node);
	}
	//1.B leafNode�� �ƴ� -> �¿� �Ѵ� ����
	else {
		VisitNode(Node->left, vAVLTreeNodes);
		VisitNode(Node->right, vAVLTreeNodes);
	}

	return;
}

void AVLTree::VisitAllNodesAndPutLeafNodesIntoVector(std::vector<AVLTreeNode*>& vAVLTreeNodes)
{
	//0. ��Ʈ ��� null check
	if (!this->RootNode) return;

	//1. ��Ʈ ��� �湮 ����
	VisitNode(this->RootNode, vAVLTreeNodes);
}

void AVLTree::MeasureHeights()
{
	//0. ��Ʈ ��尡 ����ִ��� üũ�Ѵ�.
	if (!this->RootNode) return;

	std::vector<AVLTreeNode*> vAVLTreeNodes;
	//1. ���� Ʈ���� �����ϴ� ��� leafnode���� ���Ѵ�.
	//1.1 Ʈ���� �����ϴ� ��� ������ �湮�Ͽ�, leafnode���� ���Ϳ� ��´�.
	VisitAllNodesAndPutLeafNodesIntoVector(vAVLTreeNodes);

	//2. ��� leafnode��κ��� RootNode���� �ö󰡴µ�, �ö� ������ ������ ���� ���� �����Ѵ�.
	AVLTreeNode* TmpNode = nullptr;
	for (auto it = vAVLTreeNodes.begin(), end = vAVLTreeNodes.end(); it != end; it++) {
		//2.1 leafnode�� heights�� 0
		TmpNode = *it;
		TmpNode->heights = 0;

		//2.2 leafnode ���� root ���� �ö󰣴�.
		while (TmpNode->parent != nullptr) {
			//2.2.1 �θ��� heights�� �����Ѵ�. ( ���� height��, �ڽĳ��height +1 �� �ִ밪���� ���� )
			TmpNode->parent->heights = std::max(TmpNode->parent->heights, TmpNode->heights + 1);

			//2.2.2 �ӽ� ��带 �θ� ���� �ٲ۴�.
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