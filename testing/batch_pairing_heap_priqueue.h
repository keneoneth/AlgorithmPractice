#ifndef BATCH_PAIRING_HEAP_PRIQUEUE
#define BATCH_PAIRING_HEAP_PRIQUEUE

#include <cassert>
#include <stack>

template <typename T>
struct heap_node {
    const T key;
    heap_node<T> * left_child;
    heap_node<T> * next_sibling;

    heap_node(const T &key, heap_node<T> * left_child=nullptr, heap_node<T> * next_sibling=nullptr) : key(key),left_child(left_child), next_sibling(next_sibling)  {}

    void add_child(heap_node<T> * node) {
        if (left_child==nullptr){
            // printf("left node\n");
            left_child = node;
        } else {
            // printf("sib node\n");
            node->next_sibling = left_child;
            left_child = node;
        }
    }
};

template <typename T>
struct two_pass_ret {
    heap_node<T> * merge_node = nullptr;
    heap_node<T> * new_node = nullptr;
};


template <typename T>
class batch_pairing_heap_priqueue {

private:
    typedef T key_type;
    heap_node<key_type> * root = nullptr;
    size_t total_num = 0;


    heap_node<T> * insert(heap_node<T> * node, const T& key){
        auto * node_ptr = new heap_node(key);
        return merge(node, node_ptr);
    }

    heap_node<T> * merge(heap_node<T> *&A, heap_node<T> *&B) {
 
        // If any of the two-nodes is None
        // then return the not None node
        if(A == nullptr)
            return B;
        if(B == nullptr)
            return A;

        // printf("> check B->key%d\n",(B->key));
        // printf("> check A->key%d\n",(A->key));

        // To maintain the min heap condition compare
        // the nodes and node with minimum value become
        // parent of the other node
        if(A->key < B->key) {
            // printf("A->add_child(B)\n");
            A->add_child(B);
            // printf("check A->left_child%d\n",(A->left_child==nullptr));
            // printf("check A->left_child->key%d\n",(A->left_child->key));
            // printf("check B->key%d\n",(B->key));
            // printf("check A->key%d\n",(A->key));
            // printf("check A->next_sibling%d\n",(A->next_sibling==nullptr));
            return A;
        }
        
        B->add_child(A);
        // printf("check B->left_child%d\n",(B->left_child==nullptr));
        // printf("check B->left_child->key%d\n",(B->left_child->key));
        // printf("check B->key%d\n",(B->key));
        // printf("check A->key%d\n",(A->key));
        // printf("check B->next_sibling%d\n",(B->next_sibling==nullptr));
        return B;
    }

    heap_node<T> * erase(heap_node<T> * & node) {
        // printf("erase %d\n",node->key);
        
        heap_node<T> * cur_node = node->left_child;
        heap_node<T> * merge_node = nullptr;

        // std::stack<heap_node<T> *> to_merge_stk;
        while(true) {
            if(cur_node == nullptr || cur_node->next_sibling == nullptr)
                break;
            
            two_pass_ret<T> ret = two_pass_merge(cur_node);
            // printf("new_node %d\n",ret.new_node->key);
            // printf("merge %d\n",ret.merge->key);
            // to_merge_stk.push(std::move(ret.merge_node));
            
            cur_node = ret.new_node;
            merge_node = merge(merge_node,ret.merge_node);
        }

        // while (!to_merge_stk.empty()) {
        //     cur_node = merge(cur_node,to_merge_stk.top());
        //     // printf("merge cur_node %d\n",cur_node->key);
        //     to_merge_stk.pop();
        // }
        
        delete node; // remove node
        return merge(cur_node,merge_node); // return node
    }

    two_pass_ret<T> two_pass_merge(heap_node<T> * node) {
        // printf("(node == nullptr || node->next_sibling == nullptr) %d\n",(node == nullptr || node->next_sibling == nullptr));

        
        
        auto A = node;
        auto B = node->next_sibling;
        // if(A!=nullptr) printf("A node %d\n",A->key);
        // if(B!=nullptr) printf("B node %d\n",B->key);

        auto new_node = node->next_sibling->next_sibling;
        // if(new_node!=nullptr) printf("new_node node %d\n",new_node->key);
    
        A->next_sibling = nullptr;
        B->next_sibling = nullptr;
    
        // return merge(merge(A, B), two_pass_merge(new_node));
        two_pass_ret<T> ret;
        ret.merge_node = merge(A, B);
        ret.new_node = new_node;
        return ret;
    }
        

public:
    
    size_t size(void) const {
        return total_num;
    }

    bool empty(void) const {
        // return root==nullptr;
        return total_num == 0;
    } 

    void push(const key_type& key) {
        // push new node
        // printf("insert key %d\n",key);
        // if(root!=nullptr) printf(">> root %d\n",root->key);
        root = insert(root,key);
        // printf(">> root %d\n",root->key);
        total_num++;
    }

    void batch_push(T* arr, size_t num) {
        // push by batch
        total_num += num;
    }

    void pop(void) {
        assert(!empty());
        // printf("!! pop %d\n",(root==nullptr));
        // pop top node
        root = erase(root);
        total_num--;
    }

    void batch_pop(size_t num) {
        assert(size()>=num);
        // pop by batch
        total_num -= num;
    }

    const T& top(void) const {
        assert(!empty());
        return root->key;
    }

    const T*& batch_top(void) const {
        assert(!empty());
        return nullptr;
    }



    


};


#endif //BATCH_PAIRING_HEAP_PRIQUEUE