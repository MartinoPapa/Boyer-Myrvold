using namespace std;

template <typename T>
struct List_Node
{
    T data;
    List_Node<T> *next;
    List_Node<T> *prev;
    List_Node(T data)
    {
        this->data = data;
    }
};

template <typename T>
class CircularLinkedList
{
public:
    int count;
    List_Node<T> *head;
    List_Node<T> *tail;

public:
    CircularLinkedList()
    {
        count = 0;
    }
    /// restituisce la posizione del nodo inserito
    List_Node<T> *push_back(T data)
    {
        List_Node<T> *nodo = new List_Node<T>(data);
        if (count == 0)
        {
            head = nodo;
            tail = nodo;
        }
        else
        {
            tail->next = nodo;
            nodo->prev = tail;
            tail = nodo;
        }
        tail->next = head; //circolarità
        head->prev = tail; //circolarità
        count++;
        return nodo;
    }
    // O(n)
    void removeAtPosition(List_Node<T>* element){
        element->prev->next = element->next;
        element->next->prev = element->prev;
        if(element == tail){
            tail = element->prev;
            head->prev=tail;
        }
        if(element == head){
            head = element->next;
            tail->next = head;
        }
        count--;
    }

    bool isFirst(List_Node<T>* node){
        return node == head;
    }

    bool hasNext(List_Node<T>* node){
        return node->next!=head;
    }
};
