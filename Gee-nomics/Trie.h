#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
#include <list>
using namespace std;

template<typename ValueType>
class Trie
{
public:
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;

      // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
	struct Node {
		char label;
		vector<ValueType> values;
		list<Node*> children;
	};
	Node* m_root;
	void insertHelper(const std::string& key, const ValueType& value, Node* n);
	void findHelper(const std::string& key, bool exactMatchOnly, Node* n, std::vector<ValueType>& v, bool isFirst) const;
	void clearAll(Node* n);
};

template<typename ValueType>
Trie<ValueType>::Trie() {
	m_root = new Node;
}

// work on this
template<typename ValueType>
void Trie<ValueType>::clearAll(Node* n) {
	// stopping condition, leaf node
	if (n->children.size() == 0) {
		delete n;
		return;
	}
	// recursion for every subtree
	typename list<Node*>::iterator it = n->children.begin();
	while (it != n->children.end()) {
		clearAll(*it);
		it = n->children.erase(it);
	}
	// delete current node
	delete n;
}

template<typename ValueType>
Trie<ValueType>::~Trie() {
	clearAll(m_root);
}

template<typename ValueType>
void Trie<ValueType>::reset() {
	clearAll(m_root);
	m_root = new Node;
}

template<typename ValueType>
void Trie<ValueType>::insert(const string& key, const ValueType& value) {
	insertHelper(key, value, m_root);
}

// private helper function
template<typename ValueType>
void Trie<ValueType>::insertHelper(const string& key, const ValueType& value, Node* n) {
	// stopping condition, insert key
	if (key.size() == 0) {
		n->values.push_back(value);
		return;
	}
	// go to corresponding child node
	char label = key[0];
	typename list<Node*>::const_iterator it = n->children.begin();
	while (it != n->children.end()) {
		if ((*it)->label < label) {
			it++;
		}
		else {
			break;
		}
	}
	// insert new child at the end
	if (it == n->children.end()) {
		Node* temp = new Node();
		temp->label = label;
		n->children.insert(it, temp);
		it--;
		insertHelper(key.substr(1), value, *it);
	}
	// if child already exists
	else if ((*it)->label == label) {
		insertHelper(key.substr(1), value, *it);
	}
	// else insert new child in correct spot
	else {
		Node* temp = new Node();
		temp->label = label;
		n->children.insert(it, temp);
		it--;
		insertHelper(key.substr(1), value, *it);
	}
}

template<typename ValueType>
vector<ValueType> Trie<ValueType>::find(const string& key, bool exactMatchOnly) const {
	vector<ValueType> v;
	findHelper(key, exactMatchOnly, m_root, v, true);
	return v;
}

// done?
template<typename ValueType>
void Trie<ValueType>::findHelper(const string& key, bool exactMatchOnly, Node* n, vector<ValueType>& v, bool isFirst) const {
	if (key.size() == 0) {
		for (int i = 0; i < n->values.size(); i++) {
			//if (n->values[i] != nullptr) {
				v.push_back(n->values[i]);
			//}
		}
		return;
	}
	if (exactMatchOnly || isFirst) {
		typename list<Node*>::iterator it = n->children.begin();
		while (it != n->children.end()) {
			if (key[0] == (*it)->label) {
				findHelper(key.substr(1), exactMatchOnly, *it, v, false);
				break;
			}
			it++;
		}
	}
	else {
		typename list<Node*>::iterator it = n->children.begin();
		while (it != n->children.end()) {
			if (key[0] == (*it)->label) {
				findHelper(key.substr(1), false, *it, v, false);
			}
			else {
				findHelper(key.substr(1), true, *it, v, false);
			}
			it++;
		}
	}
}

#endif // TRIE_INCLUDED
