#include <memory>
#include <functional>
#include <iterator>
#include <utility>
#include <cstddef>

namespace bst {

    template <typename T>
    struct TreeNode {
        T value;//значение узла
        TreeNode* left = nullptr;//левое поддерево
        TreeNode* right = nullptr;//правое поддерево
        TreeNode* parent = nullptr;//родитель
        explicit TreeNode(const T& val) : value(val) {}//запрещает неявное преобразование
    };

    template <typename NodePtr, typename T>
    class InOrderIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = std::ptrdiff_t;

        InOrderIterator(NodePtr root = nullptr, NodePtr node = nullptr)
            : root_(root), node_(node) {
        }

        reference operator*() const { return node_->value; }
        pointer   operator->() const { return &node_->value; }

        InOrderIterator& operator++() {
            if (!node_) {
                node_ = root_;
                while (node_ && node_->left) node_ = node_->left;
                return *this;
            }
            if (node_->right) {
                node_ = node_->right;
                while (node_->left) node_ = node_->left;
            }
            else {
                NodePtr p = node_->parent;
                while (p && node_ == p->right) {
                    node_ = p;
                    p = p->parent;
                }
                node_ = p;
            }
            return *this;
        }
        InOrderIterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        InOrderIterator& operator--() {
            if (!node_) {
                node_ = root_;
                while (node_ && node_->right) node_ = node_->right;
                return *this;
            }
            if (node_->left) {
                node_ = node_->left;
                while (node_->right) node_ = node_->right;
            }
            else {
                NodePtr p = node_->parent;
                while (p && node_ == p->left) {
                    node_ = p;
                    p = p->parent;
                }
                node_ = p;
            }
            return *this;
        }
        InOrderIterator operator--(int) { auto tmp = *this; --*this; return tmp; }

        bool operator==(const InOrderIterator& o) const { return node_ == o.node_; }
        bool operator!=(const InOrderIterator& o) const { return node_ != o.node_; }

    private:
        NodePtr root_;
        NodePtr node_;
    };

    template <typename NodePtr, typename T>
    class PreOrderIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = std::ptrdiff_t;

        PreOrderIterator(NodePtr node = nullptr) : node_(node) {}
        reference operator*() const { return node_->value; }
        pointer   operator->() const { return &node_->value; }

        PreOrderIterator& operator++() {
            if (!node_) return *this;
            if (node_->left) node_ = node_->left;
            else if (node_->right) node_ = node_->right;
            else {
                NodePtr p = node_->parent;
                while (p) {
                    if (node_ == p->left && p->right) {
                        node_ = p->right;
                        return *this;
                    }
                    node_ = p;
                    p = p->parent;
                }
                node_ = nullptr;
            }
            return *this;
        }
        PreOrderIterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        PreOrderIterator& operator--() {
            return *this;
        }
        PreOrderIterator operator--(int) { auto tmp = *this; --*this; return tmp; }

        bool operator==(const PreOrderIterator& o) const { return node_ == o.node_; }
        bool operator!=(const PreOrderIterator& o) const { return node_ != o.node_; }

    private:
        NodePtr node_;
    };

    template <typename NodePtr, typename T>
    class PostOrderIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using difference_type = std::ptrdiff_t;

        PostOrderIterator(NodePtr node = nullptr) : node_(node) {}
        reference operator*() const { return node_->value; }
        pointer   operator->() const { return &node_->value; }

        PostOrderIterator& operator++() {
            if (!node_) return *this;
            NodePtr p = node_->parent;
            if (!p) {
                node_ = nullptr;
                return *this;
            }
            if (node_ == p->left && p->right) {
                node_ = p->right;
                while (node_->left || node_->right) {
                    if (node_->left)  node_ = node_->left;
                    else              node_ = node_->right;
                }
            }
            else {
                node_ = p;
            }
            return *this;
        }
        PostOrderIterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        PostOrderIterator& operator--() {
            return *this;
        }
        PostOrderIterator operator--(int) { auto tmp = *this; --*this; return tmp; }

        bool operator==(const PostOrderIterator& o) const { return node_ == o.node_; }
        bool operator!=(const PostOrderIterator& o) const { return node_ != o.node_; }

    private:
        NodePtr node_;
    };

    template <typename T,
        typename Compare = std::less<T>,
        typename Allocator = std::allocator<T>>
        class BinarySearchTree {
        public:
            using value_type = T;
            using key_type = T;
            using key_compare = Compare;
            using allocator_type = Allocator;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = T&;
            using const_reference = const T&;

            using inorder_iterator = InOrderIterator<TreeNode<T>*, T>;
            using const_inorder_iterator = InOrderIterator<const TreeNode<T>*, T>;
            using preorder_iterator = PreOrderIterator<TreeNode<T>*, T>;
            using const_preorder_iterator = PreOrderIterator<const TreeNode<T>*, T>;
            using postorder_iterator = PostOrderIterator<TreeNode<T>*, T>;
            using const_postorder_iterator = PostOrderIterator<const TreeNode<T>*, T>;
            using reverse_iterator = std::reverse_iterator<inorder_iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_inorder_iterator>;

            BinarySearchTree(const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                : root_(nullptr), node_count_(0), comp_(comp), alloc_(alloc) {
            }

            ~BinarySearchTree() { clear(); }

            bool empty() const noexcept { return node_count_ == 0; }
            size_type size() const noexcept { return node_count_; }

            std::pair<inorder_iterator, bool> insert(const T& value) {
                TreeNode<T>* parent = nullptr;
                auto curr = root_;
                while (curr) {
                    parent = curr;
                    if (comp_(value, curr->value))       curr = curr->left;
                    else if (comp_(curr->value, value))  curr = curr->right;
                    else return { inorder_iterator(root_, curr), false };
                }
                auto node = new TreeNode<T>(value);
                node->parent = parent;
                if (!parent)       root_ = node;
                else if (comp_(value, parent->value)) parent->left = node;
                else                       parent->right = node;
                ++node_count_;
                return { inorder_iterator(root_, node), true };
            }

            size_type erase(const T& key) {
                auto node = find_node(key);
                if (!node) return 0;
                remove_node(node);
                --node_count_;
                return 1;
            }

            void clear() noexcept {
                destroy_subtree(root_);
                root_ = nullptr;
                node_count_ = 0;
            }

            inorder_iterator begin() noexcept {
                TreeNode<T>* curr = root_;
                while (curr && curr->left) curr = curr->left;
                return inorder_iterator(root_, curr);
            }
            inorder_iterator end() noexcept {
                return inorder_iterator(root_, nullptr);
            }
            const_inorder_iterator begin() const noexcept { return const_cast<BinarySearchTree*>(this)->begin(); }
            const_inorder_iterator end()   const noexcept { return const_cast<BinarySearchTree*>(this)->end(); }

            preorder_iterator pre_begin() noexcept { return preorder_iterator(root_); }
            preorder_iterator pre_end()   noexcept { return preorder_iterator(nullptr); }
            const_preorder_iterator pre_begin() const noexcept { return const_cast<BinarySearchTree*>(this)->pre_begin(); }
            const_preorder_iterator pre_end()   const noexcept { return const_cast<BinarySearchTree*>(this)->pre_end(); }

            postorder_iterator post_begin() noexcept {
                TreeNode<T>* curr = root_;
                while (curr && (curr->left || curr->right)) {
                    if (curr->left)  curr = curr->left;
                    else             curr = curr->right;
                }
                return postorder_iterator(curr);
            }
            postorder_iterator post_end()   noexcept { return postorder_iterator(nullptr); }
            const_postorder_iterator post_begin() const noexcept { return const_cast<BinarySearchTree*>(this)->post_begin(); }
            const_postorder_iterator post_end()   const noexcept { return const_cast<BinarySearchTree*>(this)->post_end(); }

            inorder_iterator find(const T& key) { return inorder_iterator(root_, find_node(key)); }
            const_inorder_iterator find(const T& key) const { return const_inorder_iterator(root_, find_node(key)); }

            reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
            reverse_iterator rend()   noexcept { return reverse_iterator(begin()); }
            const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
            const_reverse_iterator rend()   const noexcept { return const_reverse_iterator(begin()); }

        private:
            TreeNode<T>* root_ = nullptr;
            size_type    node_count_ = 0;
            Compare      comp_;
            Allocator    alloc_;

            void destroy_subtree(TreeNode<T>* node) noexcept {
                if (!node) return;
                destroy_subtree(node->left);
                destroy_subtree(node->right);
                delete node;
            }

            TreeNode<T>* find_node(const T& key) const {
                auto curr = root_;
                while (curr) {
                    if (comp_(key, curr->value))       curr = curr->left;
                    else if (comp_(curr->value, key))  curr = curr->right;
                    else return curr;
                }
                return nullptr;
            }

            void transplant(TreeNode<T>* u, TreeNode<T>* v) {
                if (!u->parent)            root_ = v;
                else if (u == u->parent->left)  u->parent->left = v;
                else                       u->parent->right = v;
                if (v) v->parent = u->parent;
            }

            void remove_node(TreeNode<T>* z) {
                if (!z->left) {
                    transplant(z, z->right);
                }
                else if (!z->right) {
                    transplant(z, z->left);
                }
                else {
                    auto y = z->right;
                    while (y->left) y = y->left;
                    if (y->parent != z) {
                        transplant(y, y->right);
                        y->right = z->right;
                        y->right->parent = y;
                    }
                    transplant(z, y);
                    y->left = z->left;
                    y->left->parent = y;
                }
                delete z;
            }
    };

} 