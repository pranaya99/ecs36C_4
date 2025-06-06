#ifndef Map_H_
#define Map_H_

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

template <typename K, typename V>
class Map {
 public:
  unsigned int Size();
  const V& Get(const K& key);
  bool Contains(const K& key);
  const K& Max();
  const K& Min();
  void Insert(const K& key, const V& value);
  void Remove(const K& key);
  void Print();

 private:
  enum Color { RED, BLACK };
  struct Node {
    K key;
    std::vector<V> values;  // Store a list of values for the same key
    bool color;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
  };
  std::unique_ptr<Node> root;
  unsigned int cur_size = 0;

  Node* Get(Node* n, const K& key);
  Node* Min(Node* n);
  void Insert(std::unique_ptr<Node>& n, const K& key, const V& value);
  void Remove(std::unique_ptr<Node>& n, const K& key);
  void Print(Node* n);

  bool IsRed(Node* n);
  void FlipColors(Node* n);
  void RotateRight(std::unique_ptr<Node>& prt);
  void RotateLeft(std::unique_ptr<Node>& prt);
  void FixUp(std::unique_ptr<Node>& n);
  void MoveRedRight(std::unique_ptr<Node>& n);
  void MoveRedLeft(std::unique_ptr<Node>& n);
  void DeleteMin(std::unique_ptr<Node>& n);
};

template <typename K, typename V>
unsigned int Map<K, V>::Size() {
  return cur_size;
}

template <typename K, typename V>
typename Map<K, V>::Node* Map<K, V>::Get(Node* n, const K& key) {
  while (n) {
    if (key == n->key) return n;
    if (key < n->key) n = n->left.get();
    else n = n->right.get();
  }
  return nullptr;
}

template <typename K, typename V>
const V& Map<K, V>::Get(const K& key) {
  Node* n = Get(root.get(), key);
  if (!n) throw std::runtime_error("Error: cannot find key");
  return n->values.front();  // Return the first value
}

template <typename K, typename V>
bool Map<K, V>::Contains(const K& key) {
  return Get(root.get(), key) != nullptr;
}

template <typename K, typename V>
const K& Map<K, V>::Max() {
  Node* n = root.get();
  while (n->right) n = n->right.get();
  return n->key;
}

template <typename K, typename V>
const K& Map<K, V>::Min() {
  return Min(root.get())->key;
}

template <typename K, typename V>
typename Map<K, V>::Node* Map<K, V>::Min(Node* n) {
  if (n->left) return Min(n->left.get());
  else return n;
}

template <typename K, typename V>
bool Map<K, V>::IsRed(Node* n) {
  if (!n) return false;
  return (n->color == RED);
}

template <typename K, typename V>
void Map<K, V>::FlipColors(Node* n) {
  n->color = !n->color;
  n->left->color = !n->left->color;
  n->right->color = !n->right->color;
}

template <typename K, typename V>
void Map<K, V>::RotateRight(std::unique_ptr<Node>& prt) {
  std::unique_ptr<Node> chd = std::move(prt->left);
  prt->left = std::move(chd->right);
  chd->color = prt->color;
  prt->color = RED;
  chd->right = std::move(prt);
  prt = std::move(chd);
}

template <typename K, typename V>
void Map<K, V>::RotateLeft(std::unique_ptr<Node>& prt) {
  std::unique_ptr<Node> chd = std::move(prt->right);
  prt->right = std::move(chd->left);
  chd->color = prt->color;
  prt->color = RED;
  chd->left = std::move(prt);
  prt = std::move(chd);
}

template <typename K, typename V>
void Map<K, V>::FixUp(std::unique_ptr<Node>& n) {
  if (IsRed(n->right.get()) && !IsRed(n->left.get())) RotateLeft(n);
  if (IsRed(n->left.get()) && IsRed(n->left->left.get())) RotateRight(n);
  if (IsRed(n->left.get()) && IsRed(n->right.get())) FlipColors(n.get());
}

template <typename K, typename V>
void Map<K, V>::MoveRedRight(std::unique_ptr<Node>& n) {
  FlipColors(n.get());
  if (IsRed(n->left->left.get())) {
    RotateRight(n);
    FlipColors(n.get());
  }
}

template <typename K, typename V>
void Map<K, V>::MoveRedLeft(std::unique_ptr<Node>& n) {
  FlipColors(n.get());
  if (IsRed(n->right->left.get())) {
    RotateRight(n->right);
    RotateLeft(n);
    FlipColors(n.get());
  }
}

template <typename K, typename V>
void Map<K, V>::DeleteMin(std::unique_ptr<Node>& n) {
  if (!n->left) {
    n = nullptr;
    return;
  }

  if (!IsRed(n->left.get()) && !IsRed(n->left->left.get()))
    MoveRedLeft(n);

  DeleteMin(n->left);
  FixUp(n);
}

template <typename K, typename V>
void Map<K, V>::Remove(const K& key) {
  if (!Contains(key)) return;
  Remove(root, key);
  cur_size--;
  if (root) root->color = BLACK;
}

template <typename K, typename V>
void Map<K, V>::Remove(std::unique_ptr<Node>& n, const K& key) {
  if (!n) return;

  if (key < n->key) {
    if (!IsRed(n->left.get()) && !IsRed(n->left->left.get()))
      MoveRedLeft(n);
    Remove(n->left, key);
  } else {
    if (IsRed(n->left.get())) RotateRight(n);

    if (key == n->key && n->values.size() == 1) {
      n = nullptr;
      return;
    }

    if (!IsRed(n->right.get()) && !IsRed(n->right->left.get()))
      MoveRedRight(n);

    if (key == n->key) {
      n->values.erase(n->values.begin());
      if (n->values.empty()) {
        n = nullptr;
        return;
      }
    } else {
      Remove(n->right, key);
    }
  }

  FixUp(n);
}

template <typename K, typename V>
void Map<K, V>::Insert(const K& key, const V& value) {
  Insert(root, key, value);
  cur_size++;
  root->color = BLACK;
}

template <typename K, typename V>
void Map<K, V>::Insert(std::unique_ptr<Node>& n, const K& key, const V& value) {
  if (!n) {
    n = std::unique_ptr<Node>(new Node{key, {value}, RED});
  } else if (key < n->key) {
    Insert(n->left, key, value);
  } else if (key > n->key) {
    Insert(n->right, key, value);
  } else {
    n->values.push_back(value);
  }

  FixUp(n);
}

template <typename K, typename V>
void Map<K, V>::Print() {
  Print(root.get());
  std::cout << std::endl;
}

template <typename K, typename V>
void Map<K, V>::Print(Node* n) {
  if (!n) return;
  Print(n->left.get());
  std::cout << "<" << n->key << ": ";
  for (const auto& val : n->values) {
    std::cout << val << " ";
  }
  std::cout << "> ";
  Print(n->right.get());
}

#endif  // Map_H_
