template<class T>
class list {
    struct _list_node {
        _list_node * next;
        _list_node * prev;
        T data;
    };

    int _size;
    _list_node head[1];

    void _insert(const T & _data, _list_node * now) {
        _list_node * new_node = new _list_node(); 
        new_node->data = _data;
        new_node->prev = now;
        new_node->next = now->next;
        new_node->prev->next = new_node;
        new_node->next->prev = new_node;
        _size++;
    };

    T _remove(_list_node * deleted) {
        T ret = deleted->data;
        if (_size > 0) {
            deleted->prev->next = deleted->next;
            deleted->next->prev = deleted->prev;
            delete deleted;
            _size--;
        }
        return ret;
    };

public:
    list() : head(), _size(0) {
        head->prev = head;
        head->next = head;
    };

    void push_front(const T & _data) {
        _insert(_data, head);
    };

    void push_back(const T & _data) {
        _insert(_data, head->prev);
    };

    T pop_front() {
        return _remove(head->next);
    };

    T pop_back(){
        return _remove(head->prev);
    };

    int size() {
        return _size;
    };

    ~list() {
        for (_list_node * temp = head->next; temp != head;) {
            _list_node * next = temp->next;
            delete temp;
            temp = next;
        }
    };
};
