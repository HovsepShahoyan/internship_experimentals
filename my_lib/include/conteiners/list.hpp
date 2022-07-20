#include <iostream>
#include <vector>

template<typename T> 
class Node {
public:
    Node<T>(): value{} {
        next = nullptr;
    }
    Node<T>(const T& m_value) {
        value = m_value;
    }
    Node<T>(Node<T>&& obj) {
        value = obj.value;
        next = obj.next;
    }
    Node<T>(const Node<T>& other): value{other.value}, next{other.next} {}
    ~Node<T>() {
        next = nullptr;
    }
public:
    T value;
    Node<T>* next;
};

template <class Node>
class list_allocator {
private:
   Node* _buf;
   size_t size;
public:
    void allocate(size_t count = 100) {
        _buf = reinterpret_cast<Node>(new uint8_t(count * sizeof(Node)));
        size = count;
    }
    template <typename... Args>
    void construct(Node* ptr, Args... args) {
        for (size_t i = 0; i < size; i++){
            if((_buf + i) == 0) {
                new (ptr) Node(args...);
                return;
            }
        }
        Node* buf_copy = new Node[size * 2];
        for(int i = 0; i < size; i++) {
            (buf_copy + i) = (_buf + i);  
        }
        _buf = nullptr;
        _buf = buf_copy;
    }
    void destroy(Node* ptr) {
        ptr->~Node();
    }
    void deallocate() {
        delete[] reinterpret_cast<uint8_t*>(_buf);
    }
};

template <typename T, typename Alloc = list_allocator<Node<T>>>
class Forward_List {
private:
    Node<T>* head;
    list_allocator<Node<T>> allocator;
public:
    Forward_List() {
        allocator.allocate();
        construct(head);
    }
    Forward_List(int count) {
        allocator.allocate(count);
        allocator.construct(head);
        auto curr = head;
        for(int i = 0; i < count - 1; ++i) {
            allocator.construct(curr->next);
            curr = curr->next;
        }
    }
    Forward_List(int count, const T& value) {
        head = new Node<T>(value);
        auto curr = head;
        for(int i = 0; i < count - 1; ++i) {
            curr->next = new Node<T>(value);
            curr = curr->next;
        }
    }
    Forward_List(const Forward_List<T>& obj) {
        allocator.construct(head, obj.head->value);
        Node<T>* curr = head;
        Node<T>* curr1 = obj.head->next;
        while(curr1 != nullptr) {
            allocator.construct(curr->next, curr1->value);
            curr = curr->next;
            curr1 = curr1->next;
        }
    }
    Forward_List(Forward_List&& obj) {
        head = obj.head;
        obj.head = nullptr;
    }
    ~Forward_List() {
        allocator.destroy(head);
        head = nullptr;
    }
    bool empty() const{
        return head == nullptr;
    }
    void print() const{
        auto curr = head;
        int i = 0;
        while(curr) {
            std::cout << i << ": -->" << curr->value << std::endl;
            curr = curr->next;
            i++; 
        }
    }
    void clear() {
        delete head;
        head = nullptr;
    }
    void insert(const T& value, int pos) {
        auto curr = head;
        int i = 0;
        while(pos != i && curr != nullptr) {
            ++i;
            curr = curr->next;
        }
        Node<T>* tmp = curr->next;
        tmp->value = value;
        curr->next = tmp;
        tmp = nullptr;
    }
    void erase_after(int pos) {
        auto curr = head;
        int i = 0;;
        while(i != pos && curr) {
            curr = curr->next;
            ++i;
        }
        if(curr->next) {
            Node<T>* tmp = curr->next;
            curr->next = curr->next->next;
            tmp = nullptr;
        }
    } 
    void push_front(const T& value) {
        Node<T>* tmp;
        allocator.allocate(tmp, value);
        tmp->next = head;
        head = tmp;
        tmp = nullptr;
    }
    void pop_front() {
        if(head != nullptr){
            Node<T>* tmp = head;
            head = head->next;
            delete tmp;
        }
    }
    void unique() {
        auto curr = head;
        while(curr->next != nullptr) {
            if(curr->value == curr->next->value) {
                auto tmp = curr->next;
                curr->next = curr->next->next;
                delete tmp;
            }
            else {
                curr = curr->next;
            }
        }    
    }
    void sort() {
        auto curr = head;
        int i = 0;
        while(curr != nullptr) {
            ++i;
            curr = curr->next;
        }
        curr = head;
        for(int j = i - 1 ; j > 0; j--) {
            curr = head;
            for(int k = 0; k < j; k++) {
                if(curr->value > curr->next->value){
                    T tmp = curr->value;
                    curr->value = curr->next->value;
                    curr->next->value = tmp;
                }
                curr = curr->next;
            }
        }
    }
};

int main() {
    Forward_List<int> f1(3,4);
    f1.push_front(1239);
    f1.push_front(1);
    f1.push_front(5);
    f1.push_front(10);
    f1.push_front(8);
    f1.push_front(9);
    f1.sort();
    f1.print();
}