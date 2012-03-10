#ifndef __LIST_HPP__
#define __LIST_HPP__

template<class T>
class list {
    class _node {
    public:
        T data;
        _node * prev;
        _node * next;

        _node() { prev = this; next = this; };
        _node(const T & data, _node * prev, _node * next) :
            data(data), prev(prev), next(next) {};
        ~_node() {};
    };

    _node head[1];
    unsigned int _size;

    void _insert(_node * now, const T & data) {
        _node * newNode = new _node(data, now, now->next);
        newNode->prev->next = newNode;
        newNode->next->prev = newNode;
        _size++;
    };
    void _remove(_node * now) {
        if (_size > 0) {
            now->prev->next = now->next;
            now->next->prev = now->prev;
            delete now;
            _size--;
        }
    };
public:
    struct iterator {
        _node * _iter;

        iterator() {};
        explicit iterator(_node * _iter) : _iter(_iter) {};
        T & operator*() const { return _iter->data; };
        iterator & operator++() { _iter = _iter->next; return *this; };
        iterator & operator--() { _iter = _iter->prev; return *this; };
        bool operator!=(const iterator & other) const { return _iter != other._iter; };
    };

    struct const_iterator {
        const _node * _iter;

        const_iterator() {};
        explicit const_iterator(const _node * _iter) : _iter(_iter) {};
        const_iterator(const iterator & other) : _iter(other._iter) {};
        const T & operator*() const { return _iter->data; };
        const_iterator & operator++() { _iter = _iter->next; return *this; };
        const_iterator & operator--() { _iter = _iter->prev; return *this; };
        bool operator!=(const const_iterator & other) const { return _iter != other._iter; };
    };

    list() : head(), _size(0) {};
    ~list() {
        for (_node * temp = head->next; temp != head; temp = temp->next) {
            delete temp;
        }
    }
    void push_front(const T & data) { _insert(head, data); };
    void pop_front() { return _remove(head->next); };
    void push_back(const T & data) { _insert(head->prev, data); };
    void pop_back() { return _remove(head->prev); };
    T & front() { return head->next->data; };
    T & back() { return head->prev->data; };
    unsigned int size() { return _size; };

    iterator begin() { return iterator(head->next); };
    const_iterator begin() const { return const_iterator(head->next); };
    iterator end() { return iterator(head); };
    const_iterator end() const { return const_iterator(head); };
};

#endif
